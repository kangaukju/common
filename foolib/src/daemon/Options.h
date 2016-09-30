/*
 * Options.h
 *
 *  Created on: 2016. 9. 30.
 *      Author: root
 */

#ifndef DAEMON_OPTIONS_H_
#define DAEMON_OPTIONS_H_

#include <map>
#include <string>
#include <stdio.h>

namespace kinow {

using namespace std;

class Options {
public:
	static const int OPT_DEFAULT          = 0x00;
	static const int OPT_REQUIRE          = 0x01;
	static const int OPT_REQUIRE_VALUE    = 0x02;


public:
	Options(int argc, char** argv) : m_argc(argc), m_argv(argv) {}
	virtual ~Options();
	bool addOption(char code, const char* desc, int require = OPT_DEFAULT, const char* defValue = NULL);
	bool validOptions(char *errbuf = NULL, size_t errbuflen = 0);
	bool isRequire(char code);
	bool isRequireValue(char code);
	const char* optionValue(char code);
	void showOptions(const char* title = NULL);
	void usage();
	void help();

private:
	class Option {
	public:
		Option(char code, int require, const char *desc);
		~Option();
		int require();
		char code();
		const char* desc();
		const char* value();
		void value(const char* value);
		bool checked();
		void check(bool checked);
		string toString();

	private:
		char m_code;
		int m_require;
		char *m_desc;
		char *m_value;
		bool m_checked;
	};

private:
	int m_argc;
	char **m_argv;
	map<char, Option*> m_optionMap;
	string getOptionsString();
};

} /* namespace kinow */

#endif /* DAEMON_OPTIONS_H_ */
