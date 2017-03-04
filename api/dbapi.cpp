#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "dbapi.h"
#include "myhelper.h"

/* 说明:数据库查询及翻页模块
 * 功能:数据库查询及翻页处理
 * 作者:刘典武  QQ:517216493
 * 时间:2014-8-27  检查:2014-9-10
 */
DBAPI *DBAPI::_instance = 0;
DBAPI::DBAPI(QObject *parent) :
    QObject(parent)
{
    startIndex = 0;
    tempSql = "";
    sql = "";
    resultCurrent = 0;
    resultCount = 0;
    pageCount = 0;
    pageCurrent = 1;

    this->tableView = 0;
    this->labInfo = 0;
    this->labPageCount = 0;
    this->labPageCurrent = 0;
    this->labResultCount = 0;
    this->labResultCurrent = 0;

    this->btnFirst = 0;
    this->btnPre = 0;
    this->btnNext = 0;
    this->btnLast = 0;
}

//设置显示数据的表格控件,当前翻页信息的标签控件等
void DBAPI::SetControl(QTableView *tableView, QLabel *labInfo,
                       QAbstractButton *btnFirst, QAbstractButton *btnPre,
                       QAbstractButton *btnNext, QAbstractButton *btnLast)
{
    this->tableView = tableView;
    this->labInfo = labInfo;
    this->labPageCount = 0;
    this->labPageCurrent = 0;
    this->labResultCount = 0;
    this->labResultCurrent = 0;

    this->btnFirst = btnFirst;
    this->btnPre = btnPre;
    this->btnNext = btnNext;
    this->btnLast = btnLast;

    this->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    queryModel = new QSqlQueryModel(tableView);

    //挂载翻页按钮事件
    connect(btnFirst, SIGNAL(clicked()), this, SLOT(first()));
    connect(btnPre, SIGNAL(clicked()), this, SLOT(previous()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(next()));
    connect(btnLast, SIGNAL(clicked()), this, SLOT(last()));
}

void DBAPI::SetControl(QTableView *tableView, QLabel *labPageCount, QLabel *labPageCurrent,
                       QLabel *labResultCount, QLabel *labResultCurrent,
                       QAbstractButton *btnFirst, QAbstractButton *btnPre,
                       QAbstractButton *btnNext, QAbstractButton *btnLast)
{
    this->tableView = tableView;
    this->labInfo = 0;
    this->labPageCount = labPageCount;
    this->labPageCurrent = labPageCurrent;
    this->labResultCount = labResultCount;
    this->labResultCurrent = labResultCurrent;

    this->btnFirst = btnFirst;
    this->btnPre = btnPre;
    this->btnNext = btnNext;
    this->btnLast = btnLast;

    this->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    queryModel = new QSqlQueryModel(tableView);

    //挂载翻页按钮事件
    connect(btnFirst, SIGNAL(clicked()), this, SLOT(first()));
    connect(btnPre, SIGNAL(clicked()), this, SLOT(previous()));
    connect(btnNext, SIGNAL(clicked()), this, SLOT(next()));
    connect(btnLast, SIGNAL(clicked()), this, SLOT(last()));
}

//绑定数据到下拉框
void DBAPI::BindData(QString columnName, QString orderColumn, QString tableName, QComboBox *cbox)
{
    QSqlQuery query;
    query.exec("select " + columnName + " from " + tableName + " order by " + orderColumn + " asc");
    while (query.next()) {
        cbox->addItem(query.value(0).toString());
    }
}

void DBAPI::BindData(QString columnName, QString orderColumn, QString tableName, QList<QComboBox *> cboxs)
{
    QSqlQuery query;
    query.exec("select " + columnName + " from " + tableName + " order by " + orderColumn + " asc");
    while (query.next()) {
        foreach (QComboBox * cbox, cboxs) {
            cbox->addItem(query.value(0).toString());
        }
    }
}

void DBAPI::BindData(QString sql)
{
    queryModel->setQuery(sql);
    tableView->setModel(queryModel);

    if (labInfo != 0) {
        labInfo->setText(QString("共 %1 条  每页 %2 条  共 %3 页  第 %4 页").arg(resultCount).arg(resultCurrent).arg(pageCount).arg(pageCurrent));
    } else {
        labPageCount->setText(QString("共 %1 页").arg(pageCount));
        labPageCurrent->setText(QString("第 %1 页").arg(pageCurrent));
        labResultCount->setText(QString("共 %1 条").arg(resultCount));
        labResultCurrent->setText(QString("每页 %1 条").arg(resultCurrent));
    }
}

//分页绑定查询数据到表格,指定排序列
void DBAPI::BindDataSelect(QString tableName, QString orderSql, QString where, int resultCurrent,
                           QList<QString> columnNames, QList<int> columnWidths)
{
    startIndex = 0; //重置开始索引
    pageCurrent = 1;
    this->resultCurrent = resultCurrent;

    //开始分页绑定数据前,计算好总数据量以及行数
    tempSql = "select count(*) from " + tableName + " " + where;
    QSqlQuery query;
    query.prepare(tempSql);
    query.exec();
    query.first();
    resultCount = query.value(0).toInt();

    int yushu = resultCount % resultCurrent;
    //不存在余数,说明是整行,例如300%5==0
    if (yushu == 0) {
        if (resultCount > 0 && resultCount < resultCurrent) {
            pageCount = 1;
        } else {
            pageCount = resultCount / resultCurrent;
        }
    } else {
        pageCount = (resultCount / resultCurrent) + 1;
    }

    //2014-10-9增加翻页按钮可用不可用处理,如果只有一页数据,则翻页按钮不可用
    if (pageCount <= 1) {
        btnFirst->setEnabled(false);
        btnLast->setEnabled(false);
        btnNext->setEnabled(false);
        btnPre->setEnabled(false);
    } else {
        btnFirst->setEnabled(true);
        btnLast->setEnabled(true);
        btnNext->setEnabled(true);
        btnPre->setEnabled(true);
    }

    tempSql = "select * from " + tableName + " " + where + " order by " + orderSql;
    sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent); //组织分页SQL语句
    BindData(sql);

    //依次设置列标题列宽
    int count = tableView->model()->columnCount();
    for (int i = 0; i < count ; i++) {
        queryModel->setHeaderData(i, Qt::Horizontal, columnNames.at(i)); //设置列标题
        tableView->setColumnWidth(i, columnWidths.at(i)); //设置列宽
    }
}

