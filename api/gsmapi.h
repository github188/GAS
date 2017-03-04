#ifndef GSMAPI_H
#define GSMAPI_H

#include <QObject>
#include "qextserialport/qextserialport.h"

class QTimer;

class GSMAPI : public QObject
{
    Q_OBJECT
public:
    explicit GSMAPI(QObject *parent = 0);

    void SetPortName(QString portName);         //设置串口号
    bool Open();                                //打开串口
    bool IsOpen()const {
        return isOpen;                          //串口是否打开
    }
    void Init();                                //初始化短信猫
    bool Islive();                              //检测短信猫是否活着
    void Close();                               //关闭串口

    QString SendAT(QString cmd, int sleep);     //执行命令
    void SendMsgOne(QString tel, QString msg);  //单个号码发送短信
    void SendMsgAll(QString tels, QStringList msgs); //批量号码长短信发送

    QString GetGSMType();                       //获取短信猫厂商标识
    QString GetGSMCenterNum();                  //获取短信中心号码

private:
    QextSerialPort *GSM_COM;                    //短信猫通信串口对象
    QString portName;                           //串口号
    bool isOpen;                                //串口是否打开
    bool isLive;                                //短信猫是否活着
    QString headMsg;                            //打印消息头部内容

    QTimer *timerTask;                          //处理读取短信队列
    QList<QString> task;                        //读取短信和删除短信队列

private slots:
    void DoTask();                              //处理任务队列

};

#endif // GSMAPI_H
