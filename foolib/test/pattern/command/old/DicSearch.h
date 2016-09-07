/*
 * DicSearch.h
 *
 *  Created on: 2016. 8. 8.
 *      Author: root
 */

#ifndef DICSEARCH_H_
#define DICSEARCH_H_

#include <stdio.h>
#include <string.h>

namespace kinow {

typedef enum {
	eDIC_ENG,
	eDIC_KOR,
	eDIC_ENC,
} EDICTYPE;

class CDicSearch
{
public:
	char* search(EDICTYPE etype, char* searchWord) {
		switch (etype) {
		case eDIC_ENG: return searchEng(searchWord);
		case eDIC_KOR: return searchKor(searchWord);
		case eDIC_ENC: return searchEnc(searchWord);
		}
		return NULL;
	}
	char* searchEng(char* searchWord) {
		return searchWord;
	}
	char* searchKor(char* searchWord) {
		return searchWord;
	}
	char* searchEnc(char* searchWord) {
		return searchWord;
	}
};

class CDictionary {
public:
	EDICTYPE getSearchOption() {
		return eDIC_KOR;
	}
	void getSearchWord(char* pszBuf, int bufSize) {
		snprintf(pszBuf, bufSize, "%s", "c++");
	}
	void onSearch() {
		EDICTYPE etype;
		char szSearchWord[256];
		char* pSearchResult;
		CDicSearch searcher;

		// step1. search option and search word
		etype = getSearchOption();
		getSearchWord(szSearchWord, sizeof(szSearchWord));

		// step2. search
		pSearchResult = searcher.search(etype, szSearchWord);

		printf("search result: %s\n", pSearchResult);
	}
};



} /* namespace kinow */

#endif /* DICSEARCH_H_ */
