#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "api.h"
#include "myhelper.h"
#include "dbhelper.h"

API::API(QObject *parent) :
    QObject(parent)
{
}

QFrame *boxMain;                //显示地图和防区
QTableWidget *tableWidget;      //显示临时消息
QTreeWidget *treeWidget;        //显示主机防区树状图
QListWidget *listWidget;        //显示防区缩略图
int messageCount;               //已添加消息条数

//添加临时消息
void API::AddMessage(QString message)
{
    if (messageCount >= App::MsgCount) {
        tableWidget->clearContents();
        messageCount = 0;
    }

    QTableWidgetItem *itemTime = new QTableWidgetItem(QTime::currentTime().toString("HH:mm:ss"));
    QTableWidgetItem *itemMessage = new QTableWidgetItem(message);
    tableWidget->setItem(messageCount, 0, itemTime);
    tableWidget->setItem(messageCount, 1, itemMessage);
    messageCount++;
}

//传递主界面控件
void API::SetControl(QFrame *BoxMain, QTableWidget *TableWidget,
                     QTreeWidget *TreeWidget, QListWidget *ListWidget)
{
    boxMain = BoxMain;
    tableWidget = TableWidget;
    treeWidget = TreeWidget;
    listWidget = ListWidget;

    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->horizontalHeader()->setStretchLastSection(true);

    //设置列数和列宽
    tableWidget->setColumnCount(2);
#ifdef __arm__
    tableWidget->setColumnWidth(0, 90);
#else
    tableWidget->setColumnWidth(0, 70);
#endif

    //设置行高
    tableWidget->setRowCount(App::MsgCount);
    for (int i = 0; i < App::MsgCount; i++) {
        tableWidget->setRowHeight(i, 24);
    }

    QStringList headText;
    headText << "时间" << "警情";
    tableWidget->setHorizontalHeaderLabels(headText);//设置表头
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不允许双击编辑
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//选中模式为单行选中

    treeWidget->setColumnCount(1); //设置列数
    treeWidget->setColumnWidth(0, 170);
    treeWidget->header()->setVisible(false);//隐藏列标题
    treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能编辑
}

//初始化所有防区,指定防区容器,在防区管理中调用
void API::InitDefenceAll(QFrame *boxMain)
{
    qDeleteAll(boxMain->findChildren<DefenceButton *>());//清空原有防区

    QSqlQuery query;
    QString sql = QString("select DefenceID,DefenceName,SubSystem,DefenceSound,DefenceImage,DefenceX,DefenceY from DefenceInfo");

    query.exec(sql);
    while (query.next()) {
        QString tempDefenceID = query.value(0).toString();
        QString tempDefenceName = query.value(1).toString();
        int tempSubsystem = query.value(2).toInt();
        QString tempDefenceSound = query.value(3).toString();
        QString tempDefenceImage = query.value(4).toString();
        int tempDefenceX = query.value(5).toInt();
        int tempDefenceY = query.value(6).toInt();

        DefenceButton *btn = new DefenceButton(boxMain, tempDefenceID, tempDefenceName,
                                               tempSubsystem, tempDefenceSound, tempDefenceImage,
                                               tempDefenceX, tempDefenceY, DefenceButton::Status_GuZhang);

        btn->setGeometry(tempDefenceX, tempDefenceY, 25, 25);
    }
}

//初始化当前防区,指定防区容器,在防区管理中调用
void API::InitDefenceCurrent(QFrame *boxMain)
{
    if (boxMain->property("image").toString() != App::CurrentImage) {
        boxMain->setStyleSheet(QString("QFrame{border-image:url(%1/map/%2);}")
                               .arg(App::AppPath)
                               .arg(App::CurrentImage));
        boxMain->setProperty("image", App::CurrentImage);
    }

    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();

    foreach (DefenceButton * b, btn) {
        if (b->DefenceImage() == App::CurrentImage) {
            b->setVisible(true);
        } else {
            b->setVisible(false);
        }
    }
}

