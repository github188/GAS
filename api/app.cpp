#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "app.h"
#include "myhelper.h"

QString App::AppName = "GAS";
QString App::AppPath = "";
int App::DeskWidth = 1440;
int App::DeskHeight = 900;

int App::IcoMain = 0xf1aa;
int App::IcoMenu = 0xf0d7;
int App::IcoMin = 0xf068;
int App::IcoMax = 0xf079;
int App::IcoNormal = 0xf0b2;
int App::IcoClose = 0xf00d;

//存储当前登录用户信息,方便用户密码修改和用户退出操作
QString App::CurrentImage = "main.jpg";
bool App::IsMove = false;
QString App::CurrentUserName = "admin";
QString App::CurrentUserPwd = "admin";
QString App::CurrentUserType = "管理员";
QString App::PaletteColor = "#000000";
QString App::TextColor = "#FFFFFF";

QString App::KeyData = "";
bool App::KeyUseDate = false;
QString App::KeyDate = "2015-01-01";
bool App::KeyUseRun = false;
int App::KeyRun = 1;
bool App::KeyUseCount = false;
int App::KeyCount = 10;

bool App::SaveLog = false;
bool App::UseStyle = true;
QString App::StyleName = ":/qss/blue.css";

#ifdef Q_OS_WIN
int App::FontSize = 9;
QString App::FontName = "Microsoft YaHei";
#endif

#ifdef Q_OS_LINUX
#ifdef __arm__
int App::FontSize = 14;
QString App::FontName = "WenQuanYi Micro Hei";
#else
int App::FontSize = 10;
QString App::FontName = "WenQuanYi";
#endif
#endif

bool App::UseInput = false;
QString App::InputPosition = "control";
QString App::Title = "组合认证及报警控制系统";
QString App::Version = "V1.0";
QString App::Author = "上海宝学电子信息技术有限公司";

QString App::HostType = "DS7400";

#ifdef Q_OS_WIN
QString App::HostCom = "COM1";
#endif
#ifdef Q_OS_LINUX
#ifdef __arm__
QString App::HostCom = "ttySAC1";
#else
QString App::HostCom = "ttyUSB0";
#endif
#endif

QString App::AlarmServerIP = "127.0.0.1";
int App::AlarmServerPort = 9876;
#ifdef __arm__
int App::MsgCount = 35;
QString App::SnapPath = "/bin/Snap";
QString App::VideoPath = "/bin/Video";
#else
int App::MsgCount = 27;
QString App::SnapPath = "/snap";
QString App::VideoPath = "/video";
#endif

bool App::AutoBuFang = false;
bool App::VideoLink = true;
bool App::NetLink = true;
bool App::SnapLink = true;
bool App::PrintLink = false;

bool App::UseNTP = false;
QString App::NTPIP = "time.nist.gov";
int App::NTPInterval = 24;

#ifdef Q_OS_WIN
QString App::MsgPortName = "COM3";
#endif
#ifdef Q_OS_LINUX
#ifdef __arm__
QString App::MsgPortName = "ttySAC2";
#else
QString App::MsgPortName = "ttyUSB0";
#endif
#endif
int App::MsgBaudRate = 9600;
int App::MsgInterval = 10000;
QString App::MsgTel = "18001797656";

QString App::SendEmailAddr = "feiyangqingyun@163.com";
QString App::SendEmailPwd = "19880210liu";
int App::EmailInterval = 10000;
QString App::EmailAddr = "517216493@qq.com";

void App::ReadConfig()
{
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);

    //如果配置文件不存在或者内容为空,则以初始值继续运行,并生成配置文件
    QFile file(fileName);
    if (file.size() == 0) {
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
        App::Title = App::Title.toLatin1();
        App::Author = App::Author.toLatin1();
#endif
        WriteConfig();
        return;
    }

    QSettings set(fileName, QSettings::IniFormat);
    set.beginGroup("AppConfig");

    App::SaveLog = set.value("SaveLog").toBool();
    App::UseStyle = set.value("UseStyle").toBool();
    App::StyleName = set.value("StyleName").toString();
    App::FontSize = set.value("FontSize").toInt();
    App::FontName = set.value("FontName").toString();

    App::UseInput = set.value("UseInput").toBool();
    App::InputPosition = set.value("InputPosition").toString();
    App::Title = set.value("Title").toString();
    App::Version = set.value("Version").toString();
    App::Author = set.value("Author").toString();

    App::HostType = set.value("HostType").toString();
    App::HostCom = set.value("HostCom").toString();
    App::AlarmServerIP = set.value("AlarmServerIP").toString();
    App::AlarmServerPort = set.value("AlarmServerPort").toInt();
    App::MsgCount = set.value("MsgCount").toInt();
    App::SnapPath = set.value("SnapPath").toString();
    App::VideoPath = set.value("VideoPath").toString();

    App::AutoBuFang = set.value("AutoBuFang").toBool();
    App::VideoLink = set.value("VideoLink").toBool();
    App::NetLink = set.value("NetLink").toBool();
    App::SnapLink = set.value("SnapLink").toBool();
    App::PrintLink = set.value("PrintLink").toBool();

    App::UseNTP = set.value("UseNTP").toBool();
    App::NTPIP = set.value("NTPIP").toString();
    App::NTPInterval = set.value("NTPInterval").toInt();

    App::MsgPortName = set.value("MsgPortName").toString();
    App::MsgBaudRate = set.value("MsgBaudRate").toInt();
    App::MsgInterval = set.value("MsgInterval").toInt();
    App::MsgTel = set.value("MsgTel").toString();

    App::SendEmailAddr = set.value("SendEmailAddr").toString();
    App::SendEmailPwd = set.value("SendEmailPwd").toString();
    App::SendEmailPwd = myHelper::getXorEncryptDecrypt(App::SendEmailPwd, 255);
    App::EmailInterval = set.value("EmailInterval").toInt();
    App::EmailAddr = set.value("EmailAddr").toString();

    set.endGroup();
}

