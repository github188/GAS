#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmgateway.h"
#include "ui_frmgateway.h"

frmGateway::frmGateway(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmGateway)
{
    ui->setupUi(this);
    this->InitForm();
}

frmGateway::~frmGateway()
{
    delete ui;
}

void frmGateway::InitForm()
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
}

void frmGateway::button_clicked()
{
    QPushButton *btn = (QPushButton *)sender();
    QString name = btn->text();
    QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();
    foreach (QPushButton * b, btns) {
        b->setChecked(false);
    }
    btn->setChecked(true);

    if (name == "系统设置") {
        ui->stackedWidget->setCurrentIndex(0);
    }
}
