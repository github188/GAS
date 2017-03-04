#ifndef DEFENCEBUTTON_H
#define DEFENCEBUTTON_H

#include <QWidget>
#include <QTimer>

class DefenceButton: public QWidget
{
    Q_OBJECT
public:

    //防区状态  布防、撤防、报警、旁路、断开、故障
    enum Status {
        Status_BuFang = 0,
        Status_CheFang = 1,
        Status_BaoJing = 2,
        Status_PangLu = 3,
        Status_DuanKai = 4,
        Status_GuZhang = 5
    };

    explicit DefenceButton(QWidget *parent = 0);

    //构造函数,直接传入一个防区所需要的所有属性
    explicit DefenceButton(QWidget *parent, QString defenceID, QString defenceName,
                           int subSystem, QString defenceSound, QString defenceImage,
                           int defenceX, int defenceY, Status defenceStatus);

    void SetDefenceID(QString defenceID);
    void SetDefenceStatus(Status defenceStatus);

    QString DefenceID()const {
        return defenceID;
    }
    QString DefenceName()const {
        return defenceName;
    }
    int SubSystem()const {
        return subSystem;
    }
    QString DefenceSound()const {
        return defenceSound;
    }
    QString DefenceImage()const {
        return defenceImage;
    }
    int DefenceX()const {
        return defenceX;
    }
    int DefenceY()const {
        return defenceY;
    }
    Status DefenceStatus()const {
        return defenceStatus;
    }

signals:

protected:
    void paintEvent(QPaintEvent *event);    //重写绘制函数
    bool eventFilter(QObject *, QEvent *);  //过滤鼠标按下事件

private slots:
    void BaoJingChange();           //执行报警改变操作

private:
    QString defenceID;              //防区号
    QString defenceName;            //防区名
    int subSystem;                  //防区对应子系统
    QString defenceSound;           //防区报警声音
    QString defenceImage;           //防区地图
    int defenceX;                   //防区X坐标
    int defenceY;                   //防区Y坐标
    Status defenceStatus;           //防区状态

    QRadialGradient *currentRG;     //当前颜色
    bool IsRed;                     //当前报警颜色是否为红色
    QTimer *timerAlarm;             //控制报警闪烁时钟

};

#endif //DEFENCEBUTTON_H