//初始化所有防区,在主界面中调用
void API::InitDefenceAll()
{
    qDeleteAll(boxMain->findChildren<DefenceButton *>());//清空原有防区

    QSqlQuery query;
    QString sql = QString("select DefenceID,DefenceName,SubSystem,DefenceSound,DefenceImage,DefenceX,DefenceY from DefenceInfo");

    query.exec(sql);
    while (query.next()) {
        QString tempDefenceID = query.value(0).toString();
        QString tempDefenceName = query.value(1).toString();
        int tempSubsystem = query.value(2).toInt();
        QString tempDefenceSound = query.value(3).toString();
        QString tempDefenceImage = query.value(4).toString();
        int tempDefenceX = query.value(5).toInt();
        int tempDefenceY = query.value(6).toInt();

        DefenceButton *btn = new DefenceButton(boxMain, tempDefenceID, tempDefenceName,
                                               tempSubsystem, tempDefenceSound, tempDefenceImage,
                                               tempDefenceX, tempDefenceY, DefenceButton::Status_GuZhang);

        btn->setGeometry(tempDefenceX, tempDefenceY, 25, 25);

        App::CurrentImage = tempDefenceImage;
    }
}

//初始化当前防区,在主界面中调用
void API::InitDefenceCurrent()
{
    if (boxMain->property("image").toString() != App::CurrentImage) {
        boxMain->setStyleSheet(QString("QFrame{border-image:url(%1/map/%2);}")
                               .arg(App::AppPath)
                               .arg(App::CurrentImage));
        boxMain->setProperty("image", App::CurrentImage);
    }

    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();

    foreach (DefenceButton * b, btn) {
        if (b->DefenceImage() == App::CurrentImage) {
            b->setVisible(true);
        } else {
            b->setVisible(false);
        }
    }
}

//初始化主机防区树状列表,主界面中调用
void API::InitTreeWidget()
{
    treeWidget->clear();

    QTreeWidgetItem *itemHost = new QTreeWidgetItem(treeWidget, QStringList("报警主机"));
    itemHost->setIcon(0, QIcon(":/banner/tree_parent.png"));

    QSqlQuery query;
    QString sql = "select DefenceID,DefenceName from DefenceInfo order by DefenceID asc";
    query.exec(sql);

    while (query.next()) {
        QString tempDefenceID = query.value(0).toString();
        QString tempDefenceName = query.value(1).toString();
        QTreeWidgetItem *itemDefence = new QTreeWidgetItem(itemHost, QStringList(QString(tempDefenceName + "[" + tempDefenceID + "]")));
        itemDefence->setIcon(0, QIcon(":/banner/tree_child.png"));
        itemHost->addChild(itemDefence);
    }

    treeWidget->expandAll();
}

//初始化主机防区树状列表,在防区管理中调用
void API::InitTreeWidget(QTreeWidget *treeWidget)
{
    treeWidget->clear();

    QTreeWidgetItem *itemHost = new QTreeWidgetItem(treeWidget, QStringList("报警主机"));
    itemHost->setIcon(0, QIcon(":/banner/tree_parent.png"));

    QSqlQuery query;
    QString sql = "select DefenceID,DefenceName from DefenceInfo order by DefenceID asc";
    query.exec(sql);

    while (query.next()) {
        QString tempDefenceID = query.value(0).toString();
        QString tempDefenceName = query.value(1).toString();
        QTreeWidgetItem *itemDefence = new QTreeWidgetItem(itemHost, QStringList(QString(tempDefenceName + "[" + tempDefenceID + "]")));
        itemDefence->setIcon(0, QIcon(":/banner/tree_child.png"));
        itemHost->addChild(itemDefence);
    }

    treeWidget->expandAll();
}

//初始化地图缩略图列表,主界面中调用
void API::InitListWidget()
{
    listWidget->clear();
    listWidget->setViewMode(QListView::IconMode);   //设置为图标显示模式
    listWidget->setResizeMode(QListView::Adjust);   //设置为自适应
#ifdef __arm__
    listWidget->setIconSize(QSize(230, 210));       //设置图标大小
#else
    listWidget->setIconSize(QSize(170, 160));       //设置图标大小
#endif
    listWidget->setMovement(QListView::Static);     //设置为不可拖动
    QDir imagePath(App::AppPath + "/map");
    QStringList filter;
    filter << "*.jpg" << "*.bmp" << "*.png" << "*.jpeg"; //过滤图片文件拓展名
    QStringList maps = imagePath.entryList(filter);

    int count = maps.count();
    for (int i = 0; i < count; i++) {
        QString tempMap = QString("%1/map/%2").arg(App::AppPath).arg(maps.at(i));
        QListWidgetItem *listItem = new QListWidgetItem(QIcon(tempMap), maps.at(i), listWidget, i);
        listItem->setTextAlignment(Qt::AlignCenter);
    }
}

//主机上线处理
void API::Connect()
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        b->SetDefenceStatus(DefenceButton::Status_CheFang);
    }

    AddMessage("主机上线");
    DBHelper::AddEventInfoHost("主机上线");
}

