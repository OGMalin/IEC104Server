#pragma once
#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class IEC104Thread : public QThread
{
	Q_OBJECT

public:
	explicit IEC104Thread(int ID, QObject *parent=0);
	void run();

private:
	QTcpSocket * socket;
	int socketDescriptor;

signals:
	void error(QTcpSocket::SocketError socketerror);

public slots:
	void readyRead();
	void disconnected();
};
