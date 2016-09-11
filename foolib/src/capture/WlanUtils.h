/*
 * WlanUtils.h
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#ifndef CAPTURE_WLANUTILS_H_
#define CAPTURE_WLANUTILS_H_

#include <stdint.h>

namespace kinow {

#ifndef _MAC_FMT_
 #define _MAC_FMT_ "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef _MAC7_FMT_
 #define _MAC7_FMT_ "%02x:%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifndef _MAC_FMT_FILL_
 #define _MAC_FMT_FILL_(x) \
		((uint8_t*)(x))[0], \
		((uint8_t*)(x))[1], \
		((uint8_t*)(x))[2], \
		((uint8_t*)(x))[3], \
		((uint8_t*)(x))[4], \
		((uint8_t*)(x))[5]
#endif

#ifndef _MAC7_FMT_FILL_
 #define _MAC7_FMT_FILL_(x) \
		((uint8_t*)(x))[0], \
		((uint8_t*)(x))[1], \
		((uint8_t*)(x))[2], \
		((uint8_t*)(x))[3], \
		((uint8_t*)(x))[4], \
		((uint8_t*)(x))[5], \
		((uint8_t*)(x))[6]
#endif

#ifndef _MAC_FMT_FILL_64_
 #define _MAC_FMT_FILL_64_(x) \
		((uint8_t*)(&x))[5], \
		((uint8_t*)(&x))[4], \
		((uint8_t*)(&x))[3], \
		((uint8_t*)(&x))[2], \
		((uint8_t*)(&x))[1], \
		((uint8_t*)(&x))[0]
#endif

#ifndef _MAC7_FMT_FILL_64_
 #define _MAC7_FMT_FILL_64_(x) \
		((uint8_t*)(&x))[6], \
		((uint8_t*)(&x))[5], \
		((uint8_t*)(&x))[4], \
		((uint8_t*)(&x))[3], \
		((uint8_t*)(&x))[2], \
		((uint8_t*)(&x))[1], \
		((uint8_t*)(&x))[0]
#endif

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
	static void showMacfmt(const char* title, const uint8_t* mac, bool newline);
	static void showMacfmt64(const char* title, const uint64_t *mac64, bool newline);
	static void showFrameType(uint8_t type, uint8_t subtype, bool newline);
};

} /* namespace kinow */

#endif /* CAPTURE_WLANUTILS_H_ */
