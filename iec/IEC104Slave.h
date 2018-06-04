#pragma once

#include <QObject>
#include <QString>

class QTcpServer;
class QNetworkSession;

class IEC104Slave : public QObject
{
	Q_OBJECT

private:
	QTcpServer * tcpServer = nullptr;
	QNetworkSession* networkSession = nullptr;
	QString masterAddress;
	int port;
public:
	IEC104Slave(QObject *parent);
	~IEC104Slave();
};
