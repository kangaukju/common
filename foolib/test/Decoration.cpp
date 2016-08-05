/*
 * Decoration.cpp
 *
 *  Created on: 2016. 8. 4.
 *      Author: root
 */


#include <stdio.h>

class ILOG {
public:
	virtual void log() = 0;
	ILOG(ILOG* ilog) : m_ilog(ilog) {}
protected:
	void callLog() {
		if (m_ilog) m_ilog->log();
	}
private:
	ILOG *m_ilog;
};

class CDBLOG : public ILOG {
public:
	virtual void log() {
		callLog();
		printf("call CDBLOG::log\n");
	}
	CDBLOG(ILOG* ilog) : ILOG(ilog) {}
};

class CCONSOLELOG : public ILOG {
public:
	virtual void log() {
		callLog();
		printf("call CCONSOLELOG::log\n");
	}
	CCONSOLELOG(ILOG* ilog) : ILOG(ilog) {}
};

class CFILELOG : public ILOG {
public:
	virtual void log() {
		callLog();
		printf("call CFILELOG::log\n");
	}
	CFILELOG(ILOG* ilog) : ILOG(ilog) {}
};

int main()
{
	ILOG* log = new CDBLOG(new CCONSOLELOG(new CFILELOG(NULL)));
	log->log();
}
