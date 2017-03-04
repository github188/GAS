#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmvisitor.h"
#include "ui_frmvisitor.h"
#include "api/myhelper.h"

frmVisitor::frmVisitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmVisitor)
{
    ui->setupUi(this);
    this->InitForm();
}

frmVisitor::~frmVisitor()
{
    delete ui;
}

void frmVisitor::InitForm()
{
#ifdef __arm__
    ui->widget_left->setMinimumWidth(150);
    ui->widget_left->setMaximumWidth(150);
#endif

    QList<QPushButton *> btns = ui->widget_left->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(button_clicked()));
    }

    ui->btnMain->click();
}

void frmVisitor::button_clicked()
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
