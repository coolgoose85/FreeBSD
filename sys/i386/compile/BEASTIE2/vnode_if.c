/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from $FreeBSD$
 */

#include <sys/param.h>
#include <sys/event.h>
#include <sys/mount.h>
#include <sys/systm.h>
#include <sys/vnode.h>

struct vnodeop_desc vop_default_desc = {
	"default",
	0,
	(vop_bypass_t *)vop_panic,
	NULL,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_islocked_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_islocked_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_ISLOCKED_AP(struct vop_islocked_args *a)
{

	return(VOP_ISLOCKED_APV(a->a_vp->v_op, a));
}

int
VOP_ISLOCKED_APV(struct vop_vector *vop, struct vop_islocked_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_islocked_desc, a->a_vp,
	    ("Wrong a_desc in vop_islocked(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_islocked == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_islocked(%p, %p)", a->a_vp, a));
	if (vop->vop_islocked != NULL)
		rc = vop->vop_islocked(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR1(KTR_VOP,
	    "VOP_ISLOCKED(vp 0x%lX)",
	    a->a_vp);
	if (rc == 0) {
	} else {
	}
	return (rc);
}

struct vnodeop_desc vop_islocked_desc = {
	"vop_islocked",
	0,
	(vop_bypass_t *)VOP_ISLOCKED_AP,
	vop_islocked_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_lookup_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_lookup_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_LOOKUP_AP(struct vop_lookup_args *a)
{

	return(VOP_LOOKUP_APV(a->a_dvp->v_op, a));
}

int
VOP_LOOKUP_APV(struct vop_vector *vop, struct vop_lookup_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_lookup_desc, a->a_dvp,
	    ("Wrong a_desc in vop_lookup(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_lookup == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_lookup(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP");
	ASSERT_VOP_LOCKED(a->a_dvp, "VOP_LOOKUP");
	vop_lookup_pre(a);
	if (vop->vop_lookup != NULL)
		rc = vop->vop_lookup(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_LOOKUP(dvp 0x%lX, vpp 0x%lX, cnp 0x%lX)",
	    a->a_dvp, a->a_vpp, a->a_cnp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_LOOKUP");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_LOOKUP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_LOOKUP");
	}
	vop_lookup_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_lookup_desc = {
	"vop_lookup",
	0,
	(vop_bypass_t *)VOP_LOOKUP_AP,
	vop_lookup_vp_offsets,
	VOPARG_OFFSETOF(struct vop_lookup_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_lookup_args,a_cnp),
};

static int vop_cachedlookup_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_cachedlookup_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_CACHEDLOOKUP_AP(struct vop_cachedlookup_args *a)
{

	return(VOP_CACHEDLOOKUP_APV(a->a_dvp->v_op, a));
}

int
VOP_CACHEDLOOKUP_APV(struct vop_vector *vop, struct vop_cachedlookup_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_cachedlookup_desc, a->a_dvp,
	    ("Wrong a_desc in vop_cachedlookup(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_cachedlookup == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_cachedlookup(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
	ASSERT_VOP_LOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
	if (vop->vop_cachedlookup != NULL)
		rc = vop->vop_cachedlookup(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_CACHEDLOOKUP(dvp 0x%lX, vpp 0x%lX, cnp 0x%lX)",
	    a->a_dvp, a->a_vpp, a->a_cnp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_CACHEDLOOKUP");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_CACHEDLOOKUP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CACHEDLOOKUP");
	}
	return (rc);
}

struct vnodeop_desc vop_cachedlookup_desc = {
	"vop_cachedlookup",
	0,
	(vop_bypass_t *)VOP_CACHEDLOOKUP_AP,
	vop_cachedlookup_vp_offsets,
	VOPARG_OFFSETOF(struct vop_cachedlookup_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_cachedlookup_args,a_cnp),
};

static int vop_create_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_create_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_CREATE_AP(struct vop_create_args *a)
{

	return(VOP_CREATE_APV(a->a_dvp->v_op, a));
}

int
VOP_CREATE_APV(struct vop_vector *vop, struct vop_create_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_create_desc, a->a_dvp,
	    ("Wrong a_desc in vop_create(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_create == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_create(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE");
	if (vop->vop_create != NULL)
		rc = vop->vop_create(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_CREATE(dvp 0x%lX, vpp 0x%lX, cnp 0x%lX, vap 0x%lX)",
	    a->a_dvp, a->a_vpp, a->a_cnp, a->a_vap);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_CREATE");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_CREATE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_CREATE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_CREATE");
	}
	vop_create_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_create_desc = {
	"vop_create",
	0,
	(vop_bypass_t *)VOP_CREATE_AP,
	vop_create_vp_offsets,
	VOPARG_OFFSETOF(struct vop_create_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_create_args,a_cnp),
};

static int vop_whiteout_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_whiteout_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_WHITEOUT_AP(struct vop_whiteout_args *a)
{

	return(VOP_WHITEOUT_APV(a->a_dvp->v_op, a));
}

int
VOP_WHITEOUT_APV(struct vop_vector *vop, struct vop_whiteout_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_whiteout_desc, a->a_dvp,
	    ("Wrong a_desc in vop_whiteout(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_whiteout == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_whiteout(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT");
	if (vop->vop_whiteout != NULL)
		rc = vop->vop_whiteout(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_WHITEOUT(dvp 0x%lX, cnp 0x%lX, flags %ld)",
	    a->a_dvp, a->a_cnp, a->a_flags);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_WHITEOUT");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_WHITEOUT");
	}
	return (rc);
}

struct vnodeop_desc vop_whiteout_desc = {
	"vop_whiteout",
	0,
	(vop_bypass_t *)VOP_WHITEOUT_AP,
	vop_whiteout_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_whiteout_args,a_cnp),
};

static int vop_mknod_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_mknod_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_MKNOD_AP(struct vop_mknod_args *a)
{

	return(VOP_MKNOD_APV(a->a_dvp->v_op, a));
}

int
VOP_MKNOD_APV(struct vop_vector *vop, struct vop_mknod_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_mknod_desc, a->a_dvp,
	    ("Wrong a_desc in vop_mknod(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_mknod == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_mknod(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD");
	if (vop->vop_mknod != NULL)
		rc = vop->vop_mknod(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_MKNOD(dvp 0x%lX, vpp 0x%lX, cnp 0x%lX, vap 0x%lX)",
	    a->a_dvp, a->a_vpp, a->a_cnp, a->a_vap);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_MKNOD");
		ASSERT_VOP_LOCKED(*a->a_vpp, "VOP_MKNOD");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKNOD");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKNOD");
	}
	vop_mknod_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_mknod_desc = {
	"vop_mknod",
	0,
	(vop_bypass_t *)VOP_MKNOD_AP,
	vop_mknod_vp_offsets,
	VOPARG_OFFSETOF(struct vop_mknod_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_mknod_args,a_cnp),
};

