/*
 * FilterMember.cpp
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#include "FilterMember.h"
#include "ieee80211_prism.h"
#include "ieee80211_radiotap.h"
#include "ieee80211_frame.h"
#include "WlanUtils.h"
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


namespace kinow {

struct sock_filter_st {
	uint32_t count;
	struct sock_filter* filters;
};

void debugHex(const char* title, uint8_t *buf, int len) {
	int i;
	printf("%s (%d)\n", title, len);
	for (i=1; i<=len; i++) {
		printf("%02x ", buf[i-1]);
		if (i%8 == 0)  printf(" ");
		if (i%16 == 0) printf("\n");
	}
	printf("\n");
}

void socketFilterFree(struct sock_filter_st *sf) {
	if (sf) {
		if (sf->filters) {
			free(sf->filters);
		}
	}
}

struct sock_filter_st* socketFilterFactory(const char *ifname, const char *filterExpression) {
	FILE *fp;
	char buf[256];
	char tcpdumpCommand[256];
	char *tok;
	char *last;
	int filterCount = 0;
	struct sock_filter *filters;
	int line = 0;
	struct sock_filter_st* sf;

	sf = (struct sock_filter_st*)malloc(sizeof(struct sock_filter_st));
	memset(sf, 0, sizeof(struct sock_filter_st));

	snprintf(tcpdumpCommand, sizeof(tcpdumpCommand),
			"/usr/sbin/tcpdump -i %s \"%s\" -ddd", ifname, filterExpression);

	printf("%s\n", tcpdumpCommand);

	fp = popen(tcpdumpCommand, "r");
	if (fp == NULL) {
		free(sf);
		return NULL;
	}

	if (fgets(buf, sizeof(buf), fp)) {
		// BPF filter count
		sscanf(buf, "%d", &filterCount);

		if (filterCount == 0) {
			free(sf);
			pclose(fp);
			return NULL;
		}

		filters = (struct sock_filter*)malloc(sizeof(struct sock_filter) * filterCount);

		while (fgets(buf, sizeof(buf), fp) != NULL && (line < filterCount)) {
			// BPF Filter block code
			// Actual filter code
			if ((tok = strtok_r(buf, " ", &last))) {
				sscanf(tok, "%hd", (__u16*)&filters[line].code);
			}
			// Jump true
			if ((tok = strtok_r(NULL, " ", &last))) {
				sscanf(tok, "%u", (__u32*)&filters[line].jt);
			}
			// Jump false
			if ((tok = strtok_r(NULL, " ", &last))) {
				sscanf(tok, "%u", (__u32*)&filters[line].jf);
			}
			// Generic multiuse field
			if ((tok = strtok_r(NULL, " ", &last))) {
				sscanf(tok, "%d", (__u32*)&filters[line].k);
			}
			line++;
		}
		sf->count = filterCount;
		sf->filters = filters;
		pclose(fp);
		return sf;
	}

	free(sf);
	pclose(fp);
	return NULL;
}

void socketFilterDebug(struct sock_filter_st* sf) {
	int i = 0;
	int count = 0;

	if (sf) {
		count = sf->count;
		fprintf(stdout, "%d\n", count);
		for (i=0; i<count; i++) {
			fprintf(stdout, "{ 0x%x, %d, %d, 0x%08x },\n",
					sf->filters[i].code, sf->filters[i].jt,
					sf->filters[i].jf, sf->filters[i].k);
		}
	}
}

FilterMember::FilterMember(const char* name, const char *ifname, const char *filterExpression) {
	m_name = NULL;
	m_ifname = NULL;
	m_filterExpression = NULL;
	m_sock = -1;

	if (name) m_name = strdup(name);
	if (ifname) m_ifname = strdup(ifname);
	if (filterExpression) m_filterExpression = strdup(filterExpression);
}

FilterMember::~FilterMember() {
	if (m_name) free(m_name);
	if (m_ifname) free(m_ifname);
	if (m_filterExpression) free(m_filterExpression);
	if (m_sock != -1) close(m_sock);
}

bool FilterMember::initialize() {
	m_sock = createFilterSocket();
	if (m_sock == -1) {
		return false;
	}
	return true;
}

int FilterMember::createFilterSocket() {
	int sock = -1;
	bool ok = false;
	struct sock_fprog bpf;
	struct ifreq ifopts;
	int sockopt = 1;
	struct sock_filter_st *sf = NULL;

	sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock == -1) {
		failure("FilterMember[%s]: Failed to sock(): %s", m_name, strerror(errno));
		goto out;
	}
	if (m_filterExpression) {
		sf = socketFilterFactory(m_ifname, m_filterExpression);
		if (sf == NULL) {
			failure("FilterMember[%s]: Failed to get filter code: %s", m_name, m_filterExpression);
			goto out;
		}

		socketFilterDebug(sf);

		bpf.len = sf->count;
		bpf.filter = sf->filters;
		if(setsockopt(sock, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf)) < 0) {
			failure("FilterMember[%s]: Failed to setsockopt(SO_ATTACH_FILTER): %s", m_name, strerror(errno));
			goto out;
		}
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		failure("FilterMember[%s]: Failed to setsockopt(SO_REUSEADDR): %s", m_name, strerror(errno));
		goto out;
	}

	if (m_ifname) {
		strncpy(ifopts.ifr_name, m_ifname, IFNAMSIZ-1);
		ioctl(sock, SIOCGIFFLAGS, &ifopts);
		ifopts.ifr_flags |= IFF_PROMISC;
		ioctl(sock, SIOCSIFFLAGS, &ifopts);

		if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, m_ifname, IFNAMSIZ-1) == -1)	{
			failure("FilterMember[%s]: Failed to setsockopt(SO_BINDTODEVICE): %s", m_name, strerror(errno));
			goto out;
		}
	}

	bzero(&m_sockaddr_ll, sizeof(m_sockaddr_ll));
	if ((ioctl(sock, SIOCGIFINDEX, &ifopts)) == -1) {
		failure("FilterMember[%s]: Failed to getsockopt(SIOCGIFINDEX): %s", m_name, strerror(errno));
		goto out;
	}
    m_sockaddr_ll.sll_family    = AF_PACKET;
    m_sockaddr_ll.sll_ifindex   = ifopts.ifr_ifindex;
    m_sockaddr_ll.sll_protocol  = htons(ETH_P_ALL);

    if (bind(sock, (struct sockaddr *)&m_sockaddr_ll, sizeof(m_sockaddr_ll)) == -1) {
		failure("FilterMember[%s]: Failed to bin(): %s", m_name, strerror(errno));
		goto out;
    }

	ok = true;

out:
	if (sf) socketFilterFree(sf);
	if (ok == false) {
		if (sock != -1) {
			close(sock);
			sock = -1;
		}
	}
	return sock;
}

bool parsingWirelessRadiotap(u_char* buf, size_t len, FilterMember *fm) {
	struct ieee80211_radiotap_header *pRadioHdr;
	uint8_t channel;
	uint16_t channelFreq;
	int8_t signal;
	int8_t noise;
	uint8_t rate;
	int frameLength;
	int fcslength = 0;
	bool debugFlag = false;

	pRadioHdr = (struct ieee80211_radiotap_header *) buf;

	if (len < (int) sizeof(*pRadioHdr)) {
		return false;
	}
	if (pRadioHdr->it_version != 0) {
		fprintf(stderr, "ieee_radiotap.it_version is not 0\n");
		return false;
	}

	// test present flags of radiotap header
	uint8_t* pOrg = (uint8_t*)pRadioHdr + sizeof(struct ieee80211_radiotap_header);
	uint8_t* pRadioData = pOrg;
	{
		uint32_t presentFlag = pRadioHdr->it_present;

		if (presentFlag & (1<<IEEE80211_RADIOTAP_TSFT)) {
			if (debugFlag) printf("+TSFT ");
			pRadioData += sizeof(uint64_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_FLAGS)) {
			if (debugFlag) printf("+FLAGS ");
			uint8_t flags = *pRadioData;
			if (flags & (IEEE80211_RADIOTAP_F_BAD_FCS)) {
				return false;
			}
			if (flags & (1<<IEEE80211_RADIOTAP_F_FCS)) {
				fcslength = 4;
			}
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_RATE)) {
			if (debugFlag) printf("+RATE ");
			rate = (*pRadioData) / 2;
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_CHANNEL)) {
			if (debugFlag) printf("+CHANNEL ");
			channelFreq = *(uint16_t*)pRadioData;
			channel = WlanUtils::channelToFreq(channelFreq);
			pRadioData += sizeof(uint16_t);

			uint16_t channelFlags = *(uint16_t*)pRadioData;
			pRadioData += sizeof(uint16_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_FHSS)) {
			if (debugFlag) printf("+FHSS ");
			pRadioData += sizeof(uint16_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_DBM_ANTSIGNAL)) {
			if (debugFlag) printf("+SIGNAL ");
			signal = *pRadioData;
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_DBM_ANTNOISE)) {
			if (debugFlag) printf("+NOISE ");
			noise = *pRadioData;
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_LOCK_QUALITY)) {
			pRadioData += sizeof(uint16_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_TX_ATTENUATION)) {
			pRadioData += sizeof(uint16_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_DB_TX_ATTENUATION)) {
			pRadioData += sizeof(uint16_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_DBM_TX_POWER)) {
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_ANTENNA)) {
			if (debugFlag) printf("+ANTENNA ");
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_DB_ANTSIGNAL)) {
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_DB_ANTNOISE)) {
			pRadioData += sizeof(uint8_t);
		}
		if (presentFlag & (1<<IEEE80211_RADIOTAP_RX_FLAGS)) {
			if (debugFlag) printf("+RX_FLAGS ");
			pRadioData += sizeof(uint16_t);
		}
		// TODO: Who one fill next flags.
		// from CHANNEL_PLUS to VENDOR_NS_NEXT
		if (debugFlag) printf("\n");
	}

	frameLength = len - pRadioHdr->it_len - fcslength;
//	debugHex("packet", buf, frameLength);

//	printf("ch: %d, rate: %d, signal: %d, noise: %d, framelen: %d, dist:%ld\n",
//			channel, rate, signal, noise, frameLength, pRadioData-pOrg);

	printf("{%s} ", fm->getName());
	printf("ch: %d(%d), rate: %d, rssi: %d, len: %d",
			channel, channelFreq, rate, signal, frameLength);


	uint8_t *pData = pRadioData;
	FRAME_CONTROL *fc = (FRAME_CONTROL *)pData;
	{
//		debugHex("FC", (uint8_t*)fc, sizeof(FRAME_CONTROL));
		uint8_t version = fc->version;
		uint8_t type = fc->type;
		uint8_t subtype = fc->subtype;
//		printf("ver:%d, type:%d, subtype:%d fromds:%d, tods:%d\n", version, type, subtype, fc->from_ds, fc->to_ds);
		pData += sizeof(FRAME_CONTROL);

		uint16_t duration = ntohs(*(uint16_t*)pData); pData += sizeof(uint16_t);
		WlanUtils::showFrameType(type, subtype, false);
		WlanUtils::showMacfmt(" Receiver", pData, false); pData += 6;
		WlanUtils::showMacfmt(" Transmit", pData, false); pData += 6;
		WlanUtils::showMacfmt(" Source", pData, false);   pData += 6;
	}
	printf("\n");
	return true;
}

bool parsingEthernet(u_char* buf, size_t len) {
	struct ip *iph;
	struct tcphdr *tcph;
	struct ether_header *ep;
	unsigned short ether_type;
	u_char* packet;

//	debugHex(getName(), buf, len);

	packet = buf;
	ep = (struct ether_header *) packet;
	packet += sizeof(struct ether_header);
	ether_type = ntohs(ep->ether_type);

	if (ether_type == ETHERTYPE_IP) {
		// IP 헤더에서 데이타 정보를 출력한다.
		iph = (struct ip *) packet;
		printf("%s -> ", inet_ntoa(iph->ip_src));
		printf("%s", inet_ntoa(iph->ip_dst));

		if (iph->ip_p == IPPROTO_TCP) {
			tcph = (struct tcphdr *) (packet + iph->ip_hl * 4);
			printf("[%d -> ", ntohs(tcph->source));
			printf("%d]", ntohs(tcph->dest));
		}
		printf("\n\n");
	}
	return true;
}

void FilterMember::filterHandler(u_char* buf, size_t len) {
	static int dlt = WlanUtils::getDLT(m_ifname);
	switch (dlt) {
	case DLT_RADIOTAP:
		parsingWirelessRadiotap(buf, len, this);
		break;
	default:
		fprintf(stderr, "unsupport DLT: %d\n", dlt);
		return;
	}
}

const char* FilterMember::toString() {
	if (!m_desc[0]) {
		snprintf(m_desc, sizeof(m_desc), "FilterMember[name=%s, sock=%d, filter=%s, ifname=%s]",
				m_name, m_sock, m_filterExpression, m_ifname);
	}
	return m_desc;
}

} /* namespace kinow */
