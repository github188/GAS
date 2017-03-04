#ifndef SCREENAPI_H
#define SCREENAPI_H

#ifndef FRMSCREEN_H
#define FRMSCREEN_H

#include <QPoint>
#include <QSize>

#include <QDialog>
#include <QMutex>
#include <QMenu>

class ScreenAPI
{
public:
	enum STATUS {SELECT, MOV, SET_W_H};
	ScreenAPI() {}
	ScreenAPI(QSize size);

	void setStart(QPoint pos);
	void setEnd(QPoint pos);
	QPoint getStart();
	QPoint getEnd();

	QPoint getLeftUp();
	QPoint getRightDown();

	STATUS getStatus();
	void setStatus(STATUS status);

	int width();
	int height();
	bool isInArea(QPoint pos);          // 检测pos是否在截图区域内
	void move(QPoint p);                // 按 p 移动截图区域

private:
	QPoint leftUpPos, rightDownPos;     //记录 截图区域 左上角、右下角
	QPoint startPos, endPos;            //记录 鼠标开始位置、结束位置
	int maxWidth, maxHeight;            //记录屏幕大小
	STATUS status;                      //三个状态 :  选择区域、移动区域、设置width height

	void cmpPoint(QPoint &s, QPoint &e);//比较两位置，判断左上角、右下角
};

#endif // SCREENAPI_H

class frmScreen : public QDialog
{
	Q_OBJECT

public:
	explicit frmScreen(QWidget *parent = 0);
    static frmScreen *Instance()
    {
        static QMutex mutex;

        if (!_instance) {
            QMutexLocker locker(&mutex);

            if (!_instance) {
                _instance = new frmScreen;
            }
        }

        return _instance;
    }

private:
    static frmScreen *_instance;
	QMenu *menu;            //右键菜单对象
	ScreenAPI *screen;      //截屏对象
	QPixmap *fullScreen;    //保存全屏图像
	QPixmap *bgScreen;      //模糊背景图
	QPoint movPos;          //坐标

protected:
	void contextMenuEvent(QContextMenuEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *);

private slots:
	void InitForm();
	void SaveScreen();
	void SaveScreenOther();
	void SaveFullScreen();

};

#endif // FRMSCREEN_H

