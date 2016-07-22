/*
 * Auto generated file
 * nlmon.c -- Utility for monitoring netlink communication of vRouter-Agent
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>

#include <net/if.h>

#include <sys/socket.h>
#if defined(__linux__)
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_ether.h>
#elif defined(__FreeBSD__)
#include <net/ethernet.h>
#endif
#include <linux/genetlink.h>

//#include <libxml/xmlmemory.h>
//#include <libxml/parser.h>

#include "vr_genetlink.h"
#include "nl_util.h"

#include "ini_parser.h"
#include "vr_types.h"

#include "vr_nexthop.h"
//#include <vr_types.h>
//#include <vt_gen_lib.h>
//#include <vtest.h>

#define GROUP_ID 31
static struct nl_client *cl;

static bool reader = true;
void
vr_nexthop_req_process(void *s)
{
    vr_nexthop_req *req = (vr_nexthop_req *)s;
    printf("vr_nexthop_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("nhr_type : %d\n", req->nhr_type);
    printf("nhr_family : %d\n", req->nhr_family);
    printf("nhr_id : %" PRId32 "\n", req->nhr_id);
    printf("nhr_rid : %" PRId32 "\n", req->nhr_rid);
    printf("nhr_encap_oif_id : %" PRId32 "\n", req->nhr_encap_oif_id);
    printf("nhr_encap_len : %" PRId32 "\n", req->nhr_encap_len);
    printf("nhr_encap_family : %" PRId32 "\n", req->nhr_encap_family);
    printf("nhr_vrf : %" PRId32 "\n", req->nhr_vrf);
    printf("nhr_tun_sip : %" PRIu32 "\n", req->nhr_tun_sip);
    printf("nhr_tun_dip : %" PRIu32 "\n", req->nhr_tun_dip);
    printf("nhr_ref_cnt : %" PRId32 "\n", req->nhr_ref_cnt);
    printf("nhr_marker : %" PRId32 "\n", req->nhr_marker);
    printf("nhr_flags : %" PRIu32 "\n", req->nhr_flags);
    printf("nhr_encap list values : ");
    i = 0;
    while(req->nhr_encap[i])
    {
        printf("%d  ", req->nhr_encap[i]);
        i++;
    }
    printf("\n");
    printf("nhr_nh_list list values : ");
    i = 0;
    while(req->nhr_nh_list[i])
    {
        printf("%" PRId32 "  ", req->nhr_nh_list[i]);
        i++;
    }
    printf("\n");
    printf("nhr_label : %" PRId32 "\n", req->nhr_label);
    printf("nhr_label_list list values : ");
    i = 0;
    while(req->nhr_label_list[i])
    {
        printf("%" PRId32 "  ", req->nhr_label_list[i]);
        i++;
    }
    printf("\n");
    printf("nhr_tun_sip6 list values : ");
    i = 0;
    while(req->nhr_tun_sip6[i])
    {
        printf("%d  ", req->nhr_tun_sip6[i]);
        i++;
    }
    printf("\n");
    printf("nhr_tun_dip6 list values : ");
    i = 0;
    while(req->nhr_tun_dip6[i])
    {
        printf("%d  ", req->nhr_tun_dip6[i]);
        i++;
    }
    printf("\n");

    return;
}
void
vr_interface_req_process(void *s)
{
    vr_interface_req *req = (vr_interface_req *)s;
    printf("vr_interface_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("vifr_core : %" PRIu32 "\n", req->vifr_core);
    printf("vifr_type : %" PRId32 "\n", req->vifr_type);
    printf("vifr_flags : %" PRId32 "\n", req->vifr_flags);
    printf("vifr_vrf : %" PRId32 "\n", req->vifr_vrf);
    printf("vifr_idx : %" PRId32 "\n", req->vifr_idx);
    printf("vifr_rid : %" PRId32 "\n", req->vifr_rid);
    printf("vifr_os_idx : %" PRId32 "\n", req->vifr_os_idx);
    printf("vifr_mtu : %" PRId32 "\n", req->vifr_mtu);
    printf("vifr_name : %s", req->vifr_name);
    printf("vifr_ibytes : %" PRId64 "\n", req->vifr_ibytes);
    printf("vifr_ipackets : %" PRId64 "\n", req->vifr_ipackets);
    printf("vifr_ierrors : %" PRId64 "\n", req->vifr_ierrors);
    printf("vifr_obytes : %" PRId64 "\n", req->vifr_obytes);
    printf("vifr_opackets : %" PRId64 "\n", req->vifr_opackets);
    printf("vifr_oerrors : %" PRId64 "\n", req->vifr_oerrors);
    printf("vifr_queue_ipackets : %" PRId64 "\n", req->vifr_queue_ipackets);
    printf("vifr_queue_ierrors : %" PRId64 "\n", req->vifr_queue_ierrors);
    printf("vifr_queue_ierrors_to_lcore list values : ");
    i = 0;
    while(req->vifr_queue_ierrors_to_lcore[i])
    {
        printf("%" PRId64 "  ", req->vifr_queue_ierrors_to_lcore[i]);
        i++;
    }
    printf("\n");
    printf("vifr_queue_opackets : %" PRId64 "\n", req->vifr_queue_opackets);
    printf("vifr_queue_oerrors : %" PRId64 "\n", req->vifr_queue_oerrors);
    printf("vifr_port_ipackets : %" PRId64 "\n", req->vifr_port_ipackets);
    printf("vifr_port_ierrors : %" PRId64 "\n", req->vifr_port_ierrors);
    printf("vifr_port_isyscalls : %" PRId64 "\n", req->vifr_port_isyscalls);
    printf("vifr_port_inombufs : %" PRId64 "\n", req->vifr_port_inombufs);
    printf("vifr_port_opackets : %" PRId64 "\n", req->vifr_port_opackets);
    printf("vifr_port_oerrors : %" PRId64 "\n", req->vifr_port_oerrors);
    printf("vifr_port_osyscalls : %" PRId64 "\n", req->vifr_port_osyscalls);
    printf("vifr_dev_ibytes : %" PRId64 "\n", req->vifr_dev_ibytes);
    printf("vifr_dev_ipackets : %" PRId64 "\n", req->vifr_dev_ipackets);
    printf("vifr_dev_ierrors : %" PRId64 "\n", req->vifr_dev_ierrors);
    printf("vifr_dev_inombufs : %" PRId64 "\n", req->vifr_dev_inombufs);
    printf("vifr_dev_obytes : %" PRId64 "\n", req->vifr_dev_obytes);
    printf("vifr_dev_opackets : %" PRId64 "\n", req->vifr_dev_opackets);
    printf("vifr_dev_oerrors : %" PRId64 "\n", req->vifr_dev_oerrors);
    printf("vifr_ref_cnt : %" PRId32 "\n", req->vifr_ref_cnt);
    printf("vifr_marker : %" PRId32 "\n", req->vifr_marker);
    printf("vifr_mac list values : ");
    i = 0;
    while(req->vifr_mac[i])
    {
        printf("%d  ", req->vifr_mac[i]);
        i++;
    }
    printf("\n");
    printf("vifr_ip : %" PRIu32 "\n", req->vifr_ip);
    printf("vifr_context : %" PRId32 "\n", req->vifr_context);
    printf("vifr_speed : %" PRId32 "\n", req->vifr_speed);
    printf("vifr_duplex : %" PRId32 "\n", req->vifr_duplex);
    printf("vifr_parent_vif_idx : %" PRId32 "\n", req->vifr_parent_vif_idx);
    printf("vifr_cross_connect_idx : %" PRId32 "\n", req->vifr_cross_connect_idx);
    printf("vifr_src_mac list values : ");
    i = 0;
    while(req->vifr_src_mac[i])
    {
        printf("%d  ", req->vifr_src_mac[i]);
        i++;
    }
    printf("\n");
    printf("vifr_bridge_idx : %" PRId32 "\n", req->vifr_bridge_idx);
    printf("vifr_transport : %d\n", req->vifr_transport);
    printf("vifr_fat_flow_protocol_port list values : ");
    i = 0;
    while(req->vifr_fat_flow_protocol_port[i])
    {
        printf("%" PRId32 "  ", req->vifr_fat_flow_protocol_port[i]);
        i++;
    }
    printf("\n");

    return;
}
void
vr_vxlan_req_process(void *s)
{
    vr_vxlan_req *req = (vr_vxlan_req *)s;
    printf("vr_vxlan_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("vxlanr_vnid : %" PRId32 "\n", req->vxlanr_vnid);
    printf("vxlanr_nhid : %" PRId32 "\n", req->vxlanr_nhid);

    return;
}
void
vr_route_req_process(void *s)
{
    vr_route_req *req = (vr_route_req *)s;
    printf("vr_route_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("rtr_vrf_id : %" PRId32 "\n", req->rtr_vrf_id);
    printf("rtr_family : %" PRId32 "\n", req->rtr_family);
    printf("rtr_prefix list values : ");
    i = 0;
    while(req->rtr_prefix[i])
    {
        printf("%d  ", req->rtr_prefix[i]);
        i++;
    }
    printf("\n");
    printf("rtr_prefix_len : %" PRId32 "\n", req->rtr_prefix_len);
    printf("rtr_label : %" PRId32 "\n", req->rtr_label);
    printf("rtr_nh_id : %" PRId32 "\n", req->rtr_nh_id);
    printf("rtr_marker list values : ");
    i = 0;
    while(req->rtr_marker[i])
    {
        printf("%d  ", req->rtr_marker[i]);
        i++;
    }
    printf("\n");
    printf("rtr_marker_plen : %" PRId32 "\n", req->rtr_marker_plen);
    printf("rtr_mac list values : ");
    i = 0;
    while(req->rtr_mac[i])
    {
        printf("%d  ", req->rtr_mac[i]);
        i++;
    }
    printf("\n");
    printf("rtr_replace_plen : %" PRId32 "\n", req->rtr_replace_plen);
    printf("rtr_index : %" PRId32 "\n", req->rtr_index);

    return;
}
void
vr_mpls_req_process(void *s)
{
    vr_mpls_req *req = (vr_mpls_req *)s;
    printf("vr_mpls_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("mr_label : %" PRId32 "\n", req->mr_label);
    printf("mr_nhid : %" PRId32 "\n", req->mr_nhid);
    printf("mr_marker : %" PRId32 "\n", req->mr_marker);

    return;
}
void
vr_mirror_req_process(void *s)
{
    vr_mirror_req *req = (vr_mirror_req *)s;
    printf("vr_mirror_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("mirr_nhid : %" PRId32 "\n", req->mirr_nhid);
    printf("mirr_users : %" PRId32 "\n", req->mirr_users);
    printf("mirr_flags : %" PRId32 "\n", req->mirr_flags);
    printf("mirr_marker : %" PRId32 "\n", req->mirr_marker);

    return;
}
void
vr_flow_req_process(void *s)
{
    vr_flow_req *req = (vr_flow_req *)s;
    printf("vr_flow_req received :\n");
    int i = 0;
    printf("fr_op : %d\n", req->fr_op);
    printf("fr_index : %" PRId32 "\n", req->fr_index);
    printf("fr_ftable_size : %" PRId32 "\n", req->fr_ftable_size);
    printf("fr_rindex : %" PRId32 "\n", req->fr_rindex);
    printf("fr_family : %" PRId32 "\n", req->fr_family);
    printf("fr_flow_ip list values : ");
    i = 0;
    while(req->fr_flow_ip[i])
    {
        printf("%d  ", req->fr_flow_ip[i]);
        i++;
    }
    printf("\n");
    printf("fr_flow_proto : %d\n", req->fr_flow_proto);
    printf("fr_mir_sip : %" PRIu32 "\n", req->fr_mir_sip);
    printf("fr_pcap_meta_data list values : ");
    i = 0;
    while(req->fr_pcap_meta_data[i])
    {
        printf("%d  ", req->fr_pcap_meta_data[i]);
        i++;
    }
    printf("\n");
    printf("fr_ecmp_nh_index : %" PRId32 "\n", req->fr_ecmp_nh_index);
    printf("fr_src_nh_index : %" PRId32 "\n", req->fr_src_nh_index);
    printf("fr_flow_nh_id : %" PRId32 "\n", req->fr_flow_nh_id);
    printf("fr_file_path : %s", req->fr_file_path);
    printf("fr_processed : %" PRId64 "\n", req->fr_processed);
    printf("fr_created : %" PRId64 "\n", req->fr_created);
    printf("fr_added : %" PRId64 "\n", req->fr_added);
    printf("fr_cpus : %" PRId32 "\n", req->fr_cpus);
    printf("fr_hold_oflows : %" PRId32 "\n", req->fr_hold_oflows);
    printf("fr_hold_stat list values : ");
    i = 0;
    while(req->fr_hold_stat[i])
    {
        printf("%" PRId32 "  ", req->fr_hold_stat[i]);
        i++;
    }
    printf("\n");
    printf("fr_flow_bytes : %" PRIu32 "\n", req->fr_flow_bytes);
    printf("fr_flow_packets : %" PRIu32 "\n", req->fr_flow_packets);
    printf("fr_flow_stats_oflow : %" PRIu32 "\n", req->fr_flow_stats_oflow);
    printf("fr_oflow_entries : %" PRId32 "\n", req->fr_oflow_entries);
    printf("fr_gen_id : %d\n", req->fr_gen_id);

    return;
}
void
vr_vrf_assign_req_process(void *s)
{
    vr_vrf_assign_req *req = (vr_vrf_assign_req *)s;
    printf("vr_vrf_assign_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("var_vif_vrf : %" PRId32 "\n", req->var_vif_vrf);
    printf("var_nh_id : %" PRId32 "\n", req->var_nh_id);

    return;
}
void
vr_vrf_stats_req_process(void *s)
{
    vr_vrf_stats_req *req = (vr_vrf_stats_req *)s;
    printf("vr_vrf_stats_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("vsr_vrf : %" PRId32 "\n", req->vsr_vrf);
    printf("vsr_discards : %" PRId64 "\n", req->vsr_discards);
    printf("vsr_resolves : %" PRId64 "\n", req->vsr_resolves);
    printf("vsr_receives : %" PRId64 "\n", req->vsr_receives);
    printf("vsr_ecmp_composites : %" PRId64 "\n", req->vsr_ecmp_composites);
    printf("vsr_l2_mcast_composites : %" PRId64 "\n", req->vsr_l2_mcast_composites);
    printf("vsr_fabric_composites : %" PRId64 "\n", req->vsr_fabric_composites);
    printf("vsr_udp_tunnels : %" PRId64 "\n", req->vsr_udp_tunnels);
    printf("vsr_udp_mpls_tunnels : %" PRId64 "\n", req->vsr_udp_mpls_tunnels);
    printf("vsr_gre_mpls_tunnels : %" PRId64 "\n", req->vsr_gre_mpls_tunnels);
    printf("vsr_l2_encaps : %" PRId64 "\n", req->vsr_l2_encaps);
    printf("vsr_encaps : %" PRId64 "\n", req->vsr_encaps);
    printf("vsr_gros : %" PRId64 "\n", req->vsr_gros);
    printf("vsr_diags : %" PRId64 "\n", req->vsr_diags);
    printf("vsr_encap_composites : %" PRId64 "\n", req->vsr_encap_composites);
    printf("vsr_evpn_composites : %" PRId64 "\n", req->vsr_evpn_composites);
    printf("vsr_vrf_translates : %" PRId64 "\n", req->vsr_vrf_translates);
    printf("vsr_vxlan_tunnels : %" PRId64 "\n", req->vsr_vxlan_tunnels);
    printf("vsr_arp_virtual_proxy : %" PRId64 "\n", req->vsr_arp_virtual_proxy);
    printf("vsr_arp_virtual_stitch : %" PRId64 "\n", req->vsr_arp_virtual_stitch);
    printf("vsr_arp_virtual_flood : %" PRId64 "\n", req->vsr_arp_virtual_flood);
    printf("vsr_arp_physical_stitch : %" PRId64 "\n", req->vsr_arp_physical_stitch);
    printf("vsr_arp_tor_proxy : %" PRId64 "\n", req->vsr_arp_tor_proxy);
    printf("vsr_arp_physical_flood : %" PRId64 "\n", req->vsr_arp_physical_flood);
    printf("vsr_l2_receives : %" PRId64 "\n", req->vsr_l2_receives);
    printf("vsr_uuc_floods : %" PRId64 "\n", req->vsr_uuc_floods);

    return;
}
void
vr_response_process(void *s)
{
    vr_response *req = (vr_response *)s;
    printf("vr_response received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("resp_code : %" PRId32 "\n", req->resp_code);

    return;
}
void
vrouter_ops_process(void *s)
{
    vrouter_ops *req = (vrouter_ops *)s;
    printf("vrouter_ops received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("vo_rid : %" PRId32 "\n", req->vo_rid);
    printf("vo_mpls_labels : %" PRId32 "\n", req->vo_mpls_labels);
    printf("vo_nexthops : %" PRId32 "\n", req->vo_nexthops);
    printf("vo_bridge_entries : %" PRId32 "\n", req->vo_bridge_entries);
    printf("vo_oflow_bridge_entries : %" PRId32 "\n", req->vo_oflow_bridge_entries);
    printf("vo_flow_entries : %" PRId32 "\n", req->vo_flow_entries);
    printf("vo_oflow_entries : %" PRId32 "\n", req->vo_oflow_entries);
    printf("vo_interfaces : %" PRId32 "\n", req->vo_interfaces);
    printf("vo_mirror_entries : %" PRId32 "\n", req->vo_mirror_entries);
    printf("vo_vrfs : %" PRId32 "\n", req->vo_vrfs);
    printf("vo_build_info : %s", req->vo_build_info);
    printf("vo_log_level : %" PRIu32 "\n", req->vo_log_level);
    printf("vo_log_type_enable list values : ");
    i = 0;
    while(req->vo_log_type_enable[i])
    {
        printf("%" PRId32 "  ", req->vo_log_type_enable[i]);
        i++;
    }
    printf("\n");
    printf("vo_log_type_disable list values : ");
    i = 0;
    while(req->vo_log_type_disable[i])
    {
        printf("%" PRId32 "  ", req->vo_log_type_disable[i]);
        i++;
    }
    printf("\n");
    printf("vo_perfr : %" PRId32 "\n", req->vo_perfr);
    printf("vo_perfs : %" PRId32 "\n", req->vo_perfs);
    printf("vo_from_vm_mss_adj : %" PRId32 "\n", req->vo_from_vm_mss_adj);
    printf("vo_to_vm_mss_adj : %" PRId32 "\n", req->vo_to_vm_mss_adj);
    printf("vo_perfr1 : %" PRId32 "\n", req->vo_perfr1);
    printf("vo_perfr2 : %" PRId32 "\n", req->vo_perfr2);
    printf("vo_perfr3 : %" PRId32 "\n", req->vo_perfr3);
    printf("vo_perfp : %" PRId32 "\n", req->vo_perfp);
    printf("vo_perfq1 : %" PRId32 "\n", req->vo_perfq1);
    printf("vo_perfq2 : %" PRId32 "\n", req->vo_perfq2);
    printf("vo_perfq3 : %" PRId32 "\n", req->vo_perfq3);
    printf("vo_udp_coff : %" PRId32 "\n", req->vo_udp_coff);
    printf("vo_flow_hold_limit : %" PRId32 "\n", req->vo_flow_hold_limit);
    printf("vo_mudp : %" PRId32 "\n", req->vo_mudp);
    printf("vo_flow_used_entries : %" PRIu32 "\n", req->vo_flow_used_entries);
    printf("vo_flow_used_oentries : %" PRIu32 "\n", req->vo_flow_used_oentries);
    printf("vo_bridge_used_entries : %" PRIu32 "\n", req->vo_bridge_used_entries);
    printf("vo_bridge_used_oentries : %" PRIu32 "\n", req->vo_bridge_used_oentries);

    return;
}
void
vr_mem_stats_req_process(void *s)
{
    vr_mem_stats_req *req = (vr_mem_stats_req *)s;
    printf("vr_mem_stats_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("vms_alloced : %" PRId64 "\n", req->vms_alloced);
    printf("vms_freed : %" PRId64 "\n", req->vms_freed);
    printf("vms_assembler_table_object : %" PRId64 "\n", req->vms_assembler_table_object);
    printf("vms_bridge_mac_object : %" PRId64 "\n", req->vms_bridge_mac_object);
    printf("vms_btable_object : %" PRId64 "\n", req->vms_btable_object);
    printf("vms_build_info_object : %" PRId64 "\n", req->vms_build_info_object);
    printf("vms_defer_object : %" PRId64 "\n", req->vms_defer_object);
    printf("vms_drop_stats_object : %" PRId64 "\n", req->vms_drop_stats_object);
    printf("vms_drop_stats_req_object : %" PRId64 "\n", req->vms_drop_stats_req_object);
    printf("vms_flow_queue_object : %" PRId64 "\n", req->vms_flow_queue_object);
    printf("vms_flow_req_object : %" PRId64 "\n", req->vms_flow_req_object);
    printf("vms_flow_req_path_object : %" PRId64 "\n", req->vms_flow_req_path_object);
    printf("vms_flow_hold_stat_object : %" PRId64 "\n", req->vms_flow_hold_stat_object);
    printf("vms_flow_link_local_object : %" PRId64 "\n", req->vms_flow_link_local_object);
    printf("vms_flow_metadata_object : %" PRId64 "\n", req->vms_flow_metadata_object);
    printf("vms_flow_table_info_object : %" PRId64 "\n", req->vms_flow_table_info_object);
    printf("vms_fragment_object : %" PRId64 "\n", req->vms_fragment_object);
    printf("vms_fragment_queue_object : %" PRId64 "\n", req->vms_fragment_queue_object);
    printf("vms_fragment_queue_element_object : %" PRId64 "\n", req->vms_fragment_queue_element_object);
    printf("vms_fragment_scanner_object : %" PRId64 "\n", req->vms_fragment_scanner_object);
    printf("vms_hpacket_pool_object : %" PRId64 "\n", req->vms_hpacket_pool_object);
    printf("vms_htable_object : %" PRId64 "\n", req->vms_htable_object);
    printf("vms_interface_object : %" PRId64 "\n", req->vms_interface_object);
    printf("vms_interface_mac_object : %" PRId64 "\n", req->vms_interface_mac_object);
    printf("vms_interface_req_object : %" PRId64 "\n", req->vms_interface_req_object);
    printf("vms_interface_req_mac_object : %" PRId64 "\n", req->vms_interface_req_mac_object);
    printf("vms_interface_req_name_object : %" PRId64 "\n", req->vms_interface_req_name_object);
    printf("vms_interface_stats_object : %" PRId64 "\n", req->vms_interface_stats_object);
    printf("vms_interface_table_object : %" PRId64 "\n", req->vms_interface_table_object);
    printf("vms_interface_vrf_table_object : %" PRId64 "\n", req->vms_interface_vrf_table_object);
    printf("vms_itable_object : %" PRId64 "\n", req->vms_itable_object);
    printf("vms_malloc_object : %" PRId64 "\n", req->vms_malloc_object);
    printf("vms_message_object : %" PRId64 "\n", req->vms_message_object);
    printf("vms_message_response_object : %" PRId64 "\n", req->vms_message_response_object);
    printf("vms_message_dump_object : %" PRId64 "\n", req->vms_message_dump_object);
    printf("vms_mem_stats_req_object : %" PRId64 "\n", req->vms_mem_stats_req_object);
    printf("vms_mirror_object : %" PRId64 "\n", req->vms_mirror_object);
    printf("vms_mirror_table_object : %" PRId64 "\n", req->vms_mirror_table_object);
    printf("vms_mirror_meta_object : %" PRId64 "\n", req->vms_mirror_meta_object);
    printf("vms_mtrie_object : %" PRId64 "\n", req->vms_mtrie_object);
    printf("vms_mtrie_bucket_object : %" PRId64 "\n", req->vms_mtrie_bucket_object);
    printf("vms_mtrie_stats_object : %" PRId64 "\n", req->vms_mtrie_stats_object);
    printf("vms_mtrie_table_object : %" PRId64 "\n", req->vms_mtrie_table_object);
    printf("vms_network_address_object : %" PRId64 "\n", req->vms_network_address_object);
    printf("vms_nexthop_object : %" PRId64 "\n", req->vms_nexthop_object);
    printf("vms_nexthop_component_object : %" PRId64 "\n", req->vms_nexthop_component_object);
    printf("vms_nexthop_req_list_object : %" PRId64 "\n", req->vms_nexthop_req_list_object);
    printf("vms_nexthop_req_encap_object : %" PRId64 "\n", req->vms_nexthop_req_encap_object);
    printf("vms_nexthop_req_object : %" PRId64 "\n", req->vms_nexthop_req_object);
    printf("vms_route_table_object : %" PRId64 "\n", req->vms_route_table_object);
    printf("vms_route_req_mac_object : %" PRId64 "\n", req->vms_route_req_mac_object);
    printf("vms_timer_object : %" PRId64 "\n", req->vms_timer_object);
    printf("vms_usock_object : %" PRId64 "\n", req->vms_usock_object);
    printf("vms_usock_poll_object : %" PRId64 "\n", req->vms_usock_poll_object);
    printf("vms_usock_buf_object : %" PRId64 "\n", req->vms_usock_buf_object);
    printf("vms_usock_iovec_object : %" PRId64 "\n", req->vms_usock_iovec_object);
    printf("vms_vrouter_req_object : %" PRId64 "\n", req->vms_vrouter_req_object);
    printf("vms_interface_fat_flow_config_object : %" PRId64 "\n", req->vms_interface_fat_flow_config_object);
    printf("vms_qos_map_object : %" PRId64 "\n", req->vms_qos_map_object);
    printf("vms_fc_object : %" PRId64 "\n", req->vms_fc_object);

    return;
}
void
vr_drop_stats_req_process(void *s)
{
    vr_drop_stats_req *req = (vr_drop_stats_req *)s;
    printf("vr_drop_stats_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("vds_core : %" PRIu32 "\n", req->vds_core);
    printf("vds_discard : %" PRId64 "\n", req->vds_discard);
    printf("vds_pull : %" PRId64 "\n", req->vds_pull);
    printf("vds_invalid_if : %" PRId64 "\n", req->vds_invalid_if);
    printf("vds_arp_no_where_to_go : %" PRId64 "\n", req->vds_arp_no_where_to_go);
    printf("vds_garp_from_vm : %" PRId64 "\n", req->vds_garp_from_vm);
    printf("vds_invalid_arp : %" PRId64 "\n", req->vds_invalid_arp);
    printf("vds_trap_no_if : %" PRId64 "\n", req->vds_trap_no_if);
    printf("vds_nowhere_to_go : %" PRId64 "\n", req->vds_nowhere_to_go);
    printf("vds_flow_queue_limit_exceeded : %" PRId64 "\n", req->vds_flow_queue_limit_exceeded);
    printf("vds_flow_no_memory : %" PRId64 "\n", req->vds_flow_no_memory);
    printf("vds_flow_invalid_protocol : %" PRId64 "\n", req->vds_flow_invalid_protocol);
    printf("vds_flow_nat_no_rflow : %" PRId64 "\n", req->vds_flow_nat_no_rflow);
    printf("vds_flow_action_drop : %" PRId64 "\n", req->vds_flow_action_drop);
    printf("vds_flow_action_invalid : %" PRId64 "\n", req->vds_flow_action_invalid);
    printf("vds_flow_unusable : %" PRId64 "\n", req->vds_flow_unusable);
    printf("vds_flow_table_full : %" PRId64 "\n", req->vds_flow_table_full);
    printf("vds_interface_tx_discard : %" PRId64 "\n", req->vds_interface_tx_discard);
    printf("vds_interface_drop : %" PRId64 "\n", req->vds_interface_drop);
    printf("vds_duplicated : %" PRId64 "\n", req->vds_duplicated);
    printf("vds_push : %" PRId64 "\n", req->vds_push);
    printf("vds_ttl_exceeded : %" PRId64 "\n", req->vds_ttl_exceeded);
    printf("vds_invalid_nh : %" PRId64 "\n", req->vds_invalid_nh);
    printf("vds_invalid_label : %" PRId64 "\n", req->vds_invalid_label);
    printf("vds_invalid_protocol : %" PRId64 "\n", req->vds_invalid_protocol);
    printf("vds_interface_rx_discard : %" PRId64 "\n", req->vds_interface_rx_discard);
    printf("vds_invalid_mcast_source : %" PRId64 "\n", req->vds_invalid_mcast_source);
    printf("vds_head_alloc_fail : %" PRId64 "\n", req->vds_head_alloc_fail);
    printf("vds_head_space_reserve_fail : %" PRId64 "\n", req->vds_head_space_reserve_fail);
    printf("vds_pcow_fail : %" PRId64 "\n", req->vds_pcow_fail);
    printf("vds_flood : %" PRId64 "\n", req->vds_flood);
    printf("vds_mcast_clone_fail : %" PRId64 "\n", req->vds_mcast_clone_fail);
    printf("vds_no_memory : %" PRId64 "\n", req->vds_no_memory);
    printf("vds_rewrite_fail : %" PRId64 "\n", req->vds_rewrite_fail);
    printf("vds_misc : %" PRId64 "\n", req->vds_misc);
    printf("vds_invalid_packet : %" PRId64 "\n", req->vds_invalid_packet);
    printf("vds_cksum_err : %" PRId64 "\n", req->vds_cksum_err);
    printf("vds_clone_fail : %" PRId64 "\n", req->vds_clone_fail);
    printf("vds_no_fmd : %" PRId64 "\n", req->vds_no_fmd);
    printf("vds_cloned_original : %" PRId64 "\n", req->vds_cloned_original);
    printf("vds_invalid_vnid : %" PRId64 "\n", req->vds_invalid_vnid);
    printf("vds_frag_err : %" PRId64 "\n", req->vds_frag_err);
    printf("vds_invalid_source : %" PRId64 "\n", req->vds_invalid_source);
    printf("vds_mcast_df_bit : %" PRId64 "\n", req->vds_mcast_df_bit);
    printf("vds_arp_no_route : %" PRId64 "\n", req->vds_arp_no_route);
    printf("vds_l2_no_route : %" PRId64 "\n", req->vds_l2_no_route);
    printf("vds_fragment_queue_fail : %" PRId64 "\n", req->vds_fragment_queue_fail);
    printf("vds_vlan_fwd_tx : %" PRId64 "\n", req->vds_vlan_fwd_tx);
    printf("vds_vlan_fwd_enq : %" PRId64 "\n", req->vds_vlan_fwd_enq);
    printf("vds_drop_new_flow : %" PRId64 "\n", req->vds_drop_new_flow);

    return;
}
void
vr_qos_map_req_process(void *s)
{
    vr_qos_map_req *req = (vr_qos_map_req *)s;
    printf("vr_qos_map_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("qmr_dscp list values : ");
    i = 0;
    while(req->qmr_dscp[i])
    {
        printf("%d  ", req->qmr_dscp[i]);
        i++;
    }
    printf("\n");
    printf("qmr_dscp_fc_id list values : ");
    i = 0;
    while(req->qmr_dscp_fc_id[i])
    {
        printf("%d  ", req->qmr_dscp_fc_id[i]);
        i++;
    }
    printf("\n");
    printf("qmr_mpls_qos list values : ");
    i = 0;
    while(req->qmr_mpls_qos[i])
    {
        printf("%d  ", req->qmr_mpls_qos[i]);
        i++;
    }
    printf("\n");
    printf("qmr_mpls_qos_fc_id list values : ");
    i = 0;
    while(req->qmr_mpls_qos_fc_id[i])
    {
        printf("%d  ", req->qmr_mpls_qos_fc_id[i]);
        i++;
    }
    printf("\n");
    printf("qmr_dotonep list values : ");
    i = 0;
    while(req->qmr_dotonep[i])
    {
        printf("%d  ", req->qmr_dotonep[i]);
        i++;
    }
    printf("\n");
    printf("qmr_dotonep_fc_id list values : ");
    i = 0;
    while(req->qmr_dotonep_fc_id[i])
    {
        printf("%d  ", req->qmr_dotonep_fc_id[i]);
        i++;
    }
    printf("\n");

    return;
}
void
vr_fc_map_req_process(void *s)
{
    vr_fc_map_req *req = (vr_fc_map_req *)s;
    printf("vr_fc_map_req received :\n");
    int i = 0;
    printf("h_op : %d\n", req->h_op);
    printf("fmr_id list values : ");
    i = 0;
    while(req->fmr_id[i])
    {
        printf("%" PRId16 "  ", req->fmr_id[i]);
        i++;
    }
    printf("\n");
    printf("fmr_dscp list values : ");
    i = 0;
    while(req->fmr_dscp[i])
    {
        printf("%d  ", req->fmr_dscp[i]);
        i++;
    }
    printf("\n");
    printf("fmr_mpls_qos list values : ");
    i = 0;
    while(req->fmr_mpls_qos[i])
    {
        printf("%d  ", req->fmr_mpls_qos[i]);
        i++;
    }
    printf("\n");
    printf("fmr_dotonep list values : ");
    i = 0;
    while(req->fmr_dotonep[i])
    {
        printf("%d  ", req->fmr_dotonep[i]);
        i++;
    }
    printf("\n");
    printf("fmr_queue_id list values : ");
    i = 0;
    while(req->fmr_queue_id[i])
    {
        printf("%d  ", req->fmr_queue_id[i]);
        i++;
    }
    printf("\n");

    return;
}
void reader_nl()
{
    int ret;
    ret = nlmon_recvmsg_decoded(cl,true);
    if (ret < 0)
        printf("ret < 0.\n");
    else
    {
        printf("ret = %d\n", ret);
    }
}
void signal_handler(int dummy)
{
    reader = false;
    nl_free_client(cl);
    printf("\nInterrupted Signal\nClient freed.\n");
    signal(SIGINT, signal_handler);
}
int main(int argc, char *argv[]) {
    int group = GROUP_ID;

    int opt, option_index;
    signal(SIGINT, signal_handler);
    cl = nlmon_connect(group);
    printf("connected..\n");
    while(reader)
    {
        reader_nl();
    }
    return 0;
}
