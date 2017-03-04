#ifndef SENDMSGTHREAD_H
#define SENDMSGTHREAD_H

#include <QThread>
#include "gsmapi.h"

class SendMsgThread : public QThread
{
    Q_OBJECT
public:
    explicit SendMsgThread(QObject *parent = 0);

    void SetPar(GSMAPI *gsm, QString tel, QString msg);

protected:
    void run();

private:
    GSMAPI *gsm;
    QString tel;
    QString msg;
    QString headMsg;                            //打印消息头部内容

    QString getTel(QString tel);                //获取手机号码(内存编码)
    int getMsgLen(QString msg);                 //获取短信内容长度
    QString getMsgHex(QString msg);             //获取汉字16进制格式表示

    QString unicodeToTel(QString unicode);      //unicode编码转为号码字符串
    QString unicodeToTime(QString unicode);     //unicode编码转为时间字符串
    QString unicodeToMsg(QString unicode);      //unicode编码转为中文字符串

signals:

public slots:
};

#endif // SENDMSGTHREAD_H
