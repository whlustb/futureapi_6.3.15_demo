#include "main.h"
#include "MainForm.h"
#include <QtWidgets/QApplication>
#include <regex>
#include <conio.h>
#include <iostream>
#include <QRegExp>
#include "functions.h"
#include "LoginForm.h"
#include <QTextCodec>
#include "define.h"
#include <QThread>

using namespace std;

LoginForm* w_login = nullptr;
MainWindow* w_main = nullptr;



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	FILE *logfile;

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

    //CtpArb w;
    //w.show();
	//w_main->show();

	w_login = new LoginForm();
	w_login->show();

    return a.exec();
} 