static int vop_open_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_open_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_OPEN_AP(struct vop_open_args *a)
{

	return(VOP_OPEN_APV(a->a_vp->v_op, a));
}

int
VOP_OPEN_APV(struct vop_vector *vop, struct vop_open_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_open_desc, a->a_vp,
	    ("Wrong a_desc in vop_open(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_open == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_open(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN");
	if (vop->vop_open != NULL)
		rc = vop->vop_open(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_OPEN(vp 0x%lX, mode %ld, cred 0x%lX, td 0x%lX, fp 0x%lX)",
	    a->a_vp, a->a_mode, a->a_cred, a->a_td, a->a_fp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPEN");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPEN");
	}
	return (rc);
}

struct vnodeop_desc vop_open_desc = {
	"vop_open",
	0,
	(vop_bypass_t *)VOP_OPEN_AP,
	vop_open_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_open_args,a_cred),
	VOPARG_OFFSETOF(struct vop_open_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_close_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_close_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_CLOSE_AP(struct vop_close_args *a)
{

	return(VOP_CLOSE_APV(a->a_vp->v_op, a));
}

int
VOP_CLOSE_APV(struct vop_vector *vop, struct vop_close_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_close_desc, a->a_vp,
	    ("Wrong a_desc in vop_close(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_close == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_close(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_CLOSE");
	if (vop->vop_close != NULL)
		rc = vop->vop_close(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_CLOSE(vp 0x%lX, fflag %ld, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_fflag, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_CLOSE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_CLOSE");
	}
	return (rc);
}

struct vnodeop_desc vop_close_desc = {
	"vop_close",
	0,
	(vop_bypass_t *)VOP_CLOSE_AP,
	vop_close_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_close_args,a_cred),
	VOPARG_OFFSETOF(struct vop_close_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_access_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_access_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_ACCESS_AP(struct vop_access_args *a)
{

	return(VOP_ACCESS_APV(a->a_vp->v_op, a));
}

int
VOP_ACCESS_APV(struct vop_vector *vop, struct vop_access_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_access_desc, a->a_vp,
	    ("Wrong a_desc in vop_access(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_access == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_access(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS");
	if (vop->vop_access != NULL)
		rc = vop->vop_access(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_ACCESS(vp 0x%lX, accmode %ld, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_accmode, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACCESS");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_ACCESS");
	}
	return (rc);
}

struct vnodeop_desc vop_access_desc = {
	"vop_access",
	0,
	(vop_bypass_t *)VOP_ACCESS_AP,
	vop_access_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_access_args,a_cred),
	VOPARG_OFFSETOF(struct vop_access_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_getattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_GETATTR_AP(struct vop_getattr_args *a)
{

	return(VOP_GETATTR_APV(a->a_vp->v_op, a));
}

int
VOP_GETATTR_APV(struct vop_vector *vop, struct vop_getattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_getattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_getattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR");
	if (vop->vop_getattr != NULL)
		rc = vop->vop_getattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_GETATTR(vp 0x%lX, vap 0x%lX, cred 0x%lX)",
	    a->a_vp, a->a_vap, a->a_cred);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_getattr_desc = {
	"vop_getattr",
	0,
	(vop_bypass_t *)VOP_GETATTR_AP,
	vop_getattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_getattr_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_setattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_SETATTR_AP(struct vop_setattr_args *a)
{

	return(VOP_SETATTR_APV(a->a_vp->v_op, a));
}

int
VOP_SETATTR_APV(struct vop_vector *vop, struct vop_setattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_setattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_setattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR");
	if (vop->vop_setattr != NULL)
		rc = vop->vop_setattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_SETATTR(vp 0x%lX, vap 0x%lX, cred 0x%lX)",
	    a->a_vp, a->a_vap, a->a_cred);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETATTR");
	}
	vop_setattr_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_setattr_desc = {
	"vop_setattr",
	0,
	(vop_bypass_t *)VOP_SETATTR_AP,
	vop_setattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setattr_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_markatime_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_markatime_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_MARKATIME_AP(struct vop_markatime_args *a)
{

	return(VOP_MARKATIME_APV(a->a_vp->v_op, a));
}

int
VOP_MARKATIME_APV(struct vop_vector *vop, struct vop_markatime_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_markatime_desc, a->a_vp,
	    ("Wrong a_desc in vop_markatime(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_markatime == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_markatime(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MARKATIME");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_MARKATIME");
	if (vop->vop_markatime != NULL)
		rc = vop->vop_markatime(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR1(KTR_VOP,
	    "VOP_MARKATIME(vp 0x%lX)",
	    a->a_vp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MARKATIME");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_MARKATIME");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_MARKATIME");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_MARKATIME");
	}
	return (rc);
}

struct vnodeop_desc vop_markatime_desc = {
	"vop_markatime",
	0,
	(vop_bypass_t *)VOP_MARKATIME_AP,
	vop_markatime_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_read_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_read_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_READ_AP(struct vop_read_args *a)
{

	return(VOP_READ_APV(a->a_vp->v_op, a));
}

int
VOP_READ_APV(struct vop_vector *vop, struct vop_read_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_read_desc, a->a_vp,
	    ("Wrong a_desc in vop_read(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_read == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_read(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ");
	if (vop->vop_read != NULL)
		rc = vop->vop_read(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_READ(vp 0x%lX, uio 0x%lX, ioflag %ld, cred 0x%lX)",
	    a->a_vp, a->a_uio, a->a_ioflag, a->a_cred);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READ");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READ");
	}
	return (rc);
}

struct vnodeop_desc vop_read_desc = {
	"vop_read",
	0,
	(vop_bypass_t *)VOP_READ_AP,
	vop_read_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_read_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_write_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_write_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_WRITE_AP(struct vop_write_args *a)
{

	return(VOP_WRITE_APV(a->a_vp->v_op, a));
}

int
VOP_WRITE_APV(struct vop_vector *vop, struct vop_write_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_write_desc, a->a_vp,
	    ("Wrong a_desc in vop_write(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_write == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_write(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_WRITE");
	VOP_WRITE_PRE(a);
	if (vop->vop_write != NULL)
		rc = vop->vop_write(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_WRITE(vp 0x%lX, uio 0x%lX, ioflag %ld, cred 0x%lX)",
	    a->a_vp, a->a_uio, a->a_ioflag, a->a_cred);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_WRITE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_WRITE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_WRITE");
	}
	VOP_WRITE_POST(a, rc);
	return (rc);
}

struct vnodeop_desc vop_write_desc = {
	"vop_write",
	0,
	(vop_bypass_t *)VOP_WRITE_AP,
	vop_write_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_write_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_lease_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_lease_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_LEASE_AP(struct vop_lease_args *a)
{

	return(VOP_LEASE_APV(a->a_vp->v_op, a));
}

int
VOP_LEASE_APV(struct vop_vector *vop, struct vop_lease_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_lease_desc, a->a_vp,
	    ("Wrong a_desc in vop_lease(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_lease == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_lease(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LEASE");
	if (vop->vop_lease != NULL)
		rc = vop->vop_lease(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_LEASE(vp 0x%lX, td 0x%lX, cred 0x%lX, flag %ld)",
	    a->a_vp, a->a_td, a->a_cred, a->a_flag);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LEASE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LEASE");
	}
	return (rc);
}

struct vnodeop_desc vop_lease_desc = {
	"vop_lease",
	0,
	(vop_bypass_t *)VOP_LEASE_AP,
	vop_lease_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_lease_args,a_cred),
	VOPARG_OFFSETOF(struct vop_lease_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_ioctl_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_ioctl_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_IOCTL_AP(struct vop_ioctl_args *a)
{

	return(VOP_IOCTL_APV(a->a_vp->v_op, a));
}

int
VOP_IOCTL_APV(struct vop_vector *vop, struct vop_ioctl_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_ioctl_desc, a->a_vp,
	    ("Wrong a_desc in vop_ioctl(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_ioctl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_ioctl(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL");
	if (vop->vop_ioctl != NULL)
		rc = vop->vop_ioctl(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_IOCTL(vp 0x%lX, command %ld, data 0x%lX, fflag %ld, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_command, a->a_data, a->a_fflag, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_IOCTL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_IOCTL");
	}
	return (rc);
}

struct vnodeop_desc vop_ioctl_desc = {
	"vop_ioctl",
	0,
	(vop_bypass_t *)VOP_IOCTL_AP,
	vop_ioctl_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_ioctl_args,a_cred),
	VOPARG_OFFSETOF(struct vop_ioctl_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_poll_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_poll_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_POLL_AP(struct vop_poll_args *a)
{

	return(VOP_POLL_APV(a->a_vp->v_op, a));
}

int
VOP_POLL_APV(struct vop_vector *vop, struct vop_poll_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_poll_desc, a->a_vp,
	    ("Wrong a_desc in vop_poll(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_poll == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_poll(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL");
	if (vop->vop_poll != NULL)
		rc = vop->vop_poll(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_POLL(vp 0x%lX, events %ld, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_events, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_POLL");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_POLL");
	}
	return (rc);
}

struct vnodeop_desc vop_poll_desc = {
	"vop_poll",
	0,
	(vop_bypass_t *)VOP_POLL_AP,
	vop_poll_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_poll_args,a_cred),
	VOPARG_OFFSETOF(struct vop_poll_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_kqfilter_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_kqfilter_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_KQFILTER_AP(struct vop_kqfilter_args *a)
{

	return(VOP_KQFILTER_APV(a->a_vp->v_op, a));
}

int
VOP_KQFILTER_APV(struct vop_vector *vop, struct vop_kqfilter_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_kqfilter_desc, a->a_vp,
	    ("Wrong a_desc in vop_kqfilter(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_kqfilter == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_kqfilter(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	if (vop->vop_kqfilter != NULL)
		rc = vop->vop_kqfilter(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_KQFILTER(vp 0x%lX, kn 0x%lX)",
	    a->a_vp, a->a_kn);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_KQFILTER");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_KQFILTER");
	}
	return (rc);
}

struct vnodeop_desc vop_kqfilter_desc = {
	"vop_kqfilter",
	0,
	(vop_bypass_t *)VOP_KQFILTER_AP,
	vop_kqfilter_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_revoke_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_revoke_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_REVOKE_AP(struct vop_revoke_args *a)
{

	return(VOP_REVOKE_APV(a->a_vp->v_op, a));
}

int
VOP_REVOKE_APV(struct vop_vector *vop, struct vop_revoke_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_revoke_desc, a->a_vp,
	    ("Wrong a_desc in vop_revoke(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_revoke == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_revoke(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE");
	if (vop->vop_revoke != NULL)
		rc = vop->vop_revoke(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_REVOKE(vp 0x%lX, flags %ld)",
	    a->a_vp, a->a_flags);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REVOKE");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_REVOKE");
	}
	return (rc);
}

struct vnodeop_desc vop_revoke_desc = {
	"vop_revoke",
	0,
	(vop_bypass_t *)VOP_REVOKE_AP,
	vop_revoke_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_fsync_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_fsync_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_FSYNC_AP(struct vop_fsync_args *a)
{

	return(VOP_FSYNC_APV(a->a_vp->v_op, a));
}

int
VOP_FSYNC_APV(struct vop_vector *vop, struct vop_fsync_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_fsync_desc, a->a_vp,
	    ("Wrong a_desc in vop_fsync(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_fsync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_fsync(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FSYNC");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_FSYNC");
	if (vop->vop_fsync != NULL)
		rc = vop->vop_fsync(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_FSYNC(vp 0x%lX, waitfor %ld, td 0x%lX)",
	    a->a_vp, a->a_waitfor, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FSYNC");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_FSYNC");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_FSYNC");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_FSYNC");
	}
	return (rc);
}

struct vnodeop_desc vop_fsync_desc = {
	"vop_fsync",
	0,
	(vop_bypass_t *)VOP_FSYNC_AP,
	vop_fsync_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_fsync_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_remove_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_remove_args,a_dvp),
	VOPARG_OFFSETOF(struct vop_remove_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_REMOVE_AP(struct vop_remove_args *a)
{

	return(VOP_REMOVE_APV(a->a_dvp->v_op, a));
}

int
VOP_REMOVE_APV(struct vop_vector *vop, struct vop_remove_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_remove_desc, a->a_dvp,
	    ("Wrong a_desc in vop_remove(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_remove == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_remove(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE");
	if (vop->vop_remove != NULL)
		rc = vop->vop_remove(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_REMOVE(dvp 0x%lX, vp 0x%lX, cnp 0x%lX)",
	    a->a_dvp, a->a_vp, a->a_cnp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_REMOVE");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REMOVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REMOVE");
	}
	vop_remove_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_remove_desc = {
	"vop_remove",
	0,
	(vop_bypass_t *)VOP_REMOVE_AP,
	vop_remove_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_remove_args,a_cnp),
};

static int vop_link_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_link_args,a_tdvp),
	VOPARG_OFFSETOF(struct vop_link_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_LINK_AP(struct vop_link_args *a)
{

	return(VOP_LINK_APV(a->a_tdvp->v_op, a));
}

int
VOP_LINK_APV(struct vop_vector *vop, struct vop_link_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_link_desc, a->a_tdvp,
	    ("Wrong a_desc in vop_link(%p, %p)", a->a_tdvp, a));
	while(vop != NULL && \
	    vop->vop_link == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_tdvp, ("No vop_link(%p, %p)", a->a_tdvp, a));
	ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK");
	ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK");
	if (vop->vop_link != NULL)
		rc = vop->vop_link(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_LINK(tdvp 0x%lX, vp 0x%lX, cnp 0x%lX)",
	    a->a_tdvp, a->a_vp, a->a_cnp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_tdvp, "VOP_LINK");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LINK");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_LINK");
	}
	vop_link_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_link_desc = {
	"vop_link",
	0,
	(vop_bypass_t *)VOP_LINK_AP,
	vop_link_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_link_args,a_cnp),
};

static int vop_rename_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_rename_args,a_fdvp),
	VOPARG_OFFSETOF(struct vop_rename_args,a_fvp),
	VOPARG_OFFSETOF(struct vop_rename_args,a_tdvp),
	VOPARG_OFFSETOF(struct vop_rename_args,a_tvp),
	VDESC_NO_OFFSET
};

