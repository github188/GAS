#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "hostapi.h"
#include "myhelper.h"
#include "api.h"
#include "dbhelper.h"
#include "ntpapi.h"

HostAPI *HostAPI::_instance = 0;
HostAPI::HostAPI(QObject *parent) :
    QObject(parent)
{
    vista128_com = 0;
    ds7400_com = 0;
    sound = new QSound("");
}

HostAPI::~HostAPI()
{
    checkTaskThread->stop();
    checkTaskThread->quit();
    checkTaskThread->wait();
    checkTaskThread->deleteLater();
}

void HostAPI::Init()
{
    if (App::HostType == "DS7400") {
        ds7400_com = new DS7400_COM(this);
        ds7400_com->SetHostType("DS7400");
        ds7400_com->SetHostID("100001");
        ds7400_com->SetHostName("DS7400主机");
        ds7400_com->SetHostConnectValue1(App::HostCom);
        ds7400_com->SetHostConnectValue2("");

        if (ds7400_com->Open()) {
            connect(ds7400_com, SIGNAL(HostConnect(QString, QString)),
                    this, SLOT(HostConnect(QString, QString)));
            connect(ds7400_com, SIGNAL(HostDisConnect(QString, QString)),
                    this, SLOT(HostDisConnect(QString, QString)));
            connect(ds7400_com, SIGNAL(HostError(QString, QString, QString)),
                    this, SLOT(HostError(QString, QString, QString)));
            connect(ds7400_com, SIGNAL(HostBuFangSubSystem(QString, QString, int)),
                    this, SLOT(HostBuFangSubSystem(QString, QString, int)));
            connect(ds7400_com, SIGNAL(HostCheFangSubSystem(QString, QString, int)),
                    this, SLOT(HostCheFangSubSystem(QString, QString, int)));
            connect(ds7400_com, SIGNAL(DefenceBaoJing(QString, QString, QString)),
                    this, SLOT(DefenceBaoJing(QString, QString, QString)));
            connect(ds7400_com, SIGNAL(DefenceGuZhang(QString, QString, QString)),
                    this, SLOT(DefenceGuZhang(QString, QString, QString)));
            connect(ds7400_com, SIGNAL(DefenceHuiFu(QString, QString, QString)),
                    this, SLOT(DefenceHuiFu(QString, QString, QString)));
            connect(ds7400_com, SIGNAL(DefencePangLu(QString, QString, QString)),
                    this, SLOT(DefencePangLu(QString, QString, QString)));

            connect(ds7400_com, SIGNAL(HostReceiveData(QString, QString, QString)),
                    this, SIGNAL(HostReceiveData(QString, QString, QString)));

            API::AddMessage("串口打开成功");
            DBHelper::AddEventInfoUser("串口打开成功");
        } else {
            API::AddMessage("串口打开失败");
            DBHelper::AddEventInfoUser("串口打开失败");
        }
    } else if (App::HostType == "VISTA128") {
        vista128_com = new VISTA128_COM(this);
        vista128_com->SetHostType("VISTA128");
        vista128_com->SetHostID("100002");
        vista128_com->SetHostName("VISTA128主机");
        vista128_com->SetHostConnectValue1(App::HostCom);
        vista128_com->SetHostConnectValue2("");

        if (vista128_com->Open()) {
            connect(vista128_com, SIGNAL(HostConnect(QString, QString)),
                    this, SLOT(HostConnect(QString, QString)));
            connect(vista128_com, SIGNAL(HostDisConnect(QString, QString)),
                    this, SLOT(HostDisConnect(QString, QString)));
            connect(vista128_com, SIGNAL(HostError(QString, QString, QString)),
                    this, SLOT(HostError(QString, QString, QString)));
            connect(vista128_com, SIGNAL(HostBuFangSubSystem(QString, QString, int)),
                    this, SLOT(HostBuFangSubSystem(QString, QString, int)));
            connect(vista128_com, SIGNAL(HostCheFangSubSystem(QString, QString, int)),
                    this, SLOT(HostCheFangSubSystem(QString, QString, int)));
            connect(vista128_com, SIGNAL(DefenceBaoJing(QString, QString, QString)),
                    this, SLOT(DefenceBaoJing(QString, QString, QString)));
            connect(vista128_com, SIGNAL(DefenceGuZhang(QString, QString, QString)),
                    this, SLOT(DefenceGuZhang(QString, QString, QString)));
            connect(vista128_com, SIGNAL(DefenceHuiFu(QString, QString, QString)),
                    this, SLOT(DefenceHuiFu(QString, QString, QString)));
            connect(vista128_com, SIGNAL(DefencePangLu(QString, QString, QString)),
                    this, SLOT(DefencePangLu(QString, QString, QString)));

            connect(vista128_com, SIGNAL(HostReceiveData(QString, QString, QString)),
                    this, SIGNAL(HostReceiveData(QString, QString, QString)));

            API::AddMessage("串口打开成功");
            DBHelper::AddEventInfoUser("串口打开成功");
        } else {
            API::AddMessage("串口打开失败");
            DBHelper::AddEventInfoUser("串口打开失败");
        }
    }

    //初始化NTP服务定时器
    timerNTP = new QTimer(this);
    connect(timerNTP, SIGNAL(timeout()), this, SLOT(CheckTime()));
    connect(NTPAPI::Instance(), SIGNAL(ReceiveTime(QDateTime)), this, SLOT(ReceiveTime(QDateTime)));
    //如果间隔为0,则以1分钟间隔作为校准间隔
    if (App::UseNTP) {
        if (App::NTPInterval == 0) {
            timerNTP->start(60 * 1000);
        } else {
            timerNTP->start(App::NTPInterval * 60 * 60 * 1000);
        }
    } else {
        timerNTP->stop();
    }

    //初始化临时任务处理器
    timer128 = new QTimer(this);
    connect(timer128, SIGNAL(timeout()), this, SLOT(CheckTask()));
    timer128->start(3000);

    //初始化短信猫
    GSM = new GSMAPI(this);
    GSM->SetPortName(App::MsgPortName);
    GSM->Open();
    GSM->Init();

    timerMsg = new QTimer(this);
    connect(timerMsg, SIGNAL(timeout()), this, SLOT(CheckMsg()));
    //如果间隔是0则按照10秒间隔发送,如果是10000则不启动
    if (App::MsgInterval == 0) {
        timerMsg->start(10000);
    } else if (App::MsgInterval == 10000) {
        timerMsg->stop();
    } else {
        timerMsg->start(App::MsgInterval * 60 * 1000);
    }

    timerEmail = new QTimer(this);
    connect(timerEmail, SIGNAL(timeout()), this, SLOT(CheckEmail()));
    //如果间隔是0秒则按照10秒间隔发送,如果是10000则不启动
    if (App::EmailInterval == 0) {
        timerEmail->start(10000);
    } else if (App::EmailInterval == 10000) {
        timerEmail->stop();
    } else {
        timerEmail->start(App::EmailInterval * 60 * 1000);
    }

    //启动任务处理线程,处理打印警/告警图像抓拍/告警视频联动等
    checkTaskThread = new CheckTaskThread(this);
    checkTaskThread->start();
}

