#ifndef FRMCONFIG_H
#define FRMCONFIG_H

#include <QWidget>
#include <QDateTime>

namespace Ui
{
    class frmConfig;
}

class frmConfig : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfig(QWidget *parent = 0);
    ~frmConfig();

private slots:
    void SaveConfig();
    void ReceiveTime(QDateTime dateTime);
    void button_clicked();
    void on_btnSetTime_clicked();
    void on_btnSetNet_clicked();
    void on_btnNTP_clicked();

private:
    Ui::frmConfig *ui;

    void InitForm();

};

#endif // FRMCONFIG_H
