/*
 * ControlLayer.h
 *
 *  Created on: 2016. 8. 8.
 *      Author: root
 */

#ifndef CONTROLLAYER_H_
#define CONTROLLAYER_H_

#include <stdio.h>

namespace kinow {

class IControlLayer {
public:
	virtual void connect() = 0;
	virtual void sendStat() = 0;
	virtual void sendTraffic() = 0;
	virtual void close() = 0;
};

// server
class CControlLayer : public IControlLayer {
public:
	CControlLayer() {}
	virtual ~CControlLayer() {}
public:
	virtual void connect() {
		printf("connect\n");
	}
	virtual void sendStat() {
		printf("send stat\n");
	}
	virtual void sendTraffic() {
		printf("send traffic\n");
	}
	virtual void close() {
		printf("close\n");
	}
};

/*

 client                            server
           method1: connectionful
          ------------------------->

           method2: connectionless
          ------------------------->
 */

// proxy
class CConnlessControlProxy : public IControlLayer {
public:
	CConnlessControlProxy() {}
	virtual ~CConnlessControlProxy() {}
public:
	virtual void connect() {
		// connection less
		// Do Nothing.
	}
	virtual void close() {
		// connection less
		// Do Nothing.
	}
	virtual void sendStat() {
		CControlLayer layer;

		// connectionless step: connect - send - close
		layer.connect();
		layer.sendStat();
		layer.close();
	}
	virtual void sendTraffic() {
		CControlLayer layer;

		// connectionless step: connect - send - close
		layer.connect();
		layer.sendTraffic();
		layer.close();
	}
};

} /* namespace kinow */

#endif /* CONTROLLAYER_H_ */
