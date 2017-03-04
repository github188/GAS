#ifndef APPLIVE_H
#define APPLIVE_H

#include <QObject>
#include <QMutex>

class QUdpSocket;

class AppLive : public QObject
{
	Q_OBJECT
public:
	explicit AppLive(QObject *parent = 0);
    static AppLive *Instance()
    {
        static QMutex mutex;

        if (!_instance) {
            QMutexLocker locker(&mutex);

            if (!_instance) {
                _instance = new AppLive;
            }
        }

        return _instance;
    }

    bool Start(int port);
    void Stop();

private slots:
	void ReadData();

private:
    static AppLive *_instance;
	QUdpSocket *udpServer;

signals:

public slots:
};

#endif // APPLIVE_H
