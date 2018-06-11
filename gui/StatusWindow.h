#pragma once

#include <QWidget>
#include <QString>
#include <QByteArray>

class QTextEdit;

class StatusWindow : public QWidget
{
	Q_OBJECT

public slots:
	void iecReadMessage(const QByteArray&);
	void iecWriteMessage(const QByteArray&);

public:
	StatusWindow(QWidget* parent=0);
	virtual ~StatusWindow();
private:
	QTextEdit* iecLog;
};

