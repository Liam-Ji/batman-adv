/*
 * Copyright (C) 2007-2012 B.A.T.M.A.N. contributors:
 *
 * Marek Lindner, Simon Wunderlich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 *
 */

#ifndef _NET_BATMAN_ADV_MAIN_H_
#define _NET_BATMAN_ADV_MAIN_H_

#define DRIVER_AUTHOR "Marek Lindner <lindner_marek@yahoo.de>, " \
		      "Simon Wunderlich <siwu@hrz.tu-chemnitz.de>"
#define DRIVER_DESC   "B.A.T.M.A.N. advanced"
#define DRIVER_DEVICE "batman-adv"

#ifndef SOURCE_VERSION
#define SOURCE_VERSION "2012.2.0"
#endif

/* B.A.T.M.A.N. parameters */

#define TQ_MAX_VALUE 255
#define JITTER 20

 /* Time To Live of broadcast messages */
#define TTL 50

/* purge originators after time in seconds if no valid packet comes in
 * -> TODO: check influence on TQ_LOCAL_WINDOW_SIZE */
#define PURGE_TIMEOUT 200000 /* 200 seconds */
#define TT_LOCAL_TIMEOUT 3600000 /* in miliseconds */
#define TT_CLIENT_ROAM_TIMEOUT 600000 /* in miliseconds */
/* sliding packet range of received originator messages in sequence numbers
 * (should be a multiple of our word size) */
#define TQ_LOCAL_WINDOW_SIZE 64
#define TT_REQUEST_TIMEOUT 3000 /* miliseconds we have to keep
				 * pending tt_req */

#define TQ_GLOBAL_WINDOW_SIZE 5
#define TQ_LOCAL_BIDRECT_SEND_MINIMUM 1
#define TQ_LOCAL_BIDRECT_RECV_MINIMUM 1
#define TQ_TOTAL_BIDRECT_LIMIT 1

#define TT_OGM_APPEND_MAX 3 /* number of OGMs sent with the last tt diff */

#define ROAMING_MAX_TIME 20000 /* Time in which a client can roam at most
				* ROAMING_MAX_COUNT times in miliseconds*/
#define ROAMING_MAX_COUNT 5

#define NO_FLAGS 0

#define NULL_IFINDEX 0 /* dummy ifindex used to avoid iface checks */

#define NUM_WORDS BITS_TO_LONGS(TQ_LOCAL_WINDOW_SIZE)

/* msecs after which an ARP_REQUEST is sent in broadcast as fallback */
#define ARP_REQ_DELAY 250
/* numbers of originator to contact for any PUT/GET DHT operation */
#define DHT_CANDIDATES_NUM 3
/* Factor which default ARP timeout values of the soft_iface table are
 * multiplied by */
#define ARP_TIMEOUT_FACTOR 4

#define LOG_BUF_LEN 8192	  /* has to be a power of 2 */

#define VIS_INTERVAL 5000	/* 5 seconds */

/* how much worse secondary interfaces may be to be considered as bonding
 * candidates */
#define BONDING_TQ_THRESHOLD	50

/* should not be bigger than 512 bytes or change the size of
 * forw_packet->direct_link_flags */
#define MAX_AGGREGATION_BYTES 512
#define MAX_AGGREGATION_MS 100

#define BLA_PERIOD_LENGTH	10000	/* 10 seconds */
#define BLA_BACKBONE_TIMEOUT	(BLA_PERIOD_LENGTH * 3)
#define BLA_CLAIM_TIMEOUT	(BLA_PERIOD_LENGTH * 10)

#define DUPLIST_SIZE		16
#define DUPLIST_TIMEOUT		500	/* 500 ms */
/* don't reset again within 30 seconds */
#define RESET_PROTECTION_MS 30000
#define EXPECTED_SEQNO_RANGE	65536

enum mesh_state {
	MESH_INACTIVE,
	MESH_ACTIVE,
	MESH_DEACTIVATING
};

#define BCAST_QUEUE_LEN		256
#define BATMAN_QUEUE_LEN	256

enum uev_action {
	UEV_ADD = 0,
	UEV_DEL,
	UEV_CHANGE
};

enum uev_type {
	UEV_GW = 0
};

#define GW_THRESHOLD	50

#define DHT_CANDIDATE_NOT_FOUND	0
#define DHT_CANDIDATE_ORIG	1

/* Debug Messages */
#ifdef pr_fmt
#undef pr_fmt
#endif
/* Append 'batman-adv: ' before kernel messages */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/* all messages related to routing / flooding / broadcasting / etc */
enum dbg_level {
	DBG_BATMAN = 1 << 0,
	DBG_ROUTES = 1 << 1, /* route added / changed / deleted */
	DBG_TT	   = 1 << 2, /* translation table operations */
	DBG_BLA    = 1 << 3, /* bridge loop avoidance */
	DBG_DAT    = 1 << 4, /* snooped arp messages / dat operations */
	DBG_ALL    = 31
};

/* Kernel headers */

#include <linux/mutex.h>	/* mutex */
#include <linux/module.h>	/* needed by all modules */
#include <linux/netdevice.h>	/* netdevice */
#include <linux/etherdevice.h>  /* ethernet address classification */
#include <linux/if_ether.h>	/* ethernet header */
#include <linux/poll.h>		/* poll_table */
#include <linux/kthread.h>	/* kernel threads */
#include <linux/pkt_sched.h>	/* schedule types */
#include <linux/workqueue.h>	/* workqueue */
#include <linux/percpu.h>
#include <linux/slab.h>
#include <net/sock.h>		/* struct sock */
#include <linux/jiffies.h>
#include <linux/seq_file.h>
#include "compat.h"

