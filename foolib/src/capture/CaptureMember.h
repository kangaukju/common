/*
 * CaptureMember.h
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#ifndef CAPTURE_CAPTUREMEMBER_H_
#define CAPTURE_CAPTUREMEMBER_H_

#include "FilterMember.h"
#include "Error.h"
#include <vector>
#include <string>

namespace kinow {

using namespace std;

class CaptureMember : public Error {
public:
	void addFilterMember(FilterMember *fm);
	bool capture();
	CaptureMember(const char* name);
	virtual ~CaptureMember();
	const char* toString();

private:
	int getFilterMemberMaxFD();
	bool initialize();

private:
	vector<FilterMember*> m_filterMembers;
	char *m_name;
};

} /* namespace kinow */

#endif /* CAPTURE_CAPTUREMEMBER_H_ */
