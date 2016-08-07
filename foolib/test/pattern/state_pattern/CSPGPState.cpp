/*
 * CSPGPState.cpp
 *
 *  Created on: 2016. 8. 7.
 *      Author: root
 */

#include "CSPGPState.h"

//////////////////////////////////////////////////////////////////
// CSPGPClientHello
//////////////////////////////////////////////////////////////////
ESPGP_ERR CSPGPClientHello::send(char* pBuf, int nBufSize, int* nUsedSize) {
	*nUsedSize = snprintf(pBuf, nBufSize, "cSPGPClientHello::send>");
	m_pContext->changeState(new CSPGPServerHello);
	printf("\tCSPGPClientHello -> CSPGPServerHello\n");
	return eSPGP_ERR_SUCCESS;
}
ESPGP_ERR CSPGPClientHello::receive(char* pBuf, int nBufSize, int* nUsedSize) {
	return eSPGP_ERR_FAIL;
}
//////////////////////////////////////////////////////////////////
// CSPGPServerHello
//////////////////////////////////////////////////////////////////
ESPGP_ERR CSPGPServerHello::send(char* pBuf, int nBufSize, int* nUsedSize) {
	return eSPGP_ERR_FAIL;
}
ESPGP_ERR CSPGPServerHello::receive(char* pBuf, int nBufSize, int* nUsedSize) {
	*nUsedSize = snprintf(pBuf, nBufSize, "cSPGPServerHello::receive>");
	m_pContext->changeState(new CSPGPServerInfo);
	printf("\tCSPGPServerHello -> CSPGPServerInfo\n");
	return eSPGP_RECEIVE; // recv server info message
}
//////////////////////////////////////////////////////////////////
// CSPGPServerInfo
//////////////////////////////////////////////////////////////////
ESPGP_ERR CSPGPServerInfo::send(char* pBuf, int nBufSize, int* nUsedSize) {
	return eSPGP_ERR_FAIL;
}
ESPGP_ERR CSPGPServerInfo::receive(char* pBuf, int nBufSize, int* nUsedSize) {
	*nUsedSize = snprintf(pBuf, nBufSize, "CSPGPServerInfo::receive>");
	m_pContext->changeState(new CSPGPClientInfo);
	printf("\tCSPGPServerInfo -> CSPGPClientInfo\n");
	return eSPGP_SEND; // next state is send function for client info
}
//////////////////////////////////////////////////////////////////
// CSPGPClientInfo
//////////////////////////////////////////////////////////////////
ESPGP_ERR CSPGPClientInfo::send(char* pBuf, int nBufSize, int* nUsedSize) {
	*nUsedSize = snprintf(pBuf, nBufSize, "CSPGPClientInfo::send>");
	m_pContext->changeState(new CSPGPClientFinish);
	printf("\tCSPGPClientInfo -> CSPGPClientFinish\n");
	return eSPGP_SEND;
}
ESPGP_ERR CSPGPClientInfo::receive(char* pBuf, int nBufSize, int* nUsedSize) {
	return eSPGP_ERR_FAIL;
}
//////////////////////////////////////////////////////////////////
// CSPGPClientFinish
//////////////////////////////////////////////////////////////////
ESPGP_ERR CSPGPClientFinish::send(char* pBuf, int nBufSize, int* nUsedSize) {
	*nUsedSize = snprintf(pBuf, nBufSize, "CSPGPClientFinish::send>");
	m_pContext->changeState(new CSPGPServerFinish);
	printf("\tCSPGPClientFinish -> CSPGPServerFinish\n");
	return eSPGP_RECEIVE;
}
ESPGP_ERR CSPGPClientFinish::receive(char* pBuf, int nBufSize, int* nUsedSize) {
	return eSPGP_ERR_FAIL;
}
//////////////////////////////////////////////////////////////////
// CSPGPServerFinish
//////////////////////////////////////////////////////////////////
ESPGP_ERR CSPGPServerFinish::send(char* pBuf, int nBufSize, int* nUsedSize) {
	return eSPGP_ERR_FAIL;
}
ESPGP_ERR CSPGPServerFinish::receive(char* pBuf, int nBufSize, int* nUsedSize) {
	*nUsedSize = snprintf(pBuf, nBufSize, "CSPGPServerFinish::receive>");
	m_pContext->changeState(new CSPGPClientInfo);
	printf("\tCSPGPServerFinish -> OK\n");
	return eSPGP_ERR_SUCCESS;
}

//////////////////////////////////////////////////////////////////
// CSPGPLayer
//////////////////////////////////////////////////////////////////
CSPGPLayer::CSPGPLayer() {
	m_pState = new CSPGPClientHello();
	m_pState->setInfo(&m_info);
	m_pState->setContext(this);
}
CSPGPLayer::~CSPGPLayer() {
	if (m_pState) {
		delete m_pState;
	}
}
// ISPGPContext
void CSPGPLayer::changeState(CSPGPState* pNextState) {
	// step1. clear current state
	if (m_pState) {
		delete m_pState;
		m_pState = NULL;
	}
	// step2. change state
	pNextState->setInfo(&m_info);
	pNextState->setContext(this);
	m_pState = pNextState;
}
void CSPGPLayer::close() {

}
void CSPGPLayer::send() {
	char szBuf[4096];
	int  nUsedSize = 0;
	ESPGP_ERR err;

	do {
		nUsedSize = 0;
		// call send of current state
		err = m_pState->send(szBuf, sizeof(szBuf), &nUsedSize);
		if (nUsedSize > 0) {
			printf("[send packet in network] %s (len:%d)\n", szBuf, nUsedSize);
		}
	} while (err == eSPGP_SEND);

	if (err != eSPGP_ERR_SUCCESS) {
		close();
	}
}
void CSPGPLayer::onReceive(char* pBuf, int nBufSize) {
	int nUsedSize = 0;
	ESPGP_ERR err;
	char* pCufBuf = pBuf;
	int nCurBufSize = nBufSize;

	// step1. analyze for current state
	do {
		nUsedSize = 0;
		err = m_pState->receive(pCufBuf, nCurBufSize, &nUsedSize);
		if (nUsedSize > 0) {
			pCufBuf     += nUsedSize;
			nCurBufSize -= nUsedSize;
		}
		printf("[recv packet in network] %s (len:%d)\n", pCufBuf, nUsedSize);
	} while (err == eSPGP_RECEIVE);

	// step2. determine next state
	switch (err) {
	case eSPGP_SEND:				send();	break;
	case eSPGP_ERR_SUCCESS:					break;
	default: 								close();break;
	}
}

int main()
{
	char szBuf[1024];
	int nBufSize = sizeof(szBuf);

	CSPGPLayer player;

	// client hello
	player.send();

	player.onReceive(szBuf, nBufSize);
	player.onReceive(szBuf, nBufSize);

}
