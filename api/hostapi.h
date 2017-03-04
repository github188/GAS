#ifndef HOSTAPI_H
#define HOSTAPI_H

#include <QObject>
#include <QSound>
#include <QStringList>
#include "ds7400_com.h"
#include "vista128_com.h"
#include "api/gsmapi.h"
#include "api/sendemailthread.h"
#include "api/checktaskthread.h"

class HostAPI : public QObject
{
    Q_OBJECT
public:
    explicit HostAPI(QObject *parent = 0);
    ~HostAPI();

    static HostAPI *Instance() {
        static QMutex mutex;
        if (!_instance) {
            QMutexLocker locker(&mutex);
            if (!_instance) {
                _instance = new HostAPI;
            }
        }
        return _instance;
    }

    void Init();
    void BuFang();
    void BuFang(int subSystem);
    void CheFang();
    void CheFang(int subSystem);
    void PangLu(QString defenceID);
    void HuiFu(QString defenceID);

private:
    static HostAPI *_instance;

    VISTA128_COM *vista128_com;         //VISTA128主机处理类
    DS7400_COM *ds7400_com;             //DS7400主机处理类
    GSMAPI *GSM;                        //短信告警处理类
    CheckTaskThread *checkTaskThread;   //任务处理线程
    QSound *sound;                      //报警声音对象

    QTimer *timerNTP;                   //NTP服务定时器
    QTimer *timer128;                   //128主机连续布防撤防
    QStringList task;                   //临时任务队列处理

    //短信告警邮件告警算法
    //定时器取出每次将所有告警信息一次性发出
    QTimer *timerMsg;
    QTimer *timerEmail;

private slots:
    //定时器校时
    void CheckTime();
    void ReceiveTime(QDateTime dateTime);
    //定时器处理短信告警
    void CheckMsg();
    //定时器处理邮件告警
    void CheckEmail();
    //定时器处理队列任务
    void CheckTask();

signals:
    void HostReceiveData(QString hostID, QString hostName, QString data);       //主机收到数据信号

public slots:
    void HostConnect(QString, QString);                         //主机上线
    void HostDisConnect(QString, QString);                      //主机下线
    void HostError(QString, QString, QString error);            //主机错误,例如电池掉电
    void HostBuFang(QString, QString);                          //主机布防
    void HostCheFang(QString, QString);                         //主机撤防
    void HostBuFangSubSystem(QString, QString, int subSystem);  //主机子系统布防
    void HostCheFangSubSystem(QString, QString, int subSystem); //主机子系统撤防
    void DefenceBaoJing(QString, QString, QString defenceID);   //防区报警
    void DefencePangLu(QString, QString, QString defenceID);    //防区旁路
    void DefenceGuZhang(QString, QString, QString defenceID);   //防区故障
    void DefenceHuiFu(QString, QString, QString defenceID);     //防区恢复

    void SetNTPInterval();
    void SetMsgInterval();
    void SetEmailInterval();

};

#endif // HOSTAPI_H
