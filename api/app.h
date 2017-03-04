#ifndef APP_H
#define APP_H

#include <QList>

class App
{
public:
    static QString AppName;         //应用程序英文名称
    static QString AppPath;         //应用程序路径
    static int DeskWidth;           //运行环境桌面宽度
    static int DeskHeight;          //运行环境桌面高度

    static int IcoMain;             //左上角图标
    static int IcoMenu;             //下来菜单图标
    static int IcoMin;              //最小化图标
    static int IcoMax;              //最大化图标
    static int IcoNormal;           //正常图标
    static int IcoClose;            //关闭图标

    static QString CurrentImage;    //当前防区对应地图
    static bool IsMove;             //防区是否可以移动
    static QString CurrentUserName; //当前用户名
    static QString CurrentUserPwd;  //当前用户密码
    static QString CurrentUserType; //当前用户类型（值班员、管理员）
    static QString PaletteColor;    //画板背景色
    static QString TextColor;       //文字颜色

    static QString KeyData;         //注册码密文
    static bool KeyUseDate;         //是否启用运行日期时间限制
    static QString KeyDate;         //到期时间字符串
    static bool KeyUseRun;          //是否启用可运行时间限制
    static int KeyRun;              //可运行时间
    static bool KeyUseCount;        //是否启用设备数量限制
    static int KeyCount;            //设备限制数量

    static bool SaveLog;            //输出日志文件
    static bool UseStyle;           //启用样式
    static QString StyleName;       //应用程序样式
    static int FontSize;            //应用程序字体大小
    static QString FontName;        //应用程序字体名称

    static bool UseInput;           //是否启用输入法
    static QString InputPosition;   //输入法显示位置
    static QString Title;           //应用程序标题
    static QString Version;         //应用程序版本
    static QString Author;          //版权所有

    static QString HostType;        //主机类型
    static QString HostCom;         //主机通信端口
    static QString AlarmServerIP;   //报警上传IP
    static int AlarmServerPort;     //报警上传端口
    static int MsgCount;            //临时警情条数
    static QString SnapPath;        //抓拍图像保存路径
    static QString VideoPath;       //联动视频保存路径

    static bool AutoBuFang;         //是否启用自动布防
    static bool VideoLink;          //是否启用视频联动
    static bool NetLink;            //是否启用报警上传
    static bool SnapLink;           //是否启用报警抓拍
    static bool PrintLink;          //是否启用警情打印

    static bool UseNTP;             //是否启用NTP校时
    static QString NTPIP;           //NTP IP地址
    static int NTPInterval;         //校时间隔

    static QString MsgPortName;     //短信猫通信串口号
    static int MsgBaudRate;         //短信猫通信波特率
    static int MsgInterval;         //短信告警间隔
    static QString MsgTel;          //告警短信接收号码

    static QString SendEmailAddr;   //发件人地址
    static QString SendEmailPwd;    //发件人密码
    static int EmailInterval;       //邮件告警间隔
    static QString EmailAddr;       //告警邮件接收地址

    static void ReadConfig();       //读取配置文件,在main函数最开始加载程序载入
    static void WriteConfig();      //写入配置文件,在更改配置文件程序关闭时调用

    static void WriteError(QString str);//写入错误信息
    static void WriteStartTime();       //写入启动时间
    static void NewDir(QString dirName);//新建目录

    static QList<QString> TaskList;             //待处理操作任务链表
    static QList<QString> SnapList;             //待保存的抓拍告警图像名称
    static QList<QString> EmailList;            //待发送告警邮件链表
    static QList<QString> MsgList;              //待发送告警邮件链表
    static void AppendTask(QString task);       //插入待处理操作任务
    static void AppendSnap(QString snap);       //插入待保存告警图像名称队列
    static void AppendEmail(QString email);     //插入待发送告警邮件链表
    static void AppendMsg(QString msg);         //插入待发送告警邮件链表

};

#endif // APP_H
