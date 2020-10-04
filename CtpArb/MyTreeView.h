#pragma once
#include <QTreeView>
#include <QMouseEvent>

class MyTreeView :
	public QTreeView
{
	Q_OBJECT

public:
	explicit MyTreeView(QWidget *parent = 0);
	~MyTreeView();

protected:
	virtual void mousePressEvent(QMouseEvent *e);  //添加鼠标点击事件

signals:
	void clicked();  //自定义点击信号，在mousePressEvent事件发生时触发
};

