/*
 * WlanUtils.cpp
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#include "WlanUtils.h"
#include <sys/socket.h>
#include <linux/types.h>
#include <stdint.h>
#include <linux/if_arp.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>    /* struct sockaddr_ll */
#include <linux/wireless.h>     /* srtuct iwreq */
#include <linux/if_ether.h>
#include <errno.h>
#include <linux/if.h>           /* struct ifr */
#include <linux/if_packet.h>    /* struct sockaddr_ll */
#include <linux/wireless.h>     /* srtuct iwreq */


namespace kinow {

int IEEE80211Freq[][2] = {
	{1, 2412},
	{2, 2417},
	{3, 2422},
	{4, 2427},
	{5, 2432},
	{6, 2437},
	{7, 2442},
	{8, 2447},
	{9, 2452},
	{10, 2457},
	{11, 2462},
	{12, 2467},
	{13, 2472},
	{14, 2484},
	// We could do the math here, but what about 4ghz nonsense?
	// We'll do table lookups for now.
	{36, 5180},
	{37, 5185},
	{38, 5190},
	{39, 5195},
	{40, 5200},
	{41, 5205},
	{42, 5210},
	{43, 5215},
	{44, 5220},
	{45, 5225},
	{46, 5230},
	{47, 5235},
	{48, 5240},
	{52, 5260},
	{53, 5265},
	{54, 5270},
	{55, 5275},
	{56, 5280},
	{57, 5285},
	{58, 5290},
	{59, 5295},
	{60, 5300},
	{64, 5320},
	/* UNET Channel Add by park 2011.05.13 */
	{100, 5500},
	{104, 5520},
	{108, 5540},
	{112, 5560},
	{116, 5580},
	{120, 5600},
	{124, 5620},
	{128, 5640},
	{132, 5660},
	{136, 5680},
	{140, 5700},
	/* End */
	{149, 5745},
	{150, 5750},
	{152, 5760},
	{153, 5765},
	{157, 5785},
	{160, 5800},
	{161, 5805},
	{165, 5825},
	{0, 0}
};

int WlanUtils::getDLT(const char* ifname) {
	int dlt;
	struct ifreq ifr;
	int arp_type;
	int sockfd;

	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd < 0) {
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", ifname);
	if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
		close(sockfd);
		return DLT_UNKOWN;
	}

	arp_type = ifr.ifr_hwaddr.sa_family;
	switch (arp_type) {
#ifdef ARPHRD_IEEE80211
	case ARPHRD_IEEE80211:
		dlt = DLT_RADIO;
		break;
#endif

#ifdef ARPHRD_IEEE80211_PRISM
	case ARPHRD_IEEE80211_PRISM:
		dlt = DLT_PRISM;
		break;
#endif

#ifdef ARPHRD_IEEE80211_RADIOTAP
	case ARPHRD_IEEE80211_RADIOTAP:
		dlt = DLT_RADIOTAP;
		break;
#endif
	default:
		dlt = DLT_UNKOWN;
	}

	close(sockfd);
	return dlt;
}

int WlanUtils::channelToFreq(int freq) {
#if 0
	int x = 0;
	// 80211b frequencies to channels

	while (IEEE80211Freq[x][0] != 0) {
		if (IEEE80211Freq[x][0] == freq) {
			return IEEE80211Freq[x][1];
		}
		x++;
	}
	return freq;
#else
	if (freq < 5000) {
		return ((freq - 2412) / 5) + 1;
	} else {
		return ((freq - 5180) / 5) + 36;
	}
#endif
}

} /* namespace kinow */
