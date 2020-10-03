#include "CtpArb.h"
#include <QtWidgets/QApplication>
#include "main.h"
#include <regex>

#include <conio.h>
#include <iostream>
#include <QRegExp>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	//cout << "中国";
	//printf("中国");

	//printf("\tInstrumentID [%s]\n", "中国");


	
	


	//_getch();






    CtpArb w;
    w.show();
	w.setWindowTitle("妥妥的套利");
    return a.exec();
} 
