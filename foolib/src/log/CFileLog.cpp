/*
 * Log.cpp
 *
 *  Created on: 2016. 8. 2.
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include "CFileLog.h"
#include "../sys/STDLib.h"

namespace kinow {

/*
<List of colors for prompt and Bash>

txtblk='\e[0;30m' # Black - Regular
txtred='\e[0;31m' # Red
txtgrn='\e[0;32m' # Green
txtylw='\e[0;33m' # Yellow
txtblu='\e[0;34m' # Blue
txtpur='\e[0;35m' # Purple
txtcyn='\e[0;36m' # Cyan
txtwht='\e[0;37m' # White
bldblk='\e[1;30m' # Black - Bold
bldred='\e[1;31m' # Red
bldgrn='\e[1;32m' # Green
bldylw='\e[1;33m' # Yellow
bldblu='\e[1;34m' # Blue
bldpur='\e[1;35m' # Purple
bldcyn='\e[1;36m' # Cyan
bldwht='\e[1;37m' # White
unkblk='\e[4;30m' # Black - Underline
undred='\e[4;31m' # Red
undgrn='\e[4;32m' # Green
undylw='\e[4;33m' # Yellow
undblu='\e[4;34m' # Blue
undpur='\e[4;35m' # Purple
undcyn='\e[4;36m' # Cyan
undwht='\e[4;37m' # White
bakblk='\e[40m'   # Black - Background
bakred='\e[41m'   # Red
bakgrn='\e[42m'   # Green
bakylw='\e[43m'   # Yellow
bakblu='\e[44m'   # Blue
bakpur='\e[45m'   # Purple
bakcyn='\e[46m'   # Cyan
bakwht='\e[47m'   # White
txtrst='\e[0m'    # Text Reset
 */

static const char* VTC_BLUE   = "\x1b[34m" //!< Colour following text blue.
static const char* VTC_RED    = "\x1b[31m" //!< Colour following text red.
static const char* VTC_GREEN  = "\x1b[32m" //!< Colour following text creen.
static const char* VTC_YELLOW = "\x1b[33m" //!< Colour following text yellow.
static const char* VTC_BOLD   = "\x1b[1m" //!< Embolden following text.
static const char* VTC_RESET  = "\x1b[0m" //!< Reset terminal text to default style/colour.

CFileLog::CFileLog(const char* processname) {
	CFileLog(processname, "/var/log", L_ALL);
}

CFileLog::CFileLog(const char* processname, const char* logFilePath, L_LV lvl) {
	m_pProcessName = strdup(processname);
	m_pLogPath = strdup(logFilePath);
	m_lvl = lvl;
	m_pFileName = NULL;
	m_pErrFileName = NULL;
}

CFileLog::~CFileLog() {
	if (m_pProcessName) free(m_pProcessName);
	if (m_pLogPath) free(m_pLogPath);
	if (m_pFileName) free(m_pFileName);
	if (m_pErrFileName) free(m_pErrFileName);
}

void CFileLog::logWrite(L_LV lvl, const char* fmt, va_list ap, const char* color, bool bold) {

}

