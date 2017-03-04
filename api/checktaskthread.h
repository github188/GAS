#ifndef CHECKTASKTHREAD_H
#define CHECKTASKTHREAD_H

#include <QThread>
#include <QMutex>

class CheckTaskThread : public QThread
{
    Q_OBJECT
public:
    explicit CheckTaskThread(QObject *parent = 0);

    void stop();

protected:
    void run();

private:
    QMutex mutex;
    volatile bool stopped;

    QByteArray GetHtml(QString url);
    void Snap(QString ip, QString ch, QString time, QString fileName);

};

#endif // CHECKTASKTHREAD_H
