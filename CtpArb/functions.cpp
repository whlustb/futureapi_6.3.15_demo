#include "functions.h"
#include <windows.h>
#include <QString>

char * gbk2utf8(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//将QString复制到char*中。
char*  QStrToCh(QString str) {
	QByteArray bts = str.toLatin1();
	int len = str.count();
	char* ch = new char[len];
	strcpy(ch, bts.data());
	return ch;

}
