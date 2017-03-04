#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>

namespace Ui
{
    class frmAbout;
}

class frmAbout : public QDialog
{
    Q_OBJECT

public:
    explicit frmAbout(QWidget *parent = 0);
    ~frmAbout();

private:
    Ui::frmAbout *ui;

    void InitForm();

};

#endif // FRMABOUT_H