//主机下线处理
void API::DisConnect()
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        b->SetDefenceStatus(DefenceButton::Status_GuZhang);
    }

    AddMessage("主机下线");
    DBHelper::AddEventInfoHost("主机下线");

    QString str = QString("告警类型:%1;告警时间:%2").arg("主机下线").arg(DATETIME);
    if (App::MsgInterval <= 1440) {
        App::AppendMsg(str);
    }
    if (App::EmailInterval <= 1440) {
        App::AppendEmail(str);
    }
}

//主机收到错误消息处理
void API::Error(QString error)
{
    //对应主界面添加快捷信息
    AddMessage(error);

    //将操作记录添加到数据库
    DBHelper::AddEventInfoHost(error);
}

//用户对整台主机进行布防操作
void API::BuFang()
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->DefenceStatus() != DefenceButton::Status_PangLu) {
            b->SetDefenceStatus(DefenceButton::Status_BuFang);
        }
    }

    AddMessage("主机布防");
    DBHelper::AddEventInfoUser("主机布防");
}

//用户对主机子系统进行布防
void API::BuFang(int subSystem)
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->SubSystem() == subSystem) {
            if (b->DefenceStatus() != DefenceButton::Status_PangLu) {
                b->SetDefenceStatus(DefenceButton::Status_BuFang);
            }
        }
    }

    AddMessage(QString("子系统%1布防").arg(subSystem));
    DBHelper::AddEventInfoUser("主机布防");
}

//用户对单个主机进行撤防操作
void API::CheFang()
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        b->SetDefenceStatus(DefenceButton::Status_CheFang);
    }

    AddMessage("主机撤防");
    DBHelper::AddEventInfoUser("主机撤防");
}

//用户对单个主机对应子系统进行撤防操作,对应区分子系统的主机
void API::CheFang(int subSystem)
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->SubSystem() == subSystem) {
            b->SetDefenceStatus(DefenceButton::Status_CheFang);
        }
    }

    AddMessage(QString("子系统%1撤防").arg(subSystem));
    DBHelper::AddEventInfoUser("主机撤防");
}

//主机故障旁路
void API::PangLu()
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        b->SetDefenceStatus(DefenceButton::Status_PangLu);
    }

    AddMessage("主机旁路");
    DBHelper::AddEventInfoUser("主机旁路");
}

//主机防区旁路
void API::PangLu(QString defenceID)
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->DefenceID() == defenceID) {
            b->SetDefenceStatus(DefenceButton::Status_PangLu);
            AddMessage(b->DefenceName() + "旁路");
            DBHelper::AddEventInfo("主机上报", "防区旁路", defenceID, b->DefenceName());
            break;
        }
    }
}

//主机防区报警时触发
void API::BaoJing(QString defenceID)
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->DefenceID() == defenceID) {
            //切换到当前防区地图
            App::CurrentImage = b->DefenceImage();
            InitDefenceCurrent();

            b->SetDefenceStatus(DefenceButton::Status_BaoJing);
            b->raise();
            AddMessage(b->DefenceName() + "报警");
            DBHelper::AddEventInfo("主机上报", "防区报警", defenceID, b->DefenceName());

            break;
        }
    }
}

//主机防区断开和故障时触发
void API::GuZhang(QString defenceID)
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->DefenceID() == defenceID) {
            //如果该防区处于报警或者故障状态,则无需进行故障处理
            if (b->DefenceStatus() == DefenceButton::Status_BaoJing ||
                    b->DefenceStatus() == DefenceButton::Status_GuZhang) {
                return;
            }
            b->SetDefenceStatus(DefenceButton::Status_GuZhang);
            AddMessage(b->DefenceName() + "故障");
            DBHelper::AddEventInfo("主机上报", "防区故障", defenceID, b->DefenceName());
            break;
        }
    }
}

//主机现场防区开路或者短路恢复触发
void API::HuiFu(QString defenceID)
{
    QList<DefenceButton *> btn = boxMain->findChildren<DefenceButton *>();
    foreach (DefenceButton * b, btn) {
        if (b->DefenceStatus() != DefenceButton::Status_BaoJing) {
            if (b->DefenceID() == defenceID) {
                b->SetDefenceStatus(DefenceButton::Status_CheFang);
                AddMessage(b->DefenceName() + "现场复位");
                DBHelper::AddEventInfo("主机上报", "现场复位", defenceID, b->DefenceName());
                break;
            }
        }
    }
}
