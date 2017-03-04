#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "imageview.h"
#include "api/myhelper.h"

ImageNum::ImageNum(QWidget *parent): QWidget(parent)
{
    totalNum = 0;
    value = 0;

#ifdef __arm__
    setFixedSize(185, 35);
#else
    setFixedSize(150, 30);
#endif

    setWindowFlags(Qt::FramelessWindowHint);
}

void ImageNum::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
    drawBg(&painter);
    drawText(&painter);
}

void ImageNum::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    QLinearGradient BgGradient(QPoint(0, 0), QPoint(0, height()));
    BgGradient.setColorAt(0.0, NUM_BG_START_COLOR);
    BgGradient.setColorAt(1.0, NUM_BG_END_COLOR);

    painter->setBrush(BgGradient);
    painter->drawRoundedRect(rect(), 2, 2);
    painter->restore();
}

void ImageNum::drawText(QPainter *painter)
{
    painter->save();

    QString strText ;
    strText = QString("第 %1 张 / 共 %2 张").arg(value).arg(totalNum);
    painter->setPen(Qt::white);

    QFont TextFont;
    TextFont.setBold(true);
    painter->setFont(TextFont);
    painter->drawText(rect(), strText, Qt::AlignVCenter | Qt::AlignHCenter);

    painter->restore();
}

void ImageNum::setTotal(int totalNum)
{
    this->value = 1;
    this->totalNum = totalNum;
    update();
}

void ImageNum::setValue(int value)
{
    this->value = value;
    update();
}

ImageView::ImageView(QWidget *parent) :
    QWidget(parent)
{
    init();
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet(".QToolButton{color:#E7ECF0;background-color:rgba(0,0,0,0);border-style:none;}");
}

void ImageView::init()
{
    fade = false;
    fill = false;
    currImage = 0;
    totalNum = 0;
    currIndex = 0;

    num = new ImageNum(this);
    connect(this, SIGNAL(sig_setTotal(int)), num, SLOT(setTotal(int)));
    connect(this, SIGNAL(sig_setValue(int)), num, SLOT(setValue(int)));

    prevButton = new QToolButton(this);
    nextButton = new QToolButton(this);

    prevButton->setIcon(QIcon(":/image/image_prev.png"));
    nextButton->setIcon(QIcon(":/image/image_next.png"));

    prevButton->setIconSize(ICO_SIZE);
    nextButton->setIconSize(ICO_SIZE);

    prevButton->setFixedSize(BUTTON_SIZE);
    nextButton->setFixedSize(BUTTON_SIZE);

    prevButton->setAutoRaise(true);
    nextButton->setAutoRaise(true);

    connect(prevButton, SIGNAL(clicked()), this, SLOT(showPrevious()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(showNext()));

    imageOpacity = 1.0;
    timerOpacity = new QTimer(this);
    timerOpacity->setInterval(50);
    connect(timerOpacity, SIGNAL(timeout()), this, SLOT(DoImageFading()));
}

void ImageView::calcGeo()
{
    QPoint PrevMovePot(BUTTON_SPACE, height() - BOTTOM_SPACE - BUTTON_SIZE.height());
    prevButton->move(PrevMovePot);
    prevButton->show();

    QPoint NextMovePot(width() - BUTTON_SPACE - BUTTON_SIZE.width(), height() - BOTTOM_SPACE - BUTTON_SIZE.height());
    nextButton->move(NextMovePot);
    nextButton->show();

    qreal NumX = (qreal)width() / 2 - (qreal)num->width() / 2;
    qreal NumY = height() - BOTTOM_SPACE - BUTTON_SIZE.height() / 2 - num->height() / 2;
    QPointF NumMovePot(NumX, NumY);
    num->move(NumMovePot.toPoint());
}

void ImageView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform);
    drawBg(&painter);

    //如果图像张数大于0
    if (imageVec.count() > 0) {
        drawImage(&painter);
    }
}

