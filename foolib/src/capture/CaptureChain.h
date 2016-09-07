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

namespace kinow {

class CaptureChain {
public:
	CaptureChain();
	virtual ~CaptureChain();
	void addFilter(const char* filter);
private:
};

} /* namespace kinow */

#endif /* CAPTURECHAIN_H_ */
