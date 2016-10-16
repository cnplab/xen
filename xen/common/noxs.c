#include <xen/types.h>
#include <public/io/noxs.h>
#include <xen/noxs.h>
#include <xen/sched.h>


int noxs_init(struct domain* d)
{
    d->device_page = alloc_xenheap_pages(0, MEMF_bits(32));
    if (d->device_page == NULL) {
        return -ENOMEM;
    }

    clear_page(d->device_page);
    share_xen_page_with_guest(virt_to_page(d->device_page), d, XENSHARE_readonly);

    return 0;
}

void noxs_destroy(struct domain* d)
{
    free_xenheap_page(d->device_page);
}

int noxs_dev_add(struct domain* d, noxs_dev_page_entry_t* ndev)
{
    int i;
    noxs_dev_page_entry_t* dev;

    if (ndev->type == noxs_dev_none) {
        return -EINVAL;
    }

    if (d->device_page->dev_count > NOXS_DEV_COUNT_MAX) {
        return -ENOBUFS;
    }

    for (i = 0; i < NOXS_DEV_COUNT_MAX; i++) {
        dev = &(d->device_page->devs[i]);

        if (dev->type == noxs_dev_none) {
            memcpy(dev, ndev, sizeof(noxs_dev_page_entry_t));
            break;
        }
    }

    if (i == NOXS_DEV_COUNT_MAX) {
        /* We checked the page isn't full, there needs to be an empty spot. */
        BUG();
    }

    d->device_page->dev_count++;

    return 0;
}

int noxs_dev_rem(struct domain* d, noxs_dev_key_t* key)
{
    int i;
    noxs_dev_page_entry_t* dev;

    for (i = 0; i < d->device_page->dev_count; i++) {
        dev = &(d->device_page->devs[i]);

        if (dev->type == key->type || dev->id == key->devid) {
            memset(dev, 0, sizeof(noxs_dev_page_entry_t));
        }
    }

    d->device_page->dev_count--;

    return 0;
}

int noxs_dev_enum(struct domain* d, uint32_t* dev_count,
        noxs_dev_page_entry_t devs[NOXS_DEV_COUNT_MAX])
{
    int j;
    noxs_dev_page_entry_t* dev;

    j = 0;
    for (int i = 0; i < NOXS_DEV_COUNT_MAX; i++) {
        dev = &(d->device_page->devs[i]);

        if (dev->type != noxs_dev_none) {
            memcpy(devs + j, dev, sizeof(noxs_dev_page_entry_t));
            j++;
        }
    }

    (*dev_count) = j;

    return 0;
}
