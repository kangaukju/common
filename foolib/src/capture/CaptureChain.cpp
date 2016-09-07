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

namespace kinow {

CaptureChain::CaptureChain() {
	// TODO Auto-generated constructor stub

}

CaptureChain::~CaptureChain() {
	// TODO Auto-generated destructor stub
}

void CaptureChain::addFilter(const char* filter) {

}


} /* namespace kinow */

void capture(const u_char *packet) {
	/* ethernet headers are always exactly 14 bytes */
	#define SIZE_ETHERNET 14
	const struct sniff_ethernet *ethernet; /* The ethernet header */
	const struct sniff_ip *ip; /* The IP header */
	const struct sniff_tcp *tcp; /* The TCP header */
	const u_char *payload; /* Packet payload */

	u_int size_ip;
	u_int size_tcp;


	ethernet = (struct sniff_ethernet*)(packet);
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20) {
//	printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}
	payload = (const u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

	printf("TCP port: %d -> %d\n", tcp->th_sport, tcp->th_dport);
	return;
}

int main(int argc, char** argv) {
	pcap_t *h;
	const char* dev = argv[1];
	bpf_u_int32 net;
	bpf_u_int32 mask;
	char errbuf[PCAP_ERRBUF_SIZE];
	char filter_exp[] = ""; /* The filter expression */
	struct bpf_program fp; /* The compiled filter expression */
	const u_char *packet;
	struct pcap_pkthdr header;
	int sock;

	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s\n", dev);
		return -1;
	}
	h = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
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
			capture(packet);
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
