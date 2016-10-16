#ifndef XEN_NOXS_H_
#define XEN_NOXS_H_

#include <xen/types.h>
#include <public/io/noxs.h>
#include <xen/sched.h>


int noxs_init(struct domain* d);
void noxs_destroy(struct domain* d);

int noxs_dev_add(struct domain* d, noxs_dev_page_entry_t* ndev);
int noxs_dev_rem(struct domain* d, noxs_dev_key_t* key);
int noxs_dev_enum(struct domain* d, uint32_t* dev_count,
        noxs_dev_page_entry_t devs[NOXS_DEV_COUNT_MAX]);


#endif /* XEN_NOXS_H */
