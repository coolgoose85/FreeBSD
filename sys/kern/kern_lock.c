/*-
 * Copyright (c) 2008 Attilio Rao <attilio@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice(s), this list of conditions and the following disclaimer as
 *    the first lines of this file unmodified other than the possible
 *    addition of one or more copyright notices.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice(s), this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include "opt_ddb.h"

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/ktr.h>
#include <sys/lock.h>
#include <sys/lock_profile.h>
#include <sys/lockmgr.h>
#include <sys/mutex.h>
#include <sys/proc.h>
#include <sys/sleepqueue.h>
#ifdef DEBUG_LOCKS
#include <sys/stack.h>
#endif
#include <sys/systm.h>

#include <machine/cpu.h>

#ifdef DDB
#include <ddb/ddb.h>
#endif

CTASSERT(((LK_CANRECURSE | LK_NOSHARE) & LO_CLASSFLAGS) ==
    (LK_CANRECURSE | LK_NOSHARE));

#define	SQ_EXCLUSIVE_QUEUE	0
#define	SQ_SHARED_QUEUE		1

#ifndef INVARIANTS
#define	_lockmgr_assert(lk, what, file, line)
#define	TD_LOCKS_INC(td)
#define	TD_LOCKS_DEC(td)
#else
#define	TD_LOCKS_INC(td)	((td)->td_locks++)
#define	TD_LOCKS_DEC(td)	((td)->td_locks--)
#endif
#define	TD_SLOCKS_INC(td)	((td)->td_lk_slocks++)
#define	TD_SLOCKS_DEC(td)	((td)->td_lk_slocks--)

#ifndef DEBUG_LOCKS
#define	STACK_PRINT(lk)
#define	STACK_SAVE(lk)
#define	STACK_ZERO(lk)
#else
#define	STACK_PRINT(lk)	stack_print_ddb(&(lk)->lk_stack)
#define	STACK_SAVE(lk)	stack_save(&(lk)->lk_stack)
#define	STACK_ZERO(lk)	stack_zero(&(lk)->lk_stack)
#endif

#define	LOCK_LOG2(lk, string, arg1, arg2)				\
	if (LOCK_LOG_TEST(&(lk)->lock_object, 0))			\
		CTR2(KTR_LOCK, (string), (arg1), (arg2))
#define	LOCK_LOG3(lk, string, arg1, arg2, arg3)				\
	if (LOCK_LOG_TEST(&(lk)->lock_object, 0))			\
		CTR3(KTR_LOCK, (string), (arg1), (arg2), (arg3))

#define	GIANT_DECLARE							\
	int _i = 0;							\
	WITNESS_SAVE_DECL(Giant)
#define	GIANT_RESTORE() do {						\
	if (_i > 0) {							\
		while (_i--)						\
			mtx_lock(&Giant);				\
		WITNESS_RESTORE(&Giant.lock_object, Giant);		\
	}								\
} while (0)
#define	GIANT_SAVE() do {						\
	if (mtx_owned(&Giant)) {					\
		WITNESS_SAVE(&Giant.lock_object, Giant);		\
		while (mtx_owned(&Giant)) {				\
			_i++;						\
			mtx_unlock(&Giant);				\
		}							\
	}								\
} while (0)

#define	LK_CAN_SHARE(x)							\
	(((x) & LK_SHARE) && (((x) & LK_EXCLUSIVE_WAITERS) == 0 ||	\
	curthread->td_lk_slocks || (curthread->td_pflags & TDP_DEADLKTREAT)))
#define	LK_TRYOP(x)							\
	((x) & LK_NOWAIT)

#define	LK_CAN_WITNESS(x)						\
	(((x) & LK_NOWITNESS) == 0 && !LK_TRYOP(x))
#define	LK_TRYWIT(x)							\
	(LK_TRYOP(x) ? LOP_TRYLOCK : 0)

#define	lockmgr_disowned(lk)						\
	(((lk)->lk_lock & ~(LK_FLAGMASK & ~LK_SHARE)) == LK_KERNPROC)

#define	lockmgr_xlocked(lk)						\
	(((lk)->lk_lock & ~(LK_FLAGMASK & ~LK_SHARE)) == (uintptr_t)curthread)

static void	 assert_lockmgr(struct lock_object *lock, int how);
#ifdef DDB
static void	 db_show_lockmgr(struct lock_object *lock);
#endif
static void	 lock_lockmgr(struct lock_object *lock, int how);
static int	 unlock_lockmgr(struct lock_object *lock);

struct lock_class lock_class_lockmgr = {
	.lc_name = "lockmgr",
	.lc_flags = LC_RECURSABLE | LC_SLEEPABLE | LC_SLEEPLOCK | LC_UPGRADABLE,
	.lc_assert = assert_lockmgr,
#ifdef DDB
	.lc_ddb_show = db_show_lockmgr,
#endif
	.lc_lock = lock_lockmgr,
	.lc_unlock = unlock_lockmgr
};

static __inline struct thread *
lockmgr_xholder(struct lock *lk)
{
	uintptr_t x;

	x = lk->lk_lock;
	return ((x & LK_SHARE) ? NULL : (struct thread *)LK_HOLDER(x));
}

/*
 * It assumes sleepq_lock held and returns with this one unheld.
 * It also assumes the generic interlock is sane and previously checked.
 * If LK_INTERLOCK is specified the interlock is not reacquired after the
 * sleep.
 */
