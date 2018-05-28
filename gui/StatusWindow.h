#pragma once

#include <QWidget>

class StatusWindow : public QWidget
{
	Q_OBJECT

public:
	StatusWindow(QWidget* parent=0);
	virtual ~StatusWindow();
};

