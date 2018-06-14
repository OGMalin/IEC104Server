#include "IECWindow.h"
#include "IECDelegate.h"
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include <QBoxLayout>
#include <QStringList>
#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QComboBox>
#include <QDebug>
#include <QStandardItem>
#include <QHeaderView>

IECWindow::IECWindow(QWidget* parent)
	:QWidget(parent)
{
	QVBoxLayout* vbox = new QVBoxLayout;
	QHBoxLayout* hbox = new QHBoxLayout;
	saveTable = new QPushButton("Save");
	readTable = new QPushButton("Reload");

	model = new QStandardItemModel(0,5);
	QStringList header;
	header << "Id" << "Address" << "ASDU" << "SNMP link" << "Description";
	model->setHorizontalHeaderLabels(header);
	table = new QTableView;
	table->setModel(model);
	table->hideColumn(0);
	
	IECAsduDelegate* asduDelegate = new IECAsduDelegate;
	table->setItemDelegateForColumn(2, asduDelegate);

	read();


	hbox->addWidget(readTable);
	hbox->addWidget(saveTable);
	vbox->addLayout(hbox);
	vbox->addWidget(table);
	setLayout(vbox);

	connect(saveTable, SIGNAL(clicked()), this, SLOT(save()));
	connect(readTable, SIGNAL(clicked()), this, SLOT(read()));

	addAct = new QAction("Add", this);
	connect(addAct, &QAction::triggered, this, &IECWindow::add);
	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));
	dirty = false;
}

void IECWindow::save()
{
	if (!dirty)
		return;
	Database db;
	db.add(data);

}

void IECWindow::read()
{
	IECData test;
	test.id = 0;
	test.snmpid = 0;
	test.address = 1234;
	test.description = "For testing";
	test.type = 31;
	Database db;
	db.read(data);
	data.push_back(test);
	data.push_back(test);
	data.push_back(test);
	updateTable();
}

void IECWindow::updateTable()
{
	QStandardItem* item;
	int col, row;
	if (!data.size())
		return;

	model->setRowCount(data.size());
	QVector<IECData>::iterator it = data.begin();
	row = 0;
	while (it != data.end())
	{
		col = 0;
		item = new QStandardItem;
		item->setData(it->id, Qt::EditRole);
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(it->address,Qt::EditRole);
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(asduType[it->type], Qt::EditRole);
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(it->snmpid, Qt::EditRole);
		model->setItem(row, col++, item);
		item = new QStandardItem;
		item->setData(it->description, Qt::EditRole);
		model->setItem(row++, col, item);
		++it;
	}
/*
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

	table->setVerticalHeaderLabels(id);
	table->scrollToBottom();
	*/
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
	int rows = model->rowCount();
	model->setRowCount(rows + 1);

	QStandardItem* item;
	int col = 0;
	item = new QStandardItem;
	item->setData(iec.id, Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(iec.address, Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(asduType[iec.type], Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(iec.snmpid, Qt::EditRole);
	model->setItem(rows, col++, item);
	item = new QStandardItem;
	item->setData(iec.description, Qt::EditRole);
	model->setItem(rows, col, item);
}

void IECWindow::itemChanged(QStandardItem* item)
{
	qDebug() << "itemChanged ( " << item->row() << "," << item->column() << ")";
	int id = model->item(item->row(), 0)->data().toInt();

	switch (item->column())
	{
	case 1:
	}
	dirty = true;
	
}