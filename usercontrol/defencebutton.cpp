#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "defencebutton.h"
#include "api/myhelper.h"
#include "api/app.h"

DefenceButton::DefenceButton(QWidget *parent) : QWidget(parent)
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->defenceID = "";
    this->defenceName = "";
    this->subSystem = 0;
    this->defenceSound = "";
    this->defenceImage = "";
    this->defenceX = 0;
    this->defenceY = 0;
    this->defenceStatus = Status_GuZhang;

    currentRG = new QRadialGradient(0, 0, 80);
    currentRG->setColorAt(0, QColor(50, 50, 50));
    currentRG->setColorAt(0.6, QColor(30, 30, 30));
    currentRG->setColorAt(1, QColor(10, 10, 10));

    IsRed = false;
    timerAlarm = new QTimer(this);
    timerAlarm->setInterval(500);
    connect(timerAlarm, SIGNAL(timeout()), this, SLOT(BaoJingChange()));

    this->installEventFilter(this);//安装监听器,监听鼠标拖动
}

DefenceButton::DefenceButton(QWidget *parent, QString defenceID, QString defenceName,
                             int subSystem, QString defenceSound, QString defenceImage,
                             int defenceX, int defenceY, Status defenceStatus):
    QWidget(parent)
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
    this->defenceID = defenceID;
    this->defenceName = defenceName;
    this->subSystem = subSystem;
    this->defenceSound = defenceSound;
    this->defenceImage = defenceImage;
    this->defenceX = defenceX;
    this->defenceY = defenceY;
    this->defenceStatus = defenceStatus;

    currentRG = new QRadialGradient(0, 0, 80);
    currentRG->setColorAt(0, QColor(50, 50, 50));
    currentRG->setColorAt(0.6, QColor(30, 30, 30));
    currentRG->setColorAt(1, QColor(10, 10, 10));

    IsRed = false;
    timerAlarm = new QTimer(this);
    timerAlarm->setInterval(500);
    connect(timerAlarm, SIGNAL(timeout()), this, SLOT(BaoJingChange()));

    this->installEventFilter(this);//安装监听器,监听鼠标拖动
}

bool DefenceButton::eventFilter(QObject *, QEvent *evt)
{
    if (App::IsMove) {
        static QPoint lastPnt;
        static bool isHover = false;

        if (evt->type() == QEvent::MouseButtonPress) {
            QMouseEvent *e = static_cast<QMouseEvent *>(evt);
            if (this->rect().contains(e->pos()) && (e->button() == Qt::LeftButton)) {
                lastPnt = e->pos();
                isHover = true;
            }
        } else if (evt->type() == QEvent::MouseMove && isHover) {
            QMouseEvent *e = static_cast<QMouseEvent *>(evt);
            int dx = e->pos().x() - lastPnt.x();
            int dy = e->pos().y() - lastPnt.y();
            this->move(this->x() + dx, this->y() + dy);
        } else if (evt->type() == QEvent::MouseButtonRelease && isHover) {
            isHover = false;
        }
    }

    return false;
}

void DefenceButton::BaoJingChange()
{
    if (IsRed) {
        currentRG->setColorAt(0, QColor(50, 50, 50));
        currentRG->setColorAt(0.6, QColor(30, 30, 30));
        currentRG->setColorAt(1, QColor(10, 10, 10));
        IsRed = false;
    } else {
        currentRG->setColorAt(0, QColor(255, 0, 0));
        currentRG->setColorAt(0.6, QColor(230, 0, 0));
        currentRG->setColorAt(1, QColor(204, 0, 0));
        IsRed = true;
    }
    update();
}

void DefenceButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //设置坐标和缩放
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);  /* 使用反锯齿(如果可用) */
    painter.translate(width() / 2, height() / 2);   /* 坐标变换为窗体中心 */
    int side = qMin(width(), height());
    painter.scale(side / 200.0, side / 200.0);      /* 比例缩放 */
    painter.setPen(Qt::NoPen);

    //画外边框
    int radius = 100;
    QLinearGradient lg1(0, -radius, 0, radius);
    lg1.setColorAt(0, QColor(255, 255, 255));
    lg1.setColorAt(1, QColor(166, 166, 166));
    painter.setBrush(lg1);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);

    //画内边框
    radius -= 13;
    QLinearGradient lg2(0, -radius, 0, radius);
    lg2.setColorAt(0, QColor(166, 166, 166));
    lg2.setColorAt(1, QColor(255, 255, 255));
    painter.setBrush(lg2);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);

    //画内部指示颜色
    radius -= 4;
    painter.setBrush(*currentRG);
    painter.drawEllipse(-radius, -radius, radius << 1, radius << 1);
    painter.restore();

    //画防区号文字
    painter.save();
    this->setGeometry(this->geometry().left(), this->geometry().top(), 25, 25);
    QFont font("Arial", 8, QFont::Bold, false);
    painter.setFont(font);
    QPen pen(QColor(255, 255, 255));
    painter.setPen(pen);
    painter.drawText(rect(), Qt::AlignCenter, this->defenceID);
    painter.restore();
}

void DefenceButton::SetDefenceID(QString defenceID)
{
    if (this->defenceID != defenceID) {
        this->defenceID = defenceID;
        update();
    }
}

void DefenceButton::SetDefenceStatus(Status defenceStatus)
{
    if (this->defenceStatus != defenceStatus) {
        this->defenceStatus = defenceStatus;

        if (defenceStatus == Status_BuFang) {
            timerAlarm->stop();
            currentRG->setColorAt(0, QColor(68, 187, 92));
            currentRG->setColorAt(0.6, QColor(59, 162, 80));
            currentRG->setColorAt(1, QColor(54, 150, 74));
            update();
        } else if (defenceStatus == Status_CheFang) {
            timerAlarm->stop();
            currentRG->setColorAt(0, QColor(0, 0, 245));
            currentRG->setColorAt(0.6, QColor(0, 0, 210));
            currentRG->setColorAt(1, QColor(0, 0, 166));
            update();
        } else if (defenceStatus == Status_PangLu) {
            timerAlarm->stop();
            currentRG->setColorAt(0, QColor(180, 180, 180));
            currentRG->setColorAt(0.6, QColor(140, 140, 140));
            currentRG->setColorAt(1, QColor(129, 129, 129));
            update();
        } else if (defenceStatus == Status_DuanKai) {
            timerAlarm->stop();
            currentRG->setColorAt(0, QColor(145, 185, 235));
            currentRG->setColorAt(0.6, QColor(120, 170, 215));
            currentRG->setColorAt(1, QColor(102, 153, 204));
            update();
        } else if (defenceStatus == Status_GuZhang) {
            timerAlarm->stop();
            currentRG->setColorAt(0, QColor(50, 50, 50));
            currentRG->setColorAt(0.6, QColor(30, 30, 30));
            currentRG->setColorAt(1, QColor(10, 10, 10));
            update();
        } else if (defenceStatus == Status_BaoJing) {
            timerAlarm->start();
        }
    }
}
