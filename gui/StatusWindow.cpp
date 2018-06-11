#include "StatusWindow.h"
#include <QTextEdit>
#include <QGridLayout>
#include <QTextDocument>

StatusWindow::StatusWindow(QWidget* parent)
	:QWidget(parent)
{
	QGridLayout* grid = new QGridLayout;
	iecLog = new QTextEdit();
	iecLog->setReadOnly(true);
	iecLog->document()->setMaximumBlockCount(100);
	grid->addWidget(iecLog);
	setLayout(grid);
}


StatusWindow::~StatusWindow()
{
}

void StatusWindow::iecReadMessage(const QByteArray& data)
{
	iecLog->setTextColor(QColor("red"));
	iecLog->append(data.toHex(' '));
}

void StatusWindow::iecWriteMessage(const QByteArray& data)
{
	iecLog->setTextColor(QColor("blue"));
	iecLog->append(data.toHex(' '));
}
