#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmmessagebox.h"
#include "ui_frmmessagebox.h"
#include "api/myhelper.h"

frmMessageBox *frmMessageBox::_instance = 0;
frmMessageBox::frmMessageBox(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmMessageBox)
{
	ui->setupUi(this);
	this->InitStyle();
	this->InitForm();
	myHelper::FormInCenter(this);
}

frmMessageBox::~frmMessageBox()
{
	delete ui;
}

void frmMessageBox::InitStyle()
{
	if (App::UseStyle) {
		this->setProperty("Form", true);
		this->setProperty("CanMove", true);
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
		                     Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
		IconHelper::Instance()->SetIcoMain(ui->lab_Ico, App::FontSize + 2);
		IconHelper::Instance()->SetIcoClose(ui->btnMenu_Close);
		connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
	} else {
		this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
		this->resize(size().width(), size().height() - ui->widget_title->size().height());
		ui->widget_title->setVisible(false);
	}

#ifdef __arm__
	this->setFixedSize(300, 160);
#endif

	this->setWindowTitle(ui->lab_Title->text());
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void frmMessageBox::InitForm()
{
	closeSec = 0;
	currentSec = 0;

	QTimer *timer = new QTimer(this);
	timer->setInterval(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(CheckSec()));
	timer->start();
}

void frmMessageBox::CheckSec()
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

void frmMessageBox::SetMessage(QString msg, int type, int closeSec)
{
	this->closeSec = closeSec;
	this->currentSec = 0;
	ui->labTime->clear();
	this->setWindowTitle(ui->lab_Title->text());

	CheckSec();

	if (type == 0) {
		ui->labIcoMain->setStyleSheet("border-image: url(:/image/msg_info.png);");
		ui->btnCancel->setVisible(false);
		ui->lab_Title->setText("提示");
	} else if (type == 1) {
		ui->labIcoMain->setStyleSheet("border-image: url(:/image/msg_question.png);");
		ui->lab_Title->setText("询问");
	} else if (type == 2) {
		ui->labIcoMain->setStyleSheet("border-image: url(:/image/msg_error.png);");
		ui->btnCancel->setVisible(false);
		ui->lab_Title->setText("错误");
	}

	ui->labInfo->setText(msg);
}

void frmMessageBox::closeEvent(QCloseEvent *)
{
	closeSec = 0;
	currentSec = 0;
}

void frmMessageBox::on_btnOk_clicked()
{
	done(1);
	this->close();
}
