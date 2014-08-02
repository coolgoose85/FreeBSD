/*
 * This file is produced automatically.
 * Do not modify anything in here by hand.
 *
 * Created from source file
 *   ../../../geom/part/g_part_if.m
 * with
 *   makeobjops.awk
 *
 * See the source file for legal information
 */


#ifndef _g_part_if_h_
#define _g_part_if_h_

/** @brief Unique descriptor for the G_PART_ADD() method */
extern struct kobjop_desc g_part_add_desc;
/** @brief A function implementing the G_PART_ADD() method */
typedef int g_part_add_t(struct g_part_table *table, struct g_part_entry *entry,
                         struct g_part_parms *gpp);

static __inline int G_PART_ADD(struct g_part_table *table,
                               struct g_part_entry *entry,
                               struct g_part_parms *gpp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_add);
	return ((g_part_add_t *) _m)(table, entry, gpp);
}

/** @brief Unique descriptor for the G_PART_BOOTCODE() method */
extern struct kobjop_desc g_part_bootcode_desc;
/** @brief A function implementing the G_PART_BOOTCODE() method */
typedef int g_part_bootcode_t(struct g_part_table *table,
                              struct g_part_parms *gpp);

static __inline int G_PART_BOOTCODE(struct g_part_table *table,
                                    struct g_part_parms *gpp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_bootcode);
	return ((g_part_bootcode_t *) _m)(table, gpp);
}

/** @brief Unique descriptor for the G_PART_CREATE() method */
extern struct kobjop_desc g_part_create_desc;
/** @brief A function implementing the G_PART_CREATE() method */
typedef int g_part_create_t(struct g_part_table *table,
                            struct g_part_parms *gpp);

static __inline int G_PART_CREATE(struct g_part_table *table,
                                  struct g_part_parms *gpp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_create);
	return ((g_part_create_t *) _m)(table, gpp);
}

/** @brief Unique descriptor for the G_PART_DESTROY() method */
extern struct kobjop_desc g_part_destroy_desc;
/** @brief A function implementing the G_PART_DESTROY() method */
typedef int g_part_destroy_t(struct g_part_table *table,
                             struct g_part_parms *gpp);

static __inline int G_PART_DESTROY(struct g_part_table *table,
                                   struct g_part_parms *gpp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_destroy);
	return ((g_part_destroy_t *) _m)(table, gpp);
}

/** @brief Unique descriptor for the G_PART_DEVALIAS() method */
extern struct kobjop_desc g_part_devalias_desc;
/** @brief A function implementing the G_PART_DEVALIAS() method */
typedef int g_part_devalias_t(struct g_part_table *table,
                              struct g_part_entry *entry, char *buf,
                              size_t bufsz);

static __inline int G_PART_DEVALIAS(struct g_part_table *table,
                                    struct g_part_entry *entry, char *buf,
                                    size_t bufsz)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_devalias);
	return ((g_part_devalias_t *) _m)(table, entry, buf, bufsz);
}

/** @brief Unique descriptor for the G_PART_DUMPCONF() method */
extern struct kobjop_desc g_part_dumpconf_desc;
/** @brief A function implementing the G_PART_DUMPCONF() method */
typedef void g_part_dumpconf_t(struct g_part_table *table,
                               struct g_part_entry *entry, struct sbuf *sb,
                               const char *indent);

static __inline void G_PART_DUMPCONF(struct g_part_table *table,
                                     struct g_part_entry *entry,
                                     struct sbuf *sb, const char *indent)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_dumpconf);
	((g_part_dumpconf_t *) _m)(table, entry, sb, indent);
}

/** @brief Unique descriptor for the G_PART_DUMPTO() method */
extern struct kobjop_desc g_part_dumpto_desc;
/** @brief A function implementing the G_PART_DUMPTO() method */
typedef int g_part_dumpto_t(struct g_part_table *table,
                            struct g_part_entry *entry);

static __inline int G_PART_DUMPTO(struct g_part_table *table,
                                  struct g_part_entry *entry)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_dumpto);
	return ((g_part_dumpto_t *) _m)(table, entry);
}

