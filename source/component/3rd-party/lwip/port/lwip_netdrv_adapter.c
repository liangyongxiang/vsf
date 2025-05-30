/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_LWIP == ENABLED

// for protected member vsf_eda_t.pending_node
#define __VSF_EDA_CLASS_INHERIT__
#include "kernel/vsf_kernel.h"

#define __VSF_NETDRV_CLASS_INHERIT_NETIF__
#include "component/tcpip/vsf_tcpip.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/ethip6.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"

#include "lwip/tcpip.h"

/*============================ MACROS ========================================*/

#if VSF_KERNEL_CFG_SUPPORT_SYNC != ENABLED
#   error VSF_KERNEL_CFG_SUPPORT_SYNC MUST be enabled to use the pending_node in eda
#endif

#ifndef TCPIP_CFG_HOSTNAME
#   define TCPIP_CFG_HOSTNAME           "lwip"
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void lwip_req___addr___from_user(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway);

static vsf_err_t __lwip_netdrv_adapter_on_connect(void *netif);
static void __lwip_netdrv_adapter_on_disconnect(void *netif);
static void * __lwip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size);
static void __lwip_netdrv_adapter_free_buf(void *netbuf);
static void * __lwip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem);
static uint8_t * __lwip_netdrv_adapter_header(void *netbuf, int32_t len);
static void __lwip_netdrv_adapter_on_netbuf_outputted(void *netif, void *netbuf);
static void __lwip_netdrv_adapter_on_netlink_outputted(void *netif, vsf_err_t err);
static void __lwip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size);

static vk_netdrv_feature_t __lwip_netdrv_adapter_feature(void);
static void * __lwip_netdrv_adapter_thread(void (*entry)(void *param), void *param);

/*============================ LOCAL VARIABLES ===============================*/

static const vk_netdrv_adapter_op_t __lwip_netdrv_adapter_op = {
    .on_connect             = __lwip_netdrv_adapter_on_connect,
    .on_disconnect          = __lwip_netdrv_adapter_on_disconnect,

    .alloc_buf              = __lwip_netdrv_adapter_alloc_buf,
    .free_buf               = __lwip_netdrv_adapter_free_buf,
    .read_buf               = __lwip_netdrv_adapter_read_buf,

    .header                 = __lwip_netdrv_adapter_header,
    .on_netbuf_outputted    = __lwip_netdrv_adapter_on_netbuf_outputted,
    .on_netlink_outputted   = __lwip_netdrv_adapter_on_netlink_outputted,
    .on_inputted            = __lwip_netdrv_adapter_on_inputted,

    .feature                = __lwip_netdrv_adapter_feature,
    .thread                 = __lwip_netdrv_adapter_thread,
};

/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(lwip_req___addr___from_user)
void lwip_req___addr___from_user(ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gateway)
{
}

// ethernetif implementation
static err_t __ethernetif_low_level_output(struct netif *netif, struct pbuf *p)
{
    vk_netdrv_t *netdrv = netif->state;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    pbuf_ref(p);

    void *slot;
    vsf_protect_t orig = vsf_protect_sched();
    while ((slot = vk_netdrv_can_output(netdrv)) == NULL) {
        vsf_eda_t *eda = vsf_eda_get_cur();
        VSF_ASSERT(vsf_eda_is_stack_owner(eda));
        vsf_dlist_queue_enqueue(
            vsf_eda_t, pending_node,
            &netdrv->adapter.eda_pending_list,
            eda);
        vsf_unprotect_sched(orig);
        vsf_thread_wfe(VSF_EVT_USER);
        orig = vsf_protect_sched();
    }
    vsf_unprotect_sched(orig);

    vk_netdrv_output(netdrv, slot, p);
    return ERR_OK;
}

err_t ethernetif_init(struct netif *netif)
{
    vk_netdrv_t *netdrv;

    VSF_ASSERT(netif != NULL);
    netdrv = netif->state;
    VSF_ASSERT((netdrv != NULL) && (netdrv->macaddr.size <= NETIF_MAX_HWADDR_LEN));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = TCPIP_CFG_HOSTNAME;
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

    netif->name[0] = 'X';
    netif->name[1] = 'X';

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
#if LWIP_IPV4
    netif->output = etharp_output;
#endif
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = __ethernetif_low_level_output;

    /* initialize the hardware */
    if (VSF_ERR_NONE != vk_netdrv_init(netdrv)) {
        return ERR_IF;
    }

    netif->hwaddr_len = netdrv->macaddr.size;
    memcpy(netif->hwaddr, netdrv->macaddr.addr_buf, netif->hwaddr_len);
    netif->mtu = netdrv->mtu;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_IGMP;

    return ERR_OK;
}

