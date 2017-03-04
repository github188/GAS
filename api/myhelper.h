#ifndef MYHELPER_H
#define MYHELPER_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QtNetwork>
#include <QtXml>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#include <QtPrintSupport>
#include <QtMultimedia>
#endif
#include "usercontrol/frmmessagebox.h"
#include "usercontrol/frminputbox.h"
#include "iconhelper.h"
#include "app.h"

#define TIMEMS qPrintable (QTime::currentTime().toString("HH:mm:ss zzz"))
#define TIME qPrintable (QTime::currentTime().toString("HH:mm:ss"))
#define QDATE qPrintable (QDate::currentDate().toString("yyyy-MM-dd"))
#define QTIME qPrintable (QTime::currentTime().toString("HH-mm-ss"))
#define DATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))
#define STRDATETIMEMS qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz"))

class myHelper: public QObject
{
public:

    //设置为开机启动
    static void AutoRunWithSystem(bool IsAutoRun, QString AppName, QString AppPath)
    {
        QSettings *reg = new QSettings(
                    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                    QSettings::NativeFormat);

        if (IsAutoRun) {
            reg->setValue(AppName, AppPath);
        } else {
            reg->setValue(AppName, "");
        }
    }

    //设置编码为UTF8
    static void SetUTF8Code()
    {
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("gbk");
#else
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    }

    //设置指定样式
    static void SetStyle(const QString &qssFile)
    {
        QFile file(qssFile);

        if (file.open(QFile::ReadOnly)) {
            QString qss = QLatin1String(file.readAll());
            qApp->setStyleSheet(qss);
            App::PaletteColor = qss.mid(20, 7);
            App::TextColor = qss.mid(34, 7);
            qApp->setPalette(QPalette(QColor(App::PaletteColor)));
            file.close();
        }
    }

    //加载中文字符
    static void SetChinese()
    {
        QTranslator *translator = new QTranslator(qApp);
        translator->load(":/image/qt_zh_CN.qm");
        qApp->installTranslator(translator);
    }

    //加载字体
    static void SetFont()
    {
        qApp->setFont(QFont(App::FontName, App::FontSize));
    }

    //显示输入框
    static QString ShowInputBox(bool &ok, QString title, int type = 0, int closeSec = 0, QString defaultValue = QString(), bool pwd = false)
    {
        frmInputBox input;
        input.SetParameter(title, type, closeSec, defaultValue, pwd);
        ok = input.exec();
        return input.GetValue();
    }

    //显示信息框,仅确定按钮
    static void ShowMessageBoxInfo(QString info, int closeSec = 0)
    {
        frmMessageBox msg;
        msg.SetMessage(info, 0, closeSec);
        msg.exec();
    }

    //显示信息框,仅确定按钮 非模态
    static void ShowMessageBoxInfoX(QString info, int closeSec = 0)
    {
        frmMessageBox::Instance()->SetMessage(info, 0, closeSec);
        frmMessageBox::Instance()->show();
    }

    //显示错误框,仅确定按钮
    static void ShowMessageBoxError(QString info, int closeSec = 0)
    {
        frmMessageBox msg;
        msg.SetMessage(info, 2, closeSec);
        msg.exec();
    }

    //显示错误框,仅确定按钮 非模态
    static void ShowMessageBoxErrorX(QString info, int closeSec = 0)
    {
        frmMessageBox::Instance()->SetMessage(info, 2, closeSec);
        frmMessageBox::Instance()->show();
    }

    //显示询问框,确定和取消按钮
    static int ShowMessageBoxQuestion(QString info)
    {
        frmMessageBox msg;
        msg.SetMessage(info, 1);
        return msg.exec();
    }

    //延时
    static void Sleep(int sec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(sec);

        while (QTime::currentTime() < dieTime) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }

    //窗体居中显示
    static void FormInCenter(QWidget *frm)
    {
        int frmX = frm->width();
        int frmY = frm->height();
        QDesktopWidget w;
        int deskWidth = w.availableGeometry().width();
        int deskHeight = w.availableGeometry().height();
        QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
        frm->move(movePoint);
    }

    //设置系统日期时间
    static void SetSystemDateTime(QString year, QString month, QString day, QString hour, QString min, QString sec)
    {
#ifdef Q_OS_WIN
        QProcess p(0);
        p.start("cmd");
        p.waitForStarted();
        p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();
        p.start("cmd");
        p.waitForStarted();
        p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();
#else
        QString cmd = QString("date %1%2%3%4%5.%6").arg(month).arg(day).arg(hour).arg(min).arg(year).arg(sec);
        system(cmd.toLatin1());
        system("hwclock -w");
#endif
    }

    //16进制字符串转字节数组
    static QByteArray HexStrToByteArray(QString str)
    {
        QByteArray senddata;
        int hexdata, lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len / 2);
        char lstr, hstr;

