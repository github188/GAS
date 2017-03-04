#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frminputnew.h"
#include "ui_frminputnew.h"

frmInputNew *frmInputNew::_self = 0;
frmInputNew::frmInputNew(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmInputNew)
{
    ui->setupUi(this);
    this->initProperty();
    this->initForm();
}

frmInputNew::~frmInputNew()
{
    delete ui;
}

void frmInputNew::init(QString position, QString style, int btnFontSize, int labFontSize, int frmWidth, int frmHeight, int icoWidth, int icoHeight, int spacing, int topHeight)
{
    this->currentPosition = position;
    this->currentStyle = style;
    this->btnFontSize = btnFontSize;
    this->labFontSize = labFontSize;
    this->frmWidth = frmWidth;
    this->frmHeight = frmHeight;
    this->icoWidth = icoWidth;
    this->icoHeight = icoHeight;
    this->spacing = spacing;
    this->topHeight = topHeight;
    this->changeSize();
    this->changeStyle();
}

void frmInputNew::init(QString position)
{
    this->currentPosition = position;
}

void frmInputNew::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void frmInputNew::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void frmInputNew::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void frmInputNew::initForm()
{
    this->mousePressed = false;
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

    frmWidth = 600;
    frmHeight = 200;
    icoWidth = 20;
    icoHeight = 20;
    spacing = 6;
    topHeight = 40;

    QDesktopWidget desk;
    deskWidth = desk.availableGeometry().width();
    deskHeight = desk.availableGeometry().height();

    QSqlDatabase dbConn;
    dbConn = QSqlDatabase::addDatabase("QSQLITE", "py");
    dbConn.setDatabaseName(qApp->applicationDirPath() + "/db/py.db");
    dbConn.open();

    isFirst = true;
    isPress = false;
    timerPress = new QTimer(this);
    connect(timerPress, SIGNAL(timeout()), this, SLOT(reClicked()));

    currentWidget = 0;
    currentLineEdit = 0;
    currentTextEdit = 0;
    currentPlain = 0;
    currentBrowser = 0;
    currentEditType = "";

    //如果需要更改输入法面板的显示位置,改变currentPosition这个变量即可
    //control--显示在对应输入框的正下方 bottom--填充显示在底部  center--窗体居中显示
    currentPosition = "";

    //如果需要更改输入法面板的样式,改变currentStyle这个变量即可
    //blue--淡蓝色   black--黑色  brown--灰黑色  gray--灰色  silvery--银色
    currentStyle = "";

    upper = false;

    //输入法面板字体大小,如果需要更改面板字体大小,该这里即可
    btnFontSize = 10;
    labFontSize = 10;

    //如果需要更改输入法初始模式,改变currentType这个变量即可
    //english--英文模式  chinese--中文模式  handwrite--手写模式  char--特殊字符模式
    currentType = "english";
    changeType(currentType);

    QList<QPushButton *> btn = this->findChildren<QPushButton *>();

    foreach (QPushButton *b, btn) {
        connect(b, SIGNAL(clicked()), this, SLOT(btnClicked()));
    }

    //绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(focusChanged(QWidget *, QWidget *)));
    //绑定按键事件过滤器
    qApp->installEventFilter(this);
}

