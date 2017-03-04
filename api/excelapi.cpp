#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include "excelapi.h"
#include "myhelper.h"

ExcelAPI *ExcelAPI::_instance = 0;
ExcelAPI::ExcelAPI(QObject *parent) :
    QObject(parent)
{
}

void ExcelAPI::checkBorder(bool border)
{
    if (border) {
        html.append("  <Borders>\n");
        html.append("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
        html.append("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
        html.append("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
        html.append("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
        html.append("  </Borders>\n");
    }
}

void ExcelAPI::ToExcel(QString fileName, QString sheetName, QString title, QString subTitle,
                       QList<QString> columnNames, QList<int> columnWidths,
                       QStringList content, bool border, bool check)
{
    if (sheetName.length() == 0) {
        sheetName = "Sheet1";
    }

    //计算行数列数
    int columnCount = columnNames.count();
    int rowCount = content.count();

    //逐个拼接xml字符,再保存为xls文件
    //清空原有数据,确保每次都是新的数据
    html.clear();

    //固定头部信息
    html.append("<?xml version=\"1.0\"?>\n");
    html.append("<?mso-application progid=\"Excel.Sheet\"?>\n");
    html.append("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
    html.append(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
    html.append(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");

    //文档信息
    html.append(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append("  <LastAuthor></LastAuthor>\n");
    html.append(QString("  <Created>%1</Created>\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    html.append("  <Version>11.6360</Version>\n");
    html.append(" </DocumentProperties>\n");

    html.append(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append("  <AllowPNG/>\n");
    html.append(" </OfficeDocumentSettings>\n");

    html.append(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("  <WindowHeight>9795</WindowHeight>\n");
    html.append("  <WindowWidth>21435</WindowWidth>\n");
    html.append("  <WindowTopX>120</WindowTopX>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <ProtectWindows>False</ProtectWindows>\n");
    html.append(" </ExcelWorkbook>\n");

    //样式表
    html.append(" <Styles>\n");
    html.append("  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
    html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    html.append("   <Borders/>\n");
    html.append("   <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#000000\"/>\n");
    html.append("   <Interior/>\n");
    html.append("   <NumberFormat/>\n");
    html.append("   <Protection/>\n");
    html.append("  </Style>\n");

    //标题样式居中大号加粗字体
    html.append("  <Style ss:ID=\"s_title\">\n");
    html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    checkBorder(border);
    html.append("   <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n");
    html.append("  </Style>\n");

    //标题样式--左对齐普通字体
    html.append("  <Style ss:ID=\"s_subtitle\">\n");
    html.append("   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Center\"/>\n");
    checkBorder(border);
    html.append("   <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#000000\" ss:Bold=\"0\"/>\n");
    html.append("  </Style>\n");

    //正文样式
    html.append("  <Style ss:ID=\"s_normal\">\n");
    checkBorder(border);
    html.append(" </Style>\n");

    //正文样式--文字红色
    html.append("  <Style ss:ID=\"s_red\">\n");
    checkBorder(border);
    html.append("  <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#FF0000\" ss:Bold=\"0\"/>\n");
    html.append(" </Style>\n");

    html.append(" </Styles>\n");

    //工作表名称
    html.append(QString(" <Worksheet ss:Name=\"%1\">\n").arg(sheetName));

    //表格开始
    html.append(QString("  <Table ss:ExpandedColumnCount=\"%1\" x:FullColumns=\"1\"\n").arg(columnCount));
    html.append("   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n");

    //设置字段宽度
    for (int i = 0; i < columnCount; i++) {
        html.append(QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(columnWidths.at(i)));
    }

    //表格标题
    if (title.length() > 0) {
        html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"22\">\n");
        html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_title\"><Data ss:Type=\"String\">%2</Data></Cell>\n")
                    .arg(columnCount - 1).arg(title));
        html.append("   </Row>");
    }

    //表格副标题
    if (subTitle.length() > 0) {
        html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n");
        html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">%2</Data></Cell>\n")
                    .arg(columnCount - 1).arg(subTitle));
        html.append("   </Row>");
    }

    //逐个添加字段名称
    if (columnCount > 0) {
        html.append("   <Row ss:AutoFitHeight=\"0\">\n");
        for (int i = 0; i < columnCount; i++) {
            html.append(QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n")
                        .arg(columnNames.at(i)));
        }
        html.append("   </Row>\n");
    }

    //逐个添加数据
    for (int i = 0; i < rowCount; i++) {
        html.append("   <Row ss:AutoFitHeight=\"0\">\n");

        QString temp = content.at(i);
        QStringList value = temp.split(";");

        if (check) {
            //如果是历史记录则文字红色
            if (value.at(value.count() - 1) == "历史记录") {
                foreach (QString str, value) {
                    html.append(QString("    <Cell ss:StyleID=\"s_red\"><Data ss:Type=\"String\">%1</Data></Cell>\n").arg(str));
                }
            } else {
                foreach (QString str, value) {
                    html.append(QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n").arg(str));
                }
            }
        } else {
            foreach (QString str, value) {
                html.append(QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n").arg(str));
            }
        }

        html.append("   </Row>\n");
    }

    html.append("  </Table>\n");

    //固定结尾格式
    html.append("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("   <PageSetup>\n");
    html.append("    <Header x:Margin=\"0.3\"/>\n");
    html.append("    <Footer x:Margin=\"0.3\"/>\n");
    html.append("    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n");
    html.append("   </PageSetup>\n");
    html.append("   <Unsynced/>\n");
    html.append("   <Selected/>\n");
    html.append("   <ProtectObjects>False</ProtectObjects>\n");
    html.append("   <ProtectScenarios>False</ProtectScenarios>\n");
    html.append("  </WorksheetOptions>\n");
    html.append(" </Worksheet>\n");
    html.append("</Workbook>\n");

    //写入文件
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream text(&file);
        text.setCodec("UTF-8");
        text << html.join("");
    }
}

void ExcelAPI::ToExcel(QString fileName, QString sheetName, QString title,
                       QList<QString> columnNames, QList<int> columnWidths,
                       QStringList subTitle1, QStringList subTitle2,
                       QStringList content, bool border, bool check)
{
    if (sheetName.length() == 0) {
        sheetName = "Sheet1";
    }

    //计算列数
    int columnCount = columnNames.count();

    //逐个拼接xml字符,再保存为xls文件
    //清空原有数据,确保每次都是新的数据
    html.clear();

    //固定头部信息
    html.append("<?xml version=\"1.0\"?>\n");
    html.append("<?mso-application progid=\"Excel.Sheet\"?>\n");
    html.append("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
    html.append(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
    html.append(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    html.append(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");

    //文档信息
    html.append(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append(QString("  <LastAuthor>%1</LastAuthor>\n").arg(""));
    html.append(QString("  <Created>%1</Created>\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    html.append("  <Version>11.6360</Version>\n");
    html.append(" </DocumentProperties>\n");

    html.append(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    html.append("  <AllowPNG/>\n");
    html.append(" </OfficeDocumentSettings>\n");

    html.append(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("  <WindowHeight>9795</WindowHeight>\n");
    html.append("  <WindowWidth>21435</WindowWidth>\n");
    html.append("  <WindowTopX>120</WindowTopX>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <WindowTopY>75</WindowTopY>\n");
    html.append("  <ProtectWindows>False</ProtectWindows>\n");
    html.append(" </ExcelWorkbook>\n");

    //样式表
    html.append(" <Styles>\n");
    html.append("  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
    html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    html.append("   <Borders/>\n");
    html.append("   <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#000000\"/>\n");
    html.append("   <Interior/>\n");
    html.append("   <NumberFormat/>\n");
    html.append("   <Protection/>\n");
    html.append("  </Style>\n");

    //标题样式居中大号加粗字体
    html.append("  <Style ss:ID=\"s_title\">\n");
    html.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    checkBorder(border);
    html.append("   <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n");
    html.append("  </Style>\n");

    //标题样式--左对齐普通字体
    html.append("  <Style ss:ID=\"s_subtitle\">\n");
    html.append("   <Alignment ss:Horizontal=\"Left\" ss:Vertical=\"Center\"/>\n");
    checkBorder(border);
    html.append("   <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#000000\" ss:Bold=\"0\"/>\n");
    html.append("  </Style>\n");

    //正文样式
    html.append("  <Style ss:ID=\"s_normal\">\n");
    checkBorder(border);
    html.append(" </Style>\n");

    //正文样式--文字红色
    html.append("  <Style ss:ID=\"s_red\">\n");
    checkBorder(border);
    html.append("  <Font ss:FontName=\"微软雅黑\" x:CharSet=\"134\" ss:Size=\"10\" ss:Color=\"#FF0000\" ss:Bold=\"0\"/>\n");
    html.append(" </Style>\n");

    html.append(" </Styles>\n");

    //工作表名称
    html.append(QString(" <Worksheet ss:Name=\"%1\">\n").arg(sheetName));

    //表格开始
    html.append(QString("  <Table ss:ExpandedColumnCount=\"%1\" x:FullColumns=\"1\"\n").arg(columnCount));
    html.append("   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n");

    //设置字段宽度
    for (int i = 0; i < columnCount; i++) {
        html.append(QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(columnWidths.at(i)));
    }

    //表格标题
    if (title.length() > 0) {
        html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"22\">\n");
        html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_title\"><Data ss:Type=\"String\">%2</Data></Cell>\n")
                    .arg(columnCount - 1).arg(title));
        html.append("   </Row>");
    }

    //循环添加副标题/字段名/内容
    int count = content.count();
    for (int i = 0; i < count; i++) {
        //加个空行隔开
        html.append("   <Row ss:AutoFitHeight=\"0\">\n");
        html.append("   </Row>");

        //副标题1
        if (subTitle1.count() > 0 && subTitle1.count() > i) {
            if (subTitle1.at(i).length() > 0) {
                html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n");
                html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">%2</Data></Cell>\n")
                            .arg(columnCount - 1).arg(subTitle1.at(i)));
                html.append("   </Row>");
            }
        }

        //副标题2
        if (subTitle2.count() > 0 && subTitle2.count() > i) {
            if (subTitle2.at(i).length() > 0) {
                html.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n");
                html.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">%2</Data></Cell>\n")
                            .arg(columnCount - 1).arg(subTitle2.at(i)));
                html.append("   </Row>");
            }
        }

        //逐个添加字段名称
        if (columnCount > 0) {
            html.append("   <Row ss:AutoFitHeight=\"0\">\n");
            for (int j = 0; j < columnCount; j++) {
                html.append(QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n").arg(columnNames.at(j)));
            }
            html.append("   </Row>\n");
        }

        QStringList list = content.at(i).split(";");

        //逐个添加数据
        int rowCount = list.count();
        for (int j = 0; j < rowCount; j++) {
            html.append("   <Row ss:AutoFitHeight=\"0\">\n");
            QString temp = list.at(j);
            QStringList value = temp.split("|");
            int valueCount = value.count();

            if (check) {
                //如果是历史记录则文字红色
                if (value.at(valueCount - 1) == "1") {
                    for (int k = 0; k < valueCount - 1; k++) {
                        html.append(QString("    <Cell ss:StyleID=\"s_red\"><Data ss:Type=\"String\">%1</Data></Cell>\n")
                                    .arg(value.at(k)));
                    }
                } else {
                    for (int k = 0; k < valueCount - 1; k++) {
                        html.append(QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n")
                                    .arg(value.at(k)));
                    }
                }
            } else {
                for (int k = 0; k < valueCount; k++) {
                    html.append(QString("    <Cell ss:StyleID=\"s_normal\"><Data ss:Type=\"String\">%1</Data></Cell>\n")
                                .arg(value.at(k)));
                }
            }

            html.append("   </Row>\n");
        }
    }

    html.append("  </Table>\n");

    //固定结尾格式
    html.append("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    html.append("   <PageSetup>\n");
    html.append("    <Header x:Margin=\"0.3\"/>\n");
    html.append("    <Footer x:Margin=\"0.3\"/>\n");
    html.append("    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n");
    html.append("   </PageSetup>\n");
    html.append("   <Unsynced/>\n");
    html.append("   <Selected/>\n");
    html.append("   <ProtectObjects>False</ProtectObjects>\n");
    html.append("   <ProtectScenarios>False</ProtectScenarios>\n");
    html.append("  </WorksheetOptions>\n");
    html.append(" </Worksheet>\n");
    html.append("</Workbook>\n");

    //写入文件
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream text(&file);
        text.setCodec("UTF-8");
        text << html.join("");
    }
}
