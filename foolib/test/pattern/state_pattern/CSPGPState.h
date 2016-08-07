/*
 * CSPGPState.h
 *
 *  Created on: 2016. 8. 7.
 *      Author: root
 */

#ifndef CSPGPSTATE_H_
#define CSPGPSTATE_H_

#include <stdio.h>
#include <string.h>

//
typedef enum {
	eSPGP_ERR_SUCCESS,
	eSPGP_ERR_FAIL,
	eSPGP_SEND,
	eSPGP_RECEIVE,
} ESPGP_ERR;

//
class CSPGPInfo {

};

class CSPGPState;
class CSPGPClientHello;
class CSPGPServerHello;
class CSPGPServerInfo;
class CSPGPClientInfo;
class CSPGPClientFinish;
class CSPGPServerFinish;

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////
class ISPGPContext {
public:
	virtual void changeState(CSPGPState* pNextState) = 0;
};

/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////
class CSPGPState {
public:
	virtual ~CSPGPState() {}
public:
	virtual ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize) = 0;
	virtual ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize) = 0;
public:
	void setInfo(CSPGPInfo *pInfo) {
		m_pInfo = pInfo;
	}
	void setContext(ISPGPContext* pContext) {
		m_pContext = pContext;
	}
protected:
	CSPGPInfo* m_pInfo;
	ISPGPContext* m_pContext;
};

//
class CSPGPLayer : public ISPGPContext {
public:
	CSPGPLayer();
	virtual ~CSPGPLayer();
	// ISPGPContext
	void changeState(CSPGPState* pNextState);
public:
	void close();
	void send();
	void onReceive(char* pBuf, int nBufSize);
private:
	CSPGPInfo m_info;
	CSPGPState* m_pState;
};

/*
client                                    server
         ------ client hello ---------->
         <----- server hello ----------
         <----- server info -----------
         ----- client info ------------>
         ----- client finish ---------->
         <----- server finish ----------
 */
class CSPGPClientHello : public CSPGPState {
public:
	ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize);
	ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize);
};

class CSPGPServerHello : public CSPGPState {
public:
	ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize);
	ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize);
};

class CSPGPServerInfo : public CSPGPState {
public:
	ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize);
	ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize);
};

class CSPGPClientInfo : public CSPGPState {
public:
	ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize);
	ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize);
};

class CSPGPClientFinish : public CSPGPState {
public:
	ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize);
	ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize);
};

class CSPGPServerFinish : public CSPGPState {
public:
	ESPGP_ERR send(char* pBuf, int nBufSize, int* nUsedSize);
	ESPGP_ERR receive(char* pBuf, int nBufSize, int* nUsedSize);
};
#endif /* CSPGPSTATE_H_ */
