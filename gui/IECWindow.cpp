#include "IECWindow.h"
#include <QTableWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QStringList>
#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QComboBox>
#include <QDebug>

IECWindow::IECWindow(QWidget* parent)
	:QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	QHBoxLayout* hbox = new QHBoxLayout;
	saveTable = new QPushButton("Save");
	readTable = new QPushButton("Reload");
	table = new QTableWidget(0, 4);

	hbox->addWidget(readTable);
	hbox->addWidget(saveTable);
	vbox->addLayout(hbox);
	vbox->addWidget(table);
	setLayout(vbox);

	connect(saveTable, SIGNAL(clicked()), this, SLOT(save()));
	connect(readTable, SIGNAL(clicked()), this, SLOT(read()));

	addAct = new QAction("Add", this);
	connect(addAct, &QAction::triggered, this, &IECWindow::add);
	connect(table, SIGNAL(cellChanged(int, int)), this, SLOT(cellChanged(int, int)));
	read();
}

void IECWindow::save()
{

}

void IECWindow::read()
{
	Database db;
	db.read(data);
	updateTable();
}

void IECWindow::updateTable()
{
	QTableWidgetItem* item;
	QComboBox* citem;
	QStringList id;
	int row, col;
	table->clear();
	item = new QTableWidgetItem("Address");
	table->setHorizontalHeaderItem(0, item);
	item = new QTableWidgetItem("Type");
	table->setHorizontalHeaderItem(1, item);
	item = new QTableWidgetItem("SNMP id");
	table->setHorizontalHeaderItem(2, item);
	item = new QTableWidgetItem("Description");
	table->setHorizontalHeaderItem(3, item);
	table->setRowCount(data.size());
	QVector<IECData>::iterator it = data.begin();
	row = 0;
	while (it != data.end())
	{
		col = 0;
		id.append(QString().number(it->id));
		item = new QTableWidgetItem(QString().number(it->address));
		table->setItem(row, col++, item);
		citem = new QComboBox();
		setupAsduCombo(citem, it->type);
		table->setCellWidget(row, col++, citem);
		item = new QTableWidgetItem(QString().number(it->snmpid));
		table->setItem(row, col++, item);
		item = new QTableWidgetItem(it->description);
		table->setItem(row++, col, item);
		++it;
	}
	table->setVerticalHeaderLabels(id);
	table->scrollToBottom();
}

void IECWindow::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu* menu = new QMenu(this);
	menu->addAction(addAct);
	menu->exec(event->globalPos());
}

void IECWindow::add()
{
	IECData iec;
	iec.id = 0;
	iec.address = 0;
	iec.snmpid = 0;
	iec.type = 0;
	data.push_back(iec);
	updateTable();
}

void IECWindow::setupAsduCombo(QComboBox* combo, int selected)
{
	combo->addItem("",0);
	combo->addItem("M_SP_NA_1 (1)", 1);
	combo->addItem("M_DP_NA_1 (3)", 3);
	combo->addItem("M_ME_NA_1 (9)", 9);
	combo->addItem("M_ME_NC_1 (13)", 13);
	combo->addItem("M_SP_TB_1 (30)", 30);
	combo->addItem("M_DP_TB_1 (31)", 31);
	combo->addItem("C_SC_NA_1 (45)", 45);
	combo->addItem("C_DC_NA_1 (46)", 46);
	combo->addItem("C_SC_TA_1 (58)", 58);
	combo->addItem("C_DC_TA_1 (59)", 59);
	for (int i = 0; i < combo->count(); i++)
	{
		if (combo->itemData(i) == selected)
		{
			combo->setCurrentIndex(i);
			return;
		}
	}
}

void IECWindow::cellChanged(int row, int col)
{
	qDebug() << "cell changed ( " << row << "," << col << ")";
}