int
VOP_RENAME_AP(struct vop_rename_args *a)
{

	return(VOP_RENAME_APV(a->a_fdvp->v_op, a));
}

int
VOP_RENAME_APV(struct vop_vector *vop, struct vop_rename_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_rename_desc, a->a_fdvp,
	    ("Wrong a_desc in vop_rename(%p, %p)", a->a_fdvp, a));
	while(vop != NULL && \
	    vop->vop_rename == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_fdvp, ("No vop_rename(%p, %p)", a->a_fdvp, a));
	vop_rename_pre(a);
	if (vop->vop_rename != NULL)
		rc = vop->vop_rename(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_RENAME(fdvp 0x%lX, fvp 0x%lX, fcnp 0x%lX, tdvp 0x%lX, tvp 0x%lX, tcnp 0x%lX)",
	    a->a_fdvp, a->a_fvp, a->a_fcnp, a->a_tdvp, a->a_tvp, a->a_tcnp);
	if (rc == 0) {
	} else {
	}
	vop_rename_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_rename_desc = {
	"vop_rename",
	VDESC_VP0_WILLRELE|VDESC_VP1_WILLRELE|VDESC_VP2_WILLRELE|VDESC_VP3_WILLRELE,
	(vop_bypass_t *)VOP_RENAME_AP,
	vop_rename_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_rename_args,a_fcnp),
};

