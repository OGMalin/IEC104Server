#include "IEC104Slave.h"
#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkSession>

IEC104Slave::IEC104Slave(QObject *parent)
	: QObject(parent)
{
	QNetworkConfigurationManager manager;
	QNetworkConfiguration config=manager.defaultConfiguration();

	networkSession = new QNetworkSession(config, this);
}

IEC104Slave::~IEC104Slave()
{
}
