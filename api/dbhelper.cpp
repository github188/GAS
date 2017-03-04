#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "dbhelper.h"
#include "myhelper.h"

DBHelper::DBHelper(QObject *parent) :
    QObject(parent)
{
}

void DBHelper::AddEventInfo(QString triggerType, QString triggerContent, QString defenceID, QString defenceName)
{
    QString sql = "insert into EventInfo(TriggerTime,TriggerType,TriggerContent,DefenceID,DefenceName) values('";
    sql += QString("%1").arg(DATETIME) + "','";
    sql += triggerType + "','";
    sql += triggerContent + "','";
    sql += defenceID + "','";
    sql += defenceName + "')";

    QSqlQuery query;
    query.prepare(sql);
    query.exec();
}

void DBHelper::AddEventInfoUser(QString triggerContent)
{
    AddEventInfo("用户操作", triggerContent, "000", "000");
}

void DBHelper::AddEventInfoHost(QString triggerContent)
{
    AddEventInfo("主机上报", triggerContent, "000", "000");
}

bool DBHelper::IsExistDefence(QString defenceID)
{
    QSqlQuery query;
    QString sql = QString("select DefenceName from DefenceInfo where DefenceID='%1'").arg(defenceID);
    query.exec(sql);
    return query.next();
}

void DBHelper::GetDefenceInfo(QString defenceID, QString &defenceName, QString &defenceSound)
{
    QSqlQuery query;
    QString sql = QString("select DefenceName,DefenceSound from DefenceInfo where DefenceID='%1'").arg(defenceID);
    query.exec(sql);
    query.next();
    defenceName = query.value(0).toString();
    defenceSound = query.value(1).toString();
}

bool DBHelper::IsExistLink(QString linkName)
{
    QString sql = QString("select LinkName from LinkInfo where LinkName='%1'").arg(linkName);
    QSqlQuery query;
    query.exec(sql);
    return query.next();
}
