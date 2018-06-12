#include "IEC104Slave.h"
#
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>

IEC104Slave::IEC104Slave(QObject *parent)
	: QObject(parent)
{
	setup.port = 2404;
	setup.commonaddress = 1;
	masterSN = slaveSN = 0;
	OkToSend = false;
	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	start();
}

IEC104Slave::~IEC104Slave()
{
	APDU apdu;
	apdu.apci.format = U_FORMAT;
	apdu.apci.func = STOPDTACT;
	write(apdu);
	OkToSend = false;
	tcpSocket->close();
}

void IEC104Slave::newConnection()
{
	qDebug() << "IEC104Slave newconnection.";
	tcpSocket = tcpServer->nextPendingConnection();
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));

}

void IEC104Slave::start()
{
	if (!tcpServer->listen(QHostAddress::Any,setup.port))
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

void IEC104Slave::readyRead()
{
	QByteArray ba;
	ba=tcpSocket->readAll();
	emit iecMessage(ba,false);

	APDU apdu,res;
	InformationObject inf;
	apdu.set((BYTE*)ba.data());
	if (!apdu.valid())
		return;
	switch (apdu.apci.format)
	{
	case I_FORMAT:
		masterSN++;
		switch (apdu.asdu.dui.ident)
		{
		case C_SC_NA_1: // Enkel kommando
		case C_DC_NA_1: // Dobbel kommando
		case C_RC_NA_1: // Regulering
		case C_SC_TA_1: // Enkel kommando med tid
		case C_DC_TA_1: // Dobbel kommando med tid
		case C_RC_TA_1: // Regulerering med tid
			if (apdu.asdu.io.size() != 1)
				break;
			emit iecCommand(apdu);
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, apdu.asdu.dui.ident, COT_ACTIVATIONCONFIRM);
			inf = apdu.asdu.io[0];
			res.addIO(inf);
			write(res);
			res.clear();
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, apdu.asdu.dui.ident, COT_ACTIVATIONTERMINATION);
			inf = apdu.asdu.io[0];
			res.addIO(inf);
			write(res);
			break;
		case C_IC_NA_1: // Interrogation
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, C_IC_NA_1, COT_ACTIVATIONCONFIRM);
			inf.address = 0;
			inf.qoi = QOI_STATION_INTERROGATION;
			res.addIO(inf);
			write(res);
			sendInterrogation();
			res.clear();
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, C_IC_NA_1, COT_ACTIVATIONTERMINATION);
			inf.clear();
			inf.address = 0;
			inf.qoi = QOI_STATION_INTERROGATION;
			res.addIO(inf);
			write(res);
			break;
		case C_CI_NA_1: // Counter interrogation
			break;
		case C_CS_NA_1: // Klokke synkronisering.
			res.setAPCI(I_FORMAT);
			res.setDUI(setup.commonaddress, C_CS_NA_1, COT_ACTIVATIONCONFIRM);
			inf.address = 0;
			inf.cp56time = currentTime();
			res.addIO(inf);
			write(res);
			break;
		}
		break;
	case S_FORMAT:
		break;
	case U_FORMAT:
		res.apci.format = U_FORMAT;
		switch (apdu.apci.func)
		{
		case STARTDTACT:
			res.apci.func = STARTDTCON;
			OkToSend = true;
			write(res, false);
			spool();
			break;
		case STOPDTACT:
			res.apci.func = STOPDTCON;
			write(res,false);
			OkToSend = false;
			break;
		case TESTFRACT:
			res.apci.func = TESTFRCON;
			write(res);
			break;
		}
		break;
	}

}

void IEC104Slave::connected()
{
	qDebug() << "IEC104Naster connected.";
}

void IEC104Slave::write(APDU& apdu, bool checkspool)
{
	BYTE data[255];
	int len;
	if (checkspool && !OkToSend)
	{
		spool(apdu);
		return;
	}
	if (apdu.apci.format == I_FORMAT)
	{
		apdu.apci.ssn = slaveSN++;
		apdu.apci.rsn = masterSN;
		if (slaveSN > 32767)
			slaveSN = 0;
	}
	len = apdu.get(data);
	QByteArray ba;
	if (len)
	{
		ba.setRawData((char*)data, len);
		emit iecMessage(ba, true);
		tcpSocket->write(ba);
		Sleep(30); // Venter 30 msec slik at meldingene ikke kommer for tett. (spessielt svar på IC)
	}
}

