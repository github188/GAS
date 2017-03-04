#ifndef FRMVISITOR_H
#define FRMVISITOR_H

#include <QWidget>

namespace Ui
{
    class frmVisitor;
}

class frmVisitor : public QWidget
{
    Q_OBJECT

public:
    explicit frmVisitor(QWidget *parent = 0);
    ~frmVisitor();

private slots:
    void button_clicked();

private:
    Ui::frmVisitor *ui;

    void InitForm();

};

#endif // FRMVISITOR_H
