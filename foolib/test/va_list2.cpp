/*
 * va_list.cpp
 *
 *  Created on: 2016. 8. 4.
 *      Author: root
 */



#include <stdio.h>
#include <stdarg.h>

class VaList {
public:
	virtual void test2(const char* msg, va_list ap) = 0;
	virtual void test1(const char* msg, va_list ap) = 0;
	void show(const char* msg, ...) {
		va_list ap;
		va_start(ap, msg);
		test1(msg, ap);
		va_end(ap);
	}
};

class VaListTest1 : public VaList {
public:
	virtual void test1(const char* msg, va_list ap) {
		char buff[1024];
		vsnprintf(buff, sizeof(buff), msg, ap);
		test2(msg, ap);
	}
	virtual void test2(const char* msg, va_list ap) {
		vprintf(msg, ap);
	}
};

int main() {
	VaList *v = new VaListTest1();

	v->show("aaaa :%d   :%s\n", 20, "1111111111122222222222");
}
