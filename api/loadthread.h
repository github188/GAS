#ifndef LOADTHREAD_H
#define LOADTHREAD_H

#include <QThread>

class LoadThread : public QThread
{
    Q_OBJECT
public:
    explicit LoadThread(QObject *parent = 0);

    void stop();

protected:
    void run();

private:
    volatile bool stopped;

};

#endif // LOADTHREAD_H
