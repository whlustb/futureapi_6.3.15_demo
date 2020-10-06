#include "CtpArb.h"
#include <QtWidgets/QApplication>
#include "main.h"
#include <regex>

#include <conio.h>
#include <iostream>
#include <QRegExp>
#include "functions.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	/*
	char* ch;
	QString str = "北京";
	QByteArray cdata = str.toLocal8Bit();
	string ss =  string(cdata); //正常。
	char* ch2 = const_cast<char*>(ss.c_str()); //正常。
	QString str22 = QString((ch2)); //乱码，
	QString str2 = QString(gbk2utf8(ch2)); //正常，
	QString str3  = QString::fromStdString(ss); //乱码。
	_getch();
	*/

    CtpArb w;
    w.show();
    return a.exec();
} 
