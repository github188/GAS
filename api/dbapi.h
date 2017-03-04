#ifndef DBAPI_H
#define DBAPI_H

#include <QObject>
#include <QMutex>

class QtSql;
class QTableView;
class QLabel;
class QAbstractButton;
class QComboBox;
class QSqlQueryModel;

class DBAPI : public QObject
{
    Q_OBJECT
private:
    explicit DBAPI(QObject *parent = 0);
    static DBAPI *_instance;

    void BindData(QString sql);

    int startIndex;             //分页开始索引,每次翻页都变动
    QString tempSql;            //临时SQL语句
    QString sql;
    QSqlQueryModel *queryModel; //查询模型

    QLabel *labPageCount;       //总页数
    QLabel *labPageCurrent;     //当前页
    QLabel *labResultCount;     //总记录数
    QLabel *labResultCurrent;   //每页记录数
    QLabel *labInfo;            //总页数当前页总记录数每页记录数

    QTableView *tableView;      //显示数据的表格对象
    QAbstractButton *btnFirst;  //第一页按钮对象
    QAbstractButton *btnPre;    //上一页按钮对象
    QAbstractButton *btnNext;   //下一页按钮对象
    QAbstractButton *btnLast;   //末一页按钮对象

    int resultCurrent;          //每页显示记录数
    int resultCount;            //总记录数
    int pageCount;              //总页数
    int pageCurrent;            //当前第几页

public:
    static DBAPI *Instance() {
        static QMutex mutex;
        if (!_instance) {
            QMutexLocker locker(&mutex);
            if (!_instance) {
                _instance = new DBAPI;
            }
        }
        return _instance;
    }

    //设置需要显示数据的表格,数据翻页对应的按钮
    void SetControl(QTableView *tableView, QLabel *labInfo,
                    QAbstractButton *btnFirst, QAbstractButton *btnPre,
                    QAbstractButton *btnNext, QAbstractButton *btnLast);

    void SetControl(QTableView *tableView, QLabel *labPageCount, QLabel *labPageCurrent,
                    QLabel *labResultCount, QLabel *labResultCurrent,
                    QAbstractButton *btnFirst, QAbstractButton *btnPre,
                    QAbstractButton *btnNext, QAbstractButton *btnLast);

    //绑定查询数据,带分页/行数/条件/排序/列名/列宽
    void BindDataSelect(QString tableName, QString orderSql, QString where, int resultCurrent,
                        QList<QString> columnNames, QList<int> columnWidths);

    //绑定数据到下拉框
    void BindData(QString columnName, QString orderColumn, QString tableName, QComboBox *cbox);
    void BindData(QString columnName, QString orderColumn, QString tableName, QList<QComboBox *> cboxs);

signals:

private slots:
    void first();       //第一页
    void previous();    //上一页
    void next();        //下一页
    void last();        //末一页

};

#endif // DBAPI_H
