#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "screenapi.h"
#include "myhelper.h"

ScreenAPI::ScreenAPI(QSize size)
{
	maxWidth = size.width();
	maxHeight = size.height();

	startPos = QPoint(-1, -1);
	endPos = startPos;
	leftUpPos = startPos;
	rightDownPos = startPos;
	status = SELECT;
}

int ScreenAPI::width()
{
	return maxWidth;
}

int ScreenAPI::height()
{
	return maxHeight;
}

ScreenAPI::STATUS ScreenAPI::getStatus()
{
	return status;
}

void ScreenAPI::setStatus(STATUS status)
{
	this->status = status;
}

void ScreenAPI::setEnd(QPoint pos)
{
	endPos = pos;
	leftUpPos = startPos;
	rightDownPos = endPos;
	cmpPoint(leftUpPos, rightDownPos);
}

void ScreenAPI::setStart(QPoint pos)
{
	startPos = pos;
}

QPoint ScreenAPI::getEnd()
{
	return endPos;
}

QPoint ScreenAPI::getStart()
{
	return startPos;
}

QPoint ScreenAPI::getLeftUp()
{
	return leftUpPos;
}

QPoint ScreenAPI::getRightDown()
{
	return rightDownPos;
}

bool ScreenAPI::isInArea(QPoint pos)
{
	if (pos.x() > leftUpPos.x() && pos.x() < rightDownPos.x() && pos.y() > leftUpPos.y() && pos.y() < rightDownPos.y()) {
		return true;
	}

	return false;
}

void ScreenAPI::move(QPoint p)
{
	int lx = leftUpPos.x() + p.x();
	int ly = leftUpPos.y() + p.y();
	int rx = rightDownPos.x() + p.x();
	int ry = rightDownPos.y() + p.y();

	if (lx < 0) {
		lx = 0;
		rx -= p.x();
	}

	if (ly < 0) {
		ly = 0;
		ry -= p.y();
	}

	if (rx > maxWidth)  {
		rx = maxWidth;
		lx -= p.x();
	}

	if (ry > maxHeight) {
		ry = maxHeight;
		ly -= p.y();
	}

	leftUpPos = QPoint(lx, ly);
	rightDownPos = QPoint(rx, ry);
	startPos = leftUpPos;
	endPos = rightDownPos;
}

void ScreenAPI::cmpPoint(QPoint &leftTop, QPoint &rightDown)
{
	QPoint l = leftTop;
	QPoint r = rightDown;

	if (l.x() <= r.x()) {
		if (l.y() <= r.y()) {
			;
		} else {
			leftTop.setY(r.y());
			rightDown.setY(l.y());
		}
	} else {
		if (l.y() < r.y()) {
			leftTop.setX(r.x());
			rightDown.setX(l.x());
		} else {
			QPoint tmp;
			tmp = leftTop;
			leftTop = rightDown;
			rightDown = tmp;
		}
	}
}


frmScreen *frmScreen::_instance = 0;
frmScreen::frmScreen(QWidget *parent) : QDialog(parent)
{
	this->InitForm();
}

void frmScreen::InitForm()
{
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	menu = new QMenu(this);
	menu->addAction("保存截图", this, SLOT(SaveScreen()));
	menu->addAction("截图另存为", this, SLOT(SaveScreenOther()));
	menu->addAction("全屏截图", this, SLOT(SaveFullScreen()));
    menu->addAction("退出截图", this, SLOT(hide()));

    //取得屏幕大小
    screen = new ScreenAPI(QApplication::desktop()->size());

    //保存全屏
    fullScreen = new QPixmap();
}

