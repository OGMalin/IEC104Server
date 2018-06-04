#pragma once

#include <QObject>

class QTcpServer;
class QTcpSocket;

class IEC104Slave : public QObject
{
	Q_OBJECT

public:
	explicit IEC104Slave(QObject *parent = 0);
	virtual ~IEC104Slave();
	void start();
	void stop();

signals:

public slots:
	void newConnection();

private:
	QTcpServer * tcpServer;
	QTcpSocket * tcpSocket;
};