static int vop_mkdir_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_mkdir_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_MKDIR_AP(struct vop_mkdir_args *a)
{

	return(VOP_MKDIR_APV(a->a_dvp->v_op, a));
}

int
VOP_MKDIR_APV(struct vop_vector *vop, struct vop_mkdir_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_mkdir_desc, a->a_dvp,
	    ("Wrong a_desc in vop_mkdir(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_mkdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_mkdir(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR");
	if (vop->vop_mkdir != NULL)
		rc = vop->vop_mkdir(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_MKDIR(dvp 0x%lX, vpp 0x%lX, cnp 0x%lX, vap 0x%lX)",
	    a->a_dvp, a->a_vpp, a->a_cnp, a->a_vap);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_MKDIR");
		ASSERT_VOP_ELOCKED(*a->a_vpp, "VOP_MKDIR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_MKDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_MKDIR");
	}
	vop_mkdir_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_mkdir_desc = {
	"vop_mkdir",
	0,
	(vop_bypass_t *)VOP_MKDIR_AP,
	vop_mkdir_vp_offsets,
	VOPARG_OFFSETOF(struct vop_mkdir_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_mkdir_args,a_cnp),
};

static int vop_rmdir_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_rmdir_args,a_dvp),
	VOPARG_OFFSETOF(struct vop_rmdir_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_RMDIR_AP(struct vop_rmdir_args *a)
{

	return(VOP_RMDIR_APV(a->a_dvp->v_op, a));
}

int
VOP_RMDIR_APV(struct vop_vector *vop, struct vop_rmdir_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_rmdir_desc, a->a_dvp,
	    ("Wrong a_desc in vop_rmdir(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_rmdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_rmdir(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR");
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR");
	if (vop->vop_rmdir != NULL)
		rc = vop->vop_rmdir(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_RMDIR(dvp 0x%lX, vp 0x%lX, cnp 0x%lX)",
	    a->a_dvp, a->a_vp, a->a_cnp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_RMDIR");
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RMDIR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RMDIR");
	}
	vop_rmdir_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_rmdir_desc = {
	"vop_rmdir",
	0,
	(vop_bypass_t *)VOP_RMDIR_AP,
	vop_rmdir_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_rmdir_args,a_cnp),
};

static int vop_symlink_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_symlink_args,a_dvp),
	VDESC_NO_OFFSET
};

int
VOP_SYMLINK_AP(struct vop_symlink_args *a)
{

	return(VOP_SYMLINK_APV(a->a_dvp->v_op, a));
}

int
VOP_SYMLINK_APV(struct vop_vector *vop, struct vop_symlink_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_symlink_desc, a->a_dvp,
	    ("Wrong a_desc in vop_symlink(%p, %p)", a->a_dvp, a));
	while(vop != NULL && \
	    vop->vop_symlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_dvp, ("No vop_symlink(%p, %p)", a->a_dvp, a));
	ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK");
	ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK");
	if (vop->vop_symlink != NULL)
		rc = vop->vop_symlink(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_SYMLINK(dvp 0x%lX, vpp 0x%lX, cnp 0x%lX, vap 0x%lX, target 0x%lX)",
	    a->a_dvp, a->a_vpp, a->a_cnp, a->a_vap, a->a_target);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_SYMLINK");
		ASSERT_VOP_ELOCKED(*a->a_vpp, "VOP_SYMLINK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_dvp, "VOP_SYMLINK");
		ASSERT_VOP_ELOCKED(a->a_dvp, "VOP_SYMLINK");
	}
	vop_symlink_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_symlink_desc = {
	"vop_symlink",
	0,
	(vop_bypass_t *)VOP_SYMLINK_AP,
	vop_symlink_vp_offsets,
	VOPARG_OFFSETOF(struct vop_symlink_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_symlink_args,a_cnp),
};

static int vop_readdir_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_readdir_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_READDIR_AP(struct vop_readdir_args *a)
{

	return(VOP_READDIR_APV(a->a_vp->v_op, a));
}

