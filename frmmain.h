#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QDialog>

namespace Ui
{
    class frmMain;
}

class frmMain : public QDialog
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    void reject();

private slots:
    void button_clicked();
    void action_clicked();
    void Screen();              //截图

private:
    Ui::frmMain *ui;

    void InitStyle();   //初始化无边框窗体
    void InitForm();    //初始化窗体数据

};

#endif // FRMMAIN_H
