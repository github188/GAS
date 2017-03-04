#ifndef APPLOG_H
#define APPLOG_H

#include <QObject>
#include <QMutex>

class AppLog : public QObject
{
	Q_OBJECT
public:
	explicit AppLog(QObject *parent = 0);
	static AppLog *Instance()
	{
		static QMutex mutex;

		if (!_instance) {
			QMutexLocker locker(&mutex);

			if (!_instance) {
				_instance = new AppLog;
			}
		}

		return _instance;
	}

	void Start();
	void Stop();

private:
	static AppLog *_instance;

signals:

public slots:
};

#endif // APPLOG_H
