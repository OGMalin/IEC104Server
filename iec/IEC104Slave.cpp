#include "IEC104Slave.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

IEC104Slave::IEC104Slave(QObject *parent)
	: QObject(parent)
{
	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	start();
}

void IEC104Slave::newConnection()
{
	tcpSocket = tcpServer->nextPendingConnection();
	connect(tcpSocket, SIGNAL(readReady()), this, SLOT(readReady());
}

void IEC104Slave::start()
{
	if (!tcpServer->listen(QHostAddress::Any, 2404))
	{
		qDebug() << "IEC104Slave could not start.";
	}
	else
	{
		qDebug() << "IEC104Slave started.";
	}

}

void IEC104Slave::stop()
{
	tcpSocket->close();
}
