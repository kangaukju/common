/*
 * CaptureARP.h
 *
 *  Created on: 2016. 9. 23.
 *      Author: root
 */

#ifndef CAPTURE_CAPTUREARP_H_
#define CAPTURE_CAPTUREARP_H_

#include <sys/prctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/types.h>
#include <stdint.h>
#include <linux/if_arp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>    /* struct sockaddr_ll */
#include <linux/wireless.h>     /* srtuct iwreq */
#include <errno.h>
#include <sys/types.h>
#include <linux/filter.h>
#include <fcntl.h>

namespace kinow {

//////////////////////////////////////////////
// 802.1Q header
//////////////////////////////////////////////
/**
 * struct vlan_ethhdr - vlan ethernet header (ethhdr + vlan_hdr)
 * @h_dest: destination ethernet address
 * @h_source: source ethernet address
 * @h_vlan_proto: ethernet protocol (always 0x8100)
 * @h_vlan_tag: priority and VLAN ID
 * @h_vlan_encapsulated_proto: packet type ID or len
 */
struct vlan_ethhdr {
    uint8_t h_dest[ETH_ALEN];
    uint8_t h_source[ETH_ALEN];
    uint16_t h_vlan_proto;
    uint16_t h_vlan_tag;
    uint16_t h_proto;
};

struct arp_message
{
    struct ethhdr ethhdr; /* Ethernet header */
    uint16_t htype; /* hardware type (must be ARPHRD_ETHER) */
    uint16_t ptype; /* protocol type (must be ETH_P_IP) */
    uint8_t hlen; /* hardware address length (must be 6) */
    uint8_t plen; /* protocol address length (must be 4) */
    uint16_t operation; /* ARP opcode */
    uint8_t sHaddr[6]; /* sender's hardware address */
    uint8_t sInaddr[4]; /* sender's IP address */
    uint8_t tHaddr[6]; /* target's hardware address */
    uint8_t tInaddr[4]; /* target's IP address */
    uint8_t pad[18]; /* pad for min. Ethernet payload (60 bytes) */
};

struct dot1q_arp_message
{
    struct vlan_ethhdr ethhdr; /* Ethernet VLAN header */
    uint16_t htype; /* hardware type (must be ARPHRD_ETHER) */
    uint16_t ptype; /* protocol type (must be ETH_P_IP) */
    uint8_t hlen; /* hardware address length (must be 6) */
    uint8_t plen; /* protocol address length (must be 4) */
    uint16_t operation; /* ARP opcode */
    uint8_t sHaddr[6]; /* sender's hardware address */
    uint8_t sInaddr[4]; /* sender's IP address */
    uint8_t tHaddr[6]; /* target's hardware address */
    uint8_t tInaddr[4]; /* target's IP address */
    uint8_t pad[14]; /* pad for min. Ethernet payload (60 bytes) */
};


class CaptureARP : public CaptureMember {
public:
	CaptureARP(const char* name) : m_name(name) { }
	virtual ~CaptureARP();
};

} /* namespace kinow */

#endif /* CAPTURE_CAPTUREARP_H_ */