void HostAPI::BuFang()
{
    if (App::HostType == "DS7400") {
        API::BuFang();
    } else if (App::HostType == "VISTA128") {
        for (int i = 1; i <= 8; i++) {
            task.append(QString("BuFang|%1").arg(i));
        }
        CheckTask();
    }
}

void HostAPI::BuFang(int subSystem)
{
    if (App::HostType == "DS7400") {
        API::BuFang(subSystem);
    } else if (App::HostType == "VISTA128") {
        vista128_com->BuFang(subSystem);
    }
}

void HostAPI::CheFang()
{
    if (App::HostType == "DS7400") {
        API::CheFang();
    } else if (App::HostType == "VISTA128") {
        for (int i = 1; i <= 8; i++) {
            task.append(QString("CheFang|%1").arg(i));
        }
        CheckTask();
    }

    sound->stop();
}

void HostAPI::CheFang(int subSystem)
{
    if (App::HostType == "DS7400") {
        API::CheFang(subSystem);
    } else if (App::HostType == "VISTA128") {
        vista128_com->CheFang(subSystem);
    }

    sound->stop();
}

void HostAPI::PangLu(QString defenceID)
{
    if (App::HostType == "DS7400") {
        API::PangLu(defenceID);
    } else if (App::HostType == "VISTA128") {
        vista128_com->PangLu(defenceID);
    }
}

void HostAPI::HuiFu(QString defenceID)
{
    if (App::HostType == "DS7400") {
        API::HuiFu(defenceID);
    } else if (App::HostType == "VISTA128") {
        vista128_com->HuiFu(defenceID);
    }
}

void HostAPI::HostConnect(QString , QString )
{
    API::Connect();
    QSound tempSound(QString("%1/sound/%2").arg(App::AppPath).arg("connect.wav"));
    tempSound.play();
}

