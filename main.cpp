#include "frmmain.h"
#include <QApplication>
#include "api/appinit.h"
#include "api/myhelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    a.setApplicationName(App::AppName);
    a.setApplicationVersion("1.0");
    a.setWindowIcon(QIcon(":/main.ico"));

    AppInit::Instance()->Start();

    frmMain w;
    w.show();

    return a.exec();
}
