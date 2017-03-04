#ifndef FRMSELECT_H
#define FRMSELECT_H

#include <QDialog>

namespace Ui
{
class frmSelect;
}

class frmSelect : public QDialog
{
	Q_OBJECT

public:
	explicit frmSelect(QWidget *parent = 0);
	~frmSelect();

	//获取当前选择的开始时间和结束时间
	QString SelectStartDateTime() const
	{
		return selectStartDateTime;
	}
	QString SelectEndDateTime() const
	{
		return selectEndDateTime;
	}

private slots:
	void on_btnOk_clicked();//按下确定按钮

private:
	Ui::frmSelect *ui;

	QString selectStartDateTime;//开始时间
	QString selectEndDateTime;  //结束时间
	void InitStyle();       //初始化无边框窗体

};

#endif // FRMSELECT_H