static __inline int
sleeplk(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *wmesg, int pri, int timo, int queue)
{
	GIANT_DECLARE;
	struct lock_class *class;
	int catch, error;

	class = (flags & LK_INTERLOCK) ? LOCK_CLASS(ilk) : NULL;
	catch = pri & PCATCH;
	pri &= PRIMASK;
	error = 0;

	LOCK_LOG3(lk, "%s: %p blocking on the %s sleepqueue", __func__, lk,
	    (queue == SQ_EXCLUSIVE_QUEUE) ? "exclusive" : "shared");

	if (flags & LK_INTERLOCK)
		class->lc_unlock(ilk);
	GIANT_SAVE();
	sleepq_add(&lk->lock_object, NULL, wmesg, SLEEPQ_LK | (catch ?
	    SLEEPQ_INTERRUPTIBLE : 0), queue);
	if ((flags & LK_TIMELOCK) && timo)
		sleepq_set_timeout(&lk->lock_object, timo);

	/*
	 * Decisional switch for real sleeping.
	 */
	if ((flags & LK_TIMELOCK) && timo && catch)
		error = sleepq_timedwait_sig(&lk->lock_object, pri);
	else if ((flags & LK_TIMELOCK) && timo)
		error = sleepq_timedwait(&lk->lock_object, pri);
	else if (catch)
		error = sleepq_wait_sig(&lk->lock_object, pri);
	else
		sleepq_wait(&lk->lock_object, pri);
	GIANT_RESTORE();
	if ((flags & LK_SLEEPFAIL) && error == 0)
		error = ENOLCK;

	return (error);
}

static __inline int
wakeupshlk(struct lock *lk, const char *file, int line)
{
	uintptr_t v, x;
	int queue, wakeup_swapper;

	TD_LOCKS_DEC(curthread);
	TD_SLOCKS_DEC(curthread);
	WITNESS_UNLOCK(&lk->lock_object, 0, file, line);
	LOCK_LOG_LOCK("SUNLOCK", &lk->lock_object, 0, 0, file, line);

	wakeup_swapper = 0;
	for (;;) {
		x = lk->lk_lock;

		/*
		 * If there is more than one shared lock held, just drop one
		 * and return.
		 */
		if (LK_SHARERS(x) > 1) {
			if (atomic_cmpset_ptr(&lk->lk_lock, x,
			    x - LK_ONE_SHARER))
				break;
			continue;
		}

		/*
		 * If there are not waiters on the exclusive queue, drop the
		 * lock quickly.
		 */
		if ((x & LK_ALL_WAITERS) == 0) {
			MPASS(x == LK_SHARERS_LOCK(1));
			if (atomic_cmpset_ptr(&lk->lk_lock, LK_SHARERS_LOCK(1),
			    LK_UNLOCKED))
				break;
			continue;
		}

		/*
		 * We should have a sharer with waiters, so enter the hard
		 * path in order to handle wakeups correctly.
		 */
		sleepq_lock(&lk->lock_object);
		x = lk->lk_lock & LK_ALL_WAITERS;
		v = LK_UNLOCKED;

		/*
		 * If the lock has exclusive waiters, give them preference in
		 * order to avoid deadlock with shared runners up.
		 */
		if (x & LK_EXCLUSIVE_WAITERS) {
			queue = SQ_EXCLUSIVE_QUEUE;
			v |= (x & LK_SHARED_WAITERS);
		} else {
			MPASS(x == LK_SHARED_WAITERS);
			queue = SQ_SHARED_QUEUE;
		}

		if (!atomic_cmpset_ptr(&lk->lk_lock, LK_SHARERS_LOCK(1) | x,
		    v)) {
			sleepq_release(&lk->lock_object);
			continue;
		}
		LOCK_LOG3(lk, "%s: %p waking up threads on the %s queue",
		    __func__, lk, queue == SQ_SHARED_QUEUE ? "shared" :
		    "exclusive");
		wakeup_swapper = sleepq_broadcast(&lk->lock_object, SLEEPQ_LK,
		    0, queue);
		sleepq_release(&lk->lock_object);
		break;
	}

	lock_profile_release_lock(&lk->lock_object);
	return (wakeup_swapper);
}

