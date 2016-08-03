/*
 * CChild1.h
 *
 *  Created on: 2016. 8. 3.
 *      Author: root
 */

#ifndef CCHILD1_H_
#define CCHILD1_H_

#include "CParent.h"

namespace kinow {

class CChild1 : public CParent {
public:
	virtual void view();

public:
	CChild1();
	virtual ~CChild1();
};

} /* namespace kinow */

#endif /* CCHILD1_H_ */
