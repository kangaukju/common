/*
 * Log.h
 *
 *  Created on: 2016. 8. 2.
 *      Author: root
 */

#ifndef CFILELOG_H_
#define CFILELOG_H_

#include <stdint.h>

#include "ILog.h"

namespace kinow {

class CFileLog : public ILog {

public:
	virtual void c(const char* msg, ...);
	virtual void C(const char* msg, ...);
	virtual void e(const char* msg, ...);
	virtual void E(const char* msg, ...);
	virtual void i(const char* msg, ...);
	virtual void I(const char* msg, ...);
	virtual void d(const char* msg, ...);
	virtual void D(const char* msg, ...);
	virtual void p(const char* msg, ...);
	virtual void P(const char* msg, ...);
	virtual void chex(uint8_t* hex, int len, const char* msg, ...);
	virtual void ehex(uint8_t* hex, int len, const char* msg, ...);
	virtual void ihex(uint8_t* hex, int len, const char* msg, ...);
	virtual void dhex(uint8_t* hex, int len, const char* msg, ...);

private:
	typedef enum {
		L_CRI = 1,
		L_ERR = 2,
		L_INF = 4,
		L_DBG = 8,
		L_ALL = (L_CRI|L_ERR|L_INF|L_DBG),
	} L_LV;

public:
	CFileLog(const char* processname);
	CFileLog(const char* processname, const char* logFilePath, L_LV lvl);
	virtual ~CFileLog();

private:
	const int KB = 1024;
	const int MB = 1024 * KB;
	const int GB = 1024 * MB;
	const int MAX_LOGFILE_SIZE = 1 * GB;

private:
	char* m_pLogPath;
	char* m_pProcessName;
	L_LV  m_lvl;
	char* m_pFileName;
	char* m_pErrFileName;

private:
	void logWrite(L_LV lvl, const char* fmt, va_list ap, const char* color, bool bold);
	void logWriteHex(L_LV lvl, uint8_t *hex, int len, const char* fmt, va_list ap);
	void logRotate(const char* logfile, struct tm &t_now);
};

} /* namespace kinow */

#endif /* CFILELOG_H_ */