static void
assert_lockmgr(struct lock_object *lock, int what)
{

	panic("lockmgr locks do not support assertions");
}

static void
lock_lockmgr(struct lock_object *lock, int how)
{

	panic("lockmgr locks do not support sleep interlocking");
}

static int
unlock_lockmgr(struct lock_object *lock)
{

	panic("lockmgr locks do not support sleep interlocking");
}

void
lockinit(struct lock *lk, int pri, const char *wmesg, int timo, int flags)
{
	int iflags;

	MPASS((flags & ~LK_INIT_MASK) == 0);

	iflags = LO_RECURSABLE | LO_SLEEPABLE | LO_UPGRADABLE;
	if ((flags & LK_NODUP) == 0)
		iflags |= LO_DUPOK;
	if (flags & LK_NOPROFILE)
		iflags |= LO_NOPROFILE;
	if ((flags & LK_NOWITNESS) == 0)
		iflags |= LO_WITNESS;
	if (flags & LK_QUIET)
		iflags |= LO_QUIET;
	iflags |= flags & (LK_CANRECURSE | LK_NOSHARE);

	lk->lk_lock = LK_UNLOCKED;
	lk->lk_recurse = 0;
	lk->lk_timo = timo;
	lk->lk_pri = pri;
	lock_init(&lk->lock_object, &lock_class_lockmgr, wmesg, NULL, iflags);
	STACK_ZERO(lk);
}

void
lockdestroy(struct lock *lk)
{

	KASSERT(lk->lk_lock == LK_UNLOCKED, ("lockmgr still held"));
	KASSERT(lk->lk_recurse == 0, ("lockmgr still recursed"));
	lock_destroy(&lk->lock_object);
}

