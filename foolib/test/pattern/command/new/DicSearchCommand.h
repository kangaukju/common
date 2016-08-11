/*
 * DicSearchCommand.h
 *
 *  Created on: 2016. 8. 9.
 *      Author: root
 */

#ifndef DICSEARCHCOMMAND_H_
#define DICSEARCHCOMMAND_H_

#include <stdio.h>
#include <string.h>


namespace kinow {

typedef enum {
	eDIC_ENG,
	eDIC_KOR,
	eDIC_ENC,
} EDICTYPE;

#define interface class

///////////////////////////////////////////////////////
// interface command
///////////////////////////////////////////////////////
interface ICommand {
public:
	virtual void execute() = 0;
};

///////////////////////////////////////////////////////
// interface search parameter
///////////////////////////////////////////////////////
interface IDicSearchParam {
public:
//	virtual EDICTYPE getSearchDicType() = 0;
	virtual char* getSearchWord() = 0;
	virtual void showSearchOutputWindow() = 0;
};

///////////////////////////////////////////////////////
// dictionary search command
///////////////////////////////////////////////////////
class CDicSearchCommand : public ICommand {
public: // ICommand
	void execute() {
		char* pSearchWord = m_pSearchParam->getSearchWord();
		char* pSearchResult = NULL;

		// step1. search word and get result
		pSearchResult = internalSearch(pSearchWord);

		// step2. print result word
		displaySearchResult(pSearchResult);

		free(pSearchResult);
	}
public:
	CDicSearchCommand(IDicSearchParam* searchParam) {
		this->m_pSearchParam = searchParam;
	}
	virtual ~CDicSearchCommand() {};
protected:
	virtual char* internalSearch(char* pSearchWord) = 0;
private:
	void displaySearchResult(char* pSearchWord) {
		printf("[show search result]\n");
		printf("%s\n", pSearchWord);
	}
private:
	IDicSearchParam *m_pSearchParam;
};


///////////////////////////////////////////////////////
// implementation English search command
///////////////////////////////////////////////////////
class CEngDicSearchCommand : public CDicSearchCommand {
public: // CDicSearchCommand
	char* internalSearch(char* pSearchWord) {
		char content[1024];
		snprintf(content, sizeof(content),
				"search: %s => result: %s\n", pSearchWord, pSearchWord);
		return strdup(content);
	}
	CEngDicSearchCommand(IDicSearchParam* searchParam) :
		CDicSearchCommand(searchParam) {}
	virtual ~CEngDicSearchCommand() {}
};

///////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
class CDicCommandFactory {
public:
	static ICommand* createCommand(EDICTYPE etype, IDicSearchParam* searchParam) {
		switch (etype) {
		case eDIC_ENC: break;
		case eDIC_ENG: return new CEngDicSearchCommand(searchParam);
		case eDIC_KOR: break;
		}
		return NULL;
	}
};


///////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////
class CDictionary : public IDicSearchParam {
public:
	virtual char* getSearchWord() {
		return "Riuo~uoRi";
	}
	virtual void showSearchOutputWindow() {
	}
	EDICTYPE getSearchOption() {
		return eDIC_KOR;
	}
	void onSearch() {
		EDICTYPE etype = getSearchOption();
		ICommand* pCommand = CDicCommandFactory::createCommand(etype, this);
		if (pCommand == NULL) {
			return;
		}
		//////////////////////////////////////////////////////////
		pCommand->execute();
		//////////////////////////////////////////////////////////
		delete pCommand;
	}
};

} /* namespace kinow */

#endif /* DICSEARCHCOMMAND_H_ */
