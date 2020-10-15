#pragma once
#include <QStandardItem>
#include <QStandardItemModel>
#include <QMap>
#include <QString>
#include "ThostFtdcUserApiStruct.h"

extern QStandardItem* addTreeItem(QStandardItem *pitem, char* name);
extern void FillModelInst(QStandardItemModel *modelIns, QMap<QString, CThostFtdcInstrumentField> g_instMap);

extern QString NextOrderRef();
extern QString NextArbOrderId();