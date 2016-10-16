#include <xen/types.h>
#include <public/devctl.h>
#include <xen/guest_access.h>
#include <xen/noxs.h>
#include <xen/sched.h>


long do_devctl(XEN_GUEST_HANDLE_PARAM(xen_devctl_t) u_devctl)
{
    long ret;
    xen_devctl_t devctl;
    domid_t domid;
    struct domain* d;

    if (copy_from_guest(&devctl, u_devctl, 1)) {
        return -EFAULT;
    }

    if (devctl.version != XEN_DEVCTL_VERSION) {
        return -EACCES;
    }

    switch (devctl.cmd) {
        case XEN_DEVCTL_get:
            if (devctl.domain == DOMID_SELF) {
                domid = current->domain->domain_id;
            } else if (devctl.domain == current->domain->domain_id) {
                domid = devctl.domain;
            } else if (is_control_domain(current->domain)) {
                domid = devctl.domain;
            } else {
                return -EPERM;
            }
            break;

        default:
            if (is_control_domain(current->domain)) {
                domid = devctl.domain;
            } else {
                return -EPERM;
            }
            break;
    }

    d = rcu_lock_domain_by_id(domid);
    if (d == NULL) {
        return -ESRCH;
    }

    switch (devctl.cmd) {
        case XEN_DEVCTL_get:
            devctl.u.get.mfn = virt_to_maddr(d->device_page);
            ret = 0;
            break;

        case XEN_DEVCTL_dev_add:
            ret = noxs_dev_add(d, &(devctl.u.dev_add.dev));
            break;

        case XEN_DEVCTL_dev_rem:
            ret = noxs_dev_rem(d, &(devctl.u.dev_rem.dev));
            break;

        case XEN_DEVCTL_dev_enum:
            ret = noxs_dev_enum(d, &(devctl.u.dev_enum.dev_count), devctl.u.dev_enum.devs);
            break;

        default:
            ret = -ESRCH;
            break;
    }
    if (ret) {
        goto fail;
    }

    rcu_unlock_domain(d);

    switch (devctl.cmd) {
        case XEN_DEVCTL_get:
        case XEN_DEVCTL_dev_enum:
            ret = copy_to_guest(u_devctl, &devctl, 1);
            break;

        default:
            break;
    }
    if (ret) {
        goto fail_dev;
    }

    return 0;

fail_dev:
    /* FIXME: rollback changes  */

fail:
    rcu_unlock_domain(d);

    return ret;
}
