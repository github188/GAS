#ifndef QDELEGATE_H
#define QDELEGATE_H

#include <QItemDelegate>

class QDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit QDelegate(QObject *parent = 0);

    //设置委托类型
    void SetDelegateType(QString delegateType) {
        this->delegateType = delegateType;
    }

    //设置下拉框是否允许编辑
    void SetDelegateEdit(bool edit) {
        this->edit = edit;
    }

    //设置文本框是否密码显示
    void SetDelegatePwd(bool pwd) {
        this->pwd = pwd;
    }

    //设置委托参数
    void SetDelegateValue(QStringList delegateValue) {
        this->delegateValue = delegateValue;
    }

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index)const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index)const;

private:
    QString delegateType;       //委托类型
    bool edit;                  //可否编辑
    bool pwd;                   //可否密码显示
    QStringList delegateValue;  //委托参数

signals:

public slots:
};

#endif // QDELEGATE_H
