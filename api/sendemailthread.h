#ifndef SENDEMAILTHREAD_H
#define SENDEMAILTHREAD_H

#include <QThread>

class SendEmailThread : public QThread
{
    Q_OBJECT
public:
    explicit SendEmailThread(QObject *parent = 0);

    void SetContent(QString content);
    void SetFileName(QString fileName);

protected:
    void run();

private:
    QString content;    //告警邮件内容
    QString fileName;   //附件-告警图像
    QString headMsg;    //打印消息头部内容

signals:

public slots:
};

#endif // SENDEMAILTHREAD_H