/** @brief Unique descriptor for the G_PART_MODIFY() method */
extern struct kobjop_desc g_part_modify_desc;
/** @brief A function implementing the G_PART_MODIFY() method */
typedef int g_part_modify_t(struct g_part_table *table,
                            struct g_part_entry *entry,
                            struct g_part_parms *gpp);

static __inline int G_PART_MODIFY(struct g_part_table *table,
                                  struct g_part_entry *entry,
                                  struct g_part_parms *gpp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_modify);
	return ((g_part_modify_t *) _m)(table, entry, gpp);
}

/** @brief Unique descriptor for the G_PART_NAME() method */
extern struct kobjop_desc g_part_name_desc;
/** @brief A function implementing the G_PART_NAME() method */
typedef const char * g_part_name_t(struct g_part_table *table,
                                   struct g_part_entry *entry, char *buf,
                                   size_t bufsz);

static __inline const char * G_PART_NAME(struct g_part_table *table,
                                         struct g_part_entry *entry, char *buf,
                                         size_t bufsz)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_name);
	return ((g_part_name_t *) _m)(table, entry, buf, bufsz);
}

/** @brief Unique descriptor for the G_PART_PRECHECK() method */
extern struct kobjop_desc g_part_precheck_desc;
/** @brief A function implementing the G_PART_PRECHECK() method */
typedef int g_part_precheck_t(struct g_part_table *table, enum g_part_ctl req,
                              struct g_part_parms *gpp);

static __inline int G_PART_PRECHECK(struct g_part_table *table,
                                    enum g_part_ctl req,
                                    struct g_part_parms *gpp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_precheck);
	return ((g_part_precheck_t *) _m)(table, req, gpp);
}

/** @brief Unique descriptor for the G_PART_PROBE() method */
extern struct kobjop_desc g_part_probe_desc;
/** @brief A function implementing the G_PART_PROBE() method */
typedef int g_part_probe_t(struct g_part_table *table, struct g_consumer *cp);

static __inline int G_PART_PROBE(struct g_part_table *table,
                                 struct g_consumer *cp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_probe);
	return ((g_part_probe_t *) _m)(table, cp);
}

/** @brief Unique descriptor for the G_PART_READ() method */
extern struct kobjop_desc g_part_read_desc;
/** @brief A function implementing the G_PART_READ() method */
typedef int g_part_read_t(struct g_part_table *table, struct g_consumer *cp);

static __inline int G_PART_READ(struct g_part_table *table,
                                struct g_consumer *cp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_read);
	return ((g_part_read_t *) _m)(table, cp);
}

/** @brief Unique descriptor for the G_PART_SETUNSET() method */
extern struct kobjop_desc g_part_setunset_desc;
/** @brief A function implementing the G_PART_SETUNSET() method */
typedef int g_part_setunset_t(struct g_part_table *table,
                              struct g_part_entry *entry, const char *attrib,
                              unsigned int set);

static __inline int G_PART_SETUNSET(struct g_part_table *table,
                                    struct g_part_entry *entry,
                                    const char *attrib, unsigned int set)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_setunset);
	return ((g_part_setunset_t *) _m)(table, entry, attrib, set);
}

/** @brief Unique descriptor for the G_PART_TYPE() method */
extern struct kobjop_desc g_part_type_desc;
/** @brief A function implementing the G_PART_TYPE() method */
typedef const char * g_part_type_t(struct g_part_table *table,
                                   struct g_part_entry *entry, char *buf,
                                   size_t bufsz);

static __inline const char * G_PART_TYPE(struct g_part_table *table,
                                         struct g_part_entry *entry, char *buf,
                                         size_t bufsz)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_type);
	return ((g_part_type_t *) _m)(table, entry, buf, bufsz);
}

/** @brief Unique descriptor for the G_PART_WRITE() method */
extern struct kobjop_desc g_part_write_desc;
/** @brief A function implementing the G_PART_WRITE() method */
typedef int g_part_write_t(struct g_part_table *table, struct g_consumer *cp);

static __inline int G_PART_WRITE(struct g_part_table *table,
                                 struct g_consumer *cp)
{
	kobjop_t _m;
	KOBJOPLOOKUP(((kobj_t)table)->ops,g_part_write);
	return ((g_part_write_t *) _m)(table, cp);
}

#endif /* _g_part_if_h_ */