int
__lockmgr_args(struct lock *lk, u_int flags, struct lock_object *ilk,
    const char *wmesg, int pri, int timo, const char *file, int line)
{
	GIANT_DECLARE;
	uint64_t waittime;
	struct lock_class *class;
	const char *iwmesg;
	uintptr_t tid, v, x;
	u_int op;
	int contested, error, ipri, itimo, queue, wakeup_swapper;

	contested = 0;
	error = 0;
	waittime = 0;
	tid = (uintptr_t)curthread;
	op = (flags & LK_TYPE_MASK);
	iwmesg = (wmesg == LK_WMESG_DEFAULT) ? lk->lock_object.lo_name : wmesg;
	ipri = (pri == LK_PRIO_DEFAULT) ? lk->lk_pri : pri;
	itimo = (timo == LK_TIMO_DEFAULT) ? lk->lk_timo : timo;

	MPASS((flags & ~LK_TOTAL_MASK) == 0);
	KASSERT((op & (op - 1)) == 0,
	    ("%s: Invalid requested operation @ %s:%d", __func__, file, line));
	KASSERT((flags & (LK_NOWAIT | LK_SLEEPFAIL)) == 0 ||
	    (op != LK_DOWNGRADE && op != LK_RELEASE),
	    ("%s: Invalid flags in regard of the operation desired @ %s:%d",
	    __func__, file, line));
	KASSERT((flags & LK_INTERLOCK) == 0 || ilk != NULL,
	    ("%s: LK_INTERLOCK passed without valid interlock @ %s:%d",
	    __func__, file, line));

	class = (flags & LK_INTERLOCK) ? LOCK_CLASS(ilk) : NULL;
	if (panicstr != NULL) {
		if (flags & LK_INTERLOCK)
			class->lc_unlock(ilk);
		return (0);
	}

	if (op == LK_SHARED && (lk->lock_object.lo_flags & LK_NOSHARE))
		op = LK_EXCLUSIVE;

	wakeup_swapper = 0;
	switch (op) {
	case LK_SHARED:
		if (LK_CAN_WITNESS(flags))
			WITNESS_CHECKORDER(&lk->lock_object, LOP_NEWORDER,
			    file, line, ilk);
		for (;;) {
			x = lk->lk_lock;

			/*
			 * If no other thread has an exclusive lock, or
			 * no exclusive waiter is present, bump the count of
			 * sharers.  Since we have to preserve the state of
			 * waiters, if we fail to acquire the shared lock
			 * loop back and retry.
			 */
			if (LK_CAN_SHARE(x)) {
				if (atomic_cmpset_acq_ptr(&lk->lk_lock, x,
				    x + LK_ONE_SHARER))
					break;
				continue;
			}
			lock_profile_obtain_lock_failed(&lk->lock_object,
			    &contested, &waittime);

			/*
			 * If the lock is already held by curthread in
			 * exclusive way avoid a deadlock.
			 */
			if (LK_HOLDER(x) == tid) {
				LOCK_LOG2(lk,
				    "%s: %p already held in exclusive mode",
				    __func__, lk);
				error = EDEADLK;
				break;
			}

			/*
			 * If the lock is expected to not sleep just give up
			 * and return.
			 */
			if (LK_TRYOP(flags)) {
				LOCK_LOG2(lk, "%s: %p fails the try operation",
				    __func__, lk);
				error = EBUSY;
				break;
			}

			/*
			 * Acquire the sleepqueue chain lock because we
			 * probabilly will need to manipulate waiters flags.
			 */
			sleepq_lock(&lk->lock_object);
			x = lk->lk_lock;

			/*
			 * if the lock can be acquired in shared mode, try
			 * again.
			 */
			if (LK_CAN_SHARE(x)) {
				sleepq_release(&lk->lock_object);
				continue;
			}

			/*
			 * Try to set the LK_SHARED_WAITERS flag.  If we fail,
			 * loop back and retry.
			 */
			if ((x & LK_SHARED_WAITERS) == 0) {
				if (!atomic_cmpset_acq_ptr(&lk->lk_lock, x,
				    x | LK_SHARED_WAITERS)) {
					sleepq_release(&lk->lock_object);
					continue;
				}
				LOCK_LOG2(lk, "%s: %p set shared waiters flag",
				    __func__, lk);
			}

			/*
			 * As far as we have been unable to acquire the
			 * shared lock and the shared waiters flag is set,
			 * we will sleep.
			 */
			error = sleeplk(lk, flags, ilk, iwmesg, ipri, itimo,
			    SQ_SHARED_QUEUE);
			flags &= ~LK_INTERLOCK;
			if (error) {
				LOCK_LOG3(lk,
				    "%s: interrupted sleep for %p with %d",
				    __func__, lk, error);
				break;
			}
			LOCK_LOG2(lk, "%s: %p resuming from the sleep queue",
			    __func__, lk);
		}
		if (error == 0) {
			lock_profile_obtain_lock_success(&lk->lock_object,
			    contested, waittime, file, line);
			LOCK_LOG_LOCK("SLOCK", &lk->lock_object, 0, 0, file,
			    line);
			WITNESS_LOCK(&lk->lock_object, LK_TRYWIT(flags), file,
			    line);
			TD_LOCKS_INC(curthread);
			TD_SLOCKS_INC(curthread);
			STACK_SAVE(lk);
		}
		break;
	case LK_UPGRADE:
		_lockmgr_assert(lk, KA_SLOCKED, file, line);
		x = lk->lk_lock & LK_ALL_WAITERS;

		/*
		 * Try to switch from one shared lock to an exclusive one.
		 * We need to preserve waiters flags during the operation.
		 */
		if (atomic_cmpset_ptr(&lk->lk_lock, LK_SHARERS_LOCK(1) | x,
		    tid | x)) {
			LOCK_LOG_LOCK("XUPGRADE", &lk->lock_object, 0, 0, file,
			    line);
			WITNESS_UPGRADE(&lk->lock_object, LOP_EXCLUSIVE |
			    LK_TRYWIT(flags), file, line);
			TD_SLOCKS_DEC(curthread);
			break;
		}

		/*
		 * We have been unable to succeed in upgrading, so just
		 * give up the shared lock.
		 */
		wakeup_swapper |= wakeupshlk(lk, file, line);

		/* FALLTHROUGH */
	case LK_EXCLUSIVE:
		if (LK_CAN_WITNESS(flags))
			WITNESS_CHECKORDER(&lk->lock_object, LOP_NEWORDER |
			    LOP_EXCLUSIVE, file, line, ilk);

		/*
		 * If curthread already holds the lock and this one is
		 * allowed to recurse, simply recurse on it.
		 */
		if (lockmgr_xlocked(lk)) {
			if ((flags & LK_CANRECURSE) == 0 &&
			    (lk->lock_object.lo_flags & LK_CANRECURSE) == 0) {

				/*
				 * If the lock is expected to not panic just
				 * give up and return.
				 */
				if (LK_TRYOP(flags)) {
					LOCK_LOG2(lk,
					    "%s: %p fails the try operation",
					    __func__, lk);
					error = EBUSY;
					break;
				}
				if (flags & LK_INTERLOCK)
					class->lc_unlock(ilk);
		panic("%s: recursing on non recursive lockmgr %s @ %s:%d\n",
				    __func__, iwmesg, file, line);
			}
			lk->lk_recurse++;
			LOCK_LOG2(lk, "%s: %p recursing", __func__, lk);
			LOCK_LOG_LOCK("XLOCK", &lk->lock_object, 0,
			    lk->lk_recurse, file, line);
			WITNESS_LOCK(&lk->lock_object, LOP_EXCLUSIVE |
			    LK_TRYWIT(flags), file, line);
			TD_LOCKS_INC(curthread);
			break;
		}

		while (!atomic_cmpset_acq_ptr(&lk->lk_lock, LK_UNLOCKED,
		    tid)) {
			lock_profile_obtain_lock_failed(&lk->lock_object,
			    &contested, &waittime);

			/*
			 * If the lock is expected to not sleep just give up
			 * and return.
			 */
			if (LK_TRYOP(flags)) {
				LOCK_LOG2(lk, "%s: %p fails the try operation",
				    __func__, lk);
				error = EBUSY;
				break;
			}

			/*
			 * Acquire the sleepqueue chain lock because we
			 * probabilly will need to manipulate waiters flags.
			 */
			sleepq_lock(&lk->lock_object);
			x = lk->lk_lock;
			v = x & LK_ALL_WAITERS;

			/*
			 * if the lock has been released while we spun on
			 * the sleepqueue chain lock just try again.
			 */
			if (x == LK_UNLOCKED) {
				sleepq_release(&lk->lock_object);
				continue;
			}

			/*
			 * The lock can be in the state where there is a
			 * pending queue of waiters, but still no owner.
			 * This happens when the lock is contested and an
			 * owner is going to claim the lock.
			 * If curthread is the one successfully acquiring it
			 * claim lock ownership and return, preserving waiters
			 * flags.
			 */
			if (x == (LK_UNLOCKED | v)) {
				if (atomic_cmpset_acq_ptr(&lk->lk_lock, x,
				    tid | v)) {
					sleepq_release(&lk->lock_object);
					LOCK_LOG2(lk,
					    "%s: %p claimed by a new writer",
					    __func__, lk);
					break;
				}
				sleepq_release(&lk->lock_object);
				continue;
			}

			/*
			 * Try to set the LK_EXCLUSIVE_WAITERS flag.  If we
			 * fail, loop back and retry.
			 */
			if ((x & LK_EXCLUSIVE_WAITERS) == 0) {
				if (!atomic_cmpset_ptr(&lk->lk_lock, x,
				    x | LK_EXCLUSIVE_WAITERS)) {
					sleepq_release(&lk->lock_object);
					continue;
				}
				LOCK_LOG2(lk, "%s: %p set excl waiters flag",
				    __func__, lk);
			}

			/*
			 * As far as we have been unable to acquire the
			 * exclusive lock and the exclusive waiters flag
			 * is set, we will sleep.
			 */
			error = sleeplk(lk, flags, ilk, iwmesg, ipri, itimo,
			    SQ_EXCLUSIVE_QUEUE);
			flags &= ~LK_INTERLOCK;
			if (error) {
				LOCK_LOG3(lk,
				    "%s: interrupted sleep for %p with %d",
				    __func__, lk, error);
				break;
			}
			LOCK_LOG2(lk, "%s: %p resuming from the sleep queue",
			    __func__, lk);
		}
		if (error == 0) {
			lock_profile_obtain_lock_success(&lk->lock_object,
			    contested, waittime, file, line);
			LOCK_LOG_LOCK("XLOCK", &lk->lock_object, 0,
			    lk->lk_recurse, file, line);
			WITNESS_LOCK(&lk->lock_object, LOP_EXCLUSIVE |
			    LK_TRYWIT(flags), file, line);
			TD_LOCKS_INC(curthread);
			STACK_SAVE(lk);
		}
		break;
	case LK_DOWNGRADE:
		_lockmgr_assert(lk, KA_XLOCKED | KA_NOTRECURSED, file, line);
		LOCK_LOG_LOCK("XDOWNGRADE", &lk->lock_object, 0, 0, file, line);
		WITNESS_DOWNGRADE(&lk->lock_object, 0, file, line);
		TD_SLOCKS_INC(curthread);

		/*
		 * In order to preserve waiters flags, just spin.
		 */
		for (;;) {
			x = lk->lk_lock & LK_ALL_WAITERS;
			if (atomic_cmpset_rel_ptr(&lk->lk_lock, tid | x,
			    LK_SHARERS_LOCK(1) | x))
				break;
			cpu_spinwait();
		}
		break;
	case LK_RELEASE:
		_lockmgr_assert(lk, KA_LOCKED, file, line);
		x = lk->lk_lock;

		if ((x & LK_SHARE) == 0) {

			/*
			 * As first option, treact the lock as if it has not
			 * any waiter.
			 * Fix-up the tid var if the lock has been disowned.
			 */
			if (LK_HOLDER(x) == LK_KERNPROC)
				tid = LK_KERNPROC;
			else {
				WITNESS_UNLOCK(&lk->lock_object, LOP_EXCLUSIVE,
				    file, line);
				TD_LOCKS_DEC(curthread);
			}
			LOCK_LOG_LOCK("XUNLOCK", &lk->lock_object, 0,
			    lk->lk_recurse, file, line);

			/*
			 * The lock is held in exclusive mode.
			 * If the lock is recursed also, then unrecurse it.
			 */
			if (lockmgr_xlocked(lk) && lockmgr_recursed(lk)) {
				LOCK_LOG2(lk, "%s: %p unrecursing", __func__,
				    lk);
				lk->lk_recurse--;
				break;
			}
			lock_profile_release_lock(&lk->lock_object);

			if (atomic_cmpset_rel_ptr(&lk->lk_lock, tid,
			    LK_UNLOCKED))
				break;

			sleepq_lock(&lk->lock_object);
			x = lk->lk_lock & LK_ALL_WAITERS;
			v = LK_UNLOCKED;

			/*
		 	 * If the lock has exclusive waiters, give them
			 * preference in order to avoid deadlock with
			 * shared runners up.
			 */
			if (x & LK_EXCLUSIVE_WAITERS) {
				queue = SQ_EXCLUSIVE_QUEUE;
				v |= (x & LK_SHARED_WAITERS);
			} else {
				MPASS(x == LK_SHARED_WAITERS);
				queue = SQ_SHARED_QUEUE;
			}

			LOCK_LOG3(lk,
			    "%s: %p waking up threads on the %s queue",
			    __func__, lk, queue == SQ_SHARED_QUEUE ? "shared" :
			    "exclusive");
			atomic_store_rel_ptr(&lk->lk_lock, v);
			wakeup_swapper = sleepq_broadcast(&lk->lock_object,
			    SLEEPQ_LK, 0, queue);
			sleepq_release(&lk->lock_object);
			break;
		} else
			wakeup_swapper = wakeupshlk(lk, file, line);
		break;
	case LK_DRAIN:
		if (LK_CAN_WITNESS(flags))
			WITNESS_CHECKORDER(&lk->lock_object, LOP_NEWORDER |
			    LOP_EXCLUSIVE, file, line, ilk);

		/*
		 * Trying to drain a lock we already own will result in a
		 * deadlock.
		 */
		if (lockmgr_xlocked(lk)) {
			if (flags & LK_INTERLOCK)
				class->lc_unlock(ilk);
			panic("%s: draining %s with the lock held @ %s:%d\n",
			    __func__, iwmesg, file, line);
		}

		while (!atomic_cmpset_acq_ptr(&lk->lk_lock, LK_UNLOCKED, tid)) {
			lock_profile_obtain_lock_failed(&lk->lock_object,
			    &contested, &waittime);

			/*
			 * If the lock is expected to not sleep just give up
			 * and return.
			 */
			if (LK_TRYOP(flags)) {
				LOCK_LOG2(lk, "%s: %p fails the try operation",
				    __func__, lk);
				error = EBUSY;
				break;
			}

			/*
			 * Acquire the sleepqueue chain lock because we
			 * probabilly will need to manipulate waiters flags.
			 */
			sleepq_lock(&lk->lock_object);
			x = lk->lk_lock;
			v = x & LK_ALL_WAITERS;

			/*
			 * if the lock has been released while we spun on
			 * the sleepqueue chain lock just try again.
			 */
			if (x == LK_UNLOCKED) {
				sleepq_release(&lk->lock_object);
				continue;
			}

			if (x == (LK_UNLOCKED | v)) {
				v = x;
				if (v & LK_EXCLUSIVE_WAITERS) {
					queue = SQ_EXCLUSIVE_QUEUE;
					v &= ~LK_EXCLUSIVE_WAITERS;
				} else {
					MPASS(v & LK_SHARED_WAITERS);
					queue = SQ_SHARED_QUEUE;
					v &= ~LK_SHARED_WAITERS;
				}
				if (!atomic_cmpset_ptr(&lk->lk_lock, x, v)) {
					sleepq_release(&lk->lock_object);
					continue;
				}
				LOCK_LOG3(lk,
				"%s: %p waking up all threads on the %s queue",
				    __func__, lk, queue == SQ_SHARED_QUEUE ?
				    "shared" : "exclusive");
				wakeup_swapper |= sleepq_broadcast(
				    &lk->lock_object, SLEEPQ_LK, 0, queue);

				/*
				 * If shared waiters have been woken up we need
				 * to wait for one of them to acquire the lock
				 * before to set the exclusive waiters in
				 * order to avoid a deadlock.
				 */
				if (queue == SQ_SHARED_QUEUE) {
					for (v = lk->lk_lock;
					    (v & LK_SHARE) && !LK_SHARERS(v);
					    v = lk->lk_lock)
						cpu_spinwait();
				}
			}

			/*
			 * Try to set the LK_EXCLUSIVE_WAITERS flag.  If we
			 * fail, loop back and retry.
			 */
			if ((x & LK_EXCLUSIVE_WAITERS) == 0) {
				if (!atomic_cmpset_ptr(&lk->lk_lock, x,
				    x | LK_EXCLUSIVE_WAITERS)) {
					sleepq_release(&lk->lock_object);
					continue;
				}
				LOCK_LOG2(lk, "%s: %p set drain waiters flag",
				    __func__, lk);
			}

			/*
			 * As far as we have been unable to acquire the
			 * exclusive lock and the exclusive waiters flag
			 * is set, we will sleep.
			 */
			if (flags & LK_INTERLOCK) {
				class->lc_unlock(ilk);
				flags &= ~LK_INTERLOCK;
			}
			GIANT_SAVE();
			sleepq_add(&lk->lock_object, NULL, iwmesg, SLEEPQ_LK,
			    SQ_EXCLUSIVE_QUEUE);
			sleepq_wait(&lk->lock_object, ipri & PRIMASK);
			GIANT_RESTORE();
			LOCK_LOG2(lk, "%s: %p resuming from the sleep queue",
			    __func__, lk);
		}

		if (error == 0) {
			lock_profile_obtain_lock_success(&lk->lock_object,
			    contested, waittime, file, line);
			LOCK_LOG_LOCK("DRAIN", &lk->lock_object, 0,
			    lk->lk_recurse, file, line);
			WITNESS_LOCK(&lk->lock_object, LOP_EXCLUSIVE |
			    LK_TRYWIT(flags), file, line);
			TD_LOCKS_INC(curthread);
			STACK_SAVE(lk);
		}
		break;
	default:
		if (flags & LK_INTERLOCK)
			class->lc_unlock(ilk);
		panic("%s: unknown lockmgr request 0x%x\n", __func__, op);
	}

	if (flags & LK_INTERLOCK)
		class->lc_unlock(ilk);
	if (wakeup_swapper)
		kick_proc0();

	return (error);
}

