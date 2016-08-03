/*
 * Test.cpp
 *
 *  Created on: 2016. 8. 3.
 *      Author: root
 */

#include <stdio.h>

class Test {
public:
	Test(const double* pd) : m_pd(pd), m_i(0) { }
	Test(const int& i) : m_i(i), m_pd(NULL) { }
	void show() {
		printf("%d\n", m_i);
		if (m_pd)
			printf("%f\n", *m_pd);
	}
private:
	const double* m_pd;
	const int& m_i;
};

int main() {
	int n = 10;
	Test t(n);
	t.show();
	n = 20;
	t.show();

	double dp = 10;
	Test tt(&dp);
	tt.show();
}



