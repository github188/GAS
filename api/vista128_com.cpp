#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "vista128_com.h"
#include "myhelper.h"

//预定义软件对主机操作的命令
#define Heart           "08as0064\r\n"                  //询问心跳命令
#define Status          "08zs004B\r\n"                  //询问状态命令
#define PangLu_Defence  "0EKS141406005D\r\n"            //进入旁路模式命令
#define BuFang_System1  "16AA00123410000000000C\r\n"    //子系统1布防
#define BuFang_System2  "16AA00123420000000000B\r\n"    //子系统2布防
#define BuFang_System3  "16AA00123430000000000A\r\n"    //子系统3布防
#define BuFang_System4  "16AA001234400000000009\r\n"    //子系统4布防
#define BuFang_System5  "16AA001234500000000008\r\n"    //子系统5布防
#define BuFang_System6  "16AA001234600000000007\r\n"    //子系统6布防
#define BuFang_System7  "16AA001234700000000006\r\n"    //子系统7布防
#define BuFang_System8  "16AA001234800000000005\r\n"    //子系统8布防
#define CheFang_System1 "16AD001234100000000009\r\n"    //子系统1撤防
#define CheFang_System2 "16AD001234200000000008\r\n"    //子系统2撤防
#define CheFang_System3 "16AD001234300000000007\r\n"    //子系统3撤防
#define CheFang_System4 "16AD001234400000000006\r\n"    //子系统4撤防
#define CheFang_System5 "16AD001234500000000005\r\n"    //子系统5撤防
#define CheFang_System6 "16AD001234600000000004\r\n"    //子系统6撤防
#define CheFang_System7 "16AD001234700000000003\r\n"    //子系统7撤防
#define CheFang_System8 "16AD001234800000000002\r\n"    //子系统8撤防

VISTA128_COM::VISTA128_COM(QObject *parent) :
    QObject(parent)
{
    isOpen = false;
    isOnLine = false;
    isCheckOnline = true;
    lastHeartTime = QDateTime::currentDateTime();

    //启动定时器判断在线状态
    timerOnLine = new QTimer(this);
    timerOnLine->setInterval(5000);
    connect(timerOnLine, SIGNAL(timeout()), this, SLOT(CheckOnLine()));

    //启动定时器每隔10秒钟发送询问心跳命令
    timerHeart = new QTimer(this);
    timerHeart->setInterval(10000);
    connect(timerHeart, SIGNAL(timeout()), this, SLOT(SendHeartData()));

    //启动定时器每隔20秒发送询问防区状态命令
    timerStatus = new QTimer(this);
    timerStatus->setInterval(20000);
    connect(timerStatus, SIGNAL(timeout()), this, SLOT(SendStatusData()));
}

bool VISTA128_COM::Open()
{
    hostCOM = new QextSerialPort(this->hostConnectValue1, QextSerialPort::EventDriven);
    connect(hostCOM, SIGNAL(readyRead()), this, SLOT(ReadData()));
    bool IsOpen = hostCOM->open(QIODevice::ReadWrite);

    if (IsOpen) {
        hostCOM->flush();//清空串口缓冲区
        hostCOM->setBaudRate(BAUD9600);
        hostCOM->setDataBits(DATA_8);
        hostCOM->setParity(PAR_NONE);
        hostCOM->setStopBits(STOP_1);
        hostCOM->setFlowControl(FLOW_OFF);
        hostCOM->setTimeout(10);

        //开启定时器判断主机上线下线及发送心跳和询问防区状态命令
        timerOnLine->start();
        timerHeart->start();
        timerStatus->start();
        isOpen = true;

        //首次打开串口成功后发送询问心跳命令及询问防区状态命令
        SendHeartData();
        SendStatusData();
    }
    return IsOpen;
}

void VISTA128_COM::Close()
{
    disconnect(hostCOM, SIGNAL(readyRead()), this, SLOT(ReadData()));
    timerOnLine->stop();
    timerHeart->stop();
    timerStatus->stop();
    hostCOM->close();
    hostCOM = 0;
    isOpen = false;
}

void VISTA128_COM::BuFang(int subSystem)
{
    QString data = BuFang_System1;
    switch (subSystem) {
    case 1:
        data = BuFang_System1;
        break;
    case 2:
        data = BuFang_System2;
        break;
    case 3:
        data = BuFang_System3;
        break;
    case 4:
        data = BuFang_System4;
        break;
    case 5:
        data = BuFang_System5;
        break;
    case 6:
        data = BuFang_System6;
        break;
    case 7:
        data = BuFang_System7;
        break;
    case 8:
        data = BuFang_System8;
        break;
    }

    if (isOpen) {
        hostCOM->write(data.toLatin1());
    }
}