void HostAPI::HostDisConnect(QString , QString )
{
    API::DisConnect();
    QSound tempSound(QString("%1/sound/%2").arg(App::AppPath).arg("disconnect.wav"));
    tempSound.play();
}

void HostAPI::HostError(QString , QString , QString error)
{
    API::Error(error);
}

void HostAPI::HostBuFang(QString , QString )
{
    API::BuFang();
}

void HostAPI::HostBuFangSubSystem(QString , QString , int subSystem)
{
    API::BuFang(subSystem);
}

void HostAPI::HostCheFang(QString , QString )
{
    API::CheFang();
    sound->stop();
}

void HostAPI::HostCheFangSubSystem(QString , QString , int subSystem)
{
    API::CheFang(subSystem);
    sound->stop();
}

void HostAPI::DefenceBaoJing(QString , QString , QString defenceID)
{
    API::BaoJing(defenceID);

    QString defenceName;
    QString defenceSound;
    DBHelper::GetDefenceInfo(defenceID, defenceName, defenceSound);

    //调用热敏打印机打印警情,将需要的处理传入任务队列,由线程执行任务
    if (App::PrintLink) {
        QString task = QString("Print|%1|%2|%3|%4").arg(defenceID).arg(defenceName).arg("防区报警").arg(DATETIME);
        App::AppendTask(task);
    }

    //告警图像抓拍,将需要的处理传入任务队列,由线程执行任务
    //存储告警图像的名称,以便告警邮件以附件的形式发送
    QStringList listSnap;
    if (App::SnapLink) {
        QString sql = QString("select NVRIP,NVRCh,SnapTime,SnapCount from LinkInfo where DefenceID='%1'").arg(defenceID);
        QSqlQuery query;
        query.exec(sql);
        while (query.next()) {
            QString nvrIP = query.value(0).toString();
            int nvrCh = query.value(1).toInt();
            int snapTime = query.value(2).toInt();
            int snapCount = query.value(3).toInt();
            //判断是否抓拍多张图片
            if (snapCount > 1) {
                //倒序抓拍告警图像
                int count = 0;
                for (int i = snapTime; i >= 1; i--) {
                    QString task = QString("Snap|%1|%2|%3|%4").arg(nvrIP).arg(nvrCh).arg(snapTime).arg(i);

#ifdef __arm__
                    QString fileName = QString("%1/%2/%3-%4.jpg")
                                       .arg(App::SnapPath)
                                       .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
                                       .arg(i);
#else
                    QString fileName = QString("%1%2/%3/%4-%5.jpg")
                                       .arg(App::AppPath)
                                       .arg(App::SnapPath)
                                       .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                                       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
                                       .arg(i);
#endif

                    listSnap << fileName;

                    App::AppendTask(task);
                    App::AppendSnap(fileName);

                    //告警前时间30秒,取5张,则依次发送的timeago参数为30,29,28,27,26
                    count++;
                    if (count >= snapCount) {
                        break;
                    }
                }
            } else {
                QString task = QString("Snap|%1|%2|%3|%4").arg(nvrIP).arg(nvrCh).arg(snapTime).arg(snapCount);

#ifdef __arm__
                QString fileName = QString("%1/%2/%3-%4.jpg")
                                   .arg(App::SnapPath)
                                   .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                                   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
                                   .arg(snapCount);
#else
                QString fileName = QString("%1%2/%3/%4-%5.jpg")
                                   .arg(App::AppPath)
                                   .arg(App::SnapPath)
                                   .arg(QDate::currentDate().toString("yyyy-MM-dd"))
                                   .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
                                   .arg(snapCount);
#endif

                listSnap << fileName;

                App::AppendTask(task);
                App::AppendSnap(fileName);
            }
        }
    }

    //告警视频联动,将需要的处理传入任务队列,由线程执行任务
    if (App::VideoLink) {
        QString sql = QString("select RtspAddr from LinkInfo where DefenceID='%1'").arg(defenceID);
        QSqlQuery query;
        query.exec(sql);
        while (query.next()) {
            QString rtspAddr = query.value(0).toString();
            QString task = QString("Video|%1").arg(rtspAddr);
            App::AppendTask(task);
        }
    }

    //发送告警短信,传入短信任务队列,由定时器执行任务
    if (App::MsgInterval <= 1440) {
        QString msg = QString("防区编号:%1;防区名称:%2;告警类型:%3;告警时间:%4")
                      .arg(defenceID).arg(defenceName).arg("防区报警").arg(DATETIME);
        App::AppendMsg(msg);
    }

    //发送告警邮件,传入邮件任务队列,由定时器执行任务
    if (App::EmailInterval <= 1440) {
        QString fileName = listSnap.join(";");
        QString email = QString("防区编号:%1;防区名称:%2;告警类型:%3;告警时间:%4|%5")
                        .arg(defenceID).arg(defenceName).arg("防区报警").arg(DATETIME).arg(fileName);
        App::AppendEmail(email);
    }

    //播放告警声音
    if (defenceSound != "无") {
        QString soundFile = QString("%1/sound/%2").arg(App::AppPath).arg(defenceSound);
        sound = new QSound(soundFile);
        sound->play();
    }
}

