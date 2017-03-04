#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "ds7400_com.h"
#include "myhelper.h"

/* 说明:DS7400主机操作实现文件
 * 功能:动态识别主机上线下线及布防撤防报警等信号
 * 作者:刘典武  QQ:517216493
 * 时间:2014-1-5  检查:2014-1-10
 */
DS7400_COM::DS7400_COM(QObject *parent) :
    QObject(parent)
{
    isOnLine = false;
    isCheckOnline = true;
    lastHeartTime = QDateTime::currentDateTime();
    isOpen = false;

    errorAC = false;       //交流电源故障
    errorDC = false;       //备用电池故障
    errorCode = false;     //报告发送故障
    errorControl = false;  //控制故障
    error485 = false;      //总线故障
    errorWifi = false;     //无线接收故障
    errorPower = false;    //辅助电源故障
    errorDevice = false;   //设备故障

    //启动定时器判断在线状态
    timerOnLine = new QTimer(this);
    timerOnLine->setInterval(3000);
    connect(timerOnLine, SIGNAL(timeout()), this, SLOT(CheckOnLine()));

    //启动定时器读取串口数据
    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead,SIGNAL(timeout()),this,SLOT(ReadData()));
}

bool DS7400_COM::Open()
{
    hostCOM = new QextSerialPort(this->hostConnectValue1, QextSerialPort::Polling);
    isOpen = hostCOM->open(QIODevice::ReadOnly);

    if (isOpen) {
        hostCOM->flush();
        hostCOM->setBaudRate(BAUD2400);
        hostCOM->setDataBits(DATA_8);
        hostCOM->setParity(PAR_NONE);
        hostCOM->setStopBits(STOP_1);
        hostCOM->setFlowControl(FLOW_OFF);
        hostCOM->setTimeout(10);

        timerOnLine->start();
        timerRead->start();
    }
    return isOpen;
}

void DS7400_COM::Close()
{    
    timerOnLine->stop();
    timerRead->stop();
    hostCOM->close();
    hostCOM = 0;
    isOpen = false;
}

void DS7400_COM::CheckOnLine()
{
    //计算最后心跳时间到现在相差多少,如果小于18秒,说明有过心跳,还活着,不做下线处理
    //主机定时16秒向串口发送消息
    QDateTime timeNow = QDateTime::currentDateTime();
    int timeValue = lastHeartTime.secsTo(timeNow);

    if (timeValue <= 18) {
        return;
    }

    //如果当前主机状态为在线,并且还没有检测过下线状态,说明现在主机下线
    if (isOnLine && !isCheckOnline) {
        isOnLine = false;
        isCheckOnline = true;
        emit HostDisConnect(hostID, hostName); //发射主机下线信号
    }
}

void DS7400_COM::ReadData()
{
    //这个判断尤为重要,否则的话直接延时再接收数据,空闲时和会出现高内存占用
    if (hostCOM->bytesAvailable() <= 0) {
        return;
    }

    //将心跳时间改为当前时间,将在线状态置为真
    lastHeartTime = QDateTime::currentDateTime();
    isOnLine = true;

    //如果处于下线状态,说明是首次收到数据,代表主机上线
    if (isCheckOnline) {
        isCheckOnline = false;
        emit HostConnect(hostID, hostName);
    }

    QByteArray data = hostCOM->readAll();

    int len = data.length();
    int index = 0;
    if (len < 3) {
        return;
    }

    //转换为16进制字符串
    QString temp = myHelper::ByteArrayToHexStr(data);
    qDebug() << TIME << temp;
    emit HostReceiveData(hostID, hostName, temp);

    QStringList buffer = temp.split(" ");
    while(index < len) {
        //A0 01 5E(系统状态代码,每隔16秒钟一个数据包)
        //85 0E 00 74用户操作主机时发送的事件代码
        if (buffer.at(index) != "A0" && buffer.at(index) != "85") {
            index++;//移动指针到下一个数据头部
            continue;//返回,执行下一个循环
        }

        if (buffer.at(index) == "A0") {
            QString code = myHelper::StrHexToStrBin(buffer.at(index + 1));
            CheckCode(code);
            index = index + 3; //移动指针到下一个数据头部
            emit HostHeart(hostID, hostName); //发射心跳信号
        } else {
            QString code = buffer.at(index + 1);
            QString codex = buffer.at(index + 2);
            int tempID = myHelper::StrHexToDecimal(codex) + 1; //这里0开始编号,所以要+1

            //0E为主机布防
            //0D为撤防/0C为消警
            //07为盗警/01为火警
            //0F为强制旁路/10为防区旁路
            //18为防区开路/0A为火警防区故障/0B为盗警防区故障
            //17为防区短路状态/19为防区正常状态
            if (code == "0E") {
                emit HostBuFangSubSystem(hostID, hostName, tempID);
            } else if(code == "0D" || code == "0C") {
                emit HostCheFangSubSystem(hostID, hostName, tempID);
            } else if(code == "07" || code == "01") {
                emit DefenceBaoJing(hostID, hostName, GetDefenceID(tempID));
            } else if(code == "0F" || code == "10") {
                emit DefencePangLu(hostID, hostName, GetDefenceID(tempID));
            } else if(code == "18" || code == "0A" || code == "0B") {
                emit DefenceGuZhang(hostID, hostName, GetDefenceID(tempID));
            } else if(code == "17" || code == "19") {
                emit DefenceHuiFu(hostID, hostName, GetDefenceID(tempID));
            }

            index = index + 4; //移动指针到下一个数据头部
        }
    }
}

QString DS7400_COM::GetDefenceID(int tempID)
{
    QString defenceID;
    if (tempID < 10) {
        defenceID = QString("00%1").arg(tempID);
    } else if (tempID < 100) {
        defenceID = QString("0%1").arg(tempID);
    } else if (tempID < 1000) {
        defenceID = QString("%1").arg(tempID);
    }
    return defenceID;
}

void DS7400_COM::CheckCode(QString code)
{
    if (code.mid(7, 1) == "1") {
        if (!errorAC) {
            emit HostError(hostID, hostName, "交流电源故障");
            errorAC = true;
        }
    } else {
        errorAC = false;
    }

    if (code.mid(6, 1) == "1") {
        if (!errorDC) {
            emit HostError(hostID, hostName, "备用电池故障");
            errorDC = true;
        }
    } else {
        errorDC = false;
    }

    if (code.mid(5, 1) == "1") {
        if (!errorCode) {
            emit HostError(hostID, hostName, "报告故障");
            errorCode = true;
        }
    } else {
        errorCode = false;
    }

    if (code.mid(4, 1) == "1") {
        if (!errorControl) {
            emit HostError(hostID, hostName, "控制故障");
            errorControl = true;
        }
    } else {
        errorControl = false;
    }

    if (code.mid(3, 1) == "1") {
        if (!error485) {
            emit HostError(hostID, hostName, "总线故障");
            error485 = true;
        }
    } else {
        error485 = false;
    }

    if (code.mid(2, 1) == "1") {
        if (!errorWifi) {
            emit HostError(hostID, hostName, "无线接收故障");
            errorWifi = true;
        }
    } else {
        errorWifi = false;
    }

    if (code.mid(1, 1) == "1") {
        if (!errorPower) {
            emit HostError(hostID, hostName, "辅助电源故障");
            errorPower = true;
        }
    } else {
        errorPower = false;
    }

    if (code.mid(0, 1) == "1") {
        if (!errorDevice) {
            emit HostError(hostID, hostName, "设备故障");
            errorDevice = true;
        }
    } else {
        errorDevice = false;
    }
}