void frmScreen::paintEvent(QPaintEvent *)
{
	int x = screen->getLeftUp().x();
	int y = screen->getLeftUp().y();
	int w = screen->getRightDown().x() - x;
	int h = screen->getRightDown().y() - y;

	QPainter painter(this);

	QPen pen;
	pen.setColor(Qt::green);
	pen.setWidth(2);
	pen.setStyle(Qt::DotLine);
	painter.setPen(pen);

	QFont font;
	font.setFamily(App::FontName);
	font.setPointSize(App::FontSize + 2);
	painter.setFont(font);

	painter.drawPixmap(0, 0, *bgScreen);

	if (w != 0 && h != 0) {
		painter.drawPixmap(x, y, fullScreen->copy(x, y, w, h));
	}

	painter.drawRect(x, y, w, h);

	pen.setColor(Qt::yellow);
	painter.setPen(pen);
	painter.drawText(x + 2, y - 8, tr("截图范围：( %1 x %2 ) - ( %3 x %4 )  图片大小：( %5 x %6 )")
	                 .arg(x).arg(y).arg(x + w).arg(y + h).arg(w).arg(h));
}

void frmScreen::showEvent(QShowEvent *)
{
	QPoint point(-1, -1);
	screen->setStart(point);
	screen->setEnd(point);

    *fullScreen = QPixmap::grabWindow(QApplication::desktop()->winId(), 0, 0, screen->width(), screen->height());

    //设置透明度实现模糊背景
    QPixmap pix(screen->width(), screen->height());
    pix.fill((QColor(160, 160, 160, 200)));
    bgScreen = new QPixmap(*fullScreen);
    QPainter p(bgScreen);
    p.drawPixmap(0, 0, pix);
}

void frmScreen::SaveScreenOther()
{
	QString fileName = QFileDialog::getSaveFileName(this, "保存图片", STRDATETIME, "JPEG Files (*.jpg)");

	if (fileName.length() > 0) {
		int x = screen->getLeftUp().x();
		int y = screen->getLeftUp().y();
		int w = screen->getRightDown().x() - x;
		int h = screen->getRightDown().y() - y;

		fullScreen->copy(x, y, w, h).save(fileName, "jpg");

		close();
	}
}

void frmScreen::SaveScreen()
{
	int x = screen->getLeftUp().x();
	int y = screen->getLeftUp().y();
	int w = screen->getRightDown().x() - x;
	int h = screen->getRightDown().y() - y;

	QString fileName = QString("%1/image_screen/screen_%2.jpg").arg(App::AppPath).arg(STRDATETIME);
	fullScreen->copy(x, y, w, h).save(fileName, "jpg");

	close();
}

void frmScreen::SaveFullScreen()
{
	QString fileName = QString("%1/image_screen/full_%2.jpg").arg(App::AppPath).arg(STRDATETIME);
	fullScreen->save(fileName, "jpg");

	close();
}

void frmScreen::mouseMoveEvent(QMouseEvent *e)
{
	if (screen->getStatus() == ScreenAPI::SELECT) {
		screen->setEnd(e->pos());
	} else if (screen->getStatus() == ScreenAPI::MOV) {
		QPoint p(e->x() - movPos.x(), e->y() - movPos.y());
		screen->move(p);
		movPos = e->pos();
	}

	update();
}

void frmScreen::mousePressEvent(QMouseEvent *e)
{
	int status = screen->getStatus();

	if (status == ScreenAPI::SELECT) {
		screen->setStart(e->pos());
	} else if (status == ScreenAPI::MOV) {
		if (screen->isInArea(e->pos()) == false) {
			screen->setStart(e->pos());
			screen->setStatus(ScreenAPI::SELECT);
		} else {
			movPos = e->pos();
			this->setCursor(Qt::SizeAllCursor);
		}
	}

	update();
}

void frmScreen::mouseReleaseEvent(QMouseEvent *)
{
	if (screen->getStatus() == ScreenAPI::SELECT) {
		screen->setStatus(ScreenAPI::MOV);
	} else if (screen->getStatus() == ScreenAPI::MOV) {
		this->setCursor(Qt::ArrowCursor);
	}
}

void frmScreen::contextMenuEvent(QContextMenuEvent *)
{
	this->setCursor(Qt::ArrowCursor);
	menu->exec(cursor().pos());
}