void HostAPI::DefenceGuZhang(QString , QString , QString defenceID)
{
    API::GuZhang(defenceID);
}

void HostAPI::DefenceHuiFu(QString , QString , QString defenceID)
{
    API::HuiFu(defenceID);
}

void HostAPI::DefencePangLu(QString , QString , QString defenceID)
{
    API::PangLu(defenceID);
}

void HostAPI::CheckTime()
{
    NTPAPI::Instance()->GetTime(App::NTPIP);
}

void HostAPI::ReceiveTime(QDateTime dateTime)
{
    QString str = dateTime.toString("yyyy-MM-dd HH:mm:ss");
    QString year = str.mid(0, 4);
    QString month = str.mid(5, 2);
    QString day = str.mid(8, 2);
    QString hour = str.mid(11, 2);
    QString min = str.mid(14, 2);
    QString sec = str.mid(17, 2);
    myHelper::SetSystemDateTime(year, month, day, hour, min, sec);
}

void HostAPI::CheckTask()
{
    int count = task.count();
    if (count > 0) {
        //取出第一个任务进行处理
        QString cmd = task.takeFirst();
        QStringList list = cmd.split("|");
        QString type = list.at(0);
        if (type == "BuFang") {
            vista128_com->BuFang(list.at(1).toInt());
        } else if (type == "CheFang") {
            vista128_com->CheFang(list.at(1).toInt());
        }
    }
}

void HostAPI::CheckMsg()
{
    int count = App::MsgList.count();
    if (count == 0) {
        return;
    }

    QMutex mutex;
    mutex.lock();

    //一条短信最多70个汉字160个全英文数字字符,同时保证是一条完成的告警信息
    QStringList msgs;
    for (int i = 0; i < count; i++) {
        msgs.append(App::MsgList.takeFirst());
    }

    mutex.unlock();

    //发送告警短信(支持长短信发送)
    GSM->SendMsgAll(App::MsgTel, msgs);
}

void HostAPI::CheckEmail()
{
    int count = App::EmailList.count();
    if (count == 0) {
        return;
    }

    QMutex mutex;
    mutex.lock();

    //邮件内容,无论多长都可以拼在一起发送
    //<br>用来换行显示
    QString content = "";
    QString fileName = "";
    for (int i = 0; i < count; i++) {
        QString str = App::EmailList.takeFirst();
        QStringList list = str.split("|");
        content += QString("%1<br>").arg(list.at(0));
        fileName += QString("%1;").arg(list.at(1));
    }

    //去掉末尾<br>
    content = content.mid(0, content.length() - 4);
    //去掉末尾;
    fileName = fileName.mid(0, fileName.length() - 1);

    mutex.unlock();

    //启动发送邮件线程进行邮件发送,防止界面卡住
    SendEmailThread *sendEmailThread = new SendEmailThread(this);
    connect(sendEmailThread, SIGNAL(finished()), sendEmailThread, SLOT(deleteLater()));
    sendEmailThread->SetContent(content);
    sendEmailThread->SetFileName(fileName);
    sendEmailThread->start();
}

void HostAPI::SetNTPInterval()
{
    if (App::UseNTP) {
        if (App::NTPInterval == 0) {
            timerNTP->start(60 * 1000);
        } else {
            timerNTP->start(App::NTPInterval * 60 * 60 * 1000);
        }
    } else {
        timerNTP->stop();
    }
}

void HostAPI::SetMsgInterval()
{
    if (App::MsgInterval == 0) {
        timerMsg->start(10000);
    } else if (App::MsgInterval == 10000) {
        timerMsg->stop();
    } else {
        timerMsg->start(App::MsgInterval * 60 * 1000);
    }
}

void HostAPI::SetEmailInterval()
{
    if (App::EmailInterval == 0) {
        timerEmail->start(10000);
    } else if (App::EmailInterval == 10000) {
        timerEmail->stop();
    } else {
        timerEmail->start(App::EmailInterval * 60 * 1000);
    }
}
