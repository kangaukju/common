/*
 * ILog.h
 *
 *  Created on: 2016. 8. 2.
 *      Author: root
 */

#ifndef ILOG_H_
#define ILOG_H_

namespace kinow {

#ifndef byte
	typedef uint8_t byte;
#endif

class ILog {

public:
	virtual void c(const char* msg, ...) = 0;
	virtual void C(const char* msg, ...) = 0;
	virtual void e(const char* msg, ...) = 0;
	virtual void E(const char* msg, ...) = 0;
	virtual void i(const char* msg, ...) = 0;
	virtual void I(const char* msg, ...) = 0;
	virtual void d(const char* msg, ...) = 0;
	virtual void D(const char* msg, ...) = 0;
	virtual void p(const char* msg, ...) = 0;
	virtual void P(const char* msg, ...) = 0;
	virtual void chex(byte* hex, int len, const char* msg, ...) = 0;
	virtual void ehex(byte* hex, int len, const char* msg, ...) = 0;
	virtual void ihex(byte* hex, int len, const char* msg, ...) = 0;
	virtual void dhex(byte* hex, int len, const char* msg, ...) = 0;

public:
	ILog(const ILog* ilog) : m_iLog(ilog) { }
	virtual ~ILog();
private:
	const ILog* m_iLog;
};

} /* namespace kinow */

#endif /* ILOG_H_ */