void
_lockmgr_disown(struct lock *lk, const char *file, int line)
{
	uintptr_t tid, x;

	tid = (uintptr_t)curthread;
	_lockmgr_assert(lk, KA_XLOCKED | KA_NOTRECURSED, file, line);

	/*
	 * If the owner is already LK_KERNPROC just skip the whole operation.
	 */
	if (LK_HOLDER(lk->lk_lock) != tid)
		return;
	LOCK_LOG_LOCK("XDISOWN", &lk->lock_object, 0, 0, file, line);
	WITNESS_UNLOCK(&lk->lock_object, LOP_EXCLUSIVE, file, line);
	TD_LOCKS_DEC(curthread);

	/*
	 * In order to preserve waiters flags, just spin.
	 */
	for (;;) {
		x = lk->lk_lock & LK_ALL_WAITERS;
		if (atomic_cmpset_rel_ptr(&lk->lk_lock, tid | x,
		    LK_KERNPROC | x))
			return;
		cpu_spinwait();
	}
}

void
lockmgr_printinfo(struct lock *lk)
{
	struct thread *td;
	uintptr_t x;

	if (lk->lk_lock == LK_UNLOCKED)
		printf("lock type %s: UNLOCKED\n", lk->lock_object.lo_name);
	else if (lk->lk_lock & LK_SHARE)
		printf("lock type %s: SHARED (count %ju)\n",
		    lk->lock_object.lo_name,
		    (uintmax_t)LK_SHARERS(lk->lk_lock));
	else {
		td = lockmgr_xholder(lk);
		printf("lock type %s: EXCL by thread %p (pid %d)\n",
		    lk->lock_object.lo_name, td, td->td_proc->p_pid);
	}

	x = lk->lk_lock;
	if (x & LK_EXCLUSIVE_WAITERS)
		printf(" with exclusive waiters pending\n");
	if (x & LK_SHARED_WAITERS)
		printf(" with shared waiters pending\n");

	STACK_PRINT(lk);
}

