/*
 * ARPFilterMember.cpp
 *
 *  Created on: 2016. 9. 26.
 *      Author: root
 */

#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdint.h>
#include <linux/filter.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "WlanUtils.h"
#include "ARPFilterMember.h"

namespace kinow {

void ARPFilterMember::filterHandler(uint8_t* buf, size_t len) {
	struct ethhdr *ether = NULL;
	struct arp_message *p_arp;
	struct dot1q_arp_message *p1q_arp;
	struct arpinfo arpinfo;

	ether = (struct ethhdr *)buf;

	if (_IS_ZERO_MAC_(ether->h_source) || (ether->h_source[0] & 0x01)) {
		return;
	}

	memset(&arpinfo, 0, sizeof(arpinfo));

	// 802.1Q (vlan)
	if (ether->h_proto == htons(ETH_P_8021Q)) {

	}
	// 802.3 - Ethernet (ARP)
	else if (ether->h_proto == htons(ETH_P_ARP)) {
		p_arp = (struct arpMsg *)buf;

		// ARP_REPLY
		if (p_arp->operation == htons(ARPOP_REPLY)) {
			_MAC_COPY_(arpinfo.mac, p_arp->sHaddr);

			recv_arp.ip = ntohl(*((UINT32 *)p_arp->sInaddr));

            if (recv_arp.ip == 0 || (int)recv_arp.ip == -1) {
                continue;
            }

		}
		// ARP_REQUEST
		else if (p_arp->operation == htons(ARPOP_REQUEST)) {

		}
		else {
			return;
		}
	}

}

} /* namespace kinow */
