#ifndef FRMALARM_H
#define FRMALARM_H

#include <QWidget>

class QListWidgetItem;
class QTreeWidgetItem;
class QMenu;
class QModelIndex;

namespace Ui
{
    class frmAlarm;
}

class frmAlarm : public QWidget
{
    Q_OBJECT

public:
    explicit frmAlarm(QWidget *parent = 0);
    ~frmAlarm();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void HostReceiveData(QString, QString hostName, QString data);

private slots:
    void button_clicked();

    void Init();
    void AutoBuFang();

    void BuFang();
    void CheFang();
    void PangLu();
    void HuiFu();

    void SaveConfig();
    void LoadDefenceInfo();

    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_treeWidget_itemPressed(QTreeWidgetItem *item, int);

    void on_btnBuFang_clicked();    //用户布防
    void on_btnCheFang_clicked();   //用户撤防

    void on_btnAdd_clicked();       //添加防区
    void on_btnDelete_clicked();    //删除防区
    void on_btnUpdate_clicked();    //修改防区
    void on_btnMap_clicked();       //导入地图
    void on_btnMove_clicked();      //移动防区
    void on_btnLot_clicked();       //批量添加防区

    void on_treeDefence_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *);
    void on_cboxDefenceID_currentIndexChanged(const QString &arg1);
    void on_cboxDefenceSound_currentIndexChanged(const QString &arg1);
    void on_cboxDefenceImage_currentIndexChanged(const QString &arg1);

    void on_btnSelect_clicked();    //数据查询
    void on_btnDeleteData_clicked();//删除数据
    void on_btnPrint_clicked();     //打印数据
    void on_btnPdf_clicked();       //导出数据到PDF
    void on_btnExcel_clicked();     //导出数据到excel

    void on_ckDefenceName_toggled(bool checked);
    void on_ckType_toggled(bool checked);
    void on_ckContent_toggled(bool checked);
    void on_ckTime_toggled(bool checked);
    void on_cboxPageCount_currentIndexChanged(const QString &arg1);

    void LoadLinkInfo();
    void on_btnAddLink_clicked();
    void on_btnDeleteLink_clicked();
    void on_btnUpdateLink_clicked();
    void on_btnClearLink_clicked();
    void on_listLink_pressed(const QModelIndex &index);
    void on_cboxSnapTime_currentIndexChanged(const QString &arg1);

    void on_btnSelectVideo_clicked();
    void on_btnSaveVideo_clicked();
    void on_listVideoPath_doubleClicked(const QModelIndex &index);
    void on_cboxVideoType_currentIndexChanged(const QString &arg1);
    void on_ckFill_stateChanged(int arg1);
    void on_ckFade_stateChanged(int arg1);

private:
    Ui::frmAlarm *ui;

    void InitForm();            //初始化窗体数据
    void LoadImageFile();       //加载地图目录下所有图片文件
    void LoadSoundFile();       //加载声音目录下所有声音文件

    bool defenceChange;         //防区信息是否改过
    QString tempDefenceID;      //临时防区号,用于修改和删除防区
    QString defenceID;          //鼠标按下处的防区号
    QMenu *menu;                //右键菜单
    int debugLogCount;          //当前打印调试信息条数

    int pageCount;              //每页记录条数
    QString whereSql;           //查询条件语句

    QList<QString> columnNames; //字段名集合
    QList<int> columnWidths;    //字段宽度集合    

    //获取需要导出和打印的数据
    QStringList GetContent();

    //修改防区位置坐标信息
    void UpdateDefenceInfo(QString defenceID, int defenceX, int defenceY);

    //拷贝文件夹
    bool CopyDir(const QString &fromDir, const QString &toDir, bool cover);

};

#endif // FRMALARM_H
