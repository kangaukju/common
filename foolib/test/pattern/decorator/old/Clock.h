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

class CDateClock : public CClock {
public:
	// show clock and date
	virtual void draw() {
		CClock::draw();
		printf("draw date\n");
	}
};



                        CClock

		CAnalogClock           CDigitalClock

		CAnalogDateClock       CDigitalDateClock


		a lot of class for display date string on clock


} /* namespace kinow */

#endif /* CLOCK_H_ */
