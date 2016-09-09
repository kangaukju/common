/*
 * FilterMember.h
 *
 *  Created on: 2016. 9. 9.
 *      Author: root
 */

#ifndef CAPTURE_FILTERMEMBER_H_
#define CAPTURE_FILTERMEMBER_H_

#include "Error.h"
#include <string.h>
#include <sys/types.h>
#include <linux/if_packet.h>

namespace kinow {

using namespace std;

class FilterMember : public Error {
public:
	FilterMember(const char* name, const char *ifname, const char *filterExpression);
	virtual ~FilterMember();

public:
	int getSock() { return m_sock; }
	const char* getFilterExpression() { return m_filterExpression; }
	const char* getIfname() { return m_ifname; }
	const char* getName() { return m_name; }
	const char* toString();
	struct sockaddr_ll getSockaddr_ll() { return m_sockaddr_ll; }
	bool initialize();
	void filterHandler(u_char* buf, size_t len);

private:
	int createFilterSocket();

private:
	char *m_name;
	char *m_ifname;
	char *m_filterExpression;
	int m_sock;
	char m_desc[128];
	struct sockaddr_ll m_sockaddr_ll;
};

} /* namespace kinow */

#endif /* CAPTURE_FILTERMEMBER_H_ */