        for (int i = 0; i < len;) {
            hstr = str.at(i).toLatin1();

            if (hstr == ' ') {
                i++;
                continue;
            }

            i++;

            if (i >= len) {
                break;
            }

            lstr = str.at(i).toLatin1();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);

            if ((hexdata == 16) || (lowhexdata == 16)) {
                break;
            } else {
                hexdata = hexdata * 16 + lowhexdata;
            }

            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }

        senddata.resize(hexdatalen);
        return senddata;
    }

    static char ConvertHexChar(char ch)
    {
        if ((ch >= '0') && (ch <= '9')) {
            return ch - 0x30;
        } else if ((ch >= 'A') && (ch <= 'F')) {
            return ch - 'A' + 10;
        } else if ((ch >= 'a') && (ch <= 'f')) {
            return ch - 'a' + 10;
        } else {
            return (-1);
        }
    }

    //字节数组转16进制字符串
    static QString ByteArrayToHexStr(QByteArray data)
    {
        QString temp = "";
        QString hex = data.toHex();

        for (int i = 0; i < hex.length(); i = i + 2) {
            temp += hex.mid(i, 2) + " ";
        }

        return temp.trimmed().toUpper();
    }

    //16进制字符串转10进制
    static int StrHexToDecimal(QString strHex)
    {
        bool ok;
        return strHex.toInt(&ok, 16);
    }

    //10进制字符串转10进制
    static int StrDecimalToDecimal(QString strDecimal)
    {
        bool ok;
        return strDecimal.toInt(&ok, 10);
    }

    //2进制字符串转10进制
    static int StrBinToDecimal(QString strBin)
    {
        bool ok;
        return strBin.toInt(&ok, 2);
    }

    //16进制字符串转2进制字符串
    static QString StrHexToStrBin(QString strHex)
    {
        uchar decimal = StrHexToDecimal(strHex);
        QString bin = QString::number(decimal, 2);
        uchar len = bin.length();

        if (len < 8) {
            for (int i = 0; i < 8 - len; i++) {
                bin = "0" + bin;
            }
        }

        return bin;
    }

    //10进制转2进制字符串一个字节
    static QString DecimalToStrBin1(int decimal)
    {
        QString bin = QString::number(decimal, 2);
        uchar len = bin.length();

        if (len <= 8) {
            for (int i = 0; i < 8 - len; i++) {
                bin = "0" + bin;
            }
        }

        return bin;
    }

    //10进制转2进制字符串两个字节
    static QString DecimalToStrBin2(int decimal)
    {
        QString bin = QString::number(decimal, 2);
        uchar len = bin.length();

        if (len <= 16) {
            for (int i = 0; i < 16 - len; i++) {
                bin = "0" + bin;
            }
        }

        return bin;
    }

    //10进制转16进制字符串,补零.
    static QString DecimalToStrHex(int decimal)
    {
        QString temp = QString::number(decimal, 16);

        if (temp.length() == 1) {
            temp = "0" + temp;
        }

        return temp;
    }

    static QByteArray IntToByte(int i)
    {
        QByteArray result;
        result.resize(4);
        result[3] = (uchar)(0x000000ff & i);
        result[2] = (uchar)((0x0000ff00 & i) >> 8);
        result[1] = (uchar)((0x00ff0000 & i) >> 16);
        result[0] = (uchar)((0xff000000 & i) >> 24);
        return result;
    }

    static int ByteToInt(QByteArray data)
    {
        int i = data[3] & 0x000000FF;
        i |= ((data[2] << 8) & 0x0000FF00);
        i |= ((data[1] << 16) & 0x00FF0000);
        i |= ((data[0] << 24) & 0xFF000000);
        return i;
    }

    static QByteArray UShortToByte(ushort i)
    {
        QByteArray result;
        result.resize(2);
        result[1] = (uchar)(0x000000ff & i);
        result[0] = (uchar)((0x0000ff00 & i) >> 8);
        return result;
    }

    static int ByteToUShort(QByteArray data)
    {
        int i = data[1] & 0x000000FF;
        i |= ((data[0] << 8) & 0x0000FF00);
        return i;
    }

    //异或加密算法
    static QString getXorEncryptDecrypt(QString str, char key)
    {
        QByteArray data = str.toLatin1();
        int size = data.size();

        for (int i = 0; i < size; i++) {
            data[i] = data.at(i) ^ key;
        }

        return QLatin1String(data);
    }

    //异或校验
    static uchar GetOrCode(QByteArray data)
    {
        int len = data.length();
        uchar result = 0;

        for (int i = 0; i < len; i++) {
            result ^= data.at(i);
        }

        return result;
    }

    //计算校验码
    static uchar GetCheckCode(QByteArray data)
    {
        int len = data.length();
        uchar temp = 0;

        for (uchar i = 0; i < len; i++) {
            temp += data.at(i);
        }

        return temp % 256;
    }

    static uchar GetCheckCode(uchar data[], uchar len)
    {
        uchar temp = 0;

        for (uchar i = 0; i < len; i++) {
            temp += data[i];
        }

        return temp % 256;
    }

    //获取选择的文件
    static QString GetFileName(QString filter)
    {
        return QFileDialog::getOpenFileName(0, "选择文件", QCoreApplication::applicationDirPath(), filter);
    }

    //获取选择的文件集合
    static QStringList GetFileNames(QString filter)
    {
        return QFileDialog::getOpenFileNames(0, "选择文件", QCoreApplication::applicationDirPath(), filter);
    }

    //获取选择的目录
    static QString GetFolderName()
    {
        return QFileDialog::getExistingDirectory();;
    }

    //获取文件名,含拓展名
    static QString GetFileNameWithExtension(QString strFilePath)
    {
        QFileInfo fileInfo(strFilePath);
        return fileInfo.fileName();
    }

    //获取选择文件夹中的文件
    static QStringList GetFolderFileNames(QStringList filter)
    {
        QStringList fileList;
        QString strFolder = QFileDialog::getExistingDirectory();

        if (!strFolder.length() == 0) {
            QDir myFolder(strFolder);

            if (myFolder.exists()) {
                fileList = myFolder.entryList(filter);
            }
        }

        return fileList;
    }

    //文件夹是否存在
    static bool FolderIsExist(QString strFolder)
    {
        QDir tempFolder(strFolder);
        return tempFolder.exists();
    }

    //文件是否存在
    static bool FileIsExist(QString strFile)
    {
        QFile tempFile(strFile);
        return tempFile.exists();
    }

    //复制文件
    static bool CopyFile(QString sourceFile, QString targetFile)
    {
        bool ok;
        ok = QFile::copy(sourceFile, targetFile);
        //将复制过去的文件只读属性取消
        ok = QFile::setPermissions(targetFile, QFile::WriteOwner);
        return ok;
    }

    //删除文件夹下所有文件
    static void DeleteDirectory(QString path)
    {
        QDir dir(path);

        if (!dir.exists()) {
            return;
        }

        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        QFileInfoList fileList = dir.entryInfoList();

        foreach (QFileInfo fi, fileList) {
            if (fi.isFile()) {
                fi.dir().remove(fi.fileName());
            } else {
                DeleteDirectory(fi.absoluteFilePath());
                dir.rmdir(fi.absoluteFilePath());
            }
        }
    }

    //判断IP地址及端口是否在线
    static bool IPLive(QString ip, int port, int timeout = 1000)
    {
        QTcpSocket tcpClient;
        tcpClient.abort();
        tcpClient.connectToHost(ip, port);
        //100毫秒没有连接上则判断不在线
        return tcpClient.waitForConnected(timeout);
    }

    //获取网页所有源代码
    static  QString GetHtml(QString url)
    {
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
        QByteArray responseData;
        QEventLoop eventLoop;
        QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &eventLoop, SLOT(quit()));
        eventLoop.exec();
        responseData = reply->readAll();
        return QString(responseData);
    }

    //获取本机公网IP地址
    static  QString GetNetIP(QString webCode)
    {
        QString web = webCode.replace(" ", "");
        web = web.replace("\r", "");
        web = web.replace("\n", "");
        QStringList list = web.split("<br/>");
        QString tar = list.at(3);
        QStringList ip = tar.split("=");
        return ip.at(1);
    }

    //获取本机IP
    static QString GetLocalIP()
    {
        QString ip;
        QList<QHostAddress> addrs = QNetworkInterface::allAddresses();

        foreach (QHostAddress addr, addrs) {
            ip = addr.toString();

            if (ip != "127.0.0.1" && IsIP(ip)) {
                break;
            }
        }

        return ip;
    }

    //Url地址转为IP地址
    static QString UrlToIP(QString url)
    {
        QHostInfo host = QHostInfo::fromName(url);
        return host.addresses().at(0).toString();
    }

    //判断是否是IP地址
    static bool IsIP(QString ip)
    {
        QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
        return RegExp.exactMatch(ip);
    }

    //判断是否是MAC地址
    static bool IsMAC(QString MAC)
    {
        QRegExp RegExp("^[A-F0-9]{2}(-[A-F0-9]{2}){5}$");
        return RegExp.exactMatch(MAC);
    }

    //判断是否通外网
    static bool IsWebOk()
    {
        //能接通百度IP说明可以通外网
        return IPLive("115.239.211.112", 80);
    }
};

#endif // MYHELPER_H
