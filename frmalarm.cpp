#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmalarm.h"
#include "ui_frmalarm.h"
#include "api/myhelper.h"
#include "api/api.h"
#include "api/dbhelper.h"
#include "api/hostapi.h"
#include "api/dbapi.h"
#include "api/excelapi.h"
#include "api/pdfapi.h"
#include "api/printapi.h"
#include "frmselect.h"
#include "frmdefencelot.h"

frmAlarm::frmAlarm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmAlarm)
{
    ui->setupUi(this);

    this->InitForm();
    this->LoadImageFile();
    this->LoadSoundFile();
    this->LoadLinkInfo();

    //设置防区容器/防区树状列表/防区缩略图等
    API::SetControl(ui->boxMain, ui->tableWidget, ui->treeWidget, ui->listWidget);

    QTimer::singleShot(100, this, SLOT(Init()));
    //延时10秒钟处理自动布防
    QTimer::singleShot(10000, this, SLOT(AutoBuFang()));
}

frmAlarm::~frmAlarm()
{
    delete ui;
}

bool frmAlarm::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->gboxMain) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                ui->txtDefenceX->setText(QString::number(mouseEvent->x()));
                ui->txtDefenceY->setText(QString::number(mouseEvent->y()));
                return true;
            }
        }
    } else if (obj == ui->txtDebug->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            ui->txtDebug->clear();
            debugLogCount = 0;
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void frmAlarm::button_clicked()
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
        if (defenceChange) {
            API::InitDefenceAll();
            API::InitDefenceCurrent();
            API::InitTreeWidget();
            defenceChange = false;
        }
    } else if (name == "防区设置") {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (name == "记录查询") {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (name == "图像查询") {
        ui->stackedWidget->setCurrentIndex(3);
        ui->widget_image_view->setFocus();
    } else if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(4);
    } else if (name == "调试信息") {
        ui->stackedWidget->setCurrentIndex(5);
    } else if (name == "帮助文档") {
        ui->stackedWidget->setCurrentIndex(6);
    }
}

