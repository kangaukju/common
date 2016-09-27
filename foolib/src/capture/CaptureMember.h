/*
 * CaptureMember.h
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#ifndef CAPTURE_CAPTUREMEMBER_H_
#define CAPTURE_CAPTUREMEMBER_H_

#include <IFilterMember.h>

#include "Error.h"
#include <vector>
#include <string>

namespace kinow {

using namespace std;

class CaptureMember : public Error {
public:
	CaptureMember(const char* name);
	void addFilterMember(IFilterMember *fm);
	bool capture();
	virtual ~CaptureMember();
	const char* toString();

private:
	int getFilterMemberMaxFD();
	bool initialize();

private:
	vector<IFilterMember*> m_filterMembers;
	char *m_name;
};

} /* namespace kinow */

#endif /* CAPTURE_CAPTUREMEMBER_H_ */
