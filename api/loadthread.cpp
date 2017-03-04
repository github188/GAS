#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "loadthread.h"
#include <QMovie>
#include <QLabel>
#include <QDesktopWidget>
#include <QApplication>

LoadThread::LoadThread(QObject *parent) : QThread(parent)
{
    stopped = false;
}

void LoadThread::stop()
{
    stopped = true;
}

void LoadThread::run()
{
    QMovie *movie = new QMovie(":/image/loading.gif");
    QLabel *label = new QLabel("", 0);

    label->setAttribute(Qt::WA_DeleteOnClose);
    label->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    label->setScaledContents(true);
    label->setMovie(movie);
    movie->start();
    label->show();

    int frmX = label->width();
    int frmY = label->height();
    QDesktopWidget w;
    int deskWidth = w.availableGeometry().width();
    int deskHeight = w.availableGeometry().height();
    QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
    label->move(movePoint);

    while(!stopped) {
        qApp->processEvents();
        msleep(10);
    }

    label->close();
    delete movie;
}