void IEC104Slave::sendInterrogation()
{

	int n;
	APDU apdu;
	QVector<IECValue>::iterator it;
	InformationObject inf;

	// Send enkelt meldinger
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_SP_NA_1)
		{
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_SP_NA_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.siq.SPI = it->nVal;
			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

	// Send doble meldinger
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_DP_NA_1)
		{
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_DP_NA_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.diq.DPI = it->nVal;
			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

	// Send normalised målinger
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_ME_NA_1)
		{
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_ME_NA_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.nva = it->nVal;
			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

	// Send floating point målinger
	it = stored.begin();
	n = 0;
	while (it != stored.end())
	{
		if (it->type == M_ME_NC_1)
		{
			if (n == 0)
			{
				apdu.clear();
				apdu.setAPCI(I_FORMAT);
				apdu.setDUI(setup.commonaddress, M_ME_NC_1, COT_INTERROGATION);
			}
			inf.clear();
			inf.address = it->id;
			inf.value = it->fVal;
			apdu.addIO(inf);
			n++;
			if (n == 10)
			{
				write(apdu);
				n = 0;
			}
		}
		++it;
	}
	if ((n>0) && (n<10))
		write(apdu);

}

void IEC104Slave::setVal(int ident, int addr, float val)
{
	QVector<IECValue>::iterator sit;
	QList<APDU>::iterator it;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->fVal == val)
				return;
			sit->fVal = val;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_ME_NC_1;
		v.fVal = val;
		stored.push_back(v);
	}

	// Lag meldingen
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.value = val;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Check if this message is in the spoolQue
	it = spoolQue.begin();
	while (it != spoolQue.end())
	{
		if ((it->asdu.dui.ident == ident) && (it->asdu.dui.qualifier.Number == 1))
		{
			if (it->asdu.io[0].address == addr)
			{
				spoolQue.erase(it);
				break;
			}
		}
		++it;
	}
	// Send meldingen
	write(apdu);
}

void IEC104Slave::setNva(int ident, int addr, int nva)
{
	QVector<IECValue>::iterator sit;
	QList<APDU>::iterator it;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->nVal == nva)
				return;
			sit->nVal = nva;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_ME_NA_1;
		v.nVal = nva;
		stored.push_back(v);
	}

	// Lag meldingen
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.nva = nva;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Check if this message is in the spoolQue
	it = spoolQue.begin();
	while (it != spoolQue.end())
	{
		if ((it->asdu.dui.ident == ident) && (it->asdu.dui.qualifier.Number == 1))
		{
			if (it->asdu.io[0].address == addr)
			{
				spoolQue.erase(it);
				break;
			}
		}
		++it;
	}

	// Send meldingen
	write(apdu);
}

void IEC104Slave::setSpi(int ident, int addr, int spi)
{
	bool exist = false;
	QVector<IECValue>::iterator sit;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->nVal == spi)
				return;
			sit->nVal = spi;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_SP_NA_1;
		v.nVal = spi;
		stored.push_back(v);
	}

	// Lag meldingen
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.siq.SPI = spi;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Send meldingen
	write(apdu);
}

void IEC104Slave::setDpi(int ident, int addr, int dpi)
{
	bool exist = false;
	QVector<IECValue>::iterator sit;
	APDU apdu;
	ASDU asdu;
	InformationObject inf;

	// Lagre verdien
	sit = stored.begin();
	while (sit != stored.end())
	{
		if (sit->id == addr)
		{
			if (sit->nVal == dpi)
				return;
			sit->nVal = dpi;
			break;
		}
		++sit;
	}
	if (sit == stored.end())
	{
		IECValue v;
		v.id = addr;
		v.type = M_DP_NA_1;
		v.nVal = dpi;
		stored.push_back(v);
	}

	// Lag meldingen
	apdu.setAPCI(I_FORMAT);
	apdu.setDUI(setup.commonaddress, ident, COT_SPONTANEOUS);
	inf.address = addr;
	inf.diq.DPI = dpi;
	inf.cp56time = currentTime();
	apdu.addIO(inf);

	// Send meldingen
	write(apdu);
}

void IEC104Slave::spool()
{
	if (!OkToSend)
		return;

	QList<APDU>::iterator it = spoolQue.begin();
	while (it != spoolQue.end())
	{
		write(*it);
		++it;
	}
	spoolQue.clear();
}

void IEC104Slave::spool(APDU& apdu)
{
	spoolQue.push_back(apdu);
}
