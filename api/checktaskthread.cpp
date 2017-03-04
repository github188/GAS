#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "checktaskthread.h"
#include "api/myhelper.h"
#include "api/api.h"
#include "api/dbhelper.h"

CheckTaskThread::CheckTaskThread(QObject *parent) : QThread(parent)
{
    stopped = false;
}

void CheckTaskThread::stop()
{
    stopped = true;
}

void CheckTaskThread::run()
{
    while(!stopped) {
        int taskCount = App::TaskList.count();
        if (taskCount > 0) {
            mutex.lock();
            //每次取出一个任务进行处理
            QString task = App::TaskList.takeFirst();
            mutex.unlock();
            qDebug() << TIMEMS << "task:" << task;
            QStringList list = task.split("|");
            QString type = list.at(0);
            if (type == "Print") {
                QString str = QString("防区:%1[%2]\n类型:%3\n时间:%4")
                              .arg(list.at(2)).arg(list.at(1)).arg(list.at(3)).arg(list.at(4));
                //调用打印函数

                API::AddMessage("打印警情");
                DBHelper::AddEventInfoUser("打印警情");
            } else if (type == "Snap") {
                int snapCount = App::SnapList.count();
                //只有告警图像名称存在时
                if (snapCount > 0) {
                    mutex.lock();
                    QString fileName = App::SnapList.takeFirst();
                    mutex.unlock();

                    //抓拍告警联动图像
                    Snap(list.at(1), list.at(2), list.at(4), fileName);

                    API::AddMessage("告警抓拍图像");
                    DBHelper::AddEventInfoUser("告警抓拍图像");
                }
            } else if (type == "Video") {
                //弹出告警联动视频

                API::AddMessage("告警联动视频");
                DBHelper::AddEventInfoUser("告警联动视频");
            }
        }

        msleep(100);
    }
    stopped = false;
}

QByteArray CheckTaskThread::GetHtml(QString url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
    QEventLoop eventLoop;
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    return reply->readAll();
}

void CheckTaskThread::Snap(QString ip, QString ch, QString time, QString fileName)
{
    //判断文件夹是否存在,不存在则先创建
#ifdef __arm__
    QString savePath = QString("%1/%2")
                       .arg(App::SnapPath)
                       .arg(QDate::currentDate().toString("yyyy-MM-dd"));
#else
    QString savePath = QString("%1%2/%3")
                       .arg(App::AppPath)
                       .arg(App::SnapPath)
                       .arg(QDate::currentDate().toString("yyyy-MM-dd"));
#endif

    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkpath(savePath);
    }

    //构建抓拍告警图像url地址
    QString url = QString("http://%1/doSnapshot?channel=%2&stream=%3&timeago=%4")
                  .arg(ip).arg(ch).arg(1).arg(time);

    QImage image;
    image.loadFromData(GetHtml(url));
    image.save(fileName, "jpg");
}
