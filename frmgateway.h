#ifndef FRMGATEWAY_H
#define FRMGATEWAY_H

#include <QWidget>

namespace Ui
{
    class frmGateway;
}

class frmGateway : public QWidget
{
    Q_OBJECT

public:
    explicit frmGateway(QWidget *parent = 0);
    ~frmGateway();

private slots:
    void button_clicked();

private:
    Ui::frmGateway *ui;

    void InitForm();

};

#endif // FRMGATEWAY_H
