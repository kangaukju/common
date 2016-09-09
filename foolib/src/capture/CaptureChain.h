/*
 * CaptureChain.h
 *
 *  Created on: 2016. 9. 7.
 *      Author: root
 */

#ifndef CAPTURECHAIN_H_
#define CAPTURECHAIN_H_

#include <vector>
#include <pcap.h>
#include <stdint.h>
#include "Error.h"

struct socket_filter_st {
	u_int count;
	struct sock_filter* filters;
};

namespace kinow {

class CaptureChain : public Error {
public:
	CaptureChain();
	virtual ~CaptureChain();
	void addFilter(const char* filter);

	void debugSocketFilters(struct socket_filter_st* sf);
	void debugCapture(const u_char *packet);
	struct socket_filter_st* buildSocketFilters(const char* expression);
private:
};

} /* namespace kinow */

#endif /* CAPTURECHAIN_H_ */