//第一页
void DBAPI::first()
{
    if (pageCount > 1) {
        startIndex = 0;
        pageCurrent = 1;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        BindData(sql);
        btnLast->setEnabled(true);
        btnNext->setEnabled(true);
    }

    btnFirst->setEnabled(false);
    btnPre->setEnabled(false);
}

//上一页
void DBAPI::previous()
{
    if (pageCurrent > 1) {
        pageCurrent--;
        startIndex -= resultCurrent;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        BindData(sql);
        btnLast->setEnabled(true);
        btnNext->setEnabled(true);
    }

    if (pageCurrent == 1) {
        btnFirst->setEnabled(false);
        btnPre->setEnabled(false);
    }
}

//下一页
void DBAPI::next()
{
    if (pageCurrent < pageCount) {
        pageCurrent++;
        startIndex += resultCurrent;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        BindData(sql);
        btnFirst->setEnabled(true);
        btnPre->setEnabled(true);
    }

    if (pageCurrent == pageCount) {
        btnLast->setEnabled(false);
        btnNext->setEnabled(false);
    }
}

//末一页
void DBAPI::last()
{
    if (pageCount > 0) {
        startIndex = (pageCount - 1) * resultCurrent;
        pageCurrent = pageCount;
        sql = QString("%1 limit %2,%3;").arg(tempSql).arg(startIndex).arg(resultCurrent);
        BindData(sql);
        btnFirst->setEnabled(true);
        btnPre->setEnabled(true);
    }

    btnLast->setEnabled(false);
    btnNext->setEnabled(false);
}