void VISTA128_COM::CheFang(int subSystem)
{
    QString data = CheFang_System1;
    switch (subSystem) {
    case 1:
        data = CheFang_System1;
        break;
    case 2:
        data = CheFang_System2;
        break;
    case 3:
        data = CheFang_System3;
        break;
    case 4:
        data = CheFang_System4;
        break;
    case 5:
        data = CheFang_System5;
        break;
    case 6:
        data = CheFang_System6;
        break;
    case 7:
        data = CheFang_System7;
        break;
    case 8:
        data = CheFang_System8;
        break;
    }

    if (isOpen) {
        hostCOM->write(data.toLatin1());
    }
}

QString VISTA128_COM::GetCode(QString defenceID)
{
    QString code;
    int id;

    if (defenceID.mid(0, 2) == "00") {
        id = defenceID.mid(2, 1).toInt();
    } else if (defenceID.mid(0, 1) == "0") {
        id = defenceID.mid(1, 2).toInt();
    } else {
        id = defenceID.toInt();
    }

    if (id < 10) {
        int index = 206 - id;
        code = QString::number(index, 16);
    } else if (id < 20) {
        int index = 215 - id;
        code = QString::number(index, 16);
    } else if (id < 30) {
        int index = 224 - id;
        code = QString::number(index, 16);
    } else if (id < 40) {
        int index = 233 - id;
        code = QString::number(index, 16);
    } else if (id < 50) {
        int index = 242 - id;
        code = QString::number(index, 16);
    } else if (id < 60) {
        int index = 251 - id;
        code = QString::number(index, 16);
    } else if (id < 70) {
        int index = 260 - id;
        code = QString::number(index, 16);
    } else if (id < 80) {
        int index = 269 - id;
        code = QString::number(index, 16);
    } else if (id < 90) {
        int index = 278 - id;
        code = QString::number(index, 16);
    } else if (id < 100) {
        int index = 287 - id;
        code = QString::number(index, 16);
    } else if (id < 110) {
        int index = 305 - id;
        code = QString::number(index, 16);
    } else if (id < 120) {
        int index = 314 - id;
        code = QString::number(index, 16);
    } else if (id < 130) {
        int index = 323 - id;
        code = QString::number(index, 16);
    }

    return code.toUpper();
}

void VISTA128_COM::PangLu(QString defenceID)
{
    this->HuiFu(defenceID);
}

void VISTA128_COM::HuiFu(QString defenceID)
{
    if (isOpen) {
        hostCOM->write(PangLu_Defence);
        myHelper::Sleep(300);
        QString data = QString("0CKS1%1%2%3%4")
                       .arg(defenceID)
                       .arg("00")
                       .arg(GetCode(defenceID))
                       .arg("\r\n");
        hostCOM->write(data.toLatin1());
    }
}

void VISTA128_COM::SendHeartData()
{
    if (isOpen) {
        hostCOM->write(Heart);
    }
}

void VISTA128_COM::SendStatusData()
{
    if (isOpen) {
        hostCOM->write(Status);
    }
}

void VISTA128_COM::CheckOnLine()
{
    //计算最后心跳时间到现在相差多少,如果小于30秒,说明有过心跳,还活着,不做下线处理
    //软件定时10秒向主机发送询问心跳命令
    QDateTime timeNow = QDateTime::currentDateTime();
    int timeValue = lastHeartTime.secsTo(timeNow);

    //超过两次没有接收到心跳消息判断为下线
    if (timeValue < 20) {
        return;
    } else if (timeValue < 30) {
        //当超过三次没有接收到心跳回复后发送旁路命令重新取得消息
        PangLu("128");
        return;
    }

    //如果当前主机状态为在线,并且还没有检测过下线状态,说明现在主机下线
    if (isOnLine && !isCheckOnline) {
        isOnLine = false;
        isCheckOnline = true;
        emit HostDisConnect(hostID, hostName); //发射主机下线信号
    }
}

