/*
 * StaticMember.cpp
 *
 *  Created on: 2016. 8. 3.
 *      Author: root
 */

#include "StaticMember.h"
#include <stdio.h>

namespace kinow {

StaticMember::StaticMember() {
	// TODO Auto-generated constructor stub

}

StaticMember::~StaticMember() {
	// TODO Auto-generated destructor stub
}

// TODO: not declared static keyword !!!
// static
void StaticMember::print() {
	printf("StaticMember::print\n");
}

} /* namespace kinow */