void ImageView::keyPressEvent(QKeyEvent *keyEvent)
{
    switch (keyEvent->key()) {
    case Qt::Key_Left:
        showPrevious();
        break;
    case Qt::Key_Up:
        showPrevious();
        break;
    case Qt::Key_Down:
        showNext();
        break;
    case Qt::Key_Right:
        showNext();
        break;
    case Qt::Key_Space:
        showNext();
        break;
    default:
        QWidget::keyPressEvent(keyEvent);
    }
}

void ImageView::showEvent(QShowEvent *)
{
    calcGeo();
}

void ImageView::resizeEvent(QResizeEvent *)
{
    calcGeo();
}

void ImageView::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);

    QLinearGradient BgGradient(QPoint(0, 0), QPoint(0, height()));
    BgGradient.setColorAt(0.0, VIEW_BG_START_COLOR);
    BgGradient.setColorAt(1.0, VIEW_BG_END_COLOR);
    painter->setBrush(BgGradient);

    painter->drawRect(rect());
    painter->restore();
}

void ImageView::drawImage(QPainter *painter)
{
    painter->save();
    painter->setOpacity(imageOpacity);

    if (fill) {
        painter->drawImage(rect(), *currImage);
        painter->restore();
    } else {
        qreal imageWidth = currImage->width();
        qreal imageHeight = currImage->height();
        qreal ImageX = rect().center().x() - imageWidth / 2;
        qreal ImageY = rect().center().y() - imageHeight / 2;
        QPointF ImagePot(ImageX, ImageY);
        painter->drawImage(ImagePot, *currImage);
        painter->restore();
    }
}

void ImageView::DoImageFading()
{
    imageOpacity += 0.05;

    if(imageOpacity > 1.0) {
        imageOpacity = 1.0;
        timerOpacity->stop();
    }
    update();
}

void ImageView::setFill(bool fill)
{
    this->fill = fill;
    update();
}

void ImageView::setFade(bool fade)
{
    this->fade = fade;
    update();
}

void ImageView::load(const QString &strFolder)
{
    QDir imageFolder(strFolder);

    if (imageFolder.exists()) {
        QStringList filter = QStringList() << "*.png" << "*.jpg" << "*.gif" << "*.jpeg" << "*.bmp" ;
        QStringList ImageList = imageFolder.entryList(filter);
        if (ImageList.count() > 0) {
            //清空原有图片,并将对象删除后置为0,否则内存一直在
            for (int i = 0; i < imageVec.count(); i++) {
                delete imageVec.at(i);
                imageVec[i] = 0;
            }
            imageVec.clear();

            QString strFullPath;
            foreach(const QString strImage, ImageList) {
                strFullPath = QString("%1/%2").arg(strFolder).arg(strImage);
                QImage *img = new QImage(strFullPath);
                imageVec.push_back(img);
            }

            if(imageVec.size() > 0) {
                currImage = imageVec.at(0);
                currIndex = 0;
            }

            emit sig_setTotal(ImageList.count());
            timerOpacity->start();
        } else {
            currImage = 0;
            totalNum = 0;
            currIndex = 0;
            update();
        }
    }
}

void ImageView::showNext()
{
    if(imageVec.isEmpty()) {
        return ;
    }

    currIndex++;
    if(currIndex >= imageVec.count()) {
        currIndex = imageVec.count() - 1;
        return;
    }

    currImage = imageVec.at(currIndex);
    emit sig_setValue(currIndex + 1);

    if (fade) {
        imageOpacity = 0.0;
        timerOpacity->start();
    } else {
        update();
    }
}

void ImageView::showPrevious()
{
    if(imageVec.isEmpty()) {
        return ;
    }

    currIndex--;
    if(currIndex < 0) {
        currIndex = 0;
        return;
    }

    currImage = imageVec.at(currIndex);
    emit sig_setValue(currIndex + 1);

    if (fade) {
        imageOpacity = 0.0;
        timerOpacity->start();
    } else {
        update();
    }
}
