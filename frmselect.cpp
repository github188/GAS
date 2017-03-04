#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "frmselect.h"
#include "ui_frmselect.h"
#include "api/myhelper.h"

frmSelect::frmSelect(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::frmSelect)
{
	ui->setupUi(this);
	this->InitStyle();
	myHelper::FormInCenter(this);
}

frmSelect::~frmSelect()
{
	delete ui;
}

void frmSelect::InitStyle()
{
	this->setWindowTitle(ui->lab_Title->text());
	this->setProperty("Form", true);
	this->setProperty("CanMove", true);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
	IconHelper::Instance()->SetIcoMain(ui->lab_Ico, App::FontSize + 2);
	IconHelper::Instance()->SetIcoClose(ui->btnMenu_Close);
	connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(close()));

	int width = 20;
#ifdef __arm__
	width = 25;
#endif
	QString qss = QString("QComboBox::drop-down,QDateEdit::drop-down,QTimeEdit::drop-down,QDateTimeEdit::drop-down{width:%1px;}").arg(width);
	this->setStyleSheet(qss);

	ui->dateStart->setDate(QDate::currentDate());
	ui->dateEnd->setDate(QDate::currentDate().addDays(1));

	ui->dateStart->calendarWidget()->setLocale(QLocale::Chinese);
	ui->dateEnd->calendarWidget()->setLocale(QLocale::Chinese);
}

void frmSelect::on_btnOk_clicked()
{
	if (ui->dateStart->dateTime() > ui->dateEnd->dateTime()) {
		myHelper::ShowMessageBoxErrorX("开始时间不能大于结束时间!");
		return;
	}

	this->selectStartDateTime = ui->dateStart->dateTime().toString("yyyy-MM-dd HH:mm:ss");
	this->selectEndDateTime = ui->dateEnd->dateTime().toString("yyyy-MM-dd HH:mm:ss");

	done(1);
	this->close();
}
