#pragma once

#include "Database.h"
#include <QWidget>

class QTableView;
class QStandardItemModel;
class QPushButton;
class QAction;
class QContextMenuEvent;
class QComboBox;
class QStandardItem;

class IECWindow : public QWidget
{
	Q_OBJECT

public slots:
	void save();
	void read();
	void updateTable();
	void add();
	void itemChanged(QStandardItem*);
public:
	IECWindow(QWidget* parent = 0);

private:
	bool dirty;
	QTableView* table;
	QStandardItemModel* model;
	QPushButton* saveTable;
	QPushButton* readTable;
	QVector<IECData> data;
	QAction* addAct;

	void contextMenuEvent(QContextMenuEvent* event);
};
