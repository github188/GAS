#ifndef API_H
#define API_H

#include <QObject>
#include "usercontrol/defencebutton.h"

class QFrame;
class QTableWidget;
class QTreeWidget;
class QListWidget;

class API : public QObject
{
    Q_OBJECT
public:
    explicit API(QObject *parent = 0);

    //添加快捷提示信息
    static void AddMessage(QString message);

    //设置控件（必须最前面设置）
    static void SetControl(QFrame *BoxMain, QTableWidget *TableWidget,
                           QTreeWidget *TreeWidget, QListWidget *ListWidget);

    //初始化所有防区及当前防区
    static void InitDefenceAll(QFrame *boxMain);
    static void InitDefenceCurrent(QFrame *boxMain);
    static void InitDefenceAll();
    static void InitDefenceCurrent();

    //初始化主机防区树状图/防区缩略图
    static void InitTreeWidget();
    static void InitTreeWidget(QTreeWidget *treeWidget);
    static void InitListWidget();

    //主机上线下线及错误处理
    static void Connect();
    static void DisConnect();
    static void Error(QString error);

    //布防撤防操作
    static void BuFang();
    static void BuFang(int subSystem);
    static void CheFang();
    static void CheFang(int subSystem);

    //主机及防区旁路/防区报警/防区断开及故障/防区恢复及软件复位操作
    static void PangLu();
    static void PangLu(QString defenceID);
    static void BaoJing(QString defenceID);
    static void GuZhang(QString defenceID);
    static void HuiFu(QString defenceID);

};

#endif // API_H
