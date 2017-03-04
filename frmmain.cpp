#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmmain.h"
#include "ui_frmmain.h"
#include "api/myhelper.h"
#include "api/api.h"
#include "api/hostapi.h"
#include "api/showdatetime.h"
#include "api/showcpumemory.h"
#include "api/showdevicesize.h"
#include "api/screenapi.h"
#include "frmalarm.h"
#include "frmvisitor.h"
#include "frmgateway.h"
#include "frmconfig.h"
#include "frmabout.h"

frmMain::frmMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->InitStyle();
    this->InitForm();
    this->setGeometry(qApp->desktop()->availableGeometry());
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::reject()
{
#ifdef __arm__
    close();
#else
    HostAPI::Instance()->~HostAPI();
    exit(0);
#endif
}

void frmMain::InitStyle()
{
#ifdef __arm__
    ui->labCPUMemory->setFixedWidth(470);
    ui->lcdNumber->setFixedWidth(470);
#else
    ui->labCPUMemory->setFixedWidth(380);
    ui->lcdNumber->setFixedWidth(380);
#endif

    ui->line1->setFixedWidth(1);
    ui->line2->setFixedWidth(1);
    ui->line3->setFixedWidth(1);
    ui->line4->setFixedWidth(1);
    ui->lab_Title->setText(App::Title);
    this->setProperty("Form", true);
    this->setGeometry(qApp->desktop()->availableGeometry());
    this->setWindowTitle(ui->lab_Title->text());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
#ifdef __arm__
    IconHelper::Instance()->SetIcoMain(ui->lab_Ico, 50);
    ui->lab_Title->setFont(QFont(App::FontName, 30));
#else
    IconHelper::Instance()->SetIcoMain(ui->lab_Ico, 40);
    ui->lab_Title->setFont(QFont(App::FontName, 20));
#endif

#ifdef __arm__
    ui->btnExit->setText("重启");
    ui->widget_title->setMinimumHeight(90);
    ui->widget_title->setMaximumHeight(90);
#endif

#ifdef __arm__
    int width = 25;
#else
    int width = 20;
#endif
    QString qss = QString("QComboBox::drop-down,QDateEdit::drop-down,QTimeEdit::drop-down,QDateTimeEdit::drop-down{width:%1px;}").arg(width);
    this->setStyleSheet(qss);
}

void frmMain::InitForm()
{
    int fontSize = (App::FontSize > 9 ? App::FontSize - 1 : 9);
    QFont font = QFont(App::FontName, fontSize);
    QList<QLabel *> labs = ui->widget_bottom->findChildren<QLabel *>();

    foreach (QLabel *lab, labs) {
        lab->setFont(font);
    }

    ui->labWelcom->setText(QString("欢迎使用%1 %2").arg(App::Title).arg(App::Version));
    ui->labAuthor->setText(QString("版权所有: %1").arg(App::Author));
    ui->labUser->setText(QString("当前用户: %1【%2】").arg(App::CurrentUserName).arg("管理员"));

    ShowDateTime *showDateTime = new ShowDateTime(this);
    showDateTime->SetLab(ui->labDateTime, ui->labLive);
    showDateTime->SetLcdNumber(ui->lcdNumber);
    showDateTime->Start(1000);

    ShowCPUMemory *showCPUMemory = new ShowCPUMemory(this);
    showCPUMemory->SetLab(ui->labCPUMemory);
#ifdef __arm__
    QFont f(App::FontName, App::FontSize - 1);
    showCPUMemory->Start(3000);
#else
    QFont f(App::FontName, App::FontSize + 1);
    showCPUMemory->Start(1000);
#endif

    f.setBold(true);
    ui->labCPUMemory->setFont(f);

    QString qss = QString("QLabel,QLCDNumber{background-color:rgb(0,0,0);color:%1;}").arg("yellow");
    //QString qss = QString("QLabel,QLCDNumber{background-color:rgba(0,0,0,0);color:%1;}").arg(App::TextColor);
    ui->lcdNumber->setStyleSheet(qss);
    ui->labCPUMemory->setStyleSheet(qss);

    QList<QToolButton *> btns = ui->widget_top->findChildren<QToolButton *>();
    foreach (QToolButton * btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(button_clicked()));
#ifdef __arm__
        btn->setMinimumWidth(110);
        btn->setMaximumWidth(110);
        btn->setIconSize(QSize(40, 40));
#endif
    }

    frmAlarm *alarm = new frmAlarm;
    ui->stackedWidget->addWidget(alarm);

    frmVisitor *visitor = new frmVisitor;
    ui->stackedWidget->addWidget(visitor);

    frmGateway *gateway = new frmGateway;
    ui->stackedWidget->addWidget(gateway);

    frmConfig *config = new frmConfig;
    ui->stackedWidget->addWidget(config);

    ui->btnAlarm->click();

    //ui->btnAlarm->setVisible(false);
    //ui->btnGateway->setVisible(false);

    QAction *actionScreen = new QAction("截图", this);
    connect(actionScreen, SIGNAL(triggered(bool)), this, SLOT(Screen()));
    this->addAction(actionScreen);

    return;
    
    //2017-2-14增加顶部导航下拉菜单效果,如果不需要可以删除
    QMenu *menu = new QMenu(this);
    menu->addAction("端口管理", this, SLOT(action_clicked()));
    menu->addAction("设备管理", this, SLOT(action_clicked()));
    menu->addAction("告警设置", this, SLOT(action_clicked()));
    menu->addSeparator();
    menu->addAction("系统设置", this, SLOT(action_clicked()));
    menu->addAction("修改密码", this, SLOT(action_clicked()));
    menu->addSeparator();
    menu->addAction("同步间隔", this, SLOT(action_clicked()));
    ui->btnVisitor->setPopupMode(QToolButton::InstantPopup);
    ui->btnVisitor->setMenu(menu);
}

void frmMain::Screen()
{
    frmScreen::Instance()->showMaximized();
}

void frmMain::button_clicked()
{
    QToolButton *btn = (QToolButton *)sender();
    QString name = btn->text();

    if (name == "报警控制") {
        ui->stackedWidget->setCurrentIndex(0);
        ui->btnAlarm->setChecked(true);
        ui->btnVisitor->setChecked(false);
        ui->btnGateway->setChecked(false);
        ui->btnConfig->setChecked(false);
    } else if (name == "组合认证") {
        ui->stackedWidget->setCurrentIndex(1);
        ui->btnAlarm->setChecked(false);
        ui->btnVisitor->setChecked(true);
        ui->btnGateway->setChecked(false);
        ui->btnConfig->setChecked(false);
    } else if (name == "智能网关") {
        ui->stackedWidget->setCurrentIndex(2);
        ui->btnAlarm->setChecked(false);
        ui->btnVisitor->setChecked(false);
        ui->btnGateway->setChecked(true);
        ui->btnConfig->setChecked(false);
    } else if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(3);
        ui->btnAlarm->setChecked(false);
        ui->btnVisitor->setChecked(false);
        ui->btnGateway->setChecked(false);
        ui->btnConfig->setChecked(true);
    } else if (name == "关于") {
        frmAbout about;
        about.exec();
    } else if (name == "重启") {
        system("reboot");
    } else if (name == "退出") {
        HostAPI::Instance()->~HostAPI();
        exit(0);
    }
}

void frmMain::action_clicked()
{
    QAction *action = (QAction *)sender();
    QString name = action->text();
    myHelper::ShowMessageBoxInfo("您单击了 "+name);
}
