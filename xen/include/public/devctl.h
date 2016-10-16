#ifndef XEN_PUBLIC_DEVCTL_H_
#define XEN_PUBLIC_DEVCTL_H_

#include "xen.h"
#include "io/noxs.h"

#define XEN_DEVCTL_VERSION 0x00000001


struct xen_devctl_dev_add {
    /* IN */
    noxs_dev_page_entry_t dev;

    /* OUT */
};
typedef struct xen_devctl_dev_add xen_devctl_dev_add_t;

struct xen_devctl_dev_rem {
    /* IN */
    noxs_dev_key_t dev;

    /* OUT */
};
typedef struct xen_devctl_dev_rem xen_devctl_dev_rem_t;

struct xen_devctl_dev_enum {
    /* IN */

    /* OUT */
	uint32_t dev_count;
	noxs_dev_page_entry_t devs[NOXS_DEV_COUNT_MAX];
};
typedef struct xen_devctl_dev_enum xen_devctl_dev_enum_t;

struct xen_devctl_get {
    /* IN */

    /* OUT */
    unsigned long mfn;
};
typedef struct xen_devctl_get xen_devctl_get_t;

struct xen_devctl {
    uint32_t version;

    uint32_t cmd;
#define XEN_DEVCTL_get      1
#define XEN_DEVCTL_dev_add  2
#define XEN_DEVCTL_dev_rem  3
#define XEN_DEVCTL_dev_enum 4

    domid_t domain;
    union {
        xen_devctl_get_t get;
        xen_devctl_dev_add_t dev_add;
        xen_devctl_dev_rem_t dev_rem;
        xen_devctl_dev_enum_t dev_enum;
    } u;
};
typedef struct xen_devctl xen_devctl_t;
DEFINE_XEN_GUEST_HANDLE(xen_devctl_t);

#endif /* XEN_PUBLIC_DEVCTL_H */
