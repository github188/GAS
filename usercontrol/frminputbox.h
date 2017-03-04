#ifndef FRMINPUTBOX_H
#define FRMINPUTBOX_H

#include <QDialog>
#include <QMutex>

namespace Ui
{
class frmInputBox;
}

class frmInputBox : public QDialog
{
	Q_OBJECT

public:
	explicit frmInputBox(QWidget *parent = 0);
	~frmInputBox();

	static frmInputBox *Instance()
	{
		static QMutex mutex;

		if (!_instance) {
			QMutexLocker locker(&mutex);

			if (!_instance) {
				_instance = new frmInputBox;
			}
		}

		return _instance;
	}

    void SetParameter(QString title, int type = 0, int closeSec = 0, QString defaultValue = QString(), bool pwd = false);

	QString GetValue()const
	{
		return value;
	}

protected:
    void closeEvent(QCloseEvent *);

private slots:
	void on_btnOk_clicked();

private:
	Ui::frmInputBox *ui;
	static frmInputBox *_instance;
	QString value;

    int closeSec;
    int currentSec;

private slots:
    void InitStyle();
    void InitForm();
    void CheckSec();

};

#endif // FRMINPUTBOX_H
