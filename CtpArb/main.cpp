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

	using namespace std;

    //CtpArb w;
    //w.show();
	//w_main->show();

	w_login = new LoginForm();
	w_login->show();

    return a.exec();
} 
