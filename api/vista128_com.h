#ifndef VISTA128_COM_H
#define VISTA128_COM_H

#include <QObject>
#include "qextserialport/qextserialport.h"
#include <QTimer>
#include <QDateTime>

class VISTA128_COM : public QObject
{
    Q_OBJECT
public:
    explicit VISTA128_COM(QObject *parent = 0);

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

    void BuFang(int subSystem);
    void CheFang(int subSystem);

    void PangLu(QString defenceID);
    void HuiFu(QString defenceID);
    QString GetCode(QString defenceID);

private:
    QextSerialPort *hostCOM;

    QTimer *timerHeart;             //定时器发送询问心跳
    QTimer *timerStatus;            //定时器发送询问状态

    bool isOpen;                    //打开状态
    bool isOnLine;                  //在线状态,初始值为假
    bool isCheckOnline;             //检测下线状态,初始值为真
    QDateTime lastHeartTime;        //存储最后一次心跳时间
    QTimer *timerOnLine;            //定时器检测主机上线

    QString hostType;               //主机类型
    QString hostID;                 //主机编号
    QString hostName;               //主机名称
    QString hostConnectValue1;      //主机参数1
    QString hostConnectValue2;      //主机参数2

    bool subSystemError[8];
    bool subSystemBuFang[8];

private slots:
    void ReadData();                //读取数据槽函数
    void CheckOnLine();             //检测在线状态槽函数
    void SendHeartData();           //发送询问心跳命令
    void SendStatusData();          //发送询问防区状态命令

signals:
    void HostConnect(QString hostID, QString hostName);                         //主机上线
    void HostDisConnect(QString hostID, QString hostName);                      //主机下线
    void HostError(QString hostID, QString hostName, QString error);            //主机错误,例如电池掉电
    void HostBuFangSubSystem(QString hostID, QString hostName, int subSystem);  //主机子系统布防
    void HostCheFangSubSystem(QString hostID, QString hostName, int subSystem); //主机子系统撤防
    void DefenceBaoJing(QString hostID, QString hostName, QString defenceID);   //防区报警
    void DefencePangLu(QString hostID, QString hostName, QString defenceID);    //防区旁路
    void DefenceGuZhang(QString hostID, QString hostName, QString defenceID);   //防区故障
    void DefenceHuiFu(QString hostID, QString hostName, QString defenceID);     //防区恢复

    void HostReceiveData(QString hostID, QString hostName, QString data);       //主机收到数据信号
    void HostHeart(QString hostID, QString hostName);                           //主机心跳信号
    void DefenceStatus(QString hostID, QString hostName);                       //所有防区状态信号

public slots:

};

#endif // VISTA128_COM_H