void VISTA128_COM::ReadData()
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

        //默认八个子系统都正常并且处于撤防状态
        for (int i = 0; i < 8; i++) {
            subSystemError[i] = false;
            subSystemBuFang[i] = false;
        }
    }

    if (hostCOM->canReadLine()) {
        //接收到的数据带有回车符，需要去掉所有回车符再判断
        QByteArray buffer = hostCOM->readLine();
        buffer = buffer.replace("\r\n", "");
        QString data = QLatin1String(buffer);
        qDebug() << TIME << data;
        emit HostReceiveData(hostID, hostName, data);

        int len = data.length();
        if (len > 4) {
            //08XF为进入编程 08XN为退出编程
            //1Bnq为系统故障报警防区报警等
            //0AFV为主机收到命令应答
            //10AS为八个子系统状态
            //08OK为主机执行命令成功
            //69ZS为主机返回防区状态命令
            if (data.startsWith("0AFV")) {
            } else if (data.startsWith("08OK")) {
            } else if (data.startsWith("08XF009A") && len == 8) {
                //主机进入编程之后不响应心跳,停止发送心跳及停止判断主机下线,
                timerHeart->stop();
                timerOnLine->stop();
                emit HostError(hostID, hostName, "进入编程");
            } else if (data.startsWith("08XN0092") && len == 8) {
                //主机退出编程之后重新发送心跳及判断主机下线
                timerHeart->start();
                timerOnLine->start();
                emit HostError(hostID, hostName, "退出编程");
            } else if (data.startsWith("10AS") && len >= 12) {
                QString hostStatus = data.mid(4, 8);
                QStringList sta = hostStatus.split("");
                for (int i = 1; i <= 8; i++) {
                    //对应位为A则说明处于布防状态
                    if (sta.at(i) == "A") {
                        if (!subSystemBuFang[i - 1]) {
                            subSystemBuFang[i - 1] = true;
                            emit HostBuFangSubSystem(hostID, hostName, i);
                        }
                    } else if (sta.at(i) == "N") {
                        if (!subSystemError[i - 1]) {
                            subSystemError[i - 1] = true;
                            emit HostError(hostID, hostName, QString("子系统%1故障").arg(i));
                            SendStatusData();
                        }
                    } else {
                        subSystemError[i - 1] = false;
                    }
                }
                emit HostHeart(hostID, hostName);
            } else if (data.startsWith("69ZS")) {
                int tempLen = 99;
                if (tempLen > (len - 4)) {
                    tempLen = (len - 4);
                }
                if (len >= (4 + tempLen)) {
                    QString temp = data.mid(4, tempLen);
                    QStringList defenceStatus = temp.split("");
                    for (int i = 0; i <= tempLen; i++) {
                        if (defenceStatus.at(i) == "1") {
                            QString defenceID;
                            if (i < 10) {
                                defenceID = QString("00%1").arg(i);
                            } else if (i < 100) {
                                defenceID = QString("0%1").arg(i);
                            } else {
                                defenceID = QString("%1").arg(i);
                            }
                            //发送防区故障命令
                            emit DefenceGuZhang(hostID, hostName, defenceID);
                        }
                    }
                }
                emit DefenceStatus(hostID, hostName);
            } else if (data.startsWith("1Bnq") && len >= 6) {
                //命令标识符
                QString cmd = data.mid(4, 2);

                //子系统
                int subSystem = 1;
                if (len >= 13) {
                    subSystem = data.mid(12, 1).toInt();
                }

                //防区号
                QString defenceID = "001";
                if (len >= 9) {
                    defenceID = data.mid(6, 3);
                }

                if (cmd == "4E") {//主机编程改动
                    emit HostError(hostID, hostName, "主机编程改动");
                } else if (cmd == "29") {//电池低电
                    emit HostError(hostID, hostName, "电池低电");
                } else if (cmd == "07" && len >= 13) { //主机布防
                    if (!subSystemBuFang[subSystem - 1]) {
                        emit HostBuFangSubSystem(hostID, hostName, subSystem);
                        subSystemBuFang[subSystem - 1] = true;
                    }
                } else if (cmd == "08" && len >= 13) { //主机撤防
                    if (subSystemBuFang[subSystem - 1]) {
                        emit HostCheFangSubSystem(hostID, hostName, subSystem);
                        subSystemBuFang[subSystem - 1] = false;
                    }
                } else if (cmd == "5F" && len >= 13) { //24小时防区报警后撤防
                    if (subSystemBuFang[subSystem - 1]) {
                        emit HostCheFangSubSystem(hostID, hostName, subSystem);
                        subSystemBuFang[subSystem - 1] = false;
                    }
                } else if (cmd == "F5" && len >= 9) { //防区故障
                    emit DefenceGuZhang(hostID, hostName, defenceID);
                } else if (cmd == "F6" && len >= 9) { //防区故障恢复
                    emit DefenceHuiFu(hostID, hostName, defenceID);
                } else if (cmd == "73" && len >= 9) { //不存在防区报错
                    emit DefenceGuZhang(hostID, hostName, defenceID);
                } else if (cmd == "05" && len >= 9) { //防区旁路
                    emit DefencePangLu(hostID, hostName, defenceID);
                } else if (cmd == "06" && len >= 9) { //防区旁路恢复
                    emit DefenceHuiFu(hostID, hostName, defenceID);
                } else if (cmd == "41" && len >= 9) { //即时防区报警
                    emit DefenceBaoJing(hostID, hostName, defenceID);
                } else if (cmd == "31" && len >= 9) { //24小时防区报警
                    emit DefenceBaoJing(hostID, hostName, defenceID);
                } else if (cmd == "C3" && len >= 9) { //火警防区报警
                    emit DefenceBaoJing(hostID, hostName, defenceID);
                } else if (cmd == "32" && len >= 9) { //24小时防区恢复
                    emit DefenceHuiFu(hostID, hostName, defenceID);
                } else if (cmd == "01" && len >= 9) { //24小时防区短路
                    emit DefenceGuZhang(hostID, hostName, defenceID);
                } else if (cmd == "02" && len >= 9) { //24小时防区短路恢复
                    emit DefenceHuiFu(hostID, hostName, defenceID);
                }
            }
        }
    }
}
