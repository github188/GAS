#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "ntpapi.h"

NTPAPI *NTPAPI::_instance = 0;
NTPAPI::NTPAPI(QObject *parent) : QObject(parent)
{
	udpSocket = 0;
}

void NTPAPI::GetTime(QString ntpIP)
{
	udpSocket = new QUdpSocket(this);
	connect(udpSocket, SIGNAL(connected()), this, SLOT(SendData()));
	connect(udpSocket, SIGNAL(readyRead()), this, SLOT(ReadData()));
	udpSocket->connectToHost(ntpIP, 123);
}

void NTPAPI::SendData()
{
	qint8 LI = 0;
	qint8 VN = 3;
	qint8 MODE = 3;
	qint8 STRATUM = 0;
	qint8 POLL = 4;
	qint8 PREC = -6;
	QDateTime Epoch(QDate(1900, 1, 1));
	qint32 second = quint32(Epoch.secsTo(QDateTime::currentDateTime()));

	qint32 temp = 0;
	QByteArray timeRequest(48, 0);
	timeRequest[0] = (LI << 6) | (VN << 3) | (MODE);
	timeRequest[1] = STRATUM;
	timeRequest[2] = POLL;
	timeRequest[3] = PREC & 0xff;
	timeRequest[5] = 1;
	timeRequest[9] = 1;
	timeRequest[40] = (temp = (second & 0xff000000) >> 24);
	temp = 0;
	timeRequest[41] = (temp = (second & 0x00ff0000) >> 16);
	temp = 0;
	timeRequest[42] = (temp = (second & 0x0000ff00) >> 8);
	temp = 0;
	timeRequest[43] = ((second & 0x000000ff));

	udpSocket->flush();
	udpSocket->write(timeRequest);
	udpSocket->flush();
}

void NTPAPI::ReadData()
{
	QByteArray newTime;
	QDateTime Epoch(QDate(1900, 1, 1));
	QDateTime unixStart(QDate(1970, 1, 1));

	do {
		newTime.resize(udpSocket->pendingDatagramSize());
		udpSocket->read(newTime.data(), newTime.size());
	} while (udpSocket->hasPendingDatagrams());

	QByteArray TransmitTimeStamp ;
	TransmitTimeStamp = newTime.right(8);
	quint32 seconds = TransmitTimeStamp.at(0);
	quint8 temp = 0;

	for (int i = 1; i <= 3; i++) {
		seconds = (seconds << 8);
		temp = TransmitTimeStamp.at(i);
		seconds = seconds + temp;
	}

	QDateTime dateTime;
	dateTime.setTime_t(seconds - Epoch.secsTo(unixStart));

#ifdef __arm__
    //dateTime = dateTime.addSecs(60 * 60 * 8);
#endif
	udpSocket->disconnectFromHost();
	udpSocket->close();
	udpSocket = 0;

	emit ReceiveTime(dateTime);
}
