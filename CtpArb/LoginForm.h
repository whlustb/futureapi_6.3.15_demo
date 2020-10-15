#pragma once
#include <QWidget>
#include "ui_LoginForm.h"
#include "HandlerVars.h"
#include "HandlerQuote.h"
#include "HandlerTrade.h"

 

class LoginForm : public QWidget
{
	Q_OBJECT

public:
	LoginForm(QWidget *parent = Q_NULLPTR);
	~LoginForm();

public slots:
	void Login();

private:
	Ui::LoginForm ui;

	
};
