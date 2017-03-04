#ifndef FRMDEFENCELOT_H
#define FRMDEFENCELOT_H

#include <QDialog>

namespace Ui
{
    class frmDefenceLot;
}

class frmDefenceLot : public QDialog
{
    Q_OBJECT

public:
    explicit frmDefenceLot(QWidget *parent = 0);
    ~frmDefenceLot();

    void SetSubSystem(QString subSystem);

private slots:
    void on_btnAdd_clicked();

signals:
    void AddDefence();

private:
    Ui::frmDefenceLot *ui;

    QString subSystem;  //子系统
    void InitStyle();   //初始化无边框窗体
    void InitForm();    //初始化窗体数据

};

#endif // FRMDEFENCELOT_H
