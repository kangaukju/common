/*
 * Clock.h
 *
 *  Created on: 2016. 8. 9.
 *      Author: root
 */

#ifndef CLOCK_H_
#define CLOCK_H_

namespace kinow {

class CClock {
public:
	// show clock
	virtual void draw() {
		printf("draw clock\n");
	}
public:
	int getYear();
	int getMonth();
	int getDay();
	int getDayOfWeek();
	int getHour();
	int getMinute();
	int getSecond();
};

class CAnalogClock : public CClock {
public:
	// show clock
	virtual void draw() {
		CClock::draw();
		printf("display analog\n");
	}
};

class CDigitalClock : public CClock {
public:
	// show clock
	virtual void draw() {
		CClock::draw();
		printf("display digital\n");
	}
};

class CClockDateDecorator : public CClock {

};

} /* namespace kinow */

#endif /* CLOCK_H_ */
