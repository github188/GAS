#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmconfig.h"
#include "ui_frmconfig.h"
#include "api/myhelper.h"
#include "api/hostapi.h"
#include "api/ntpapi.h"
#include "inputnew/frminputnew.h"

frmConfig::frmConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmConfig)
{
    ui->setupUi(this);
    this->InitForm();
    QTimer::singleShot(3000,this,SLOT(on_btnNTP_clicked()));
}

frmConfig::~frmConfig()
{
    delete ui;
}

void frmConfig::InitForm()
{
#ifdef __arm__
    ui->widget_left->setMinimumWidth(150);
    ui->widget_left->setMaximumWidth(150);
#endif

    QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(button_clicked()));
    }

    ui->btnConfig->click();

    //加载配置文件对应界面展示信息
    ui->btnUseInput->SetCheck(App::UseInput);
    connect(ui->btnUseInput, SIGNAL(pressed()), this, SLOT(SaveConfig()));

    ui->cboxInputPosition->addItem("控件下方");
    ui->cboxInputPosition->addItem("底部填充");
    ui->cboxInputPosition->addItem("屏幕居中");

    int index = 0;
    if (App::InputPosition == "control") {
        index = 0;
    } else if (App::InputPosition == "bottom") {
        index = 1;
    } else if (App::InputPosition == "center") {
        index = 2;
    }
    ui->cboxInputPosition->setCurrentIndex(index);
    connect(ui->cboxInputPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->txtTitle->setText(App::Title);
    connect(ui->txtTitle, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->txtVersion->setText(App::Version);
    connect(ui->txtVersion, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    QStringList qssName;
    qssName << "黑色风格" << "淡蓝色风格" << "蓝色风格" << "灰黑色风格"
            << "灰色风格" << "浅灰色风格" << "深灰色风格" << "银色风格";
    ui->cboxStyleName->addItems(qssName);

    if (App::StyleName == ":/qss/black.css") {
        ui->cboxStyleName->setCurrentIndex(0);
    } else if (App::StyleName == ":/qss/blue.css") {
        ui->cboxStyleName->setCurrentIndex(1);
    } else if (App::StyleName == ":/qss/dev.css") {
        ui->cboxStyleName->setCurrentIndex(2);
    } else if (App::StyleName == ":/qss/brown.css") {
        ui->cboxStyleName->setCurrentIndex(3);
    } else if (App::StyleName == ":/qss/gray.css") {
        ui->cboxStyleName->setCurrentIndex(4);
    } else if (App::StyleName == ":/qss/lightgray.css") {
        ui->cboxStyleName->setCurrentIndex(5);
    } else if (App::StyleName == ":/qss/darkgray.css") {
        ui->cboxStyleName->setCurrentIndex(6);
    } else if (App::StyleName == ":/qss/silvery.css") {
        ui->cboxStyleName->setCurrentIndex(7);
    }

    connect(ui->cboxStyleName, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->txtAuthor->setText(App::Author);
    connect(ui->txtAuthor, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->btnUseNTP->SetCheck(App::UseNTP);
    connect(ui->btnUseNTP, SIGNAL(pressed()), this, SLOT(SaveConfig()));
    ui->txtNTPIP->setText(App::NTPIP);
    connect(ui->txtNTPIP, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));
    for (int i = 0; i <= 24; i++) {
        ui->cboxNTPInterval->addItem(QString("%1小时").arg(i));
    }
    ui->cboxNTPInterval->setCurrentIndex(App::NTPInterval);
    connect(ui->cboxNTPInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    for (int i = 2015; i <= 2025; i++) {
        ui->cboxYear->addItem(QString::number(i));
    }
    for (int i = 1; i <= 12; i++) {
        ui->cboxMonth->addItem(QString::number(i));
    }
    for (int i = 1; i <= 31; i++) {
        ui->cboxDay->addItem(QString::number(i));
    }
    for (int i = 0; i < 24; i++) {
        ui->cboxHour->addItem(QString::number(i));
    }
    for (int i = 0; i < 60; i++) {
        ui->cboxMin->addItem(QString::number(i));
        ui->cboxSec->addItem(QString::number(i));
    }

    QString now = QDateTime::currentDateTime().toString("yyyy-M-d-H-m-s");
    QStringList str = now.split("-");
    ui->cboxYear->setCurrentIndex(ui->cboxYear->findText(str.at(0)));
    ui->cboxMonth->setCurrentIndex(ui->cboxMonth->findText(str.at(1)));
    ui->cboxDay->setCurrentIndex(ui->cboxDay->findText(str.at(2)));
    ui->cboxHour->setCurrentIndex(ui->cboxHour->findText(str.at(3)));
    ui->cboxMin->setCurrentIndex(ui->cboxMin->findText(str.at(4)));
    ui->cboxSec->setCurrentIndex(ui->cboxSec->findText(str.at(5)));

    //读取IP地址等信息
    QString fileName("/etc/eth0-setting");
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QString netInfo = QLatin1String(file.readAll());
    file.close();
    QStringList list = netInfo.split("\n");

    if (list.count() >= 4) {
        QString netIP = list.at(0).split("=").at(1);
        QString netMask = list.at(1).split("=").at(1);
        QString netGateway = list.at(2).split("=").at(1);
        QString netDNS = list.at(3).split("=").at(1);
        QString netMac = list.at(4).split("=").at(1);

        ui->txtNetIP->setText(netIP);
        ui->txtNetMask->setText(netMask);
        ui->txtNetGateway->setText(netGateway);
        ui->txtNetDNS->setText(netDNS);
        ui->txtNetMac->setText(netMac);
    }

    connect(NTPAPI::Instance(), SIGNAL(ReceiveTime(QDateTime)), this, SLOT(ReceiveTime(QDateTime)));
}

void frmConfig::button_clicked()
{
    QPushButton *btn = (QPushButton *)sender();
    QString name = btn->text();
    QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();
    foreach (QPushButton * b, btns) {
        b->setChecked(false);
    }
    btn->setChecked(true);

    if (name == "主界面") {
        ui->stackedWidget->setCurrentIndex(0);
    } else if (name == "记录查询") {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void frmConfig::SaveConfig()
{
    //保存配置文件信息
    App::UseInput = ui->btnUseInput->GetCheck();
    if (!App::UseInput) {
        frmInputNew::Instance()->hide();
    }

    QString str = ui->cboxInputPosition->currentText();
    QString position ;

    if (str == "控件下方") {
        position = "control";
    } else if (str == "底部填充") {
        position = "bottom";
    } else if (str == "屏幕居中") {
        position = "center";
    }

    if (position != App::InputPosition) {
        App::InputPosition = position;
#ifdef __arm__
    frmInputNew::Instance()->init(App::InputPosition, "black", App::FontSize + 2, App::FontSize - 2, 800, 230, 20, 20, 6, 45);
#else
    frmInputNew::Instance()->init(App::InputPosition, "black", App::FontSize + 2, App::FontSize, 700, 230, 20, 20, 6, 45);
#endif
    }

    App::Title = ui->txtTitle->text();
    App::Version = ui->txtVersion->text();

    QString style;
    int styleIndex = ui->cboxStyleName->currentIndex();
    if (styleIndex == 0) {
        style = ":/qss/black.css";
    } else if (styleIndex == 1) {
        style = ":/qss/blue.css";
    } else if (styleIndex == 2) {
        style = ":/qss/dev.css";
    } else if (styleIndex == 3) {
        style = ":/qss/brown.css";
    } else if (styleIndex == 4) {
        style = ":/qss/gray.css";
    } else if (styleIndex == 5) {
        style = ":/qss/lightgray.css";
    } else if (styleIndex == 6) {
        style = ":/qss/darkgray.css";
    } else if (styleIndex == 7) {
        style = ":/qss/silvery.css";
    }

    if (style != App::StyleName) {
        App::StyleName = style;
        myHelper::SetStyle(App::StyleName);
    }

    App::Author = ui->txtAuthor->text();

    App::UseNTP = ui->btnUseNTP->GetCheck();
    App::NTPIP = ui->txtNTPIP->text();
    App::NTPInterval = ui->cboxNTPInterval->currentIndex();
    HostAPI::Instance()->SetNTPInterval();

    //调用保存配置文件函数
    App::WriteConfig();
}

void frmConfig::on_btnSetTime_clicked()
{    
    QString year = QString("%1").arg(ui->cboxYear->currentText());
    QString month = QString("%1").arg(ui->cboxMonth->currentText().toInt(), 2, 10, QChar('0'));
    QString day = QString("%1").arg(ui->cboxDay->currentText().toInt(), 2, 10, QChar('0'));
    QString hour = QString("%1").arg(ui->cboxHour->currentText().toInt(), 2, 10, QChar('0'));
    QString min = QString("%1").arg(ui->cboxMin->currentText().toInt(), 2, 10, QChar('0'));
    QString sec = QString("%1").arg(ui->cboxSec->currentText().toInt(), 2, 10, QChar('0'));
    myHelper::SetSystemDateTime(year, month, day, hour, min, sec);
}

void frmConfig::on_btnSetNet_clicked()
{
#ifndef __arm__
    return;
#endif

    QString netIP = ui->txtNetIP->text();
    QString netMask = ui->txtNetMask->text();
    QString netGateway = ui->txtNetGateway->text();
    QString netDNS = ui->txtNetDNS->text();
    QString netMac = ui->txtNetMac->text();

    if (netIP.length() == 0 || netMask.length() == 0 || netGateway.length() == 0 || netDNS.length() == 0) {
        myHelper::ShowMessageBoxError("网络参数不能为空!");
        return;
    }

    if (!myHelper::IsIP(netIP)) {
        myHelper::ShowMessageBoxError("IP地址不合法!");
        ui->txtNetIP->setFocus();
        return;
    }

    if (!myHelper::IsIP(netGateway)) {
        myHelper::ShowMessageBoxError("网关地址不合法!");
        ui->txtNetGateway->setFocus();
        return;
    }

    if (!myHelper::IsIP(netDNS)) {
        myHelper::ShowMessageBoxError("DNS地址不合法!");
        ui->txtNetDNS->setFocus();
        return;
    }

    QStringList list;
    list << QString("IP=%1\n").arg(netIP);
    list << QString("Mask=%1\n").arg(netMask);
    list << QString("Gateway=%1\n").arg(netGateway);
    list << QString("DNS=%1\n").arg(netDNS);
    list << QString("MAC=%1\n").arg(netMac);

    QString netInfo = list.join("");
    QString fileName("/etc/eth0-setting");
    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << netInfo;
    file.close();

    myHelper::ShowMessageBoxInfoX("设置成功,稍后重启!");
    myHelper::Sleep(2000);
    system("reboot");
}

void frmConfig::on_btnNTP_clicked()
{
    NTPAPI::Instance()->GetTime(App::NTPIP);
}

void frmConfig::ReceiveTime(QDateTime dateTime)
{
    QString now = dateTime.toString("yyyy-M-d-H-m-s");
    QStringList str = now.split("-");
    ui->cboxYear->setCurrentIndex(ui->cboxYear->findText(str.at(0)));
    ui->cboxMonth->setCurrentIndex(ui->cboxMonth->findText(str.at(1)));
    ui->cboxDay->setCurrentIndex(ui->cboxDay->findText(str.at(2)));
    ui->cboxHour->setCurrentIndex(ui->cboxHour->findText(str.at(3)));
    ui->cboxMin->setCurrentIndex(ui->cboxMin->findText(str.at(4)));
    ui->cboxSec->setCurrentIndex(ui->cboxSec->findText(str.at(5)));
}
