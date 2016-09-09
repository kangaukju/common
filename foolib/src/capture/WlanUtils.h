/*
 * WlanUtils.h
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#ifndef CAPTURE_WLANUTILS_H_
#define CAPTURE_WLANUTILS_H_

namespace kinow {

enum wifi_dlt {
	DLT_UNKOWN              = 0,
	DLT_PRISM               = 1,
	DLT_RADIOTAP            = 2,
	DLT_RADIO               = 3,
};

class WlanUtils {
private:
	WlanUtils() {}
	virtual ~WlanUtils() {}

public:
	static int getDLT(const char* ifname);
	static int channelToFreq(int in_chan);
};

} /* namespace kinow */

#endif /* CAPTURE_WLANUTILS_H_ */
