/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../dev/ata/ata_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _ata_if_h_
#define _ata_if_h_

/** @brief Unique descriptor for the ATA_LOCKING() method */
extern struct kobjop_desc ata_locking_desc;
/** @brief A function implementing the ATA_LOCKING() method */
typedef int ata_locking_t(device_t channel, int mode);

static __inline int ATA_LOCKING(device_t channel, int mode)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)channel)->ops,ata_locking);
	return ((ata_locking_t *) _m)(channel, mode);
}


#define         ATA_LF_LOCK             0x0001
#define         ATA_LF_UNLOCK           0x0002
#define         ATA_LF_WHICH            0x0004

/** @brief Unique descriptor for the ATA_SETMODE() method */
extern struct kobjop_desc ata_setmode_desc;
/** @brief A function implementing the ATA_SETMODE() method */
typedef void ata_setmode_t(device_t channel, device_t dev);

static __inline void ATA_SETMODE(device_t channel, device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)channel)->ops,ata_setmode);
	((ata_setmode_t *) _m)(channel, dev);
}

/** @brief Unique descriptor for the ATA_RESET() method */
extern struct kobjop_desc ata_reset_desc;
/** @brief A function implementing the ATA_RESET() method */
typedef void ata_reset_t(device_t channel);

static __inline void ATA_RESET(device_t channel)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)channel)->ops,ata_reset);
	((ata_reset_t *) _m)(channel);
}

/** @brief Unique descriptor for the ATA_REINIT() method */
extern struct kobjop_desc ata_reinit_desc;
/** @brief A function implementing the ATA_REINIT() method */
typedef int ata_reinit_t(device_t dev);

static __inline int ATA_REINIT(device_t dev)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)dev)->ops,ata_reinit);
	return ((ata_reinit_t *) _m)(dev);
}

#endif /* _ata_if_h_ */
