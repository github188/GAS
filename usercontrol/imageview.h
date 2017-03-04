#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#ifndef IMAGENUM_H
#define IMAGENUM_H

#define BOTTOM_SPACE        10
#define BUTTON_SPACE        10
#define ICO_SIZE            QSize(30,30)
#define BUTTON_SIZE         QSize(35,35)

#define NUM_BG_START_COLOR  QColor(46,132,243)
#define NUM_BG_END_COLOR    QColor(39,110,203)

#define VIEW_BG_START_COLOR QColor(56,56,56)
#define VIEW_BG_END_COLOR   QColor(52,52,52)

#include <QWidget>

class QToolButton;

class ImageNum : public QWidget
{
    Q_OBJECT
public:
    ImageNum(QWidget *parent = 0);

public slots:
    void setTotal(int totalNum);
    void setValue(int value);

protected:
    void paintEvent(QPaintEvent *);

private:
    void drawBg(QPainter *painter);
    void drawText(QPainter *painter);

private:
    int totalNum, value;

};

#endif // IMAGENUM_H

class ImageView : public QWidget
{
    Q_OBJECT
public:
    explicit ImageView(QWidget *parent = 0);

public slots:
    void setFill(bool fill);
    void setFade(bool fade);
    void load(const QString &strFolder);
    void showNext();
    void showPrevious();

protected:
    virtual void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *keyEvent);
    virtual void showEvent(QShowEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void drawBg(QPainter *painter);
    void drawImage(QPainter *painter);
    void init();
    void calcGeo();

private:
    bool fill;
    bool fade;
    int totalNum;
    int currIndex;

    QToolButton *prevButton;
    QToolButton *nextButton;

    QVector<QImage *> imageVec;
    QImage *currImage;

    ImageNum *num;

    qreal imageOpacity;
    QTimer *timerOpacity;

private slots:
    void DoImageFading();

signals:
    void sig_setValue(int value);
    void sig_setTotal(int total);

};

#endif // IMAGEVIEW_H