void CFileLog::logWriteHex(L_LV lvl, uint8_t *hex, int len, const char* fmt, va_list ap) {
	FILE *fp = NULL;
	FILE *errfp = NULL;
	struct tm stNow;
	char szStrTime[256] = {0, };
	char szBuffer[1024] = {0, };
	time_t now = time(NULL);
	int n = 0;
	int i = 0;

	localtime_r(&now, &stNow);

	snprintf(m_pFileName, sizeof(m_pFileName), "%s/%s-%04d%02d%02d.log",
			m_pLogPath, m_pProcessName, stNow.tm_year+1900, stNow.tm_mon+1, stNow.tm_mday);

	logRotate(m_pFileName, stNow);

	if (m_lvl & lvl) {
		if (m_stdout) {
			fp = stdout;
		}
		else {
			fp = fopen(m_fname, "a");
			if (fp == NULL) {
//				fprintf(stderr, "ERROR: %s open error - %s\n", m_fname, strerror(errno));
				return;
			}
		}

		switch (lvl) {
		case L_CRI:
		case L_ERR:
			if (!m_stdout) {
				snprintf(m_errfname, sizeof(m_errfname), "%s/error-%04d%02d%02d.log",
						m_logpath, stNow.tm_year+1900, stNow.tm_mon+1, stNow.tm_mday);

				logrotate(m_errfname, "error", stNow);

				errfp = fopen(m_errfname, "a");
				if (errfp == NULL) {
//					fprintf(stderr, "ERROR: %s open error - %s\n", m_errfname, strerror(errno));
					return;
				}
			}
			break;
		}

		strftime(szStrTime, sizeof(szStrTime), "%H:%M:%S", &stNow);
		n = vsnprintf(szBuffer, sizeof(szBuffer), fmt, ap);
		for (i=0; i<len; i++) {
			n += snprintf(szBuffer+n, sizeof(szBuffer)-n, " %02x", hex[i]);
		}

		switch (lvl) {
		case L_CRI:
			fprintf(fp, "%s :CRITI: %s (len:%d)\n", szStrTime, szBuffer, len);
			if (errfp) fprintf(errfp, "%s :CRITI: %s (len:%d)\n", szStrTime, szBuffer, len);
			break;
		case L_ERR:
			fprintf(fp, "%s :ERROR: %s (len:%d)\n", szStrTime, szBuffer, len);
			if (errfp) fprintf(errfp, "%s :ERROR: %s (len:%d)\n", szStrTime, szBuffer, len);
			break;
		case L_INFO:
			fprintf(fp, "%s :INFO : %s (len:%d)\n", szStrTime, szBuffer, len);
			break;
		case L_DBG:
			fprintf(fp, "%s :DEBUG: %s (len:%d)\n", szStrTime, szBuffer, len);
			break;
		}

		if (!m_stdout) {
			if (fp) fclose(fp);
		}
	}
}

void CFileLog::logRotate(const char* logfile, struct tm &t_now) {
	struct stat stStat;
	DIR *pDir = NULL;
	struct dirent *pDirent = NULL;
	char szTempFile[256] = {0, };
	char szNewFile[256] = {0, };
	char szCmd[256] = {0, };
	int nFileCount = 0;

	if (stat(logfile, &stStat) == 0) {
		// Over logfile size
		if (stStat.st_size > (long long)MAX_LOGFILE_SIZE) {
			pDir = opendir(m_pLogPath);
			if (pDir == NULL) {
				fprintf(stderr, "ERROR: '%s' can't open directory - %s\n", m_pLogPath, strerror(errno));
				exit(1);
			}
			snprintf(szTempFile, sizeof(szTempFile), "%s-%04d%02d%02d-",
					m_pProcessName, t_now.tm_year+1900, t_now.tm_mon+1, t_now.tm_mday);

			// Search next logfile name index
			while ((pDirent = readdir(pDir)) != 0) {
				if (strstr(pDirent->d_name, szTempFile)) {
					nFileCount++;
				}
			}
			(void)closedir(pDir);

			// MAX logfile count is 999. (%03d)
			snprintf(szNewFile, sizeof(szNewFile), "%s/%s-%04d%02d%02d-%03d.log",
					m_pLogPath, m_pProcessName, t_now.tm_year+1900, t_now.tm_mon+1, t_now.tm_mday, nFileCount);
			if (rename(logfile, szNewFile) == -1) {
				fprintf(stderr, "ERROR: can't rename %s to %s - %s\n", logfile, szNewFile, strerror(errno));
				exit(1);
			}
			snprintf(szCmd, sizeof(szCmd), "touch %s", logfile);
			STDLib::safeSystem(szCmd);
		}
	}
}


} /* namespace kinow */
