#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "sendemailthread.h"
#include "myhelper.h"
#include "api.h"
#include "dbhelper.h"
#include "qemail/smtpmime.h"

SendEmailThread::SendEmailThread(QObject *parent) : QThread(parent)
{
    content = "";
    headMsg = "端口:邮件端口";
}

void SendEmailThread::SetContent(QString content)
{
    this->content = content;
}

void SendEmailThread::SetFileName(QString fileName)
{
    this->fileName = fileName;
}

void SendEmailThread::run()
{
    QString str;

    //发送告警邮件
    //实例化发送邮件对象
    QStringList list = App::SendEmailAddr.split("@");
    if (list.count() < 2) {
        str = "发件人邮箱格式错误";
    } else {
        QString tempSMTP = list.at(1).split(".").at(0);
        int tempPort = 25;
        //QQ邮箱端口号为465,必须启用SSL协议.
        if (tempSMTP.toUpper() == "QQ") {
            tempPort = 465;
        }

        SmtpClient smtp(QString("smtp.%1.com").arg(tempSMTP), tempPort,
                        tempPort == 25 ? SmtpClient::TcpConnection : SmtpClient::SslConnection);

        smtp.setUser(App::SendEmailAddr);
        smtp.setPassword(App::SendEmailPwd);

        //构建邮件主题,包含发件人收件人附件等.
        MimeMessage message;
        message.setSender(new EmailAddress(App::SendEmailAddr));

        //逐个添加收件人
        QStringList receiver = App::EmailAddr.split(';');
        for (int i = 0; i < receiver.size(); i++) {
            message.addRecipient(new EmailAddress(receiver.at(i)));
        }

        //构建邮件标题
        message.setSubject(App::Title + "告警邮件");

        //构建邮件正文
        MimeHtml text;
        text.setHtml(content);
        message.addPart(&text);

        //构建附件-告警图像
        if (fileName.length() > 0) {
            QStringList attas = fileName.split(";");
            foreach (QString tempAtta, attas) {
                QFile *file = new QFile(tempAtta);
                if (file->exists()) {
                    message.addPart(new MimeAttachment(file));
                }
            }
        }

        if (!smtp.connectToHost()) {
            str = "邮件服务器连接失败";
        } else {
            if (!smtp.login()) {
                str = "邮件用户登录失败";
            } else {
                if (!smtp.sendMail(message)) {
                    str = "告警邮件发送失败";
                } else {
                    str = "告警邮件发送成功";
                }
            }
        }

        smtp.quit();
    }

    qDebug() << TIME << headMsg << "命令解析:" << str;
    API::AddMessage(str);
    DBHelper::AddEventInfoUser(str);
}
