#ifndef DS7400_COM_H
#define DS7400_COM_H

#include <QObject>
#include "qextserialport/qextserialport.h"
#include <QTimer>
#include <QDateTime>

/* 说明:DS7400主机操作头文件
 * 功能:动态识别主机上线下线及布防撤防报警等信号
 * 作者:刘典武  QQ:517216493
 * 时间:2014-1-5  检查:2014-1-10
 */
class DS7400_COM : public QObject
{
    Q_OBJECT
public:
    explicit DS7400_COM(QObject *parent = 0);

    //设置和获取主机类型
    void SetHostType(QString hostType) {
        this->hostType = hostType;
    }
    QString HostType()const {
        return hostType;
    }

    //设置和获取主机ID
    void SetHostID(QString hostID) {
        this->hostID = hostID;
    }
    QString HostID()const {
        return hostID;
    }

    //设置和获取主机名称
    void SetHostName(QString hostName) {
        this->hostName = hostName;
    }
    QString HostName()const {
        return hostName;
    }

    //设置和获取主机通讯参数1
    void SetHostConnectValue1(QString hostConnectValue1) {
        this->hostConnectValue1 = hostConnectValue1;
    }
    QString HostConnectValue1()const {
        return hostConnectValue1;
    }

    //设置和获取主机通讯参数2
    void SetHostConnectValue2(QString hostConnectValue2) {
        this->hostConnectValue2 = hostConnectValue2;
    }
    QString HostConnectValue2()const {
        return hostConnectValue2;
    }

    bool Open();
    bool IsOpen()const {
        return isOpen;
    }
    void Close();

private:
    QextSerialPort *hostCOM;

    bool isOpen;                    //打开状态
    bool isOnLine;                  //在线状态,初始值为假
    bool isCheckOnline;             //检测下线状态,初始值为真

    QDateTime lastHeartTime;        //存储最后一次心跳时间
    QTimer *timerOnLine;            //定时器检测主机上线
    QTimer *timerRead;              //定时器读取数据

    QString hostType;               //主机类型
    QString hostID;                 //主机编号
    QString hostName;               //主机名称
    QString hostConnectValue1;      //主机参数1
    QString hostConnectValue2;      //主机参数2

    bool errorAC;                   //交流电源故障
    bool errorDC;                   //备用电池故障
    bool errorCode;                 //报告发送故障
    bool errorControl;              //控制故障
    bool error485;                  //总线故障
    bool errorWifi;                 //无线接收故障
    bool errorPower;                //辅助电源故障
    bool errorDevice;               //设备故障

    void CheckCode(QString code);
    QString GetDefenceID(int tempID);

private slots:
    void ReadData();                //读取数据槽函数
    void CheckOnLine();             //检测在线状态槽函数

signals:
    void HostConnect(QString hostID, QString hostName);                      //主机上线
    void HostDisConnect(QString hostID, QString hostName);                   //主机下线
    void HostError(QString hostID, QString hostName, QString error);         //主机错误,例如电池掉电
    void HostBuFangSubSystem(QString hostID, QString hostName, int subSystem); //主机子系统布防
    void HostCheFangSubSystem(QString hostID, QString hostName, int subSystem); //主机子系统撤防
    void DefenceBaoJing(QString hostID, QString hostName, QString defenceID); //防区报警
    void DefencePangLu(QString hostID, QString hostName, QString defenceID); //防区旁路
    void DefenceGuZhang(QString hostID, QString hostName, QString defenceID); //防区故障
    void DefenceHuiFu(QString hostID, QString hostName, QString defenceID);  //防区恢复

    void HostReceiveData(QString hostID, QString hostName, QString data);   //主机收到数据信号
    void HostHeart(QString hostID, QString hostName);                       //主机心跳信号

public slots:

};

#endif // DS7400_COM_H
