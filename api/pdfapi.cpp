#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "pdfapi.h"
#include "myhelper.h"

PdfAPI *PdfAPI::_instance = 0;
PdfAPI::PdfAPI(QObject *parent) : QObject(parent)
{
}

void PdfAPI::SavePdf(QString fileName, QString title, QString subTitle, QList<QString> columnNames,
                     QList<int> columnWidths, QStringList content, bool landscape, bool check,
                     QPrinter::PageSize pageSize)
{
    //计算行数列数
    int columnCount = columnNames.count();
    int rowCount = content.count();

    //清空原有数据,确保每次都是新的数据
    html.clear();

    //表格开始
    html.append("<table border='0.5' cellspacing='0' cellpadding='3'>");

    //标题占一行,居中显示
    if (title.length() > 0) {
        html.append(QString("<tr><td align='center' style='vertical-align:middle;font-weight:bold;' colspan='%1'>")
                    .arg(columnCount));
        html.append(title);
        html.append("</td></tr>");
    }

    //副标题占一行,左对齐显示
    if (subTitle.length() > 0) {
        html.append(QString("<tr><td align='left' style='vertical-align:middle;' colspan='%1'>")
                    .arg(columnCount));
        html.append(subTitle);
        html.append("</td></tr>");
    }

    //循环写入字段名,字段名占一行,居中显示
    if (columnCount > 0) {
        html.append("<tr>");
        for (int i = 0; i < columnCount; i++) {
            html.append(QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>")
                        .arg(columnWidths.at(i)));
            html.append(columnNames.at(i));
            html.append("</td>");
        }
        html.append("</tr>");
    }

    //循环一行行构建数据
    for (int i = 0; i < rowCount; i++) {
        QStringList value = content.at(i).split(";");
        html.append("<tr>");

        if (check) {
            if (value.at(value.count() - 1) == "历史记录") {
                for (int j = 0; j < columnCount; j++) {
                    html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>"
                                        "<font color='red'>").arg(columnWidths.at(j)));
                    html.append(value.at(j));
                    html.append("</font></td>");
                }
            } else {
                for (int j = 0; j < columnCount; j++) {
                    html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                                .arg(columnWidths.at(j)));
                    html.append(value.at(j));
                    html.append("</td>");
                }
            }
        } else {
            for (int j = 0; j < columnCount; j++) {
                html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                            .arg(columnWidths.at(j)));
                html.append(value.at(j));
                html.append("</td>");
            }
        }

        html.append("</tr>");
    }

    html.append("</table>");

    //调用打印机打印PDF
    QPrinter printer;
    //设置纸张规格
    printer.setPageSize(pageSize);
    //设置横向纵向及页边距
    if (landscape) {
        printer.setOrientation(QPrinter::Landscape);
        printer.setPageMargins(10, 10, 10, 11, QPrinter::Millimeter);
    } else {
        printer.setOrientation(QPrinter::Portrait);
        printer.setPageMargins(10, 10, 10, 15, QPrinter::Millimeter);
    }
    //设置输出格式为PDF
    printer.setOutputFormat(QPrinter::PdfFormat);
    //设置输出文件保存位置
    printer.setOutputFileName(fileName);

    QTextDocument textDocument;
    textDocument.setHtml(html.join(""));
    textDocument.print(&printer);
    textDocument.end();
}

void PdfAPI::SavePdf(QString fileName, QString title, QList<QString> columnNames,
                     QList<int> columnWidths, QStringList subTitle1, QStringList subTitle2,
                     QStringList content, bool landscape, bool check, QPrinter::PageSize pageSize)
{
    //计算列数
    int columnCount = columnNames.count();

    //清空原有数据,确保每次都是新的数据
    html.clear();

    //表格开始
    html.append("<table border='0.5' cellspacing='0' cellpadding='3'>");

    //标题占一行,居中显示
    if (title.length() > 0) {
        html.append(QString("<tr><td align='center' style='vertical-align:middle;font-weight:bold;' colspan='%1'>")
                    .arg(columnCount));
        html.append(title);
        html.append("</td></tr>");
    }

    //循环添加副标题/字段名/内容
    int count = content.count();
    for (int i = 0; i < count; i++) {
        //加个空行隔开
        html.append(QString("<tr><td colspan='%1'>").arg(columnCount));
        html.append("</td></tr>");

        //副标题1
        if (subTitle1.count() > 0 && subTitle1.count() > i) {
            if (subTitle1.at(i).length() > 0) {
                html.append(QString("<tr><td align='left' style='vertical-align:middle;' colspan='%1'>")
                            .arg(columnCount));
                html.append(subTitle1.at(i));
                html.append("</td></tr>");
            }
        }

        //副标题2
        if (subTitle2.count() > 0 && subTitle2.count() > i) {
            if (subTitle2.at(i).length() > 0) {
                html.append(QString("<tr><td align='left' style='vertical-align:middle;' colspan='%1'>")
                            .arg(columnCount));
                html.append(subTitle2.at(i));
                html.append("</td></tr>");
            }
        }

        //逐个添加字段名称
        if (columnCount > 0) {
            html.append("<tr>");
            for (int i = 0; i < columnCount; i++) {
                html.append(QString("<td width='%1' bgcolor='lightgray' align='center' style='vertical-align:middle;'>")
                            .arg(columnWidths.at(i)));
                html.append(columnNames.at(i));
                html.append("</td>");
            }
            html.append("</tr>");
        }

        QStringList list = content.at(i).split(";");

        //逐个添加数据
        int rowCount = list.count();
        for (int j = 0; j < rowCount; j++) {
            html.append("<tr>");

            QString temp = list.at(j);
            QStringList value = temp.split("|");
            int valueCount = value.count();

            if (check) {
                //如果是历史记录则文字红色
                if (value.at(valueCount - 1) == "1") {
                    for (int k = 0; k < valueCount - 1; k++) {
                        html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>"
                                            "<font color='red'>").arg(columnWidths.at(k)));
                        html.append(value.at(k));
                        html.append("</font></td>");
                    }
                } else {
                    for (int k = 0; k < valueCount - 1; k++) {
                        html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                                    .arg(columnWidths.at(k)));
                        html.append(value.at(k));
                        html.append("</td>");
                    }
                }
            } else {
                for (int k = 0; k < valueCount; k++) {
                    html.append(QString("<td width='%1' align='center' style='vertical-align:middle;'>")
                                .arg(columnWidths.at(k)));
                    html.append(value.at(k));
                    html.append("</td>");
                }
            }

            html.append("</tr>");
        }
    }

    html.append("</table>");

    //调用打印机打印PDF
    QPrinter printer;
    //设置纸张规格
    printer.setPageSize(pageSize);
    //设置横向纵向及页边距
    if (landscape) {
        printer.setOrientation(QPrinter::Landscape);
        printer.setPageMargins(10, 10, 10, 11, QPrinter::Millimeter);
    } else {
        printer.setOrientation(QPrinter::Portrait);
        printer.setPageMargins(10, 10, 10, 15, QPrinter::Millimeter);
    }
    //设置输出格式为PDF
    printer.setOutputFormat(QPrinter::PdfFormat);
    //设置输出文件保存位置
    printer.setOutputFileName(fileName);

    QTextDocument textDocument;
    textDocument.setHtml(html.join(""));
    textDocument.print(&printer);
    textDocument.end();
}
