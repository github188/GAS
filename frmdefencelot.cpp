#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmdefencelot.h"
#include "ui_frmdefencelot.h"
#include "api/myhelper.h"
#include "api/dbhelper.h"

frmDefenceLot::frmDefenceLot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmDefenceLot)
{
    ui->setupUi(this);
    this->InitStyle();
    this->InitForm();
    myHelper::FormInCenter(this);
}

void frmDefenceLot::SetSubSystem(QString subSystem)
{
    this->subSystem = subSystem;
}

frmDefenceLot::~frmDefenceLot()
{
    delete ui;
}

void frmDefenceLot::InitStyle()
{
    this->setProperty("Form", true);
    this->setProperty("CanMove", true);
    this->setWindowTitle(ui->lab_Title->text());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    IconHelper::Instance()->SetIcoMain(ui->lab_Ico);
    IconHelper::Instance()->SetIcoClose(ui->btnMenu_Close);
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void frmDefenceLot::InitForm()
{
    subSystem = "1";

    //填充防区号
    for (int i = 1; i <= 255; i++) {
        if (i < 10) {
            ui->cboxDefenceIDStart->addItem(QString("00%1").arg(i));
            ui->cboxDefenceIDEnd->addItem(QString("00%1").arg(i));
            continue;
        }
        if (i < 100) {
            ui->cboxDefenceIDStart->addItem(QString("0%1").arg(i));
            ui->cboxDefenceIDEnd->addItem(QString("0%1").arg(i));
            continue;
        }
        if (i < 1000) {
            ui->cboxDefenceIDStart->addItem(QString("%1").arg(i));
            ui->cboxDefenceIDEnd->addItem(QString("%1").arg(i));
            continue;
        }
    }
}

void frmDefenceLot::on_btnAdd_clicked()
{
    int defenceIDStart = ui->cboxDefenceIDStart->currentIndex() + 1;
    int defenceIDEnd = ui->cboxDefenceIDEnd->currentIndex() + 1;

    if (defenceIDStart > defenceIDEnd) {
        myHelper::ShowMessageBoxError("防区开始编号不能大于防区结束编号!");
        return;
    }

    int count = 0;
    int defenceX = 5;
    int defenceY = 5;

    //开启数据库事务,加快执行速度
    QSqlDatabase::database().transaction();

    //根据选择的防区编号范围,逐个添加防区
    for (int i = defenceIDStart; i <= defenceIDEnd; i++) {
        QString defenceID;
        if (i < 10) {
            defenceID = QString("00%1").arg(i);
        } else if (i < 100) {
            defenceID = QString("0%1").arg(i);
        } else if(i < 1000) {
            defenceID = QString("%1").arg(i);
        }

        //判断防区是否已经存在,存在则continue
        if (DBHelper::IsExistDefence(defenceID)) {
            continue;
        }

        QSqlQuery query;
        QString sql = "insert into DefenceInfo(DefenceID,DefenceName,SubSystem,DefenceSound,DefenceImage,DefenceX,DefenceY) values('";
        sql += defenceID + "','";
        sql += QString("%1防区").arg(defenceID) + "','";
        sql += subSystem + "','";
        sql += "无','";
        sql += App::CurrentImage + "','";
        sql += QString::number(defenceX) + "','";
        sql += QString::number(defenceY) + "')";
        query.exec(sql);

        //动态改变防区坐标
        defenceX = defenceX + 30;
        count++;
        if (count == 25) {
            defenceY = defenceY + 30;
            defenceX = 5;
            count = 0;
        }
    }

    //结束事务处理,提交事务
    QSqlDatabase::database().commit();

    //发送批量添加防区信号给防区管理界面,及时更新界面数据
    emit AddDefence();

}