int
VOP_READDIR_APV(struct vop_vector *vop, struct vop_readdir_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_readdir_desc, a->a_vp,
	    ("Wrong a_desc in vop_readdir(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_readdir == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_readdir(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR");
	if (vop->vop_readdir != NULL)
		rc = vop->vop_readdir(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_READDIR(vp 0x%lX, uio 0x%lX, cred 0x%lX, eofflag 0x%lX, ncookies 0x%lX, cookies 0x%lX)",
	    a->a_vp, a->a_uio, a->a_cred, a->a_eofflag, a->a_ncookies, a->a_cookies);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READDIR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READDIR");
	}
	return (rc);
}

struct vnodeop_desc vop_readdir_desc = {
	"vop_readdir",
	0,
	(vop_bypass_t *)VOP_READDIR_AP,
	vop_readdir_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_readdir_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_readlink_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_readlink_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_READLINK_AP(struct vop_readlink_args *a)
{

	return(VOP_READLINK_APV(a->a_vp->v_op, a));
}

int
VOP_READLINK_APV(struct vop_vector *vop, struct vop_readlink_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_readlink_desc, a->a_vp,
	    ("Wrong a_desc in vop_readlink(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_readlink == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_readlink(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK");
	if (vop->vop_readlink != NULL)
		rc = vop->vop_readlink(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_READLINK(vp 0x%lX, uio 0x%lX, cred 0x%lX)",
	    a->a_vp, a->a_uio, a->a_cred);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_READLINK");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_READLINK");
	}
	return (rc);
}

struct vnodeop_desc vop_readlink_desc = {
	"vop_readlink",
	0,
	(vop_bypass_t *)VOP_READLINK_AP,
	vop_readlink_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_readlink_args,a_cred),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_inactive_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_inactive_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_INACTIVE_AP(struct vop_inactive_args *a)
{

	return(VOP_INACTIVE_APV(a->a_vp->v_op, a));
}

int
VOP_INACTIVE_APV(struct vop_vector *vop, struct vop_inactive_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_inactive_desc, a->a_vp,
	    ("Wrong a_desc in vop_inactive(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_inactive == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_inactive(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE");
	if (vop->vop_inactive != NULL)
		rc = vop->vop_inactive(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_INACTIVE(vp 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_INACTIVE");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_INACTIVE");
	}
	return (rc);
}

