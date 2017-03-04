#ifndef APPINIT_H
#define APPINIT_H

#include <QObject>
#include <QMutex>

class AppInit : public QObject
{
    Q_OBJECT
public:
    explicit AppInit(QObject *parent = 0);
    static AppInit *Instance()
    {
        static QMutex mutex;

        if (!_instance) {
            QMutexLocker locker(&mutex);

            if (!_instance) {
                _instance = new AppInit;
            }
        }

        return _instance;
    }

    void Start();

protected:
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static AppInit *_instance;

};

#endif // APPINIT_H
