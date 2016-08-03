/*
 * CChild1.cpp
 *
 *  Created on: 2016. 8. 3.
 *      Author: root
 */

#include <stdio.h>
#include "CChild1.h"

namespace kinow {

CChild1::CChild1() {
	// TODO Auto-generated constructor stub

}

CChild1::~CChild1() {
	// TODO Auto-generated destructor stub
}

void CChild1::view() {
	CParent::view();
	printf("call CChild1::view\n");
}

} /* namespace kinow */

using namespace kinow;


int main() {
	CChild1 c1;
	c1.view();
}
