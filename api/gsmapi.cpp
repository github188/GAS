#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "gsmapi.h"
#include "myhelper.h"
#include "api.h"
#include "dbhelper.h"
#include "sendmsgthread.h"

GSMAPI::GSMAPI(QObject *parent) :
    QObject(parent)
{
    portName = "COM1";
    isOpen = false;
    isLive = false;

    timerTask = new QTimer(this);
    timerTask->setInterval(10000);
    connect(timerTask, SIGNAL(timeout()), this, SLOT(DoTask()));
    timerTask->start();
}

void GSMAPI::SetPortName(QString portName)
{
    this->portName = portName;
}

bool GSMAPI::Open()
{
    headMsg = "端口:短信端口";

    GSM_COM = new QextSerialPort(portName);
    isOpen = GSM_COM->open(QIODevice::ReadWrite);
    if (isOpen) {
        GSM_COM->flush();
        GSM_COM->setBaudRate(BAUD9600);
        GSM_COM->setDataBits(DATA_8);
        GSM_COM->setParity(PAR_NONE);
        GSM_COM->setStopBits(STOP_1);
        GSM_COM->setFlowControl(FLOW_OFF);
        GSM_COM->setTimeout(10);
    }
    return isOpen;
}

void GSMAPI::Init()
{
    if (Islive()) {
        //设置短信模式为PDU模式
        SendAT("AT+CMGF=0", 100);
    }
}

bool GSMAPI::Islive()
{
    if (isOpen) {
        GSM_COM->write(QString("%1\r").arg("AT").toLatin1());
        myHelper::Sleep(300);
        QString data = GSM_COM->readAll();
        data = data.replace(" ", "");
        data = data.replace("\r", "");
        data = data.replace("\n", "");
        //收到返回ATOK说明短信猫连接成功
        if (data == "ATOK") {
            isLive = true;
        } else {
            isLive = false;
        }
    } else {
        isLive = false;
    }
    return isLive;
}

void GSMAPI::Close()
{
    GSM_COM->close();
    GSM_COM = 0;
    isLive = false;
    isOpen = false;
}

QString GSMAPI::SendAT(QString cmd, int sleep)
{
    if (!isOpen || !isLive) {
        return "";
    }

    GSM_COM->flush();
    GSM_COM->write(QString("%1\r").arg(cmd).toLatin1());
    myHelper::Sleep(sleep);
    QString data = GSM_COM->readAll();
    data = data.replace(" ", "");
    data = data.replace("\r", "");
    data = data.replace("\n", "");

    return data;
}

//对于多个号码,将号码分割开来,如果短信内容超过70个字符则分割成小于70个字符的短信
void GSMAPI::SendMsgAll(QString tels, QStringList msgs)
{
    QStringList t = tels.split(";");
    foreach (QString tel, t) {
        foreach (QString msg, msgs) {
            task.append(QString("%1|%2").arg(tel).arg(msg));
        }
    }
}

void GSMAPI::SendMsgOne(QString tel, QString msg)
{
    qDebug() << TIME << headMsg << "命令解析:" << "发送告警短信" << "号码:" << tel << "内容:" << msg;

    SendMsgThread *sendMsgThread = new SendMsgThread(this);
    connect(sendMsgThread, SIGNAL(finished()), sendMsgThread, SLOT(deleteLater()));
    sendMsgThread->SetPar(this, tel, msg);
    sendMsgThread->start();
}

QString GSMAPI::GetGSMType()
{
    QString result = SendAT("AT+CGMI", 300);
    QString type = "";
    //如果成功会返回 AT+CGMISIEMENSOK,对应类型要减去开头7位和末尾2位
    if (result.right(2) == "OK") {
        type = result.mid(7, result.length() - 7 - 2);
    }
    return type;
}

QString GSMAPI::GetGSMCenterNum()
{
    QString result = SendAT("AT+CSCA?", 300);
    QString num = "";
    //如果成功会返回 AT+CSCA?+CSCA:"+8613010314500",145OK
    if (result.right(2) == "OK") {
        num = result.split("\"")[1];
    }
    return num;
}

void GSMAPI::DoTask()
{
    int count = task.count();
    if (count <= 0) {
        return;
    }

    QString str = task.takeFirst();
    QStringList list = str.split("|");
    QString tel = list.at(0);
    QString msg = list.at(1);
    SendMsgOne(tel, msg);
}

