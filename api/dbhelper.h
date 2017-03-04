#ifndef DBHELPER_H
#define DBHELPER_H

#include <QObject>

class DBHelper : public QObject
{
    Q_OBJECT
public:
    explicit DBHelper(QObject *parent = 0);

    //添加日志记录
    static void AddEventInfo(QString triggerType, QString triggerContent, QString defenceID, QString defenceName);
    static void AddEventInfoUser(QString triggerContent);
    static void AddEventInfoHost(QString triggerContent);

    //防区是否已经存在
    static bool IsExistDefence(QString defenceID);

    //根据防区号获取防区名称和告警声音
    static void GetDefenceInfo(QString defenceID, QString &defenceName, QString &defenceSound);

    //当前联动预案是否存在
    static bool IsExistLink(QString linkName);

signals:

public slots:

};

#endif // DBHELPER_H
