#ifndef NTPAPI_H
#define NTPAPI_H

#include <QObject>
#include <QUdpSocket>
#include <QDateTime>
#include <QMutex>

class NTPAPI : public QObject
{
	Q_OBJECT
public:
	explicit NTPAPI(QObject *parent = 0);
	static NTPAPI *Instance()
	{
		static QMutex mutex;

		if (!_instance) {
			QMutexLocker locker(&mutex);

			if (!_instance) {
				_instance = new NTPAPI;
			}
		}

		return _instance;
	}

private:
	static NTPAPI *_instance;
	QUdpSocket *udpSocket;

private slots:
	void ReadData();
	void SendData();

signals:
	void ReceiveTime(QDateTime dateTime);

public slots:
	void GetTime(QString ntpIP);

};

#endif // NTPAPI_H
