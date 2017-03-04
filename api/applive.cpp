#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "applive.h"
#include "myhelper.h"

AppLive *AppLive::_instance = 0;
AppLive::AppLive(QObject *parent) : QObject(parent)
{
	udpServer  = new QUdpSocket(this);
}

bool AppLive::Start(int port)
{
	bool ok = udpServer->bind(port);

	if (ok) {
		connect(udpServer, SIGNAL(readyRead()), this, SLOT(ReadData()));
		qDebug() << TIMEMS << "Start AppLive Ok";
	}

    return ok;
}

void AppLive::Stop()
{
    udpServer->abort();
    disconnect(udpServer, SIGNAL(readyRead()), this, SLOT(ReadData()));
}

void AppLive::ReadData()
{
	QByteArray tempData;

	do {
		tempData.resize(udpServer->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;
		udpServer->readDatagram(tempData.data(), tempData.size(), &sender, &senderPort);
		QString data = QLatin1String(tempData);

		if (data == "hello") {
			udpServer->writeDatagram(QString("%1OK").arg(App::AppName).toLatin1(), sender, senderPort);
		}
	} while (udpServer->hasPendingDatagrams());
}