void App::WriteConfig()
{
    QString fileName = QString("%1/config/%2_Config.ini").arg(App::AppPath).arg(App::AppName);
    QSettings set(fileName, QSettings::IniFormat);
    set.beginGroup("AppConfig");

    set.setValue("SaveLog", App::SaveLog);
    set.setValue("UseStyle", App::UseStyle);
    set.setValue("StyleName", App::StyleName);
    set.setValue("FontSize", App::FontSize);
    set.setValue("FontName", App::FontName);

    set.setValue("UseInput", App::UseInput);
    set.setValue("InputPosition", App::InputPosition);
    set.setValue("Title", App::Title);
    set.setValue("Version", App::Version);
    set.setValue("Author", App::Author);

    set.setValue("HostType", App::HostType);
    set.setValue("HostCom", App::HostCom);
    set.setValue("AlarmServerIP", App::AlarmServerIP);
    set.setValue("AlarmServerPort", App::AlarmServerPort);
    set.setValue("MsgCount", App::MsgCount);
    set.setValue("SnapPath", App::SnapPath);
    set.setValue("VideoPath", App::VideoPath);

    set.setValue("AutoBuFang", App::AutoBuFang);
    set.setValue("VideoLink", App::VideoLink);
    set.setValue("NetLink", App::NetLink);
    set.setValue("SnapLink", App::SnapLink);
    set.setValue("PrintLink", App::PrintLink);

    set.setValue("UseNTP", App::UseNTP);
    set.setValue("NTPIP", App::NTPIP);
    set.setValue("NTPInterval", App::NTPInterval);

    set.setValue("MsgPortName", App::MsgPortName);
    set.setValue("MsgBaudRate", App::MsgBaudRate);
    set.setValue("MsgInterval", App::MsgInterval);
    set.setValue("MsgTel", App::MsgTel);

    set.setValue("SendEmailAddr", App::SendEmailAddr);
    set.setValue("SendEmailPwd", App::SendEmailPwd);
    set.setValue("SendEmailPwd", myHelper::getXorEncryptDecrypt(App::SendEmailPwd, 255));
    set.setValue("EmailInterval", App::EmailInterval);
    set.setValue("EmailAddr", App::EmailAddr);

    set.endGroup();
}

void App::WriteError(QString str)
{
    QString fileName = QString("%1/log/%2_Error_%3.txt").arg(App::AppPath).arg(App::AppName).arg(QDATE);
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << DATETIME << "  " << str << "\n";
}

void App::WriteStartTime()
{
    QString fileName = QString("%1/log/%2_Start_%3.txt").arg(App::AppPath).arg(App::AppName).arg(QDATE);
    QFile file(fileName);
    QString str = QString("start time : %1").arg(DATETIME);
    file.open(QFile::WriteOnly | QIODevice::Append | QFile::Text);
    QTextStream stream(&file);
    stream << str << "\n";
}

void App::NewDir(QString dirName)
{
    //如果路径中包含斜杠字符则说明是绝对路径
    //windows系统 路径字符带有 :/  linux系统路径字符带有 /
    if (!dirName.contains(":/") && !dirName.startsWith("/")) {
        dirName = QString("%1/%2").arg(App::AppPath).arg(dirName);
    }

    QDir dir(dirName);

    if (!dir.exists()) {
        dir.mkpath(dirName);
    }
}

QList<QString> App::TaskList = QList<QString>();
QList<QString> App::SnapList = QList<QString>();
QList<QString> App::EmailList = QList<QString>();
QList<QString> App::MsgList = QList<QString>();

void App::AppendTask(QString task)
{
    QMutex mutex;
    mutex.lock();
    App::TaskList.append(task);
    mutex.unlock();
}

void App::AppendSnap(QString snap)
{
    QMutex mutex;
    mutex.lock();
    App::SnapList.append(snap);
    mutex.unlock();
}

void App::AppendEmail(QString email)
{
    QMutex mutex;
    mutex.lock();
    App::EmailList.append(email);
    mutex.unlock();
}

void App::AppendMsg(QString msg)
{
    QMutex mutex;
    mutex.lock();
    App::MsgList.append(msg);
    mutex.unlock();
}