#include "types.h"

extern char bat_routing_algo[];
extern struct list_head hardif_list;

extern unsigned char broadcast_addr[];
extern struct workqueue_struct *bat_event_workqueue;

int mesh_init(struct net_device *soft_iface);
void mesh_free(struct net_device *soft_iface);
void inc_module_count(void);
void dec_module_count(void);
int is_my_mac(const uint8_t *addr);
int batman_skb_recv(struct sk_buff *skb, struct net_device *dev,
		    struct packet_type *ptype, struct net_device *orig_dev);
int recv_handler_register(uint8_t packet_type,
			  int (*recv_handler)(struct sk_buff *,
					      struct hard_iface *));
void recv_handler_unregister(uint8_t packet_type);
int bat_algo_register(struct bat_algo_ops *bat_algo_ops);
int bat_algo_select(struct bat_priv *bat_priv, char *name);
int bat_algo_seq_print_text(struct seq_file *seq, void *offset);

#ifdef CONFIG_BATMAN_ADV_DEBUG
int debug_log(struct bat_priv *bat_priv, const char *fmt, ...) __printf(2, 3);

#define bat_dbg(type, bat_priv, fmt, arg...)			\
	do {							\
		if (atomic_read(&bat_priv->log_level) & type)	\
			debug_log(bat_priv, fmt, ## arg);	\
	}							\
	while (0)
#else /* !CONFIG_BATMAN_ADV_DEBUG */
__printf(3, 4)
static inline void bat_dbg(int type __always_unused,
			   struct bat_priv *bat_priv __always_unused,
			   const char *fmt __always_unused, ...)
{
}
#endif

#define bat_info(net_dev, fmt, arg...)					\
	do {								\
		struct net_device *_netdev = (net_dev);                 \
		struct bat_priv *_batpriv = netdev_priv(_netdev);       \
		bat_dbg(DBG_ALL, _batpriv, fmt, ## arg);		\
		pr_info("%s: " fmt, _netdev->name, ## arg);		\
	} while (0)
#define bat_err(net_dev, fmt, arg...)					\
	do {								\
		struct net_device *_netdev = (net_dev);                 \
		struct bat_priv *_batpriv = netdev_priv(_netdev);       \
		bat_dbg(DBG_ALL, _batpriv, fmt, ## arg);		\
		pr_err("%s: " fmt, _netdev->name, ## arg);		\
	} while (0)

/**
 * returns 1 if they are the same ethernet addr
 *
 * note: can't use compare_ether_addr() as it requires aligned memory
 */

static inline int compare_eth(const void *data1, const void *data2)
{
	return (memcmp(data1, data2, ETH_ALEN) == 0 ? 1 : 0);
}

/**
 * has_timed_out - compares current time (jiffies) and timestamp + timeout
 * @timestamp:		base value to compare with (in jiffies)
 * @timeout:		added to base value before comparing (in milliseconds)
 *
 * Returns true if current time is after timestamp + timeout
 */
static inline bool has_timed_out(unsigned long timestamp, unsigned int timeout)
{
	return time_is_before_jiffies(timestamp + msecs_to_jiffies(timeout));
}

#define atomic_dec_not_zero(v)	atomic_add_unless((v), -1, 0)

/* Returns the smallest signed integer in two's complement with the sizeof x */
#define smallest_signed_int(x) (1u << (7u + 8u * (sizeof(x) - 1u)))

/* Returns the biggest unsigned integer with the sizeof x */
#define biggest_unsigned_int(x) (~(x)0)

/* Checks if a sequence number x is a predecessor/successor of y.
 * they handle overflows/underflows and can correctly check for a
 * predecessor/successor unless the variable sequence number has grown by
 * more then 2**(bitwidth(x)-1)-1.
 * This means that for a uint8_t with the maximum value 255, it would think:
 *  - when adding nothing - it is neither a predecessor nor a successor
 *  - before adding more than 127 to the starting value - it is a predecessor,
 *  - when adding 128 - it is neither a predecessor nor a successor,
 *  - after adding more than 127 to the starting value - it is a successor */
#define seq_before(x, y) ({typeof(x) _d1 = (x); \
			  typeof(y) _d2 = (y); \
			  typeof(x) _dummy = (_d1 - _d2); \
			  (void) (&_d1 == &_d2); \
			  _dummy > smallest_signed_int(_dummy); })
#define seq_after(x, y) seq_before(y, x)

/* Stop preemption on local cpu while incrementing the counter */
static inline void add_counter(struct bat_priv *bat_priv, size_t idx,
			       size_t count)
{
	int cpu = get_cpu();
	per_cpu_ptr(bat_priv->bat_counters, cpu)[idx] += count;
	put_cpu();
}

#define inc_counter(b, i) add_counter(b, i, 1)

/* Sum and return the cpu-local counters for index 'idx' */
static inline uint64_t sum_counter(struct bat_priv *bat_priv, size_t idx)
{
	uint64_t *counters;
	int cpu;
	int sum = 0;

	for_each_possible_cpu(cpu) {
		counters = per_cpu_ptr(bat_priv->bat_counters, cpu);
		sum += counters[idx];
	}

	return sum;
}

#endif /* _NET_BATMAN_ADV_MAIN_H_ */