// adapter
static vsf_err_t __lwip_netdrv_adapter_on_connect(void *netif)
{
    static uint8_t __lwip_netif_idx = 0;
    VSF_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    struct netif *lwip_netif = netif;
    ip_addr_t ipaddr = { 0 }, netmask = { 0 }, gateway = { 0 };

    lwip_req___addr___from_user(&ipaddr, &netmask, &gateway);

    LOCK_TCPIP_CORE();
#if LWIP_IPV4 && LWIP_IPV6
    lwip_netif = netif_add(lwip_netif, &ipaddr.u_addr.ip4, &netmask.u_addr.ip4,
                &gateway.u_addr.ip4, lwip_netif->state,
                ethernetif_init, tcpip_input);
#elif LWIP_IPV4
    lwip_netif = netif_add(lwip_netif, &ipaddr, &netmask,
                &gateway, lwip_netif->state,
                ethernetif_init, tcpip_input);
#endif
    lwip_netif->name[0] = 'n';
    lwip_netif->name[1] = '0' + __lwip_netif_idx++;
    UNLOCK_TCPIP_CORE();
    return lwip_netif != NULL ? VSF_ERR_NONE : VSF_ERR_FAIL;
}

static void __lwip_netdrv_adapter_on_disconnect(void *netif)
{
    VSF_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    LOCK_TCPIP_CORE();
        // TODO: make sure netif_remove does not depend on thread environment
        netif_remove((struct netif *)netif);
    UNLOCK_TCPIP_CORE();
}

static void __lwip_netdrv_adapter_on_netbuf_outputted(void *netif, void *netbuf)
{
    VSF_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));

#if ETH_PAD_SIZE
    pbuf_header((struct pbuf *)netbuf, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    pbuf_free((struct pbuf *)netbuf);
    LINK_STATS_INC(link.xmit);
}

static void __lwip_netdrv_adapter_on_netlink_outputted(void *netif, vsf_err_t err)
{
    struct netif *lwip_netif = netif;
    vk_netdrv_t *netdrv = lwip_netif->state;

    vsf_eda_t *eda;
    vsf_protect_t orig = vsf_protect_sched();
        vsf_dlist_queue_dequeue(
            vsf_eda_t, pending_node,
            &netdrv->adapter.eda_pending_list,
            eda);
    vsf_unprotect_sched(orig);

    if (eda != NULL) {
        vsf_eda_post_evt(eda, VSF_EVT_USER);
    }
}

static void __lwip_netdrv_adapter_on_inputted(void *netif, void *netbuf, uint_fast32_t size)
{
    VSF_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    struct netif *lwip_netif = netif;
    struct pbuf *pbuf = netbuf;

#if ETH_PAD_SIZE
    pbuf_header(pbuf, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    if (lwip_netif->input(pbuf, lwip_netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: input error\n"));
        pbuf_free(pbuf);
        pbuf = NULL;
    }
}

static uint8_t * __lwip_netdrv_adapter_header(void *netbuf, int32_t len)
{
    struct pbuf *pbuf = netbuf;

    pbuf_header(pbuf, len);
    return pbuf->payload;
}

static void * __lwip_netdrv_adapter_alloc_buf(void *netif, uint_fast32_t size)
{
    VSF_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    void *netbuf;

#if ETH_PAD_SIZE
    size += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

#if PBUF_POOL_SIZE > 0
    netbuf = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
#else
    netbuf = pbuf_alloc(PBUF_RAW, size, PBUF_RAM);
#endif

    if (netbuf != NULL) {
#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    }
    return netbuf;
}

static void * __lwip_netdrv_adapter_read_buf(void *netbuf, vsf_mem_t *mem)
{
    VSF_ASSERT((netbuf != NULL) && (mem != NULL));
    struct pbuf *pbuf = netbuf;

    mem->buffer = pbuf->payload;
    mem->size = pbuf->len;
    return pbuf->next;
}

static void __lwip_netdrv_adapter_free_buf(void *netbuf)
{
    VSF_ASSERT(vsf_eda_is_stack_owner(vsf_eda_get_cur()));
    pbuf_free((struct pbuf *)netbuf);
}

static vk_netdrv_feature_t __lwip_netdrv_adapter_feature(void)
{
    return VSF_NETDRV_FEATURE_THREAD;
}

static void * __lwip_netdrv_adapter_thread(void (*entry)(void *param), void *param)
{
    return sys_thread_new("netdrv_thread", entry, param, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);
}

void lwip_netif_set_netdrv(struct netif *netif, vk_netdrv_t *netdrv)
{
    netif->state = netdrv;
    netdrv->adapter.netif = (void *)netif;
    netdrv->adapter.op = &__lwip_netdrv_adapter_op;
}

vk_netdrv_t * lwip_netif_get_netdrv(struct netif *netif)
{
    return (vk_netdrv_t *)netif->state;
}

#endif      // VSF_USE_TCPIP && VSF_USE_LWIP