struct vnodeop_desc vop_inactive_desc = {
	"vop_inactive",
	0,
	(vop_bypass_t *)VOP_INACTIVE_AP,
	vop_inactive_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_inactive_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_reclaim_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_reclaim_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_RECLAIM_AP(struct vop_reclaim_args *a)
{

	return(VOP_RECLAIM_APV(a->a_vp->v_op, a));
}

int
VOP_RECLAIM_APV(struct vop_vector *vop, struct vop_reclaim_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_reclaim_desc, a->a_vp,
	    ("Wrong a_desc in vop_reclaim(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_reclaim == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_reclaim(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM");
	if (vop->vop_reclaim != NULL)
		rc = vop->vop_reclaim(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_RECLAIM(vp 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_RECLAIM");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_RECLAIM");
	}
	return (rc);
}

struct vnodeop_desc vop_reclaim_desc = {
	"vop_reclaim",
	0,
	(vop_bypass_t *)VOP_RECLAIM_AP,
	vop_reclaim_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_reclaim_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_lock1_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_lock1_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_LOCK1_AP(struct vop_lock1_args *a)
{

	return(VOP_LOCK1_APV(a->a_vp->v_op, a));
}

int
VOP_LOCK1_APV(struct vop_vector *vop, struct vop_lock1_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_lock1_desc, a->a_vp,
	    ("Wrong a_desc in vop_lock1(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_lock1 == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_lock1(%p, %p)", a->a_vp, a));
	vop_lock_pre(a);
	if (vop->vop_lock1 != NULL)
		rc = vop->vop_lock1(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_LOCK1(vp 0x%lX, flags %ld, file 0x%lX, line %ld)",
	    a->a_vp, a->a_flags, a->a_file, a->a_line);
	if (rc == 0) {
	} else {
	}
	vop_lock_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_lock1_desc = {
	"vop_lock1",
	0,
	(vop_bypass_t *)VOP_LOCK1_AP,
	vop_lock1_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_unlock_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_unlock_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_UNLOCK_AP(struct vop_unlock_args *a)
{

	return(VOP_UNLOCK_APV(a->a_vp->v_op, a));
}

int
VOP_UNLOCK_APV(struct vop_vector *vop, struct vop_unlock_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_unlock_desc, a->a_vp,
	    ("Wrong a_desc in vop_unlock(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_unlock == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_unlock(%p, %p)", a->a_vp, a));
	vop_unlock_pre(a);
	if (vop->vop_unlock != NULL)
		rc = vop->vop_unlock(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_UNLOCK(vp 0x%lX, flags %ld)",
	    a->a_vp, a->a_flags);
	if (rc == 0) {
	} else {
	}
	vop_unlock_post(a, rc);
	return (rc);
}

struct vnodeop_desc vop_unlock_desc = {
	"vop_unlock",
	0,
	(vop_bypass_t *)VOP_UNLOCK_AP,
	vop_unlock_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_bmap_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_bmap_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_BMAP_AP(struct vop_bmap_args *a)
{

	return(VOP_BMAP_APV(a->a_vp->v_op, a));
}

int
VOP_BMAP_APV(struct vop_vector *vop, struct vop_bmap_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_bmap_desc, a->a_vp,
	    ("Wrong a_desc in vop_bmap(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_bmap == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_bmap(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP");
	if (vop->vop_bmap != NULL)
		rc = vop->vop_bmap(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_BMAP(vp 0x%lX, bn %ld, bop 0x%lX, bnp 0x%lX, runp 0x%lX, runb 0x%lX)",
	    a->a_vp, a->a_bn, a->a_bop, a->a_bnp, a->a_runp, a->a_runb);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_BMAP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_BMAP");
	}
	return (rc);
}

struct vnodeop_desc vop_bmap_desc = {
	"vop_bmap",
	0,
	(vop_bypass_t *)VOP_BMAP_AP,
	vop_bmap_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_strategy_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_strategy_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_STRATEGY_AP(struct vop_strategy_args *a)
{

	return(VOP_STRATEGY_APV(a->a_vp->v_op, a));
}

int
VOP_STRATEGY_APV(struct vop_vector *vop, struct vop_strategy_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_strategy_desc, a->a_vp,
	    ("Wrong a_desc in vop_strategy(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_strategy == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_strategy(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY");
	vop_strategy_pre(a);
	if (vop->vop_strategy != NULL)
		rc = vop->vop_strategy(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_STRATEGY(vp 0x%lX, bp 0x%lX)",
	    a->a_vp, a->a_bp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_STRATEGY");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_STRATEGY");
	}
	return (rc);
}

struct vnodeop_desc vop_strategy_desc = {
	"vop_strategy",
	0,
	(vop_bypass_t *)VOP_STRATEGY_AP,
	vop_strategy_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getwritemount_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getwritemount_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_GETWRITEMOUNT_AP(struct vop_getwritemount_args *a)
{

	return(VOP_GETWRITEMOUNT_APV(a->a_vp->v_op, a));
}

int
VOP_GETWRITEMOUNT_APV(struct vop_vector *vop, struct vop_getwritemount_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getwritemount_desc, a->a_vp,
	    ("Wrong a_desc in vop_getwritemount(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_getwritemount == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getwritemount(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT");
	if (vop->vop_getwritemount != NULL)
		rc = vop->vop_getwritemount(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_GETWRITEMOUNT(vp 0x%lX, mpp 0x%lX)",
	    a->a_vp, a->a_mpp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETWRITEMOUNT");
	}
	return (rc);
}

struct vnodeop_desc vop_getwritemount_desc = {
	"vop_getwritemount",
	0,
	(vop_bypass_t *)VOP_GETWRITEMOUNT_AP,
	vop_getwritemount_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_print_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_print_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_PRINT_AP(struct vop_print_args *a)
{

	return(VOP_PRINT_APV(a->a_vp->v_op, a));
}

int
VOP_PRINT_APV(struct vop_vector *vop, struct vop_print_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_print_desc, a->a_vp,
	    ("Wrong a_desc in vop_print(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_print == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_print(%p, %p)", a->a_vp, a));
	if (vop->vop_print != NULL)
		rc = vop->vop_print(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR1(KTR_VOP,
	    "VOP_PRINT(vp 0x%lX)",
	    a->a_vp);
	if (rc == 0) {
	} else {
	}
	return (rc);
}

struct vnodeop_desc vop_print_desc = {
	"vop_print",
	0,
	(vop_bypass_t *)VOP_PRINT_AP,
	vop_print_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_pathconf_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_pathconf_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_PATHCONF_AP(struct vop_pathconf_args *a)
{

	return(VOP_PATHCONF_APV(a->a_vp->v_op, a));
}

int
VOP_PATHCONF_APV(struct vop_vector *vop, struct vop_pathconf_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_pathconf_desc, a->a_vp,
	    ("Wrong a_desc in vop_pathconf(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_pathconf == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_pathconf(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF");
	if (vop->vop_pathconf != NULL)
		rc = vop->vop_pathconf(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_PATHCONF(vp 0x%lX, name %ld, retval 0x%lX)",
	    a->a_vp, a->a_name, a->a_retval);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PATHCONF");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_PATHCONF");
	}
	return (rc);
}

struct vnodeop_desc vop_pathconf_desc = {
	"vop_pathconf",
	0,
	(vop_bypass_t *)VOP_PATHCONF_AP,
	vop_pathconf_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_advlock_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_advlock_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_ADVLOCK_AP(struct vop_advlock_args *a)
{

	return(VOP_ADVLOCK_APV(a->a_vp->v_op, a));
}

int
VOP_ADVLOCK_APV(struct vop_vector *vop, struct vop_advlock_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_advlock_desc, a->a_vp,
	    ("Wrong a_desc in vop_advlock(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_advlock == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlock(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	if (vop->vop_advlock != NULL)
		rc = vop->vop_advlock(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_ADVLOCK(vp 0x%lX, id 0x%lX, op %ld, fl 0x%lX, flags %ld)",
	    a->a_vp, a->a_id, a->a_op, a->a_fl, a->a_flags);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCK");
	}
	return (rc);
}

struct vnodeop_desc vop_advlock_desc = {
	"vop_advlock",
	0,
	(vop_bypass_t *)VOP_ADVLOCK_AP,
	vop_advlock_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_advlockasync_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_advlockasync_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_ADVLOCKASYNC_AP(struct vop_advlockasync_args *a)
{

	return(VOP_ADVLOCKASYNC_APV(a->a_vp->v_op, a));
}

int
VOP_ADVLOCKASYNC_APV(struct vop_vector *vop, struct vop_advlockasync_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_advlockasync_desc, a->a_vp,
	    ("Wrong a_desc in vop_advlockasync(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_advlockasync == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_advlockasync(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	if (vop->vop_advlockasync != NULL)
		rc = vop->vop_advlockasync(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_ADVLOCKASYNC(vp 0x%lX, id 0x%lX, op %ld, fl 0x%lX, flags %ld, task 0x%lX, cookiep 0x%lX)",
	    a->a_vp, a->a_id, a->a_op, a->a_fl, a->a_flags, a->a_task);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
		ASSERT_VOP_UNLOCKED(a->a_vp, "VOP_ADVLOCKASYNC");
	}
	return (rc);
}

struct vnodeop_desc vop_advlockasync_desc = {
	"vop_advlockasync",
	0,
	(vop_bypass_t *)VOP_ADVLOCKASYNC_AP,
	vop_advlockasync_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_reallocblks_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_reallocblks_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_REALLOCBLKS_AP(struct vop_reallocblks_args *a)
{

	return(VOP_REALLOCBLKS_APV(a->a_vp->v_op, a));
}

int
VOP_REALLOCBLKS_APV(struct vop_vector *vop, struct vop_reallocblks_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_reallocblks_desc, a->a_vp,
	    ("Wrong a_desc in vop_reallocblks(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_reallocblks == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_reallocblks(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS");
	if (vop->vop_reallocblks != NULL)
		rc = vop->vop_reallocblks(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_REALLOCBLKS(vp 0x%lX, buflist 0x%lX)",
	    a->a_vp, a->a_buflist);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_REALLOCBLKS");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_REALLOCBLKS");
	}
	return (rc);
}

struct vnodeop_desc vop_reallocblks_desc = {
	"vop_reallocblks",
	0,
	(vop_bypass_t *)VOP_REALLOCBLKS_AP,
	vop_reallocblks_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getpages_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getpages_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_GETPAGES_AP(struct vop_getpages_args *a)
{

	return(VOP_GETPAGES_APV(a->a_vp->v_op, a));
}

int
VOP_GETPAGES_APV(struct vop_vector *vop, struct vop_getpages_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getpages_desc, a->a_vp,
	    ("Wrong a_desc in vop_getpages(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_getpages == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getpages(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES");
	if (vop->vop_getpages != NULL)
		rc = vop->vop_getpages(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_GETPAGES(vp 0x%lX, m 0x%lX, count %ld, reqpage %ld, offset %ld)",
	    a->a_vp, a->a_m, a->a_count, a->a_reqpage, a->a_offset);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETPAGES");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETPAGES");
	}
	return (rc);
}

struct vnodeop_desc vop_getpages_desc = {
	"vop_getpages",
	0,
	(vop_bypass_t *)VOP_GETPAGES_AP,
	vop_getpages_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_putpages_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_putpages_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_PUTPAGES_AP(struct vop_putpages_args *a)
{

	return(VOP_PUTPAGES_APV(a->a_vp->v_op, a));
}

int
VOP_PUTPAGES_APV(struct vop_vector *vop, struct vop_putpages_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_putpages_desc, a->a_vp,
	    ("Wrong a_desc in vop_putpages(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_putpages == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_putpages(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_PUTPAGES");
	if (vop->vop_putpages != NULL)
		rc = vop->vop_putpages(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_PUTPAGES(vp 0x%lX, m 0x%lX, count %ld, sync %ld, rtvals 0x%lX, offset %ld)",
	    a->a_vp, a->a_m, a->a_count, a->a_sync, a->a_rtvals, a->a_offset);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_PUTPAGES");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_PUTPAGES");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_PUTPAGES");
	}
	return (rc);
}

struct vnodeop_desc vop_putpages_desc = {
	"vop_putpages",
	0,
	(vop_bypass_t *)VOP_PUTPAGES_AP,
	vop_putpages_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_getacl_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getacl_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_GETACL_AP(struct vop_getacl_args *a)
{

	return(VOP_GETACL_APV(a->a_vp->v_op, a));
}

int
VOP_GETACL_APV(struct vop_vector *vop, struct vop_getacl_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getacl_desc, a->a_vp,
	    ("Wrong a_desc in vop_getacl(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_getacl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getacl(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL");
	if (vop->vop_getacl != NULL)
		rc = vop->vop_getacl(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_GETACL(vp 0x%lX, type %ld, aclp 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_type, a->a_aclp, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETACL");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETACL");
	}
	return (rc);
}

struct vnodeop_desc vop_getacl_desc = {
	"vop_getacl",
	0,
	(vop_bypass_t *)VOP_GETACL_AP,
	vop_getacl_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_getacl_args,a_cred),
	VOPARG_OFFSETOF(struct vop_getacl_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_setacl_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setacl_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_SETACL_AP(struct vop_setacl_args *a)
{

	return(VOP_SETACL_APV(a->a_vp->v_op, a));
}

int
VOP_SETACL_APV(struct vop_vector *vop, struct vop_setacl_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setacl_desc, a->a_vp,
	    ("Wrong a_desc in vop_setacl(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_setacl == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setacl(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL");
	if (vop->vop_setacl != NULL)
		rc = vop->vop_setacl(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_SETACL(vp 0x%lX, type %ld, aclp 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_type, a->a_aclp, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETACL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETACL");
	}
	return (rc);
}

struct vnodeop_desc vop_setacl_desc = {
	"vop_setacl",
	0,
	(vop_bypass_t *)VOP_SETACL_AP,
	vop_setacl_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setacl_args,a_cred),
	VOPARG_OFFSETOF(struct vop_setacl_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_aclcheck_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_aclcheck_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_ACLCHECK_AP(struct vop_aclcheck_args *a)
{

	return(VOP_ACLCHECK_APV(a->a_vp->v_op, a));
}

int
VOP_ACLCHECK_APV(struct vop_vector *vop, struct vop_aclcheck_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_aclcheck_desc, a->a_vp,
	    ("Wrong a_desc in vop_aclcheck(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_aclcheck == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_aclcheck(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK");
	if (vop->vop_aclcheck != NULL)
		rc = vop->vop_aclcheck(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_ACLCHECK(vp 0x%lX, type %ld, aclp 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_type, a->a_aclp, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_ACLCHECK");
	}
	return (rc);
}

struct vnodeop_desc vop_aclcheck_desc = {
	"vop_aclcheck",
	0,
	(vop_bypass_t *)VOP_ACLCHECK_AP,
	vop_aclcheck_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_aclcheck_args,a_cred),
	VOPARG_OFFSETOF(struct vop_aclcheck_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_closeextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_closeextattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_CLOSEEXTATTR_AP(struct vop_closeextattr_args *a)
{

	return(VOP_CLOSEEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_CLOSEEXTATTR_APV(struct vop_vector *vop, struct vop_closeextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_closeextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_closeextattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_closeextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_closeextattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	if (vop->vop_closeextattr != NULL)
		rc = vop->vop_closeextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_CLOSEEXTATTR(vp 0x%lX, commit %ld, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_commit, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_CLOSEEXTATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_closeextattr_desc = {
	"vop_closeextattr",
	0,
	(vop_bypass_t *)VOP_CLOSEEXTATTR_AP,
	vop_closeextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_closeextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_closeextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_getextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_getextattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_GETEXTATTR_AP(struct vop_getextattr_args *a)
{

	return(VOP_GETEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_GETEXTATTR_APV(struct vop_vector *vop, struct vop_getextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_getextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_getextattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_getextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_getextattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR");
	if (vop->vop_getextattr != NULL)
		rc = vop->vop_getextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_GETEXTATTR(vp 0x%lX, attrnamespace %ld, name 0x%lX, uio 0x%lX, size 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_attrnamespace, a->a_name, a->a_uio, a->a_size, a->a_cred);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_GETEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_GETEXTATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_getextattr_desc = {
	"vop_getextattr",
	0,
	(vop_bypass_t *)VOP_GETEXTATTR_AP,
	vop_getextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_getextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_getextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_listextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_listextattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_LISTEXTATTR_AP(struct vop_listextattr_args *a)
{

	return(VOP_LISTEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_LISTEXTATTR_APV(struct vop_vector *vop, struct vop_listextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_listextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_listextattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_listextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_listextattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR");
	if (vop->vop_listextattr != NULL)
		rc = vop->vop_listextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_LISTEXTATTR(vp 0x%lX, attrnamespace %ld, uio 0x%lX, size 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_attrnamespace, a->a_uio, a->a_size, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_LISTEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_LISTEXTATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_listextattr_desc = {
	"vop_listextattr",
	0,
	(vop_bypass_t *)VOP_LISTEXTATTR_AP,
	vop_listextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_listextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_listextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_openextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_openextattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_OPENEXTATTR_AP(struct vop_openextattr_args *a)
{

	return(VOP_OPENEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_OPENEXTATTR_APV(struct vop_vector *vop, struct vop_openextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_openextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_openextattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_openextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_openextattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR");
	if (vop->vop_openextattr != NULL)
		rc = vop->vop_openextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR3(KTR_VOP,
	    "VOP_OPENEXTATTR(vp 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_OPENEXTATTR");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_OPENEXTATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_openextattr_desc = {
	"vop_openextattr",
	0,
	(vop_bypass_t *)VOP_OPENEXTATTR_AP,
	vop_openextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_openextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_openextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_deleteextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_deleteextattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_DELETEEXTATTR_AP(struct vop_deleteextattr_args *a)
{

	return(VOP_DELETEEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_DELETEEXTATTR_APV(struct vop_vector *vop, struct vop_deleteextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_deleteextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_deleteextattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_deleteextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_deleteextattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	if (vop->vop_deleteextattr != NULL)
		rc = vop->vop_deleteextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR5(KTR_VOP,
	    "VOP_DELETEEXTATTR(vp 0x%lX, attrnamespace %ld, name 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_attrnamespace, a->a_name, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_DELETEEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_DELETEEXTATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_deleteextattr_desc = {
	"vop_deleteextattr",
	0,
	(vop_bypass_t *)VOP_DELETEEXTATTR_AP,
	vop_deleteextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_deleteextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_deleteextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_setextattr_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setextattr_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_SETEXTATTR_AP(struct vop_setextattr_args *a)
{

	return(VOP_SETEXTATTR_APV(a->a_vp->v_op, a));
}

int
VOP_SETEXTATTR_APV(struct vop_vector *vop, struct vop_setextattr_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setextattr_desc, a->a_vp,
	    ("Wrong a_desc in vop_setextattr(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_setextattr == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setextattr(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR");
	if (vop->vop_setextattr != NULL)
		rc = vop->vop_setextattr(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR6(KTR_VOP,
	    "VOP_SETEXTATTR(vp 0x%lX, attrnamespace %ld, name 0x%lX, uio 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_attrnamespace, a->a_name, a->a_uio, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETEXTATTR");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETEXTATTR");
	}
	return (rc);
}

struct vnodeop_desc vop_setextattr_desc = {
	"vop_setextattr",
	0,
	(vop_bypass_t *)VOP_SETEXTATTR_AP,
	vop_setextattr_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setextattr_args,a_cred),
	VOPARG_OFFSETOF(struct vop_setextattr_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_setlabel_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_setlabel_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_SETLABEL_AP(struct vop_setlabel_args *a)
{

	return(VOP_SETLABEL_APV(a->a_vp->v_op, a));
}

int
VOP_SETLABEL_APV(struct vop_vector *vop, struct vop_setlabel_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_setlabel_desc, a->a_vp,
	    ("Wrong a_desc in vop_setlabel(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_setlabel == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_setlabel(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL");
	ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL");
	if (vop->vop_setlabel != NULL)
		rc = vop->vop_setlabel(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_SETLABEL(vp 0x%lX, label 0x%lX, cred 0x%lX, td 0x%lX)",
	    a->a_vp, a->a_label, a->a_cred, a->a_td);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_SETLABEL");
		ASSERT_VOP_ELOCKED(a->a_vp, "VOP_SETLABEL");
	}
	return (rc);
}

struct vnodeop_desc vop_setlabel_desc = {
	"vop_setlabel",
	0,
	(vop_bypass_t *)VOP_SETLABEL_AP,
	vop_setlabel_vp_offsets,
	VDESC_NO_OFFSET,
	VOPARG_OFFSETOF(struct vop_setlabel_args,a_cred),
	VOPARG_OFFSETOF(struct vop_setlabel_args,a_td),
	VDESC_NO_OFFSET,
};

static int vop_vptofh_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_vptofh_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_VPTOFH_AP(struct vop_vptofh_args *a)
{

	return(VOP_VPTOFH_APV(a->a_vp->v_op, a));
}

int
VOP_VPTOFH_APV(struct vop_vector *vop, struct vop_vptofh_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_vptofh_desc, a->a_vp,
	    ("Wrong a_desc in vop_vptofh(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_vptofh == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_vptofh(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH");
	if (vop->vop_vptofh != NULL)
		rc = vop->vop_vptofh(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR2(KTR_VOP,
	    "VOP_VPTOFH(vp 0x%lX, fhp 0x%lX)",
	    a->a_vp, a->a_fhp);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOFH");
	}
	return (rc);
}

struct vnodeop_desc vop_vptofh_desc = {
	"vop_vptofh",
	0,
	(vop_bypass_t *)VOP_VPTOFH_AP,
	vop_vptofh_vp_offsets,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

static int vop_vptocnp_vp_offsets[] = {
	VOPARG_OFFSETOF(struct vop_vptocnp_args,a_vp),
	VDESC_NO_OFFSET
};

int
VOP_VPTOCNP_AP(struct vop_vptocnp_args *a)
{

	return(VOP_VPTOCNP_APV(a->a_vp->v_op, a));
}

int
VOP_VPTOCNP_APV(struct vop_vector *vop, struct vop_vptocnp_args *a)
{
	int rc;

	VNASSERT(a->a_gen.a_desc == &vop_vptocnp_desc, a->a_vp,
	    ("Wrong a_desc in vop_vptocnp(%p, %p)", a->a_vp, a));
	while(vop != NULL && \
	    vop->vop_vptocnp == NULL && vop->vop_bypass == NULL)
		vop = vop->vop_default;
	VNASSERT(vop != NULL, a->a_vp, ("No vop_vptocnp(%p, %p)", a->a_vp, a));
	ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP");
	ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP");
	if (vop->vop_vptocnp != NULL)
		rc = vop->vop_vptocnp(a);
	else
		rc = vop->vop_bypass(&a->a_gen);
	CTR4(KTR_VOP,
	    "VOP_VPTOCNP(vp 0x%lX, vpp 0x%lX, buf 0x%lX, buflen 0x%lX)",
	    a->a_vp, a->a_vpp, a->a_buf, a->a_buflen);
	if (rc == 0) {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP");
		ASSERT_VI_UNLOCKED(*a->a_vpp, "VOP_VPTOCNP");
		ASSERT_VOP_UNLOCKED(*a->a_vpp, "VOP_VPTOCNP");
	} else {
		ASSERT_VI_UNLOCKED(a->a_vp, "VOP_VPTOCNP");
		ASSERT_VOP_LOCKED(a->a_vp, "VOP_VPTOCNP");
	}
	return (rc);
}

struct vnodeop_desc vop_vptocnp_desc = {
	"vop_vptocnp",
	0,
	(vop_bypass_t *)VOP_VPTOCNP_AP,
	vop_vptocnp_vp_offsets,
	VOPARG_OFFSETOF(struct vop_vptocnp_args,a_vpp),
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
	VDESC_NO_OFFSET,
};

