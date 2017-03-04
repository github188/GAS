#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "qdelegate.h"

#include <QLineEdit>
#include <QComboBox>
#include <QListView>
#include <QDateTimeEdit>

QDelegate::QDelegate(QObject *parent) : QItemDelegate(parent)
{
    delegateType = "QLineEdit";
    edit = false;
    delegateValue.append("");
}

QWidget *QDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    QWidget *editor = new QWidget(parent);

    if (delegateType == "QLineEdit") {
        QLineEdit *txt = new QLineEdit(parent);
        txt->setText(delegateValue.at(0));
        if (pwd) {
            txt->setEchoMode(QLineEdit::Password);
        }
        txt->setStyleSheet("border-radius:0px;");
        editor = txt;
    } else if (delegateType == "QComboBox") {
        QComboBox *cbox = new QComboBox(parent);
        cbox->setEditable(edit);
        cbox->addItems(delegateValue);
        cbox->setStyleSheet("border-radius:0px;");
        editor = cbox;
    } else if (delegateType == "QDateTimeEdit") {
        QDateTimeEdit *dateTime = new QDateTimeEdit(parent);
        dateTime->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
        dateTime->setCalendarPopup(true);
        editor = dateTime;
    }

    editor->installEventFilter(const_cast<QDelegate *>(this));
    return editor;
}

void QDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString str = index.model()->data(index).toString();

    if (delegateType == "QLineEdit") {
        QLineEdit *txt = static_cast<QLineEdit *>(editor);
        txt->setText(str);
    } else if (delegateType == "QComboBox") {
        QComboBox *cbox = static_cast<QComboBox *>(editor);
        cbox->setCurrentIndex(cbox->findText(str));
    } else if (delegateType == "QDateTimeEdit") {
        QDateTimeEdit *dateTime = static_cast<QDateTimeEdit *>(editor);
        dateTime->setDateTime(QDateTime::fromString(str, "yyyy-MM-dd HH:mm:ss"));
    }
}

void QDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index)const
{
    if (delegateType == "QLineEdit") {
        QLineEdit *txt = static_cast<QLineEdit *>(editor);
        QString str = txt->text();
        model->setData(index, str);
    } else if (delegateType == "QComboBox") {
        QComboBox *cbox = static_cast<QComboBox *>(editor);
        QString str = cbox->currentText();
        model->setData(index, str);
    } else if (delegateType == "QDateTimeEdit") {
        QDateTimeEdit *dateTime = static_cast<QDateTimeEdit *>(editor);
        QString str = dateTime->dateTime().toString("yyyy-MM-dd HH:mm:ss");
        model->setData(index, str);
    }
}

void QDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
