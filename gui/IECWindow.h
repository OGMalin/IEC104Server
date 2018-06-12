#pragma once

#include "Database.h"
#include <QWidget>

class QTableWidget;
class QPushButton;
class QAction;
class QContextMenuEvent;

class IECWindow : public QWidget
{
	Q_OBJECT

public slots:
	void save();
	void read();
	void updateTable();
	void add();

public:
	IECWindow(QWidget* parent = 0);

private:
	QTableWidget* table;
	QPushButton* saveTable;
	QPushButton* readTable;
	QVector<IECData> data;
	QAction* addAct;

	void contextMenuEvent(QContextMenuEvent* event);
};
