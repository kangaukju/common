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
#include "Options.h"

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
	Options options(argc, argv);
	char errbuf[64];
	uint8_t mymac[6];
	uint32_t myip;
	struct in_addr addr;
	const char *mode = NULL;
	const char *ifname = NULL;

	options.addOption('m', "mode[s: send, r: recv]", Options::REQUIRE_OPT|Options::REQUIRE_OPT_VALUE);
	options.addOption('i', "interface", Options::REQUIRE_OPT|Options::REQUIRE_OPT_VALUE);
	options.addOptionHelp();

	if (options.validOptions(errbuf, sizeof(errbuf)) == false) {
		fprintf(stderr, "%s\n", errbuf);
		exit(1);
	}

	ifname = options.optionValue('i');
	mode = options.optionValue('m');
	if (mode[0] == 's') {
		WlanUtils::getIpAddress(ifname, &myip);
		WlanUtils::getMacAddress(ifname, mymac);
		inet_aton("10.10.200.187", &addr);
		uint32_t announce_ip = addr.s_addr;
		printf("announce_ip=%u\n", announce_ip);
		uint8_t announce_mac[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
		ARPFilterMember::sendAnnounceARP(ifname,
				announce_ip, announce_mac,
				myip, mymac);
	} else
	if (mode[0] == 'r') {
		CaptureMember *capture = new CaptureARP("ARP CAPTURE");
		IFilterMember *arpFilterMember = new ARPFilterMember("ARP", ifname, "arp");
		capture->addFilterMember(arpFilterMember);
		capture->capture();
		delete capture;
	}
}
