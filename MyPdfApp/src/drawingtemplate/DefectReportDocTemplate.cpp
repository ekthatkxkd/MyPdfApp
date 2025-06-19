#include "include/drawingtemplate/DefectReportDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"
#include "include/docElement/ImageElement.h"
#include <QPixmap>

DefectReportDocTemplate::DefectReportDocTemplate() : DocumentTemplate("DefectReport", "불량발견 보고서") {

}

void DefectReportDocTemplate::setupTemplate(const QMap<QString, QList<QPair<QString, QStringList>>> &elementDatas) {
    if (templateTitle != "") {
        QFont font;
        font.setPointSize(20);
        font.setBold(true);

        // 제목
        auto title = std::make_unique<TextElement>(templateTitle,
                                                   font,
                                                   Qt::AlignHCenter);
        title->setElementId("title");
        addElement(std::move(title));
    }

    // // 세 개의 표를 같은 행에 배치
    // auto table1 = std::make_unique<TableElement>(generateTableData(0));
    // table1->setElementId("dashboard_table_1");
    // addElementBelow(std::move(table1), "title", 20);

    // auto table2 = std::make_unique<TableElement>(generateTableData(1));
    // table2->setElementId("dashboard_table_2");
    // addElementRightOf(std::move(table2), "dashboard_table_1", 10);

    // auto table3 = std::make_unique<TableElement>(generateTableData(2));
    // table3->setElementId("dashboard_table_3");
    // addElementRightOf(std::move(table3), "dashboard_table_2", 10);

    // // 이미지 (표들 아래에 배치)
    // QPixmap image(":/images/sample.png");
    // if (!image.isNull()) {
    //     auto imageElement = std::make_unique<ImageElement>(image, QSizeF(200, 150));
    //     imageElement->setElementId("main_image");
    //     addElementBelow(std::move(imageElement), "dashboard_table_1", 25);
    // }
}

QVector<QVector<QString>> DefectReportDocTemplate::generateTableData(int tableIndex) {
    QVector<QVector<QString>> data;
    // data.append({"메트릭", "값"});
    data.push_back({"메트릭", "값"});

    for (int i = 0; i < 3; ++i) {
        data.append({QString("지표 %1-%2").arg(tableIndex + 1).arg(i + 1),
                     QString("%1").arg((tableIndex + 1) * 10 + i)});
    }
    return data;
}
