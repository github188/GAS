#ifndef FRMMESSAGEBOX_H
#define FRMMESSAGEBOX_H

#include <QDialog>
#include <QMutex>

namespace Ui
{
class frmMessageBox;
}

class frmMessageBox : public QDialog
{
	Q_OBJECT

public:
	explicit frmMessageBox(QWidget *parent = 0);
	~frmMessageBox();

	static frmMessageBox *Instance()
	{
		static QMutex mutex;

		if (!_instance) {
			QMutexLocker locker(&mutex);

			if (!_instance) {
				_instance = new frmMessageBox;
			}
		}

		return _instance;
	}

    void SetMessage(QString msg, int type, int closeSec = 0);

protected:
    void closeEvent(QCloseEvent *);

private slots:
	void on_btnOk_clicked();

private:
	Ui::frmMessageBox *ui;
	static frmMessageBox *_instance;

    int closeSec;
    int currentSec;

private slots:
	void InitStyle();
    void InitForm();
    void CheckSec();

};

#endif // FRMMESSAGEBOX_H