int
lockstatus(struct lock *lk)
{
	uintptr_t v, x;
	int ret;

	ret = LK_SHARED;
	x = lk->lk_lock;
	v = LK_HOLDER(x);

	if ((x & LK_SHARE) == 0) {
		if (v == (uintptr_t)curthread || v == LK_KERNPROC)
			ret = LK_EXCLUSIVE;
		else
			ret = LK_EXCLOTHER;
	} else if (x == LK_UNLOCKED)
		ret = 0;

	return (ret);
}

#ifdef INVARIANT_SUPPORT
#ifndef INVARIANTS
#undef	_lockmgr_assert
#endif

void
_lockmgr_assert(struct lock *lk, int what, const char *file, int line)
{
	int slocked = 0;

	if (panicstr != NULL)
		return;
	switch (what) {
	case KA_SLOCKED:
	case KA_SLOCKED | KA_NOTRECURSED:
	case KA_SLOCKED | KA_RECURSED:
		slocked = 1;
	case KA_LOCKED:
	case KA_LOCKED | KA_NOTRECURSED:
	case KA_LOCKED | KA_RECURSED:
#ifdef WITNESS

		/*
		 * We cannot trust WITNESS if the lock is held in exclusive
		 * mode and a call to lockmgr_disown() happened.
		 * Workaround this skipping the check if the lock is held in
		 * exclusive mode even for the KA_LOCKED case.
		 */
		if (slocked || (lk->lk_lock & LK_SHARE)) {
			witness_assert(&lk->lock_object, what, file, line);
			break;
		}
#endif
		if (lk->lk_lock == LK_UNLOCKED ||
		    ((lk->lk_lock & LK_SHARE) == 0 && (slocked ||
		    (!lockmgr_xlocked(lk) && !lockmgr_disowned(lk)))))
			panic("Lock %s not %slocked @ %s:%d\n",
			    lk->lock_object.lo_name, slocked ? "share" : "",
			    file, line);

		if ((lk->lk_lock & LK_SHARE) == 0) {
			if (lockmgr_recursed(lk)) {
				if (what & KA_NOTRECURSED)
					panic("Lock %s recursed @ %s:%d\n",
					    lk->lock_object.lo_name, file,
					    line);
			} else if (what & KA_RECURSED)
				panic("Lock %s not recursed @ %s:%d\n",
				    lk->lock_object.lo_name, file, line);
		}
		break;
	case KA_XLOCKED:
	case KA_XLOCKED | KA_NOTRECURSED:
	case KA_XLOCKED | KA_RECURSED:
		if (!lockmgr_xlocked(lk) && !lockmgr_disowned(lk))
			panic("Lock %s not exclusively locked @ %s:%d\n",
			    lk->lock_object.lo_name, file, line);
		if (lockmgr_recursed(lk)) {
			if (what & KA_NOTRECURSED)
				panic("Lock %s recursed @ %s:%d\n",
				    lk->lock_object.lo_name, file, line);
		} else if (what & KA_RECURSED)
			panic("Lock %s not recursed @ %s:%d\n",
			    lk->lock_object.lo_name, file, line);
		break;
	case KA_UNLOCKED:
		if (lockmgr_xlocked(lk) || lockmgr_disowned(lk))
			panic("Lock %s exclusively locked @ %s:%d\n",
			    lk->lock_object.lo_name, file, line);
		break;
	default:
		panic("Unknown lockmgr assertion: %d @ %s:%d\n", what, file,
		    line);
	}
}
#endif

