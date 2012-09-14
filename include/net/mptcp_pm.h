/*
 *	MPTCP implementation
 *
 *	Initial Design & Implementation:
 *	Sébastien Barré <sebastien.barre@uclouvain.be>
 *
 *	Current Maintainer & Author:
 *	Christoph Paasch <christoph.paasch@uclouvain.be>
 *
 *	Additional authors:
 *	Jaakko Korkeaniemi <jaakko.korkeaniemi@aalto.fi>
 *	Gregory Detal <gregory.detal@uclouvain.be>
 *	Fabien Duchêne <fabien.duchene@uclouvain.be>
 *	Andreas Seelinger <Andreas.Seelinger@rwth-aachen.de>
 *	Lavkesh Lahngir <lavkesh51@gmail.com>
 *	Andreas Ripke <ripke@neclab.eu>
 *	Vlad Dogaru <vlad.dogaru@intel.com>
 *	Octavian Purdila <octavian.purdila@intel.com>
 *	John Ronan <jronan@tssg.org>
 *	Catalin Nicutar <catalin.nicutar@gmail.com>
 *	Brandon Heller <brandonh@stanford.edu>
 *
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#ifndef _MPTCP_PM_H
#define _MPTCP_PM_H

#include <linux/in.h>
#include <linux/in6.h>
#include <linux/jhash.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/spinlock_types.h>
#include <linux/types.h>

#include <net/request_sock.h>
#include <net/sock.h>
#include <net/tcp.h>

/* Max number of local or remote addresses we can store.
 * When changing, see the bitfield below in mptcp_loc4/6. */
#define MPTCP_MAX_ADDR	8

struct mptcp_loc4 {
	u8		id;
	u8		low_prio:1;
	__be16		port;
	struct in_addr	addr;
};

struct mptcp_rem4 {
	u8		id;
	u8		bitfield;
	__be16		port;
	struct in_addr	addr;
};

struct mptcp_loc6 {
	u8		id;
	u8		low_prio:1;
	__be16		port;
	struct in6_addr	addr;
};

struct mptcp_rem6 {
	u8		id;
	u8		bitfield;
	__be16		port;
	struct in6_addr	addr;
};

struct mptcp_cb;
#ifdef CONFIG_MPTCP

#define MPTCP_HASH_SIZE                1024

/* This second hashtable is needed to retrieve request socks
 * created as a result of a join request. While the SYN contains
 * the token, the final ack does not, so we need a separate hashtable
 * to retrieve the mpcb.
 */
extern struct list_head mptcp_reqsk_htb[MPTCP_HASH_SIZE];
extern spinlock_t mptcp_reqsk_hlock;	/* hashtable protection */

extern spinlock_t mptcp_reqsk_tk_hlock;	/* hashtable protection */

void mptcp_send_updatenotif(struct mptcp_cb *mpcb);

void mptcp_send_updatenotif_wq(struct work_struct *work);
struct mp_join *mptcp_find_join(struct sk_buff *skb);
u8 mptcp_get_loc_addrid(struct mptcp_cb *mpcb, struct sock *sk);
void mptcp_hash_insert(struct mptcp_cb *mpcb, u32 token);
void mptcp_hash_remove(struct mptcp_cb *mpcb);
struct mptcp_cb *mptcp_hash_find(u32 token);
int mptcp_lookup_join(struct sk_buff *skb);
int mptcp_find_token(u32 token);
int mptcp_reqsk_find_tk(u32 token);
void mptcp_reqsk_insert_tk(struct request_sock *reqsk, u32 token);
void mptcp_reqsk_remove_tk(struct request_sock *reqsk);
void mptcp_set_addresses(struct mptcp_cb *mpcb);
int mptcp_syn_recv_sock(struct sk_buff *skb);
void mptcp_address_worker(struct work_struct *work);
int mptcp_pm_addr_event_handler(unsigned long event, void *ptr, int family);
struct sock *mptcp_select_loc_sock(const struct mptcp_cb *mpcb, u16 ids);

#else /* CONFIG_MPTCP */
static inline int mptcp_find_token(u32 token)
{
	return 0;
}

#endif /* CONFIG_MPTCP */

#endif /*_MPTCP_PM_H*/