void frmInputNew::initProperty()
{
    ui->btnDot->setProperty("btnOther", true);
    ui->btnSpace->setProperty("btnOther", true);
    ui->btnEnter->setProperty("btnOther", true);
    ui->btnDelete->setProperty("btnOther", true);
    ui->btnUpper->setProperty("btnOther", true);

    ui->btn0->setProperty("btnNum", true);
    ui->btn1->setProperty("btnNum", true);
    ui->btn2->setProperty("btnNum", true);
    ui->btn3->setProperty("btnNum", true);
    ui->btn4->setProperty("btnNum", true);
    ui->btn5->setProperty("btnNum", true);
    ui->btn6->setProperty("btnNum", true);
    ui->btn7->setProperty("btnNum", true);
    ui->btn8->setProperty("btnNum", true);
    ui->btn9->setProperty("btnNum", true);

    ui->btna->setProperty("btnLetter", true);
    ui->btnb->setProperty("btnLetter", true);
    ui->btnc->setProperty("btnLetter", true);
    ui->btnd->setProperty("btnLetter", true);
    ui->btne->setProperty("btnLetter", true);
    ui->btnf->setProperty("btnLetter", true);
    ui->btng->setProperty("btnLetter", true);
    ui->btnh->setProperty("btnLetter", true);
    ui->btni->setProperty("btnLetter", true);
    ui->btnj->setProperty("btnLetter", true);
    ui->btnk->setProperty("btnLetter", true);
    ui->btnl->setProperty("btnLetter", true);
    ui->btnm->setProperty("btnLetter", true);
    ui->btnn->setProperty("btnLetter", true);
    ui->btno->setProperty("btnLetter", true);
    ui->btnp->setProperty("btnLetter", true);
    ui->btnq->setProperty("btnLetter", true);
    ui->btnr->setProperty("btnLetter", true);
    ui->btns->setProperty("btnLetter", true);
    ui->btnt->setProperty("btnLetter", true);
    ui->btnu->setProperty("btnLetter", true);
    ui->btnv->setProperty("btnLetter", true);
    ui->btnw->setProperty("btnLetter", true);
    ui->btnx->setProperty("btnLetter", true);
    ui->btny->setProperty("btnLetter", true);
    ui->btnz->setProperty("btnLetter", true);

    labCh.append(ui->labCh0);
    labCh.append(ui->labCh1);
    labCh.append(ui->labCh2);
    labCh.append(ui->labCh3);
    labCh.append(ui->labCh4);
    labCh.append(ui->labCh5);
    labCh.append(ui->labCh6);
    labCh.append(ui->labCh7);
    labCh.append(ui->labCh8);
    labCh.append(ui->labCh9);

    for (int i = 0; i < 10; i++) {
        labCh.at(i)->installEventFilter(this);
    }
}

void frmInputNew::showPanel()
{
    this->setVisible(true);
    int width = ui->btna->width();
    width = width > 45 ? width : 45;
    ui->btnPre->setFixedWidth(width);
    ui->btnNext->setFixedWidth(width);
}

