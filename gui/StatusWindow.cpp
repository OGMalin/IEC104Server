#include "StatusWindow.h"
#include <QTextEdit>
#include <QGridLayout>
#include <QTextDocument>
#include "../iec/APDU.h"

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

void StatusWindow::iecMessage(const QByteArray& data, bool write)
{
	if (write)
		iecLog->setTextColor(QColor("blue"));
	else
		iecLog->setTextColor(QColor("red"));
	iecLog->append(data.toHex(' '));
}

