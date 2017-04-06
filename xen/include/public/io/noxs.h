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
	noxs_dev_vbd,
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

#define CONFIG_NOXS_TRACE 1
#ifdef CONFIG_NOXS_TRACE
struct noxs_timestamp {
    long sec;
    long nsec;
};
typedef struct noxs_timestamp noxs_timestamp_t;
#endif

struct noxs_ctrl_hdr {
	int devid;
	int be_state;
	int fe_state;

	noxs_watch_state_t fe_watch_state;
	noxs_watch_state_t be_watch_state;

#ifdef CONFIG_NOXS_TRACE
	int ready;
	noxs_timestamp_t ts_create;
	noxs_timestamp_t ts_ready;
#endif
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
#define IF_LEN     16

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
	char bridge[IF_LEN];
};
typedef struct noxs_vif_ctrl_page noxs_vif_ctrl_page_t;


/* Block device */
struct vbd_be_features {
	uint16_t max_indirect_segments;
	uint8_t persistent:1;
	uint8_t barrier:1;
	uint8_t flush_cache:1;
	uint8_t discard:1;
};

struct vbd_fe_features {
	uint8_t persistent:1;
};

enum noxs_vbd_type {
	noxs_vbd_type_none,
	noxs_vbd_type_phy,
	noxs_vbd_type_file,
};
typedef enum noxs_vbd_type noxs_vbd_type_t;

enum noxs_vbd_mode {
	noxs_vbd_mode_none,
	noxs_vbd_mode_rdonly,
	noxs_vbd_mode_rdwr,
};
typedef enum noxs_vbd_mode noxs_vbd_mode_t;

struct noxs_ring {
	evtchn_port_t evtchn;
	grant_ref_t refs[0];
};
typedef struct noxs_ring noxs_ring_t;

struct noxs_vbd_ctrl_page {
	noxs_ctrl_hdr_t hdr;
	struct vbd_be_features be_feat;

	uint32_t major;
	uint32_t minor;
	noxs_vbd_type_t type;
	noxs_vbd_mode_t mode;

	uint32_t info;
	uint64_t sectors;
	uint64_t sector_size;
	uint64_t sector_size_physical;

	int32_t discard_enable;
	int32_t discard_granularity;
	int32_t discard_alignment;
	int32_t discard_secure;

	struct vbd_fe_features fe_feat;
	char protocol[64];

	int32_t max_rings;
	int32_t max_ring_page_order;

	int32_t num_rings;
	int32_t ring_page_order;

	noxs_ring_t rings_start_addr[0];
};
typedef struct noxs_vbd_ctrl_page noxs_vbd_ctrl_page_t;


/*
 * CONFIG
 */

struct noxs_cfg_vif {
	uint8_t mac[ETH_LEN];
	uint32_t ip;
	char bridge[IF_LEN];
};
typedef struct noxs_cfg_vif noxs_cfg_vif_t;

struct noxs_cfg_vbd {
	unsigned major;
	unsigned minor;
	noxs_vbd_type_t type;
	noxs_vbd_mode_t mode;
};
typedef struct noxs_cfg_vbd noxs_cfg_vbd_t;

#endif /* XEN_PUBLIC_IO_NOXS_H_ */