//事件过滤器,用于识别鼠标单击汉字标签处获取对应汉字
bool frmInputNew::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            if (obj == ui->labCh0) {
                setChinese(0);
            } else if (obj == ui->labCh1) {
                setChinese(1);
            } else if (obj == ui->labCh2) {
                setChinese(2);
            } else if (obj == ui->labCh3) {
                setChinese(3);
            } else if (obj == ui->labCh4) {
                setChinese(4);
            } else if (obj == ui->labCh5) {
                setChinese(5);
            } else if (obj == ui->labCh6) {
                setChinese(6);
            } else if (obj == ui->labCh7) {
                setChinese(7);
            } else if (obj == ui->labCh8) {
                setChinese(8);
            } else if (obj == ui->labCh9) {
                setChinese(9);
            } else if (currentEditType != "" && obj != ui->btnClose) {
                QString objName = obj->objectName();

                if (obj->parent() != 0x0 && !obj->property("noinput").toBool() && objName != "frmMainWindow"
                        && objName != "frmInputWindow" && objName != "qt_edit_menu" && objName != "labPY") {
                    if (obj->inherits("QGroupBox") || obj->inherits("QFrame") || obj->inherits("QMenu")) {
                        this->hide();
                    } else {
                        showPanel();
                    }
                }
            }

            btnPress = (QPushButton *)obj;

            if (checkPress()) {
                isPress = true;
                timerPress->start(500);
            }

            return false;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        btnPress = (QPushButton *)obj;

        if (checkPress()) {
            isPress = false;
            timerPress->stop();
        }

        return false;
    } else if (event->type() == QEvent::KeyPress) {
        //如果输入法窗体不可见,则不需要处理
        if (!isVisible()) {
            return QWidget::eventFilter(obj, event);
        }

        QString labText = ui->labPY->text();
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        //Shift切换输入法模式,esc键关闭输入法面板,空格取第一个汉字,退格键删除
        //中文模式下回车键取拼音,中文模式下当没有拼音时可以输入空格
        if (keyEvent->key() == Qt::Key_Space) {
            if (labText != "") {
                setChinese(0);
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (labText != "") {
                insertValue(labText);
                ui->labPY->setText("");
                selectChinese();
            } else {
                this->setVisible(false);
            }

            return true;
        } else if (keyEvent->key() == Qt::Key_Shift) {
            ui->btnType->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Escape) {
            ui->btnClose->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Backspace) {
            ui->btnDelete->click();
            return true;
        } else if (keyEvent->text() == "+" || keyEvent->text() == "=") {
            if (labText != "") {
                ui->btnNext->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->text() == "-" || keyEvent->text() == "_") {
            if (labText != "") {
                ui->btnPre->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_CapsLock) {
            ui->btnUpper->click();
            return true;
        } else {
            if (currentEditType == "QWidget") {
                return false;
            }

            QString key;

            if (currentType == "chinese") {
                key = keyEvent->text();
            } else if (currentType == "english") {
                if (upper) {
                    key = keyEvent->text().toUpper();
                } else {
                    key = keyEvent->text().toLower();
                }
            } else if (currentType == "handwrite") {
            }

            if (!key.isEmpty()) {
                QList<QPushButton *> btn = this->findChildren<QPushButton *>();

                foreach (QPushButton *b, btn) {
                    QString text = b->text();

                    if (!text.isEmpty() && text == key) {
                        b->click();
                        return true;
                    }
                }
            }
        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

bool frmInputNew::checkPress()
{
    //只有属于输入法键盘的合法按钮才继续处理
    bool num_ok = btnPress->property("btnNum").toBool();
    bool other_ok = btnPress->property("btnOther").toBool();
    bool letter_ok = btnPress->property("btnLetter").toBool();

    if (num_ok || other_ok || letter_ok) {
        return true;
    }

    return false;
}

void frmInputNew::reClicked()
{
    if (isPress) {
        timerPress->setInterval(30);
        btnPress->click();
    }
}

void frmInputNew::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    //qDebug() << "oldWidget:" << oldWidget << " nowWidget:" << nowWidget;
    if (nowWidget != 0 && !this->isAncestorOf(nowWidget)) {
        //在Qt5和linux系统中(嵌入式linux除外),当输入法面板关闭时,焦点会变成无,然后焦点会再次移到焦点控件处
        //这样导致输入法面板的关闭按钮不起作用,关闭后马上有控件获取焦点又显示.
        //为此,增加判断,当焦点是从有对象转为无对象再转为有对象时不要显示.
        //这里又要多一个判断,万一首个窗体的第一个焦点就是落在可输入的对象中,则要过滤掉
#ifndef __arm__
        if (oldWidget == 0x0 && !isFirst) {
            QTimer::singleShot(0, this, SLOT(hide()));
            return;
        }

#endif

        //如果对应属性noinput为真则不显示
        if (nowWidget->property("noinput").toBool()) {
            QTimer::singleShot(0, this, SLOT(hide()));
            return;
        }

        isFirst = false;

        if (nowWidget->inherits("QLineEdit")) {
            currentLineEdit = (QLineEdit *)nowWidget;
            currentEditType = "QLineEdit";
            showPanel();
        } else if (nowWidget->inherits("QTextEdit")) {
            currentTextEdit = (QTextEdit *)nowWidget;
            currentEditType = "QTextEdit";
            showPanel();
        } else if (nowWidget->inherits("QPlainTextEdit")) {
            currentPlain = (QPlainTextEdit *)nowWidget;
            currentEditType = "QPlainTextEdit";
            showPanel();
        } else if (nowWidget->inherits("QTextBrowser")) {
            currentBrowser = (QTextBrowser *)nowWidget;
            currentEditType = "QTextBrowser";
            showPanel();
        } else if (nowWidget->inherits("QComboBox")) {
            QComboBox *cbox = (QComboBox *)nowWidget;

            //只有当下拉选择框处于编辑模式才可以输入
            if (cbox->isEditable()) {
                currentLineEdit = cbox->lineEdit() ;
                currentEditType = "QLineEdit";
                showPanel();
            }
        } else if (nowWidget->inherits("QSpinBox") ||
                   nowWidget->inherits("QDoubleSpinBox") ||
                   nowWidget->inherits("QDateEdit") ||
                   nowWidget->inherits("QTimeEdit") ||
                   nowWidget->inherits("QDateTimeEdit")) {
            currentWidget = nowWidget;
            currentEditType = "QWidget";
        } else {
            //需要将输入法切换到最初的原始状态--小写,同时将之前的对象指针置为零
            currentWidget = 0;
            currentLineEdit = 0;
            currentTextEdit = 0;
            currentPlain = 0;
            currentBrowser = 0;
            currentEditType = "";
            currentType = "english";
            changeType(currentType);
            this->setVisible(false);
        }

        //根据用户选择的输入法位置设置-居中显示-底部填充-显示在输入框正下方
        if (currentPosition == "center") {
            QPoint pos = QPoint(deskWidth / 2 - frmWidth / 2, deskHeight / 2 - frmHeight / 2);
            this->setGeometry(pos.x(), pos.y(), frmWidth, frmHeight);
        } else if (currentPosition == "bottom") {
            this->setGeometry(0, deskHeight - frmHeight, deskWidth, frmHeight);
        } else if (currentPosition == "control") {
            QRect rect = nowWidget->rect();
            QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
            pos = nowWidget->mapToGlobal(pos);

            int x = pos.x();
            int y = pos.y();

            if (pos.x() + frmWidth > deskWidth) {
                x = deskWidth - frmWidth;
            }

            if (pos.y() + frmHeight > deskHeight) {
                y = pos.y() - frmHeight - rect.height() - 2;
            }

            this->setGeometry(x, y, frmWidth, frmHeight);
        }
    }
}

void frmInputNew::changeType(QString type)
{
    if (type == "english") {
        ui->btnType->setText("En");
        ui->widgetTop->setEnabled(false);
        changeChar(false);
    } else if (type == "chinese") {
        ui->btnType->setText("中");
        ui->widgetTop->setEnabled(true);
        changeChar(false);
    } else if (type == "handwrite") {
        ui->btnType->setText("手");
        ui->widgetTop->setEnabled(false);
        changeChar(false);
    } else if (type == "char") {
        ui->btnType->setText("符");
        ui->widgetTop->setEnabled(false);
        changeChar(true);
    }

    //每次切换到模式,都要执行清空之前中文模式下的信息
    clearChinese();
    ui->labPY->setText("");
}

void frmInputNew::changeLetter(bool isUpper)
{
    QList<QPushButton *> btn = this->findChildren<QPushButton *>();

    foreach (QPushButton *b, btn) {
        if (b->property("btnLetter").toBool()) {
            if (isUpper) {
                b->setText(b->text().toUpper());
            } else {
                b->setText(b->text().toLower());
            }
        }
    }
}

void frmInputNew::changeChar(bool isChar)
{
    if (isChar) {
        ui->btn1->setText("!");
        ui->btn2->setText("@");
        ui->btn3->setText("#");
        ui->btn4->setText("$");
        ui->btn5->setText("%");
        ui->btn6->setText("^");
        ui->btn7->setText("&&");
        ui->btn8->setText("*");
        ui->btn9->setText("(");
        ui->btn0->setText(")");

        ui->btnq->setText("~");
        ui->btnw->setText("`");
        ui->btne->setText("-");
        ui->btnr->setText("_");
        ui->btnt->setText("+");
        ui->btny->setText("=");
        ui->btnu->setText("{");
        ui->btni->setText("}");
        ui->btno->setText("[");
        ui->btnp->setText("]");

        ui->btna->setText("|");
        ui->btns->setText("\\");
        ui->btnd->setText(":");
        ui->btnf->setText(";");
        ui->btng->setText("\"");
        ui->btnh->setText("'");
        ui->btnj->setText("<");
        ui->btnk->setText(">");
        ui->btnl->setText("?");

        ui->btnz->setText(",");
        ui->btnx->setText("/");
        ui->btnc->setText("：");
        ui->btnv->setText("，");
        ui->btnb->setText("。");
        ui->btnn->setText("？");
        ui->btnm->setText("；");
    } else {
        ui->btn1->setText("1");
        ui->btn2->setText("2");
        ui->btn3->setText("3");
        ui->btn4->setText("4");
        ui->btn5->setText("5");
        ui->btn6->setText("6");
        ui->btn7->setText("7");
        ui->btn8->setText("8");
        ui->btn9->setText("9");
        ui->btn0->setText("0");

        ui->btnq->setText("q");
        ui->btnw->setText("w");
        ui->btne->setText("e");
        ui->btnr->setText("r");
        ui->btnt->setText("t");
        ui->btny->setText("y");
        ui->btnu->setText("u");
        ui->btni->setText("i");
        ui->btno->setText("o");
        ui->btnp->setText("p");

        ui->btna->setText("a");
        ui->btns->setText("s");
        ui->btnd->setText("d");
        ui->btnf->setText("f");
        ui->btng->setText("g");
        ui->btnh->setText("h");
        ui->btnj->setText("j");
        ui->btnk->setText("k");
        ui->btnl->setText("l");

        ui->btnz->setText("z");
        ui->btnx->setText("x");
        ui->btnc->setText("c");
        ui->btnv->setText("v");
        ui->btnb->setText("b");
        ui->btnn->setText("n");
        ui->btnm->setText("m");
    }
}

void frmInputNew::selectChinese()
{
    clearChinese();
    QSqlQuery query(QSqlDatabase::database("py"));
    QString currentPY = ui->labPY->text();
    QString sql = QString("select word from pinyin where pinyin='%1'").arg(currentPY);
    query.exec(sql);

    //逐个将查询到的字词加入汉字队列
    while (query.next()) {
        QString result = query.value(0).toString();
        QStringList text = result.split(" ");

        foreach (QString txt, text) {
            if (txt.length() > 0) {
                allPY.append(txt);
                currentPY_count++;
            }
        }
    }

    showChinese();
}

void frmInputNew::showChinese()
{
    //每个版面最多显示10个汉字
    int count = 0;
    currentPY.clear();

    for (int i = 0; i < 10; i++) {
        labCh.at(i)->setText("");
    }

    for (int i = currentPY_index; i < currentPY_count; i++) {
        if (count == 10) {
            break;
        }

        QString txt = QString("%1.%2").arg(count).arg(allPY[currentPY_index]);
        currentPY.append(allPY[currentPY_index]);
        labCh.at(count)->setText(txt);
        count++;
        currentPY_index++;
    }

    //qDebug() << "currentPY_index:" << currentPY_index << "currentPY_count:" << currentPY_count;
}

void frmInputNew::btnClicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (currentEditType == "") {
        return;
    }

    QString labText = ui->labPY->text();
    QPushButton *btn = (QPushButton *)sender();
    QString objectName = btn->objectName();

    if (objectName == "btnType") {
        if (currentType == "english") {
            currentType = "chinese";
        } else if (currentType == "chinese") {
            currentType = "handwrite";
        } else if (currentType == "handwrite") {
            currentType = "char";
        } else if (currentType == "char") {
            currentType = "english";
        }

        changeType(currentType);

        upper = false;
        changeLetter(upper);
    } else if (objectName == "btnUpper") {
        //大小写模式都要切换到英文状态
        currentType = "english";
        changeType(currentType);

        upper = !upper;
        changeLetter(upper);
    } else if (objectName == "btnDelete") {
        //如果当前是中文模式,则删除对应拼音,删除完拼音之后再删除对应文本输入框的内容
        if (currentType == "chinese") {
            QString txt = labText;
            int len = txt.length();

            if (len > 0) {
                ui->labPY->setText(txt.left(len - 1));
                selectChinese();
            } else {
                deleteValue();
            }
        } else {
            deleteValue();
        }
    } else if (objectName == "btnPre") {
        if (currentPY_index >= 20) {
            //每次最多显示10个汉字,所以每次向前的时候索引要减20
            if (currentPY_index % 10 == 0) {
                currentPY_index -= 20;
            } else {
                currentPY_index = currentPY_count - (currentPY_count % 10) - 10;
            }
        } else {
            currentPY_index = 0;
        }

        showChinese();
    } else if (objectName == "btnNext") {
        if (currentPY_index < currentPY_count - 1) {
            showChinese();
        }
    } else if (objectName == "btnClose") {
        this->setVisible(false);
        clearChinese();
        ui->labPY->setText("");
    } else if (objectName == "btnSpace") {
        if (currentType == "chinese" && labText != "") {
            setChinese(0);
        } else {
            insertValue(" ");
        }
    } else if (objectName == "btnEnter") {
        QWidget *widget;
        widget = 0;

        if (currentEditType == "QLineEdit") {
            widget = currentLineEdit;
        } else if (currentEditType == "QTextEdit") {
            widget = currentTextEdit;
        } else if (currentEditType == "QPlainTextEdit") {
            widget = currentPlain;
        } else if (currentEditType == "QTextBrowser") {
            widget = currentBrowser;
        } else if (currentEditType == "QWidget") {
            widget = currentWidget;
        }

        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QApplication::sendEvent(widget, &keyPress);
        this->setVisible(false);
    } else {
        QString value = btn->text();

        //如果是&按钮，因为对应&被过滤,所以真实的text为去除前面一个&字符
        if (value == "&&") {
            value = "&";
        }

        //当前不是中文模式,则单击按钮对应text为传递参数
        if (currentType != "chinese") {
            insertValue(value);
        } else {
            //中文模式下,不允许输入特殊字符,单击对应数字按键取得当前索引的汉字
            if (btn->property("btnOther").toBool()) {
                if (labText.length() == 0) {
                    insertValue(value);
                }
            } else if (btn->property("btnNum").toBool()) {
                if (labText.length() == 0) {
                    insertValue(value);
                } else if (objectName == "btn0") {
                    setChinese(0);
                } else if (objectName == "btn1") {
                    setChinese(1);
                } else if (objectName == "btn2") {
                    setChinese(2);
                } else if (objectName == "btn3") {
                    setChinese(3);
                } else if (objectName == "btn4") {
                    setChinese(4);
                } else if (objectName == "btn5") {
                    setChinese(5);
                } else if (objectName == "btn6") {
                    setChinese(6);
                } else if (objectName == "btn7") {
                    setChinese(7);
                } else if (objectName == "btn8") {
                    setChinese(8);
                } else if (objectName == "btn9") {
                    setChinese(9);
                }
            } else if (btn->property("btnLetter").toBool()) {
                ui->labPY->setText(labText + value);
                selectChinese();
            }
        }
    }
}

void frmInputNew::insertValue(QString value)
{
    if (currentEditType == "QLineEdit") {
        currentLineEdit->insert(value);
    } else if (currentEditType == "QTextEdit") {
        currentTextEdit->insertPlainText(value);
    } else if (currentEditType == "QPlainTextEdit") {
        currentPlain->insertPlainText(value);
    } else if (currentEditType == "QTextBrowser") {
        currentBrowser->insertPlainText(value);
    } else if (currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(value));
        QApplication::sendEvent(currentWidget, &keyPress);
    }
}

void frmInputNew::deleteValue()
{
    if (currentEditType == "QLineEdit") {
        currentLineEdit->backspace();
    } else if (currentEditType == "QTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentTextEdit->textCursor();

        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QPlainTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentPlain->textCursor();

        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QTextBrowser") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentBrowser->textCursor();

        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier, QString());
        QApplication::sendEvent(currentWidget, &keyPress);
    }
}

void frmInputNew::setChinese(int index)
{
    int count = currentPY.count();

    if (count > index) {
        insertValue(currentPY[index]);
        //添加完一个汉字后,清空当前汉字信息,等待重新输入
        clearChinese();
        ui->labPY->setText("");
    }
}

void frmInputNew::clearChinese()
{
    //清空汉字,重置索引
    for (int i = 0; i < 10; i++) {
        labCh.at(i)->setText("");
    }

    allPY.clear();
    currentPY.clear();
    currentPY_index = 0;
    currentPY_count = 0;
}

void frmInputNew::changeSize()
{
    QFont btnFont(font().family(), btnFontSize);
    QFont labFont(font().family(), labFontSize);

    QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();

    foreach (QPushButton *btn, btns) {
        btn->setFont(btnFont);
    }

    QList<QLabel *> labs = ui->widgetTop->findChildren<QLabel *>();

    foreach (QLabel *lab, labs) {
        lab->setFont(labFont);
    }

    ui->btnPre->setFont(btnFont);
    ui->btnNext->setFont(btnFont);

    this->setMinimumSize(frmWidth, frmHeight);

    QSize icoSize(icoWidth, icoHeight);
    QSize icoSize2(icoWidth - 5, icoHeight - 5);
    ui->btnDelete->setIconSize(icoSize);
    ui->btnSpace->setIconSize(icoSize);
    ui->btnClose->setIconSize(icoSize);
    ui->btnType->setIconSize(icoSize);
    ui->btnEnter->setIconSize(icoSize2);
    ui->btnUpper->setIconSize(icoSize2);

    ui->widgetMain->layout()->setSpacing(spacing);
    ui->widgetPreNext->layout()->setSpacing(spacing);
    ui->widgetTop->setFixedHeight(topHeight);
}

void frmInputNew::changeStyle()
{
    QString icoType = "white";

    if (currentStyle == "blue") {
        changeStyle("#377FC9", "#F3F3F3", "#4189D3", "#386487", "#F3F3F3", "#386487", "#F3F3F3");
    } else if (currentStyle == "gray") {
        changeStyle("#989898", "#FFFFFF", "#A2A2A2", "#333333", "#F3F3F3", "#2E2E2E", "#F3F3F3");
    } else if (currentStyle == "black") {
        changeStyle("#191919", "#F3F3F3", "#313131", "#4189D3", "#F3F3F3", "#F95717", "#F3F3F3");
    } else if (currentStyle == "brown") {
        changeStyle("#667481", "#F3F3F3", "#566373", "#4189D3", "#F3F3F3", "#4189D3", "#F3F3F3");
    } else if (currentStyle == "silvery") {
        changeStyle("#868690", "#000002", "#C3C2C7", "#393939", "#EFEFEF", "#393939", "#EFEFEF");
        icoType = "black";
    }

    QString ico_upper = QString(":/image/upper_%1.png").arg(icoType);
    QString ico_delete = QString(":/image/delete_%1.png").arg(icoType);
    QString ico_enter = QString(":/image/enter_%1.png").arg(icoType);
    QString ico_close = QString(":/image/close_%1.png").arg(icoType);
    QString ico_space = QString(":/image/space_%1.png").arg(icoType);

    ui->btnUpper->setIcon(QIcon(ico_upper));
    ui->btnClose->setIcon(QIcon(ico_close));
    ui->btnDelete->setIcon(QIcon(ico_delete));
    ui->btnSpace->setIcon(QIcon(ico_space));
    ui->btnEnter->setIcon(QIcon(ico_enter));
}

void frmInputNew::changeStyle(QString mainBackgroundColor, QString mainTextColor, QString btnBackgroundColor,
                              QString btnHoveColor, QString btnHoveTextColor, QString labHoveColor, QString labHoveTextColor)
{
    QStringList qss;
    qss.append(QString("QWidget#widgetMain,QWidget#widgetTop{background-color:%1;}").arg(mainBackgroundColor));
    qss.append(QString("QPushButton{border-width:0px;border-radius:3px;color:%1;}").arg(mainTextColor));
    qss.append(QString("QPushButton{background-color:%1;}").arg(btnBackgroundColor));
    qss.append(QString("QPushButton:pressed,QPushButton:hover{background-color:%1;color:%2;}").arg(btnHoveColor).arg(btnHoveTextColor));
    qss.append(QString("QLabel{border-width:0px;border-radius:3px;color:%1;background:none;}").arg(mainTextColor));
    qss.append(QString("QLabel:pressed,QLabel:hover{background-color:%1;color:%2;}").arg(labHoveColor).arg(labHoveTextColor));
    this->setStyleSheet(qss.join(""));
}
