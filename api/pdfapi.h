#ifndef PDFAPI_H
#define PDFAPI_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QPrinter>

class PdfAPI : public QObject
{
    Q_OBJECT
private:
    explicit PdfAPI(QObject *parent = 0);
    static PdfAPI *_instance;
    QStringList html;

public:
    static PdfAPI *Instance() {
        static QMutex mutex;
        if (!_instance) {
            QMutexLocker locker(&mutex);
            if (!_instance) {
                _instance = new PdfAPI;
            }
        }
        return _instance;
    }

    void SavePdf(QString fileName, QString title, QString subTitle,
                 QList<QString> columnNames, QList<int> columnWidths,
                 QStringList content, bool landscape, bool check,
                 QPrinter::PageSize pageSize = QPrinter::A4);

    void SavePdf(QString fileName, QString title,
                 QList<QString> columnNames, QList<int> columnWidths,
                 QStringList subTitle1, QStringList subTitle2,
                 QStringList content, bool landscape, bool check,
                 QPrinter::PageSize pageSize = QPrinter::A4);

};

#endif // PDFAPI_H
