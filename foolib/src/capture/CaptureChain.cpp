/*
 * CaptureChain.cpp
 *
 *  Created on: 2016. 9. 7.
 *      Author: root
 */

#include "CaptureChain.h"
#include "sniff_struct.h"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/filter.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/ethernet.h>
#include <linux/filter.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

namespace kinow {

CaptureChain::CaptureChain() {
	// TODO Auto-generated constructor stub

}

CaptureChain::~CaptureChain() {
	// TODO Auto-generated destructor stub
}

void CaptureChain::debugSocketFilters(struct socket_filter_st* sf) {
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

struct socket_filter_st*
CaptureChain::buildSocketFilters(const char* expression) {
	FILE *fp;
	char buf[256];
	char tcpdumpExpression[256];
	char *tok;
	char *last;
	int filter_cnt = 0;
	struct sock_filter *filters;
	int line = 0;
	struct socket_filter_st* sf;

	sf = (struct socket_filter_st*)malloc(sizeof(struct socket_filter_st));
	memset(sf, 0, sizeof(struct socket_filter_st));

	snprintf(tcpdumpExpression, sizeof(tcpdumpExpression),
			"/usr/sbin/tcpdump %s -ddd", expression);

	fp = popen(tcpdumpExpression, "r");
	if (fp == NULL) {
		failure("Can't popen: %s - %s", tcpdumpExpression, strerror(errno));
		free(sf);
		return NULL;
	}

	if (fgets(buf, sizeof(buf), fp)) {
		// BPF filter count
		sscanf(buf, "%d", &filter_cnt);

		if (filter_cnt == 0) {
			failure("Can't found suitalbe filter: %s", tcpdumpExpression);
			free(sf);
			pclose(fp);
			return NULL;
		}

		filters = (struct sock_filter*)malloc(sizeof(struct sock_filter) * filter_cnt);

		while (fgets(buf, sizeof(buf), fp) != NULL && (line < filter_cnt)) {
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
		sf->count = filter_cnt;
		sf->filters = filters;
		pclose(fp);
		return sf;
	}

	failure("Can't found suitalbe filter: %s", tcpdumpExpression);
	free(sf);
	pclose(fp);
	return NULL;
}

void CaptureChain::addFilter(const char* filter) {

}


} /* namespace kinow */

#if 0
int getUDS() {
	register int s, len;

	if ((s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		perror("client: socket");
		return -1;
	}
	return s;
}

int sendChain(const u_char* packet, int len) {
	static int s = getUDS();
	struct sockaddr_un sa;

	if (s < 0) {
		return -1;
	}

	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, "CaptureChain.sock");

	return sendto(s, packet, len, 0, (struct sockaddr*)&sa, sizeof(sa));
}
#endif

void capture(const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	struct ip *iph;
	struct tcphdr *tcph;
	static int count = 1;
	struct ether_header *ep;
	unsigned short ether_type;
	int chcnt = 0;
	int length = pkthdr->len;

	// 이더넷 헤더를 가져온다.
	ep = (struct ether_header *) packet;

	// IP 헤더를 가져오기 위해서
	// 이더넷 헤더 크기만큼 offset 한다.
	packet += sizeof(struct ether_header);

	// 프로토콜 타입을 알아낸다.
	ether_type = ntohs(ep->ether_type);

	// 만약 IP 패킷이라면
	if (ether_type == ETHERTYPE_IP) {
		// IP 헤더에서 데이타 정보를 출력한다.
		iph = (struct ip *) packet;
		/*
		printf("IP 패킷\n");
		printf("Version     : %d\n", iph->ip_v);
		printf("Header Len  : %d\n", iph->ip_hl);
		printf("Ident       : %d\n", ntohs(iph->ip_id));
		printf("TTL         : %d\n", iph->ip_ttl);
		*/
		printf("Src Address : %s -> ", inet_ntoa(iph->ip_src));
		printf("Dst Address : %s\n", inet_ntoa(iph->ip_dst));

		// 만약 TCP 데이타 라면
		// TCP 정보를 출력한다.
		if (iph->ip_p == IPPROTO_TCP) {
			tcph = (struct tcphdr *) (packet + iph->ip_hl * 4);
			printf("Src Port : %d -> ", ntohs(tcph->source));
			printf("Dst Port : %d\n", ntohs(tcph->dest));
		}

		// Packet 데이타 를 출력한다.
		// IP 헤더 부터 출력한다.
		/*
		while (length--) {
			printf("%02x", *(packet++));
			if ((++chcnt % 16) == 0)
				printf("\n");
		}
		*/
	}
	// IP 패킷이 아니라면
	else {
//		printf("NONE IP 패킷\n");
	}
//	printf("\n\n");
}

#define PROMISCUOUS 1
#define NONPROMISCUOUS 0

#if 1
int main(int argc, char** argv) {
	pcap_t *h;
	const char* dev = argv[1];
	bpf_u_int32 net;
	bpf_u_int32 mask;
	char errbuf[PCAP_ERRBUF_SIZE];
	char filter_exp[] = "ip and tcp and port 80"; /* The filter expression */
	struct bpf_program fp; /* The compiled filter expression */
	const u_char *packet;
	struct pcap_pkthdr header;
	int sock;

	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s\n", dev);
		return -1;
	}
	h = pcap_open_live(dev, BUFSIZ, PROMISCUOUS, -1, errbuf);
	if (h == NULL) {
		fprintf(stderr, "Couldn't open devie %s: %s\n", dev, errbuf);
		return -1;
	}
	if (pcap_compile(h, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(h));
		return -1;
	}
	if (pcap_setfilter(h, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(h));
		return -1;
	}

//	sock = pcap_fileno(h);
	sock = pcap_get_selectable_fd(h);
	printf("sock: %d\n", sock);

	struct sock_filter code;

	fd_set rset;


	while (true) {
#if 1
		packet = pcap_next(h, &header);
		if (packet != NULL)
		{
			capture(&header, packet);
		}
#else
		FD_ZERO(&rset);
		FD_SET(sock, &rset);

		printf("select\n");
		if (select(sock+1, &rset, NULL, NULL, NULL) < 0) {
			continue;
		}
		printf("recv\n");
		u_char recvbuf[1024];
		int bytes = recv(sock, recvbuf, 1024, 0);
		if (bytes > 0) {
			capture(recvbuf);
		}
#endif
	}
	pcap_close(h);
	return 0;
}

#else

using namespace kinow;
int main(int argc, char** argv) {
	CaptureChain* cc = new CaptureChain();
	cc->debugSocketFilters(cc->buildSocketFilters("port 22 and host 10.10.20.1"));
}

#endif
