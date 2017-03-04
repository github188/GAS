#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frminputbox.h"
#include "ui_frminputbox.h"
#include "api/myhelper.h"

frmInputBox *frmInputBox::_instance = 0;
frmInputBox::frmInputBox(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmInputBox)
{
	ui->setupUi(this);
	this->InitStyle();
    this->InitForm();
	myHelper::FormInCenter(this);
}

frmInputBox::~frmInputBox()
{
	delete ui;
}

void frmInputBox::InitStyle()
{
	if (App::UseStyle) {
		this->setProperty("Form", true);
		this->setProperty("CanMove", true);
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
		                     Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
		IconHelper::Instance()->SetIcoMain(ui->lab_Ico, App::FontSize + 2);
		IconHelper::Instance()->SetIcoClose(ui->btnMenu_Close);
		connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
		this->setFixedSize(280, 150);
	} else {
		this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
		this->resize(size().width(), size().height() - ui->widget_title->size().height());
		ui->widget_title->setVisible(false);
		this->setFixedSize(280, 120);
	}

#ifdef __arm__
	this->setFixedSize(300, 160);
#endif

	this->setWindowTitle(ui->lab_Title->text());
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void frmInputBox::InitForm()
{
    closeSec = 0;
    currentSec = 0;

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(CheckSec()));
    timer->start();
}

void frmInputBox::CheckSec()
{
    if (closeSec == 0) {
        return;
    }

    if (currentSec < closeSec) {
        currentSec++;
    } else {
        this->close();
    }

    if (App::UseStyle) {
        QString str = QString("关闭倒计时 %1 s").arg(closeSec - currentSec + 1);
        ui->labTime->setText(str);
    } else {
        QString str = QString("%1  关闭倒计时 %2 s").arg(ui->lab_Title->text()).arg(closeSec - currentSec + 1);
        this->setWindowTitle(str);
    }
}

void frmInputBox::SetParameter(QString title, int type, int closeSec, QString defaultValue, bool pwd)
{
    this->closeSec = closeSec;
    this->currentSec = 0;
    ui->labTime->clear();
    CheckSec();

	ui->labInfo->setText(title);

	if (type == 0) {
		ui->cboxValue->setVisible(false);
		ui->txtValue->setPlaceholderText(defaultValue);

		if (pwd) {
			ui->txtValue->setEchoMode(QLineEdit::Password);
		}
	} else if (type == 1) {
		ui->txtValue->setVisible(false);
		ui->cboxValue->addItems(defaultValue.split("|"));
    }
}

void frmInputBox::closeEvent(QCloseEvent *)
{
    closeSec = 0;
    currentSec = 0;
}

void frmInputBox::on_btnOk_clicked()
{
	if (ui->txtValue->isVisible()) {
		value = ui->txtValue->text();
	} else if (ui->cboxValue->isVisible()) {
		value = ui->cboxValue->currentText();
	}

	done(1);
	this->close();
}
