/*
 * noxs.h
 *
 *  Created on: Sep 16, 2016
 *      Author: wolf
 */

#ifndef XEN_PUBLIC_IO_NOXS_H_
#define XEN_PUBLIC_IO_NOXS_H_

#include "../xen.h"
#include "../event_channel.h"
#include "../grant_table.h"


#define NOXS_DEV_COUNT_MAX 32


typedef uint32_t noxs_dev_id_t;


enum noxs_dev_type {
	noxs_dev_none = 0,
	noxs_dev_sysctl,
	noxs_dev_console,
	noxs_dev_vif,
};
typedef enum noxs_dev_type noxs_dev_type_t;


struct noxs_dev_key {
	noxs_dev_type_t type;
	domid_t be_id;
	domid_t fe_id;
	noxs_dev_id_t devid;
};
typedef struct noxs_dev_key noxs_dev_key_t;


struct noxs_dev_comm {
	grant_ref_t grant;
	evtchn_port_t evtchn;
};
typedef struct noxs_dev_comm noxs_dev_comm_t;


struct noxs_dev_page_entry {
	noxs_dev_type_t type;
	noxs_dev_id_t id;

	domid_t be_id;
	noxs_dev_comm_t comm;
};
typedef struct noxs_dev_page_entry noxs_dev_page_entry_t;


struct noxs_dev_page {
	uint32_t version;
	uint32_t dev_count;
	noxs_dev_page_entry_t devs[NOXS_DEV_COUNT_MAX];
};
typedef struct noxs_dev_page noxs_dev_page_t;


enum noxs_watch_state {
	noxs_watch_none = 0,
	noxs_watch_requested,
	noxs_watch_updated
};
typedef enum noxs_watch_state noxs_watch_state_t;


struct noxs_ctrl_hdr {
	int devid;
	int be_state;
	int fe_state;

	noxs_watch_state_t fe_watch_state;
	noxs_watch_state_t be_watch_state;
};
typedef struct noxs_ctrl_hdr noxs_ctrl_hdr_t;


/*
 * DEVICE SPECIFIC
 */

/* Sysctl device */
struct sysctl_fe_features {
    uint8_t poweroff:1;
    uint8_t reboot:1;
    uint8_t suspend:1;
    uint8_t platform_multiprocessor_suspend:1;
};

struct noxs_sysctl_ctrl_page {
	noxs_ctrl_hdr_t hdr;
	struct sysctl_fe_features fe_feat;

	union {
	    struct {
	        uint8_t poweroff:1;
	        uint8_t reboot:1;
	        uint8_t suspend:1;
	        uint8_t crash:1;
	        uint8_t watchdog:1;
	    } bits;

	    uint8_t status;
	};
};
typedef struct noxs_sysctl_ctrl_page noxs_sysctl_ctrl_page_t;


/* Network device */
struct vif_be_features {
	uint8_t rx_notify:1;
	uint8_t sg:1;
	uint8_t gso_tcpv4:1;
	uint8_t gso_tcpv4_prefix:1;
	uint8_t gso_tcpv6:1;
	uint8_t gso_tcpv6_prefix:1;
	uint8_t no_csum_offload:1;
	uint8_t ipv6_csum_offload:1;
	uint8_t rx_copy:1;
	uint8_t rx_flip:1;
	uint8_t multicast_control:1;
	uint8_t dynamic_multicast_control:1;
	uint8_t split_event_channels:1;
	uint8_t ctrl_ring:1;
	uint8_t netmap:1;
};

struct vif_fe_features {
	uint8_t rx_notify:1;
	uint8_t persistent:1;
	uint8_t sg:1;
	uint8_t gso_tcpv4:1;
	uint8_t gso_tcpv4_prefix:1;
	uint8_t gso_tcpv6:1;
	uint8_t gso_tcpv6_prefix:1;
	uint8_t no_csum_offload:1;
	uint8_t ipv6_csum_offload:1;
	uint8_t ctrl_ring:1;
};

#define ETH_LEN    6       /* Octets in one ethernet address */

struct noxs_vif_ctrl_page {
	noxs_ctrl_hdr_t hdr;
	int vifid;
	struct vif_be_features be_feat;
	int multi_queue_max_queues;
	int multi_queue_num_queues;

	grant_ref_t tx_ring_ref;
	grant_ref_t rx_ring_ref;
	evtchn_port_t event_channel_tx;
	evtchn_port_t event_channel_rx;

	unsigned int request_rx_copy;
	struct vif_fe_features fe_feat;

	grant_ref_t ctrl_ring_ref;
	evtchn_port_t event_channel_ctrl;

	uint8_t mac[ETH_LEN];
	uint32_t ip;
};
typedef struct noxs_vif_ctrl_page noxs_vif_ctrl_page_t;


/*
 * CONFIG
 */

struct noxs_cfg_vif {
	uint8_t mac[ETH_LEN];
	uint32_t ip;
};
typedef struct noxs_cfg_vif noxs_cfg_vif_t;



#endif /* XEN_PUBLIC_IO_NOXS_H_ */
