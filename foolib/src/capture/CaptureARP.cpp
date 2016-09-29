/*
 * CaptureARP.cpp
 *
 *  Created on: 2016. 9. 23.
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
#include <stdint.h>
#include <linux/filter.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <getopt.h>

#include "WlanUtils.h"
#include "CaptureARP.h"

namespace kinow {

} /* namespace kinow */


using namespace kinow;

void help()
{
    printf("Usage: ./CaptureARP [OPTION]\n"
           "  -h                help\n"
           "  -r                recv mode\n"
           "  -s                send mode\n"
           "  -i                NIC\n");
    exit(0);
}


int main(int argc, char **argv) {
	uint8_t mymac[6];
	uint32_t myip;
	struct in_addr addr;
	int opt;
	char optMode;
	char *optIfname = NULL;

	while ((opt = getopt(argc, argv, "hsri:")) != -1) {
		switch (opt) {
		case 's':
			optMode = opt;
			break;
		case 'r':
			optMode = opt;
			break;
		case 'i':
			optIfname = strdup(optarg);
			break;
		case 'h':
			help();
			break;
		default:
			help();
			break;
		}
	}

	if (optIfname == NULL) {
		help();
		exit(1);
	}

	switch (optMode) {
		case 's':
		{
			WlanUtils::getIpAddress(optIfname, &myip);
			WlanUtils::getMacAddress(optIfname, mymac);
			inet_aton("10.10.200.187", &addr);
			uint32_t announce_ip = addr.s_addr;
			printf("announce_ip=%u\n", announce_ip);
			uint8_t announce_mac[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
			ARPFilterMember::sendAnnounceARP(optIfname,
					announce_ip, announce_mac,
					myip, mymac);
		}
		break;

		case 'r':
		{
			CaptureMember *capture = new CaptureARP("ARP CAPTURE");
			IFilterMember *arpFilterMember = new ARPFilterMember("ARP", optIfname, "arp");
			capture->addFilterMember(arpFilterMember);
			capture->capture();

			delete capture;
		}
		break;

		default:
			help();
			break;
	}

	free(optIfname);
}
