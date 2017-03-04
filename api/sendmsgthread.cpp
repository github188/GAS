#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "sendmsgthread.h"
#include "myhelper.h"
#include "api.h"
#include "dbhelper.h"

SendMsgThread::SendMsgThread(QObject *parent) : QThread(parent)
{
    gsm = 0;
    tel = "18001797656";
    msg = "hello";
    headMsg = "端口:短信端口";
}

void SendMsgThread::SetPar(GSMAPI *gsm, QString tel, QString msg)
{
    this->gsm = gsm;
    this->tel = tel;
    this->msg = msg;
}

void SendMsgThread::run()
{
    QString str;

    if (gsm->Islive()) {
        str = "告警短信发送失败";
        char ctrlz  = 26;
        QString msgContent = QString("%1%2%3%4%5")
                             .arg("0011000D91")
                             .arg(getTel(tel))
                             .arg("000801")
                             .arg(getMsgHex(msg))
                             .arg(ctrlz);
        int len = getMsgLen(msg);

        //首先进入发送短信状态
        QString result = gsm->SendAT(QString("AT+CMGS=%1\r").arg(len), 300);

        //返回结果最末一位为 > 后输入短信内容编码
        if (result.right(1) == ">") {
            //发送短信正文
            result = gsm->SendAT(msgContent, 8000);
            if (result.right(2) == "OK") {
                str = "告警短信发送成功";
            }
        }
    } else {
        str = "短信猫通讯故障";
    }

    qDebug() << TIME << headMsg << "命令解析:" << str;
    API::AddMessage(str);
    DBHelper::AddEventInfoUser(str);
}

int SendMsgThread::getMsgLen(QString msg)
{
    int len = msg.length() * 2;
    len += 15;
    return len;
}

QString SendMsgThread::getTel(QString tel)
{
    QString temp = "";
    //检查手机号码是否按照标准格式写,如果不是则补上
    if (tel.mid(0, 2) != "86") {
        temp = QString("86%1F").arg(tel);
    }

    QByteArray ba = temp.toLatin1();
    QString str = "";
    //按照内存编码格式,将每两位的顺序调换
    for (int i = 0; i < 13; i = i + 2) {
        str += QString("%1%2").arg(ba[i + 1]).arg(ba[i]);
    }
    return str;
}

QString SendMsgThread::getMsgHex(QString msg)
{
    int len = msg.length();
    wchar_t *buffer = new wchar_t[len];
    msg.toWCharArray((wchar_t *)buffer);

    QStringList data;
    for (int i = 0; i < len; i++) {
        QString temp = QString::number(buffer[i], 16);
        if (temp.length() == 2) {
            temp = "00" + temp;
        }
        data.append(temp);
    }
    //将长度及短信内容的unicode编码格式内容拼接
    QString str = QString("%1%2")
                  .arg(QString::number(len * 2, 16).toUpper())
                  .arg(data.join("").toUpper());

    delete [] buffer;
    return str;
}

QString SendMsgThread::unicodeToTel(QString unicode)
{
    QString tel = "";
    //按照内存编码格式,将每两位的顺序调换
    for (int i = 0; i < 12; i = i + 2) {
        tel += QString("%1%2").arg(unicode[i + 1]).arg(unicode[i]);
    }
    //去除最后一位 F
    tel = tel.remove("F");
    return tel;
}

QString SendMsgThread::unicodeToTime(QString unicode)
{
    QString time = "";
    //按照内存编码格式,将每两位的顺序调换
    for (int i = 0; i < 12; i = i + 2) {
        time += QString("%1%2").arg(unicode[i + 1]).arg(unicode[i]);
    }
    time = QString("20%1-%2-%3 %4:%5:%6")
           .arg(time.mid(0, 2))
           .arg(time.mid(2, 2))
           .arg(time.mid(4, 2))
           .arg(time.mid(6, 2))
           .arg(time.mid(8, 2))
           .arg(time.mid(10, 2));
    return time;
}

QString SendMsgThread::unicodeToMsg(QString unicode)
{
    QString msg = "";
    QTextCodec *codec = QTextCodec::codecForName("utf-8");

    QStringList str;
    for(int i = 0; i < unicode.length(); i = i + 4) {
        str.append(unicode.mid(i, 4));
    }

    QString s;
    foreach (const QString & unicode, str) {
        s.append(unicode.toUShort(0, 16));
    }

    msg = codec->fromUnicode(s);
    return msg;
}