void frmAlarm::InitForm()
{
#ifdef __arm__
    ui->widget_left->setMinimumWidth(150);
    ui->widget_left->setMaximumWidth(150);
    ui->widget_right_main->setMinimumWidth(250);
    ui->widget_right_main->setMaximumWidth(250);
    ui->widget_right_defence->setMinimumWidth(250);
    ui->widget_right_defence->setMaximumWidth(250);
    ui->gboxRightVideo->setMinimumWidth(250);
    ui->gboxRightVideo->setMaximumWidth(250);
#endif

#ifdef __arm__
    ui->tableMain->verticalHeader()->setDefaultSectionSize(35);
#else
    ui->tableMain->verticalHeader()->setDefaultSectionSize(25);
#endif

    ui->txtDebug->setProperty("noinput", true);
    ui->txtHelper->setProperty("noinput", true);

    //读取帮助文档加载到文本框
    ui->txtHelper->setCurrentFont(QFont(App::FontName, App::FontSize));
    QFile file(":/help.txt");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QByteArray ba = file.readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString str = codec->toUnicode(ba);
        ui->txtHelper->setText(str);
        file.close();
    }

    defenceChange = false;
    defenceID = "";

    menu = new QMenu(ui->treeWidget);
    QMenu *menu_BuFang = menu->addMenu("主机布防");
    menu_BuFang->addAction("子系统1", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统2", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统3", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统4", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统5", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统6", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统7", this, SLOT(BuFang()));
    menu_BuFang->addAction("子系统8", this, SLOT(BuFang()));

    QMenu *menu_CheFang = menu->addMenu("主机撤防");
    menu_CheFang->addAction("子系统1", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统2", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统3", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统4", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统5", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统6", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统7", this, SLOT(CheFang()));
    menu_CheFang->addAction("子系统8", this, SLOT(CheFang()));

    menu->addSeparator();
    menu->addAction("防区旁路", this, SLOT(PangLu()));
    menu->addAction("旁路恢复", this, SLOT(HuiFu()));

    connect(HostAPI::Instance(), SIGNAL(HostReceiveData(QString, QString, QString)),
            this, SLOT(HostReceiveData(QString, QString, QString)));

    debugLogCount = 0;

    ui->cboxHostType->addItem("DS7400");
    ui->cboxHostType->addItem("VISTA128");
    ui->cboxHostType->setCurrentIndex(ui->cboxHostType->findText(App::HostType));
    connect(ui->cboxHostType, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    QStringList comList;
#ifdef Q_OS_WIN32
    comList.clear();
    comList << "COM1" << "COM2" << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8";
#endif
#ifdef Q_OS_LINUX
    comList.clear();
    comList << "ttyUSB0" << "ttyUSB1" << "ttyS0" << "ttyS1";
#endif
#ifdef __arm__
    comList.clear();
    comList << "ttySAC0" << "ttySAC1" << "ttySAC2" << "ttySAC3";
#endif
    ui->cboxHostCom->addItems(comList);
    ui->cboxHostCom->setCurrentIndex(ui->cboxHostCom->findText(App::HostCom));
    connect(ui->cboxHostCom, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->txtAlarmServerIP->setText(App::AlarmServerIP);
    connect(ui->txtAlarmServerIP, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->txtAlarmServerPort->setText(QString::number(App::AlarmServerPort));
    connect(ui->txtAlarmServerPort, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    for (int i = 20; i <= 50; i++) {
        ui->cboxMsgCount->addItem(QString::number(i));
    }
    ui->cboxMsgCount->setCurrentIndex(ui->cboxMsgCount->findText(QString::number(App::MsgCount)));
    connect(ui->cboxMsgCount, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->cboxSnapPath->addItem("/snap");
    ui->cboxSnapPath->addItem("/sdcard/snap");
    ui->cboxVideoPath->addItem("/video");
    ui->cboxVideoPath->addItem("/sdcard/video");
    ui->cboxSnapPath->lineEdit()->setText(App::SnapPath);
    ui->cboxVideoPath->lineEdit()->setText(App::VideoPath);
    connect(ui->cboxSnapPath->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));
    connect(ui->cboxVideoPath->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->btnSaveLog->SetCheck(App::SaveLog);
    ui->btnAutoBuFang->SetCheck(App::AutoBuFang);
    ui->btnVideoLink->SetCheck(App::VideoLink);
    ui->btnNetLink->SetCheck(App::NetLink);
    ui->btnSnapLink->SetCheck(App::SnapLink);
    ui->btnPrintLink->SetCheck(App::PrintLink);

    connect(ui->btnSaveLog, SIGNAL(pressed()), this, SLOT(SaveConfig()));
    connect(ui->btnAutoBuFang, SIGNAL(pressed()), this, SLOT(SaveConfig()));
    connect(ui->btnVideoLink, SIGNAL(pressed()), this, SLOT(SaveConfig()));
    connect(ui->btnNetLink, SIGNAL(pressed()), this, SLOT(SaveConfig()));
    connect(ui->btnSnapLink, SIGNAL(pressed()), this, SLOT(SaveConfig()));
    connect(ui->btnPrintLink, SIGNAL(pressed()), this, SLOT(SaveConfig()));

    ui->cboxMsgPortName->addItems(comList);
    ui->cboxMsgPortName->setCurrentIndex(ui->cboxMsgPortName->findText(App::MsgPortName));
    connect(ui->cboxMsgPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->cboxMsgBaudRate->setCurrentIndex(ui->cboxMsgBaudRate->findText(QString::number(App::MsgBaudRate)));
    connect(ui->cboxMsgBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->txtMsgTel->setText(App::MsgTel);
    connect(ui->txtMsgTel, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->txtSendEmailAddr->setText(App::SendEmailAddr);
    connect(ui->txtSendEmailAddr, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->txtSendEmailPwd->setText(App::SendEmailPwd);
    connect(ui->txtSendEmailPwd, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    ui->txtEmailAddr->setText(App::EmailAddr);
    connect(ui->txtEmailAddr, SIGNAL(textChanged(QString)), this, SLOT(SaveConfig()));

    QStringList interval;
    interval << "0" << "1" << "5" << "10" << "30";
    for (int i = 60; i <= 1440; i = i + 60) {
        interval << QString::number(i);
    }
    //如果是10000则不启用
    interval << "10000";

    ui->cboxMsgInterval->addItems(interval);
    ui->cboxEmailInterval->addItems(interval);
    ui->cboxMsgInterval->setCurrentIndex(ui->cboxMsgInterval->findText(QString::number(App::MsgInterval)));
    connect(ui->cboxMsgInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    ui->cboxEmailInterval->setCurrentIndex(ui->cboxEmailInterval->findText(QString::number(App::EmailInterval)));
    connect(ui->cboxEmailInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(SaveConfig()));

    QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(button_clicked()));
    }

    ui->btnMain->click();

    //初始化报警防区设置部分
    //填充防区号
    for (int i = 1; i <= 255; i++) {
        if (i < 10) {
            ui->cboxDefenceID->addItem(QString("00%1").arg(i));
            continue;
        }
        if (i < 100) {
            ui->cboxDefenceID->addItem(QString("0%1").arg(i));
            continue;
        }
        if (i < 1000) {
            ui->cboxDefenceID->addItem(QString("%1").arg(i));
            continue;
        }
    }

    //填充子系统
    for (int i = 1; i <= 8; i++) {
        ui->cboxSubSystem->addItem(QString::number(i));
    }

    ui->treeDefence->setColumnCount(1);
    ui->treeDefence->setColumnWidth(0, 170);
    ui->treeDefence->header()->setVisible(false);
    ui->treeDefence->setEditTriggers(QAbstractItemView::NoEditTriggers);

    whereSql = "where 1=1";
    pageCount = 30;
    for (int i = 30; i <= 100; i = i + 10) {
        ui->cboxPageCount->addItem(QString::number(i));
    }

    columnNames.append("告警时间");
    columnNames.append("告警类型");
    columnNames.append("告警内容");
    columnNames.append("防区编号");
    columnNames.append("防区名称");

#ifdef __arm__
    int width = App::DeskWidth - 120;
#else
    int width = App::DeskWidth - 170;
#endif

    columnWidths.append(width * 0.28);
    columnWidths.append(width * 0.15);
    columnWidths.append(width * 0.28);
    columnWidths.append(width * 0.10);
    columnWidths.append(width * 0.15);

    ui->cboxType->addItem("主机上报");
    ui->cboxType->addItem("用户操作");

    QStringList triggerContent;
    triggerContent << "防区报警" << "主机布防" << "主机撤防" << "主机故障" << "主机上线" << "主机下线";
    triggerContent << "防区现场复位" << "防区软件复位" << "主机旁路" << "防区旁路" << "防区故障" << "电源故障";
    ui->cboxContent->addItems(triggerContent);

    //绑定下拉框
    DBAPI::Instance()->BindData("DefenceName", "DefenceID", "DefenceInfo", ui->cboxDefenceName);
    //设置需要显示数据的表格和翻页的按钮
    DBAPI::Instance()->SetControl(ui->tableMain, ui->labInfo, ui->btnFirst, ui->btnPre, ui->btnNext, ui->btnLast);
    //加载初始数据
    DBAPI::Instance()->BindDataSelect("EventInfo", "TriggerTime desc", "where 1=1", pageCount, columnNames, columnWidths);
    //最后一列自动填充
    ui->tableMain->horizontalHeader()->setStretchLastSection(true);
    //奇偶行不同颜色显示
    ui->tableMain->setAlternatingRowColors(true);

    ui->ckDefenceName->setChecked(false);
    ui->ckType->setChecked(false);
    ui->ckContent->setChecked(false);
    ui->ckTime->setChecked(false);

    ui->dateStart->setEnabled(false);
    ui->dateEnd->setEnabled(false);
    ui->cboxDefenceName->setEnabled(false);
    ui->cboxType->setEnabled(false);
    ui->cboxContent->setEnabled(false);

    ui->dateStart->setDate(QDate::currentDate());
    ui->dateEnd->setDate(QDate::currentDate().addDays(1));

    ui->dateStart->calendarWidget()->setLocale(QLocale::Chinese);
    ui->dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
    ui->dateStartVideo->calendarWidget()->setLocale(QLocale::Chinese);
    ui->dateEndVideo->calendarWidget()->setLocale(QLocale::Chinese);

    DBAPI::Instance()->BindData("DefenceID", "DefenceID", "DefenceInfo", ui->cboxDefenceIDx);
    DBAPI::Instance()->BindData("DefenceName", "DefenceID", "DefenceInfo", ui->cboxDefenceNamex);
    connect(ui->cboxDefenceNamex, SIGNAL(currentIndexChanged(int)),
            ui->cboxDefenceIDx, SLOT(setCurrentIndex(int)));
    //禁用防区编号,用户选择防区名称即可自动切换
    ui->cboxDefenceIDx->setEnabled(false);

    for (int i = 1; i <= 16; i++) {
        ui->cboxNVRCh->addItem(QString::number(i));
    }

    for (int i = 1; i <= 30; i++) {
        ui->cboxSnapTime->addItem(QString::number(i));
    }
    ui->cboxSnapTime->setCurrentIndex(4);

    ui->cboxVideoType->addItem("告警抓拍图像");
    ui->cboxVideoType->addItem("告警联动视频");
    ui->dateStartVideo->setDate(QDate::currentDate());
    ui->dateEndVideo->setDate(QDate::currentDate().addDays(1));

    ui->gboxMain->installEventFilter(this);
    ui->txtDebug->viewport()->installEventFilter(this);

    QStringList str;
    for (int i = 1; i <= 8; i++) {
        str.append(QString("子系统%1").arg(i));
    }
    str.append("所有系统");
    ui->cboxSubsystemID->addItems(str);
}

void frmAlarm::Init()
{
    API::InitDefenceAll();        //加载所有防区数据到容器
    API::InitDefenceCurrent();    //加载当前防区
    API::InitTreeWidget();        //加载主机防区树状图
    API::InitListWidget();        //加载防区缩略图

    HostAPI::Instance()->Init();

    API::InitTreeWidget(ui->treeDefence);
    API::InitDefenceAll(ui->gboxMain);
    API::InitDefenceCurrent(ui->gboxMain);

    //设置翻页按钮图形字体
    IconHelper::Instance()->SetIcon(ui->btnFirst, QChar(0xf049));
    IconHelper::Instance()->SetIcon(ui->btnPre, QChar(0xf04a));
    IconHelper::Instance()->SetIcon(ui->btnNext, QChar(0xf04e));
    IconHelper::Instance()->SetIcon(ui->btnLast, QChar(0xf050));
}

void frmAlarm::AutoBuFang()
{
    if (App::AutoBuFang) {
        HostAPI::Instance()->BuFang();
    }
}

void frmAlarm::LoadImageFile()
{
    ui->cboxDefenceImage->clear();
    QDir imagePath(App::AppPath + "/map");
    QStringList filter;
    filter << "*.jpg" << "*.bmp" << "*.png" << "*.jpeg"; //过滤图片文件拓展名
    ui->cboxDefenceImage->addItems(imagePath.entryList(filter));
}

void frmAlarm::LoadSoundFile()
{
    ui->cboxDefenceSound->clear();
    QDir soundPath(App::AppPath + "/sound");
    QStringList filter;
    filter << "*.wav";
    ui->cboxDefenceSound->addItem("无");
    ui->cboxDefenceSound->addItems(soundPath.entryList(filter));
}

void frmAlarm::SaveConfig()
{
    //保存配置文件信息
    App::HostType = ui->cboxHostType->currentText();
    App::HostCom = ui->cboxHostCom->currentText();
    App::AlarmServerIP = ui->txtAlarmServerIP->text();
    App::AlarmServerPort = ui->txtAlarmServerPort->text().toInt();
    App::MsgCount = ui->cboxMsgCount->currentText().toInt();
    App::SnapPath = ui->cboxSnapPath->currentText();
    App::VideoPath = ui->cboxVideoPath->currentText();

    App::SaveLog = ui->btnSaveLog->GetCheck();
    App::AutoBuFang = ui->btnAutoBuFang->GetCheck();
    App::VideoLink = ui->btnVideoLink->GetCheck();
    App::NetLink = ui->btnNetLink->GetCheck();
    App::SnapLink = ui->btnSnapLink->GetCheck();
    App::PrintLink = ui->btnPrintLink->GetCheck();

    App::MsgPortName = ui->cboxMsgPortName->currentText();
    App::MsgBaudRate = ui->cboxMsgBaudRate->currentText().toInt();
    int msgInterval = ui->cboxMsgInterval->currentText().toInt();
    //自动更改告警短信发送间隔
    if (msgInterval != App::MsgInterval) {
        App::MsgInterval = msgInterval;
        HostAPI::Instance()->SetMsgInterval();
    }
    App::MsgTel = ui->txtMsgTel->text();

    App::SendEmailAddr = ui->txtSendEmailAddr->text();
    App::SendEmailPwd = ui->txtSendEmailPwd->text();
    int emailInterval = ui->cboxEmailInterval->currentText().toInt();
    //自动更改告警邮件发送间隔
    if (emailInterval != App::EmailInterval) {
        App::EmailInterval = emailInterval;
        HostAPI::Instance()->SetEmailInterval();
    }
    App::EmailAddr = ui->txtEmailAddr->text();

    //调用保存配置文件函数
    App::WriteConfig();
}

void frmAlarm::LoadDefenceInfo()
{
    API::InitDefenceAll(ui->gboxMain);
    API::InitDefenceCurrent(ui->gboxMain);
    API::InitTreeWidget(ui->treeDefence);
    defenceChange = true;
}

void frmAlarm::HostReceiveData(QString , QString hostName, QString data)
{
    if (App::SaveLog) {
        if (debugLogCount > 50) {
            ui->txtDebug->clear();
            debugLogCount = 0;
        }

        QString str = QString("时间[%1][%2] >> %3").arg(TIME).arg(hostName).arg(data);
        ui->txtDebug->append(str);
        debugLogCount++;
    }
}

void frmAlarm::BuFang()
{
    QAction *action = (QAction *)sender();
    int subSystem = action->text().right(1).toInt();
    HostAPI::Instance()->BuFang(subSystem);
}

void frmAlarm::CheFang()
{
    QAction *action = (QAction *)sender();
    int subSystem = action->text().right(1).toInt();
    HostAPI::Instance()->CheFang(subSystem);
}

void frmAlarm::PangLu()
{
    if (defenceID.length() > 0) {
        HostAPI::Instance()->PangLu(defenceID);
    }
    defenceID = "";
}

void frmAlarm::HuiFu()
{
    if (defenceID.length() > 0) {
        HostAPI::Instance()->HuiFu(defenceID);
    }
    defenceID = "";
}

void frmAlarm::on_btnBuFang_clicked()
{
    int index = ui->cboxSubsystemID->currentIndex();
    if (index == 8) {
        HostAPI::Instance()->BuFang();
    } else {
        HostAPI::Instance()->BuFang(index + 1);
    }
}

void frmAlarm::on_btnCheFang_clicked()
{
    int index = ui->cboxSubsystemID->currentIndex();
    if (index == 8) {
        HostAPI::Instance()->CheFang();
    } else {
        HostAPI::Instance()->CheFang(index + 1);
    }
}

void frmAlarm::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString tempImage = item->text();
    if (App::CurrentImage != tempImage) {
        App::CurrentImage = tempImage;
        API::InitDefenceCurrent();
    }
}

void frmAlarm::on_treeWidget_itemPressed(QTreeWidgetItem *item, int )
{
    if (qApp->mouseButtons() == Qt::LeftButton) {
        return;
    }

    if (qApp->mouseButtons() == Qt::RightButton) {
        if (item->parent() != 0) {
            defenceID = item->text(0).split("[").at(1).split("]").at(0);
        }
        menu->exec(QCursor::pos());
    }
}

void frmAlarm::on_btnAdd_clicked()
{
    //判断是否启用了设备数量限制,如果启用了判断设备数量是否超过限制
    int count = ui->treeDefence->topLevelItem(0)->childCount();
    if (App::KeyUseCount) {
        if (count >= App::KeyCount) {
            myHelper::ShowMessageBoxError("防区数量超过限制,请联系供应商更新注册码!");
            return;
        }
    }

    QString defenceID = ui->cboxDefenceID->currentText();
    QString defenceName = ui->txtDefenceName->text();
    QString subSystem = ui->cboxSubSystem->currentText();
    QString defenceSound = ui->cboxDefenceSound->currentText();
    QString defenceImage = ui->cboxDefenceImage->currentText();
    QString defenceX = ui->txtDefenceX->text();
    QString defenceY = ui->txtDefenceY->text();

    if (DBHelper::IsExistDefence(defenceID)) {
        myHelper::ShowMessageBoxError("该防区号已经存在,请重新选择!");
        return;
    }

    if (defenceName == "") {
        myHelper::ShowMessageBoxError("防区名称不能为空,请重新填写!");
        ui->txtDefenceName->setFocus();
        return;
    }

    QSqlQuery query;
    QString sql = "insert into DefenceInfo(DefenceID,DefenceName,SubSystem,DefenceSound,DefenceImage,DefenceX,DefenceY) values('";
    sql += defenceID + "','";
    sql += defenceName + "','";
    sql += subSystem + "','";
    sql += defenceSound + "','";
    sql += defenceImage + "','";
    sql += defenceX + "','";
    sql += defenceY + "')";
    query.exec(sql);

    LoadDefenceInfo();

    //防区号自动递增
    int index = ui->cboxDefenceID->currentIndex();
    if (index < 255) {
        ui->cboxDefenceID->setCurrentIndex(index + 1);
    }
}

void frmAlarm::on_btnDelete_clicked()
{
    //没有选择防区或者选中的是主机
    if (ui->treeDefence->currentItem() == 0 || ui->treeDefence->currentItem()->parent() == 0) {
        myHelper::ShowMessageBoxError("请选择要删除的防区!");
        return;
    }

    tempDefenceID = ui->cboxDefenceID->currentText();
    QSqlQuery query;
    QString sql = "delete from DefenceInfo where DefenceID='" + tempDefenceID + "'";
    query.exec(sql);

    LoadDefenceInfo();
}

void frmAlarm::on_btnUpdate_clicked()
{
    //没有选择防区或者选中的是主机
    if (ui->treeDefence->currentItem() == 0 || ui->treeDefence->currentItem()->parent() == 0) {
        myHelper::ShowMessageBoxError("请选择要修改的防区!");
        return;
    }

    QString defenceID = ui->cboxDefenceID->currentText();
    QString defenceName = ui->txtDefenceName->text();
    QString subSystem = ui->cboxSubSystem->currentText();
    QString defenceSound = ui->cboxDefenceSound->currentText();
    QString defenceImage = ui->cboxDefenceImage->currentText();
    QString defenceX = ui->txtDefenceX->text();
    QString defenceY = ui->txtDefenceY->text();

    //如果防区号变动,要去与数据库中除自己外的防区号比较,如果重复则提示
    if (tempDefenceID != defenceID) {
        if (DBHelper::IsExistDefence(defenceID)) {
            myHelper::ShowMessageBoxError("该防区号已经存在,请重新选择!");
            return;
        }
    }

    if (defenceName == "") {
        myHelper::ShowMessageBoxError("防区名称不能为空,请重新填写!");
        ui->txtDefenceName->setFocus();
        return;
    }

    QSqlQuery query;
    QString sql = "update DefenceInfo set";
    sql += " DefenceID='" + defenceID;
    sql += "',DefenceName='" + defenceName;
    sql += "',SubSystem='" + subSystem;
    sql += "',DefenceSound='" + defenceSound;
    sql += "',DefenceImage='" + defenceImage;
    sql += "',DefenceX='" + defenceX;
    sql += "',DefenceY='" + defenceY;
    sql += "' where DefenceID='" + tempDefenceID + "'";
    query.exec(sql);

    LoadDefenceInfo();
}

void frmAlarm::on_btnMap_clicked()
{
    QString fileName = myHelper::GetFileName("图片文件(*.jpg *.png *.bmp *.jpeg)");
    if (fileName == "") {
        return;
    }

    bool ok = myHelper::CopyFile(fileName, App::AppPath + "/map/" + myHelper::GetFileNameWithExtension(fileName));
    if (ok) {
        LoadImageFile();
        API::InitListWidget();
        myHelper::ShowMessageBoxInfo("导入地图文件成功!");
    } else {
        myHelper::ShowMessageBoxError("导入地图文件失败!");
    }
}

void frmAlarm::UpdateDefenceInfo(QString defenceID, int defenceX, int defenceY)
{
    QSqlQuery query;
    QString sql = QString("update DefenceInfo set DefenceX='%1',DefenceY='%2' where DefenceID='%3'")
            .arg(defenceX).arg(defenceY).arg(defenceID);
    query.exec(sql);
}

void frmAlarm::on_btnMove_clicked()
{
    if (ui->btnMove->text() == "调整(&M)") {
        App::IsMove = true;
        ui->btnAdd->setEnabled(false);
        ui->btnDelete->setEnabled(false);
        ui->btnUpdate->setEnabled(false);
        ui->btnMap->setEnabled(false);
        ui->btnLot->setEnabled(false);
        ui->btnMove->setText("保存(&S)");
    } else {
        QSqlDatabase::database().transaction();

        QList<DefenceButton *> btnDefence = ui->gboxMain->findChildren<DefenceButton *>();
        foreach (DefenceButton * b, btnDefence) {
            UpdateDefenceInfo(b->DefenceID(), b->geometry().x(), b->geometry().y());
        }

        QSqlDatabase::database().commit();

        App::IsMove = false;
        ui->btnAdd->setEnabled(true);
        ui->btnDelete->setEnabled(true);
        ui->btnUpdate->setEnabled(true);
        ui->btnMap->setEnabled(true);
        ui->btnLot->setEnabled(true);
        ui->btnMove->setText("调整(&M)");

        defenceChange = true;
        myHelper::ShowMessageBoxInfo("保存防区位置成功!");
    }
}

void frmAlarm::on_btnLot_clicked()
{
    frmDefenceLot defenceLot;
    connect(&defenceLot, SIGNAL(AddDefence()), this, SLOT(LoadDefenceInfo()));
    defenceLot.SetSubSystem(ui->cboxSubSystem->currentText());
    defenceLot.exec();
}

void frmAlarm::on_treeDefence_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *)
{
    if (current == 0) {
        return;
    }
    //说明选择的是顶层主机
    if (current->parent() == 0) {
        return;
    }

    tempDefenceID = current->text(0).split("[").at(1).split("]").at(0);

    QSqlQuery query;
    QString sql = QString("select DefenceName,SubSystem,DefenceSound,DefenceImage,DefenceX,"
                          "DefenceY from DefenceInfo where DefenceID='%1'").arg(tempDefenceID);
    query.exec(sql);
    if (query.next()) {
        QString tempDefenceName = query.value(0).toString();
        QString tempSubsystem = query.value(1).toString();
        QString tempDefenceSound = query.value(2).toString();
        QString tempDefenceImage = query.value(3).toString();
        QString tempDefenceX = query.value(4).toString();
        QString tempDefenceY = query.value(5).toString();

        ui->cboxDefenceID->setCurrentIndex(ui->cboxDefenceID->findText(tempDefenceID));
        ui->txtDefenceName->setText(tempDefenceName);
        ui->cboxSubSystem->setCurrentIndex(ui->cboxSubSystem->findText(tempSubsystem));
        ui->cboxDefenceSound->setCurrentIndex(ui->cboxDefenceSound->findText(tempDefenceSound));
        ui->cboxDefenceImage->setCurrentIndex(ui->cboxDefenceImage->findText(tempDefenceImage));
        ui->txtDefenceX->setText(tempDefenceX);
        ui->txtDefenceY->setText(tempDefenceY);

        QList<DefenceButton *> btn = ui->gboxMain->findChildren<DefenceButton *>();
        //在地图中找到该防区,显示为布防颜色,区别于其他防区
        foreach (DefenceButton * b, btn) {
            if (b->DefenceID() == tempDefenceID) {
                b->SetDefenceStatus(DefenceButton::Status_BuFang);
            } else {
                b->SetDefenceStatus(DefenceButton::Status_GuZhang);
            }
        }
    }
}

void frmAlarm::on_cboxDefenceID_currentIndexChanged(const QString &arg1)
{
    ui->txtDefenceName->setText(QString("%1防区").arg(arg1));
}

void frmAlarm::on_cboxDefenceSound_currentIndexChanged(const QString &arg1)
{
    if (!this->isVisible()) {
        return;
    }
    if (arg1.length() > 0) {
        QSound sound(QString("%1/sound/%2").arg(App::AppPath).arg(arg1));
        sound.play();
    }
}

void frmAlarm::on_cboxDefenceImage_currentIndexChanged(const QString &arg1)
{
    App::CurrentImage = arg1;
    API::InitDefenceCurrent(ui->gboxMain);
}

void frmAlarm::on_ckDefenceName_toggled(bool checked)
{
    ui->cboxDefenceName->setEnabled(checked);
}

void frmAlarm::on_ckType_toggled(bool checked)
{
    ui->cboxType->setEnabled(checked);
}

void frmAlarm::on_ckContent_toggled(bool checked)
{
    ui->cboxContent->setEnabled(checked);
}

void frmAlarm::on_ckTime_toggled(bool checked)
{
    ui->dateEnd->setEnabled(checked);
    ui->dateStart->setEnabled(checked);
}

void frmAlarm::on_cboxPageCount_currentIndexChanged(const QString &arg1)
{
    if (!this->isVisible()) {
        return;
    }

    pageCount = arg1.toInt();

    //重新查询数据
    on_btnSelect_clicked();
}

void frmAlarm::on_btnSelect_clicked()
{
    //构建SQL语句
    QString sql = "where 1=1";

    if (ui->ckDefenceName->isChecked()) {
        sql += " and DefenceName='" + ui->cboxDefenceName->currentText() + "'";
    }

    if (ui->ckType->isChecked()) {
        sql += " and TriggerType='" + ui->cboxType->currentText() + "'";
    }

    if (ui->ckContent->isChecked()) {
        sql += " and TriggerContent='" + ui->cboxContent->currentText() + "'";
    }

    if (ui->dateStart->dateTime() > ui->dateEnd->dateTime()) {
        myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
        return;
    }

    if (ui->ckTime->isChecked()) {
        sql += " and datetime(TriggerTime)>='" + ui->dateStart->dateTime().toString("yyyy-MM-dd HH:mm:ss") + "'";
        sql += " and datetime(TriggerTime)<='" + ui->dateEnd->dateTime().toString("yyyy-MM-dd HH:mm:ss") + "'";
    }

    //绑定数据到表格
    DBAPI::Instance()->BindDataSelect("EventInfo", "TriggerTime desc", sql, pageCount, columnNames, columnWidths);
    whereSql = sql;
}

void frmAlarm::on_btnDeleteData_clicked()
{
    frmSelect select;//弹出日期选择界面
    if (select.exec() == 1) {
        QString timeStart = select.SelectStartDateTime();
        QString timeEnd = select.SelectEndDateTime();
        //询问是否要删除数据
        if ( myHelper::ShowMessageBoxQuestion("确定要删除该时间段的数据吗?") == 1) {
            //执行sql语句,删除数据
            QString sql = "delete from EventInfo where datetime(TriggerTime)>='";
            sql += timeStart;
            sql += "' and datetime(TriggerTime)<='";
            sql += timeEnd + "'";
            QSqlQuery query;
            query.exec(sql);

            QString TempMsg = "删除数据成功";
            DBHelper::AddEventInfoUser(TempMsg);
            myHelper::ShowMessageBoxInfo(QString("%1!").arg(TempMsg));

            //重新查询数据
            on_btnSelect_clicked();
        }
    }
}

void frmAlarm::on_btnPrint_clicked()
{
#ifdef __arm__
    myHelper::ShowMessageBoxError("未安装打印机!");
    return;
#endif

    QString type = "所有告警记录";

    QStringList content = GetContent();
    int rowCount = content.count();

    //判断数据量是否超过500条,超过则弹出提示
    if (rowCount > 500) {
        QString msg = QString("要打印的数据共 %1 条,请耐心等待!确定要打印数据吗?").arg(rowCount);
        if (myHelper::ShowMessageBoxQuestion(msg) != 1) {
            return;
        }
    }

    QList<int> columnWidths;
    columnWidths.append(150);
    columnWidths.append(120);
    columnWidths.append(180);
    columnWidths.append(100);
    columnWidths.append(150);

    PrintAPI::Instance()->PrintA4(type, "", columnNames, columnWidths, content, false, false);

    QString msg = QString("打印%1").arg(type);
    DBHelper::AddEventInfoUser(msg);
}

void frmAlarm::on_btnPdf_clicked()
{
    QString type = "所有告警记录";
    QString defaultName = QString("%1/%2.pdf").arg(App::AppPath).arg(STRDATETIME);
    QString file =  QFileDialog::getSaveFileName(this, "保存文件", defaultName, "Pdf(*.pdf)");

    if (file.length() == 0) {
        return;
    }

    QStringList content = GetContent();
    int rowCount = content.count() ;

    //判断数据量是否超过500条,超过则弹出提示
    if (rowCount > 500) {
        QString msg = QString("要导出的数据共 %1 条,请耐心等待!确定要导出数据吗?").arg(rowCount);
        if (myHelper::ShowMessageBoxQuestion(msg) != 1) {
            return;
        }
    }

    QList<int> columnWidths;
    columnWidths.append(150);
    columnWidths.append(120);
    columnWidths.append(180);
    columnWidths.append(100);
    columnWidths.append(150);

    PdfAPI::Instance()->SavePdf(file, type, "", columnNames, columnWidths, content, false, false);

    QString msg = QString("导出%1到Pdf").arg(type);
    DBHelper::AddEventInfoUser(msg);

    if (myHelper::ShowMessageBoxQuestion(msg + "成功!确定现在就打开吗?") == 1) {
        QString url = QString("file:///%1").arg(file);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}

void frmAlarm::on_btnExcel_clicked()
{
    QString type = "所有告警记录";
    QString defaultName = QString("%1/%2.xls").arg(App::AppPath).arg(STRDATETIME);
    QString file =  QFileDialog::getSaveFileName(this, "保存文件", defaultName, "Excel(*.xls)");

    if (file.length() == 0) {
        return;
    }

    QStringList content = GetContent();
    int rowCount = content.count();

    //判断数据量是否超过10000条,超过则弹出提示
    if (rowCount > 10000) {
        QString msg = QString("要导出的数据共 %1 条,请耐心等待!确定要导出数据吗?").arg(rowCount);
        if (myHelper::ShowMessageBoxQuestion(msg) != 1) {
            return;
        }
    }

    QList<int> columnWidths;
    columnWidths.append(150);
    columnWidths.append(120);
    columnWidths.append(180);
    columnWidths.append(100);
    columnWidths.append(150);

    ExcelAPI::Instance()->ToExcel(file, type, type, "", columnNames, columnWidths, content, true, false);

    QString msg = QString("导出%1到Excel").arg(type);
    DBHelper::AddEventInfoUser(msg);

    if (myHelper::ShowMessageBoxQuestion(msg + "成功!确定现在就打开吗?") == 1) {
        QString url = QString("file:///%1").arg(file);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}

QStringList frmAlarm::GetContent()
{
    QStringList content;
    QSqlQuery query;

    QString sql = "select TriggerTime,TriggerType,TriggerContent,DefenceID,DefenceName from EventInfo ";
    sql += whereSql;
    //先按照设备编号升序再按照保存时间降序
    sql += " order by TriggerTime desc";
    query.exec(sql);

    int columnCount = query.record().count();
    //循环遍历数据,存储到QStringList中
    while (query.next()) {
        QString temp = "";
        for (int i = 0; i < columnCount; i++) {
            temp += query.value(i).toString() + ";";
        }
        content.append(temp.mid(0, temp.length() - 1));
    }

    return content;
}

void frmAlarm::LoadLinkInfo()
{
    ui->listLink->clear();

    QString sql = "select LinkName from LinkInfo";
    QSqlQuery query;
    query.exec(sql);
    while(query.next()) {
        ui->listLink->addItem(query.value(0).toString());
    }
}

void frmAlarm::on_btnAddLink_clicked()
{
    QString linkName = ui->txtLinkName->text();
    QString defenceID = ui->cboxDefenceIDx->currentText();
    QString defenceName = ui->cboxDefenceNamex->currentText();
    QString nvrIP = ui->txtNVRIP->text();
    QString nvrCh = ui->cboxNVRCh->currentText();
    QString rtspAddr = ui->txtRtspAddr->text();
    QString snapTime = ui->cboxSnapTime->currentText();
    QString snapCount = ui->cboxSnapCount->currentText();

    if (linkName.length() == 0) {
        myHelper::ShowMessageBoxError("联动预案名称不能为空!");
        ui->txtLinkName->setFocus();
        return;
    }

    if (defenceName.length() == 0) {
        myHelper::ShowMessageBoxError("防区名称不能为空!");
        return;
    }

    if (nvrIP.length() == 0) {
        myHelper::ShowMessageBoxError("NVR IP地址不能为空!");
        ui->txtNVRIP->setFocus();
        return;
    }

    if (rtspAddr.length() == 0) {
        myHelper::ShowMessageBoxError("Rtsp地址不能为空!");
        ui->txtRtspAddr->setFocus();
        return;
    }

    //判断是否有相同的预案
    if (DBHelper::IsExistLink(linkName)) {
        myHelper::ShowMessageBoxError("该联动预案已经存在!");
        ui->txtLinkName->setFocus();
        return;
    }

    QString sql = "insert into LinkInfo(LinkName,DefenceID,DefenceName,NVRIP,NVRCh,RtspAddr,SnapTime,SnapCount) values('";
    sql += linkName + "','";
    sql += defenceID + "','";
    sql += defenceName + "','";
    sql += nvrIP + "','";
    sql += nvrCh + "','";
    sql += rtspAddr + "','";
    sql += snapTime + "','";
    sql += snapCount + "')";

    QSqlQuery query;
    query.exec(sql);

    LoadLinkInfo();

    int index = ui->cboxDefenceNamex->currentIndex();
    int count = ui->cboxDefenceNamex->count();
    if (index < (count - 1)) {
        ui->cboxDefenceNamex->setCurrentIndex(index + 1);
    }
}

void frmAlarm::on_btnDeleteLink_clicked()
{
    if (ui->listLink->selectedItems().count() <= 0) {
        myHelper::ShowMessageBoxError("请选择要删除的联动预案!");
        return;
    }

    QString linkName = ui->listLink->currentIndex().data().toString();
    QString sql = QString("delete from LinkInfo where LinkName='%1'").arg(linkName);
    QSqlQuery query;
    query.exec(sql);

    LoadLinkInfo();

    int count = ui->listLink->count();
    if (count > 0) {
        ui->listLink->setCurrentRow(count - 1);
    }
}

void frmAlarm::on_btnUpdateLink_clicked()
{
    if (ui->listLink->selectedItems().count() <= 0) {
        myHelper::ShowMessageBoxError("请选择要修改的联动预案!");
        return;
    }

    QString linkNamex = ui->listLink->currentIndex().data().toString();
    QString linkName = ui->txtLinkName->text();
    QString defenceID = ui->cboxDefenceIDx->currentText();
    QString defenceName = ui->cboxDefenceNamex->currentText();
    QString nvrIP = ui->txtNVRIP->text();
    QString nvrCh = ui->cboxNVRCh->currentText();
    QString rtspAddr = ui->txtRtspAddr->text();
    QString snapTime = ui->cboxSnapTime->currentText();
    QString snapCount = ui->cboxSnapCount->currentText();

    if (linkName.length() == 0) {
        myHelper::ShowMessageBoxError("联动预案名称不能为空!");
        ui->txtLinkName->setFocus();
        return;
    }

    if (defenceName.length() == 0) {
        myHelper::ShowMessageBoxError("防区名称不能为空!");
        return;
    }

    if (nvrIP.length() == 0) {
        myHelper::ShowMessageBoxError("NVR IP地址不能为空!");
        ui->txtNVRIP->setFocus();
        return;
    }

    if (rtspAddr.length() == 0) {
        myHelper::ShowMessageBoxError("Rtsp地址不能为空!");
        ui->txtRtspAddr->setFocus();
        return;
    }

    //判断是否有相同的预案
    if (linkNamex != linkName) {
        if (DBHelper::IsExistLink(linkName)) {
            myHelper::ShowMessageBoxError("该联动预案已经存在!");
            ui->txtLinkName->setFocus();
            return;
        }
    }

    QString sql = "update LinkInfo set";
    sql += " LinkName='" + linkName;
    sql += "',DefenceID='" + defenceID;
    sql += "',DefenceName='" + defenceName;
    sql += "',NVRIP='" + nvrIP;
    sql += "',NVRCh='" + nvrCh;
    sql += "',RtspAddr='" + rtspAddr;
    sql += "',SnapTime='" + snapTime;
    sql += "',SnapCount='" + snapCount;
    sql += "' where LinkName='" + linkNamex + "'";

    QSqlQuery query;
    query.exec(sql);

    LoadLinkInfo();
}

void frmAlarm::on_btnClearLink_clicked()
{
    if (myHelper::ShowMessageBoxQuestion("确定要清空联动预案吗?") == 1) {
        QString sql = "delete from LinkInfo";
        QSqlQuery query;
        query.exec(sql);

        LoadLinkInfo();
    }
}

void frmAlarm::on_listLink_pressed(const QModelIndex &index)
{
    QString linkName = index.data().toString();
    if (linkName.length() > 0) {
        //查询该预案对应的信息
        QString sql = QString("select LinkName,DefenceID,DefenceName,NVRIP,NVRCh,RtspAddr,"
                              "SnapTime,SnapCount from LinkInfo where LinkName='%1'").arg(linkName);
        QSqlQuery query;
        query.exec(sql);
        if (query.next()) {
            ui->txtLinkName->setText(query.value(0).toString());
            ui->cboxDefenceNamex->setCurrentIndex(ui->cboxDefenceNamex->findText(query.value(2).toString()));
            ui->txtNVRIP->setText(query.value(3).toString());
            ui->cboxNVRCh->setCurrentIndex(ui->cboxNVRCh->findText(query.value(4).toString()));
            ui->txtRtspAddr->setText(query.value(5).toString());
            ui->cboxSnapTime->setCurrentIndex(ui->cboxSnapTime->findText(query.value(6).toString()));
            ui->cboxSnapCount->setCurrentIndex(ui->cboxSnapCount->findText(query.value(7).toString()));
        }
    }
}

void frmAlarm::on_btnSelectVideo_clicked()
{
    if (ui->dateStartVideo->date() > ui->dateEndVideo->date()) {
        myHelper::ShowMessageBoxError("开始时间不能大于结束时间!");
        return;
    }

    ui->listVideoPath->clear();
    QString videoPath;
    if (ui->cboxVideoType->currentIndex() == 0) {
#ifdef __arm__
        videoPath = App::SnapPath;
#else
        videoPath = App::AppPath + App::SnapPath;
#endif
    } else {
#ifdef __arm__
        videoPath = App::VideoPath;
#else
        videoPath = App::AppPath + App::VideoPath;
#endif
    }

    //获取所有文件夹名称
    QDate dateStart = ui->dateStartVideo->date();
    QDate dateEnd = ui->dateEndVideo->date();

    if (ui->cboxVideoType->currentIndex() == 0) {
        while (dateStart <= dateEnd) {
            QString dirName = dateStart.toString("yyyy-MM-dd");
            QString tempPath = QString("%1/%2").arg(videoPath).arg(dirName);
            QDir dir(tempPath);
            if (dir.exists()) {
                QListWidgetItem *item = new QListWidgetItem(ui->listVideoPath);
                item->setText(dirName);
                item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                item->setCheckState(Qt::Unchecked);
            }
            dateStart = dateStart.addDays(1);
        }
    } else {
        while (dateStart <= dateEnd) {
            QString dirName = dateStart.toString("yyyy-MM-dd");
            QString tempPath = QString("%1/%2").arg(videoPath).arg(dirName);
            QDir dir(tempPath);
            if (dir.exists()) {
                QStringList filter;
                filter << "*.mp4" << "*.h264" ;
                QStringList files = dir.entryList(filter);
                foreach (QString file, files) {
                    QListWidgetItem *item = new QListWidgetItem(ui->listVideoPath);
                    item->setText(file);
                    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    item->setCheckState(Qt::Unchecked);
                    item->setData(Qt::UserRole, QString(tempPath));
                }
            }
            dateStart = dateStart.addDays(1);
        }
    }
    ui->labVideoList->setText(QString("共找到 %1 个").arg(ui->listVideoPath->count()));
}

bool frmAlarm::CopyDir(const QString &fromDir, const QString &toDir, bool cover)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()) {
        if(!targetDir.mkdir(targetDir.absolutePath())) {
            return false;
        }
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList) {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
            continue;
        }

        if(fileInfo.isDir()) {
            if(!CopyDir(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), cover)) {
                continue;
            }
        } else {
            if(cover && targetDir.exists(fileInfo.fileName())) {
                targetDir.remove(fileInfo.fileName());
            }

            if(!QFile::copy(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()))) {
                continue;
            }
        }
    }

    return true;
}

void frmAlarm::on_btnSaveVideo_clicked()
{
    QString videoPath;
    if (ui->cboxVideoType->currentIndex() == 0) {
#ifdef __arm__
        videoPath = App::SnapPath;
#else
        videoPath = App::AppPath + App::SnapPath;
#endif
    } else {
#ifdef __arm__
        videoPath = App::VideoPath;
#else
        videoPath = App::AppPath + App::VideoPath;
#endif
    }

    QStringList selectDirs;
    int count = ui->listVideoPath->count();
    for (int i = 0; i < count; i++) {
        QListWidgetItem *item = ui->listVideoPath->item(i);
        if (item->checkState() == Qt::Checked) {
            selectDirs << item->text();
        }
    }

    if (selectDirs.count() <= 0) {
        myHelper::ShowMessageBoxError("请选择要导出的文件夹!");
        return;
    }

    QString saveDir = QFileDialog::getExistingDirectory(this);
    if (saveDir.length() <= 0) {
        return;
    }

    foreach (QString dirName, selectDirs) {
        QString fromDir = QString("%1/%2").arg(videoPath).arg(dirName);
        QString toDir = QString("%1/%2").arg(saveDir).arg(dirName);
        CopyDir(fromDir, toDir, true);
    }

    API::AddMessage("导出告警抓拍图像");
    DBHelper::AddEventInfoUser("导出告警抓拍图像");
    myHelper::ShowMessageBoxInfo("导出告警抓拍图像成功!");
}

void frmAlarm::on_listVideoPath_doubleClicked(const QModelIndex &index)
{
    QString dirName = index.data().toString();
    if (dirName.length() > 0) {
        if (ui->cboxVideoType->currentIndex() == 0) {
            QString imagePath;
#ifdef __arm__
            imagePath = QString("%1/%2").arg(App::SnapPath).arg(dirName);
#else
            imagePath = QString("%1%2/%3").arg(App::AppPath).arg(App::SnapPath).arg(dirName);
#endif
            ui->widget_image_view->load(imagePath);
            ui->widget_image_view->setFocus();
        } else {

        }
    }
}

void frmAlarm::on_cboxVideoType_currentIndexChanged(const QString &arg1)
{
    if (arg1 == "告警抓拍图像") {
        ui->widget_image_view->setVisible(true);
        ui->ckFill->setVisible(true);
        ui->ckFade->setVisible(true);
        ui->labVideo->setVisible(false);
        ui->gboxBottomVideo->setVisible(false);
        ui->gridLayoutVideo->setVerticalSpacing(0);
    } else if(arg1 == "告警联动视频") {
        ui->widget_image_view->setVisible(false);
        ui->ckFill->setVisible(false);
        ui->ckFade->setVisible(false);
        ui->labVideo->setVisible(true);
        ui->gboxBottomVideo->setVisible(true);
        ui->gridLayoutVideo->setVerticalSpacing(6);
    }
}

void frmAlarm::on_ckFill_stateChanged(int arg1)
{
    ui->widget_image_view->setFill(arg1 == 0 ? false : true);
    ui->widget_image_view->setFocus();
}

void frmAlarm::on_ckFade_stateChanged(int arg1)
{
    ui->widget_image_view->setFade(arg1 == 0 ? false : true);
    ui->widget_image_view->setFocus();
}

void frmAlarm::on_cboxSnapTime_currentIndexChanged(const QString &arg1)
{
    ui->cboxSnapCount->clear();
    int index = arg1.toInt();
    for (int i = 1; i <= index; i++) {
        ui->cboxSnapCount->addItem(QString::number(i));
    }
    ui->cboxSnapCount->setCurrentIndex(index - 1);
}
