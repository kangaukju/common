/*
 * ControlLayer.cpp
 *
 *  Created on: 2016. 8. 8.
 *      Author: root
 */

#include "ControlLayer.h"

namespace kinow {



} /* namespace kinow */

using namespace kinow;


int main(int argc, char** argv) {
	CConnlessControlProxy connlessProxy;

	printf(">>>>>>>>>> request connect\n");
	connlessProxy.connect();
	printf(">>>>>>>>>> request send stat\n");
	connlessProxy.sendStat();
	printf(">>>>>>>>>> request send traffic\n");
	connlessProxy.sendTraffic();
	printf(">>>>>>>>>> request send stat\n");
	connlessProxy.sendStat();
	printf(">>>>>>>>>> request send traffic\n");
	connlessProxy.sendTraffic();
	printf(">>>>>>>>>> request close\n");
	connlessProxy.close();
}
