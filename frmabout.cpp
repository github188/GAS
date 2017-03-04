#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmabout.h"
#include "ui_frmabout.h"
#include "api/myhelper.h"

frmAbout::frmAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmAbout)
{
    ui->setupUi(this);
    this->InitForm();
    myHelper::FormInCenter(this);
}

frmAbout::~frmAbout()
{
    delete ui;
}

void frmAbout::InitForm()
{
    ui->labTitle->setText(App::Title);
    ui->lab_Title->setText(QString("关于%1").arg(App::Title));
    ui->labVesion->setText(QString("版本 : %1").arg(App::Version));
    ui->labHttp->setText("<style> a{text-decoration:none;color:white;} </style>"
                         "<a href=www.ecan-system.com>网址 : http://www.ecan-system.com</a>");
    ui->labAuthor->setText(QString("版权 : %1").arg(App::Author));

    this->setProperty("CanMove", true);
    this->setWindowTitle(ui->lab_Title->text());
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    IconHelper::Instance()->SetIcoMain(ui->lab_Ico, 15);
    IconHelper::Instance()->SetIcoClose(ui->btnMenu_Close);
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
}