#ifdef DDB
int
lockmgr_chain(struct thread *td, struct thread **ownerp)
{
	struct lock *lk;

	lk = td->td_wchan;

	if (LOCK_CLASS(&lk->lock_object) != &lock_class_lockmgr)
		return (0);
	db_printf("blocked on lockmgr %s", lk->lock_object.lo_name);
	if (lk->lk_lock & LK_SHARE)
		db_printf("SHARED (count %ju)\n",
		    (uintmax_t)LK_SHARERS(lk->lk_lock));
	else
		db_printf("EXCL\n");
	*ownerp = lockmgr_xholder(lk);

	return (1);
}

static void
db_show_lockmgr(struct lock_object *lock)
{
	struct thread *td;
	struct lock *lk;

	lk = (struct lock *)lock;

	db_printf(" state: ");
	if (lk->lk_lock == LK_UNLOCKED)
		db_printf("UNLOCKED\n");
	else if (lk->lk_lock & LK_SHARE)
		db_printf("SLOCK: %ju\n", (uintmax_t)LK_SHARERS(lk->lk_lock));
	else {
		td = lockmgr_xholder(lk);
		if (td == (struct thread *)LK_KERNPROC)
			db_printf("XLOCK: LK_KERNPROC\n");
		else
			db_printf("XLOCK: %p (tid %d, pid %d, \"%s\")\n", td,
			    td->td_tid, td->td_proc->p_pid,
			    td->td_proc->p_comm);
		if (lockmgr_recursed(lk))
			db_printf(" recursed: %d\n", lk->lk_recurse);
	}
	db_printf(" waiters: ");
	switch (lk->lk_lock & LK_ALL_WAITERS) {
	case LK_SHARED_WAITERS:
		db_printf("shared\n");
	case LK_EXCLUSIVE_WAITERS:
		db_printf("exclusive\n");
		break;
	case LK_ALL_WAITERS:
		db_printf("shared and exclusive\n");
		break;
	default:
		db_printf("none\n");
	}
}
#endif
