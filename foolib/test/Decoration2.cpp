/*
 * Decoration.cpp
 *
 *  Created on: 2016. 8. 4.
 *      Author: root
 */


#include <stdio.h>

class Dummy ;

class ILOG {
private:
	ILOG* m_iLog;
	typedef void (ILOG::*PFLOG)(ILOG* ctx);
protected:
	virtual void logComponent(ILOG* ctx) = 0;
public:
	void log() {
#if 0
		logComponent(m_iLog);
#else
		PFLOG pfLog = &ILOG::logComponent;
//		(this->*pfLogComponent)(m_iLog);
		componentDelegate(pfLog, NULL);
#endif
	}

	void componentDelegate(PFLOG pf, void *args) {
		(this->*pf)(m_iLog);
	}

	ILOG(ILOG* log) : m_iLog(log) { }
};

class CFileLog : public ILOG {
public:
	CFileLog(ILOG* log) : ILOG(log) { }
protected:
	virtual void logComponent(ILOG* ctx) {
		printf("call CFileLog::logComponent\n");
		if (ctx) {
			ctx->log();
		}
	}
};

class CDBLog : public ILOG {
public:
	CDBLog(ILOG* log) : ILOG(log) { }
protected:
	virtual void logComponent(ILOG* ctx) {
		printf("call CDBLog::logComponent\n");
		if (ctx) {
			ctx->log();
		}
	}
};

class CConsoleLog : public ILOG {
public:
	CConsoleLog(ILOG* log) : ILOG(log) { }
protected:
	virtual void logComponent(ILOG* ctx) {
		printf("call CConsoleLog::logComponent\n");
		if (ctx) {
			ctx->log();
		}
	}
};

int main()
{
	ILOG* log = new CConsoleLog(new CDBLog(new CFileLog(NULL)));
	log->log();
}
