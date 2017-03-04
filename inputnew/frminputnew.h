#ifndef FRMINPUTNEW_H
#define FRMINPUTNEW_H

#include <QtGui>
#include <QtSql>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

namespace Ui
{
class frmInputNew;
}

class frmInputNew : public QWidget
{
	Q_OBJECT

public:
	explicit frmInputNew(QWidget *parent = 0);
	~frmInputNew();

	//单例模式,保证一个程序只存在一个输入法实例对象
	static frmInputNew *Instance()
	{
		if (!_self) {
			static QMutex mutex;
			QMutexLocker locker(&mutex);

			if (!_self) {
				_self = new frmInputNew;
			}
		}

		return _self;
	}

	//初始化面板状态,包括字体大小
	void init(QString position, QString style, int btnFontSize, int labFontSize,
	          int frmWidth, int frmHeight, int icoWidth, int icoHeight,
	          int spacing, int topHeight);
	void init(QString position);

protected:
	//事件过滤器,处理鼠标在汉字标签处单击操作
	bool eventFilter(QObject *obj, QEvent *event);
	//鼠标拖动事件
	void mouseMoveEvent(QMouseEvent *e);
	//鼠标按下事件
	void mousePressEvent(QMouseEvent *e);
	//鼠标松开事件
	void mouseReleaseEvent(QMouseEvent *);

private slots:
	//焦点改变事件槽函数处理
	void focusChanged(QWidget *oldWidget, QWidget *nowWidget);
	//改变输入法面板样式
	void changeStyle(QString mainBackgroundColor, QString mainTextColor, QString btnBackgroundColor,
	                 QString btnHoveColor, QString btnHoveTextColor, QString labHoveColor, QString labHoveTextColor);
	//输入法面板按键处理
	void btnClicked();
	//定时器处理退格键
	void reClicked();

private:
	Ui::frmInputNew *ui;
	static frmInputNew *_self;  //实例对象

	int frmWidth;                   //窗体宽度
	int frmHeight;                  //窗体高度
	int icoWidth;                   //图标宽度
	int icoHeight;                  //图标高度
	int spacing;                    //按钮间隔
	int topHeight;                  //汉字显示区域高度

	int deskWidth;                  //桌面宽度
	int deskHeight;                 //桌面高度

	QPoint mousePoint;              //鼠标拖动自定义标题栏时的坐标
	bool mousePressed;              //鼠标是否按下

	bool isPress;                   //是否长按退格键
	QPushButton *btnPress;          //长按按钮
	QTimer *timerPress;             //退格键定时器
	bool checkPress();              //校验当前长按的按钮

	bool isFirst;                   //是否首次加载
	void initForm();                //初始化窗体数据
	void initProperty();            //初始化属性
	void changeStyle();             //改变样式
	void changeSize();              //改变大小
	void showPanel();               //显示输入法面板

	QWidget *currentWidget;         //当前焦点的对象
	QLineEdit *currentLineEdit;     //当前焦点的单行文本框
	QTextEdit *currentTextEdit;     //当前焦点的多行文本框
	QPlainTextEdit *currentPlain;   //当前焦点的富文本框
	QTextBrowser *currentBrowser;   //当前焦点的文本浏览框

	QString currentEditType;        //当前焦点控件的类型
	QString currentPosition;        //当前输入法面板位置类型
	QString currentStyle;           //当前输入法面板样式
	int btnFontSize;                //当前输入法面板按钮字体大小
	int labFontSize;                //当前输入法面板标签字体大小
	void insertValue(QString value);//插入值到当前焦点控件
	void deleteValue();             //删除当前焦点控件的一个字符

	bool upper;                     //当前大小写状态
	QString currentType;            //当前输入法类型
	void changeType(QString type);  //改变输入法类型
	void changeLetter(bool isUpper);//改变字母大小写
	void changeChar(bool isChar);   //按钮改变成特殊字符

	QList<QLabel *>labCh;           //汉字标签数组
	QStringList allPY;              //所有拼音链表
	QStringList currentPY;          //当前拼音链表
	int currentPY_index;            //当前拼音索引
	int currentPY_count;            //当前拼音数量
	void selectChinese();           //查询汉字
	void showChinese();             //显示查询到的汉字
	void setChinese(int index);     //设置当前汉字
	void clearChinese();            //清空当前汉字信息

};

#endif // FRMINPUTNEW_H
