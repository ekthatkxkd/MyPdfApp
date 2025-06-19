#include "include/drawingtemplate/MaterialDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"
#include <QPixmap>

MaterialDocTemplate::MaterialDocTemplate() : DocumentTemplate("Material", "자재구매 확인서") {

}

void MaterialDocTemplate::initInformTableData(const QList<QPair<QString, QStringList>> &dbDatas) {
    for (auto &dbData : dbDatas) {
        const QString id = dbData.first;
        const QString value = dbData.second[0];

        for (auto &innerDataRow : informTableInnerDatas) {
            bool isFound = false;
            for (auto &innerData : innerDataRow) {
                if (innerData.cellId == id) {
                    innerData.cellText = value;
                    isFound = true;
                    break;
                }
            }
            if (isFound)
                break;
        }
    }
}

void MaterialDocTemplate::initHistoryTableData(const QList<QPair<QString, QStringList>> &dbDatas) {
    for (auto &dbData : dbDatas) {
        const QString id = dbData.first;
        const QStringList values = dbData.second;

        if (id == "rowData") {
            QVector<CellData> newRowCellDatas = defaultHistoryInnerDatas;
            int newRowNum = historyInnerDatas.size();

            for (int cellIndex = 0; cellIndex < newRowCellDatas.size(); cellIndex++) {
                const QString foundDbValue = newRowCellDatas[cellIndex].cellId + ":";
                newRowCellDatas[cellIndex].startRow = newRowNum;

                for (auto &cellValue : values) {
                    if (cellValue.startsWith(foundDbValue)) {
                        newRowCellDatas[cellIndex].cellText = cellValue.mid(foundDbValue.length());
                        break;
                    }
                }
            }
            historyInnerDatas.push_back(newRowCellDatas);
        } else {  // "rowData" 가 아닐 경우 모두 footer 값이라고 판단.
            for (auto &cellData : historyFooterDatas) {
                if (cellData.cellId != "" ) {
                    const QString foundDbValue = cellData.cellId + ":";
                    for (auto &cellValue : values) {
                        if (cellValue.startsWith(foundDbValue)) {
                            cellData.cellText = cellValue.mid(foundDbValue.length());
                        }
                    }
                }
            }
        }
    }
}

void MaterialDocTemplate::setupTemplate(const QMap<QString, QList<QPair<QString, QStringList>>> &elementDatas) {
    if (templateTitle != "") {
        //////// 제목 (첫 번째 요소 - 기본 위치)
        QFont font;
        font.setPointSize(20);
        font.setBold(true);

        auto title = std::make_unique<TextElement>(templateTitle,
                                                   font,
                                                   Qt::AlignHCenter);
        title->setElementId("title");
        addElement(std::move(title));
    }

    //////// 첫 번째 표 (제목 아래에 배치)
    initInformTableData(elementDatas[materialComponentNames[0]]);

    auto table1 = std::make_unique<TableElement>("", QVector<CellData>(), informTableInnerDatas, QVector<CellData>(),informTableWidthRatio);
    table1->setElementId(materialComponentNames[0]);
    addElementBelow(std::move(table1), "title", 5);
    ////////


    //////// 두 번째 표 (첫 번째 표 아래에 배치)
    initHistoryTableData(elementDatas[materialComponentNames[1]]);

    auto table2 = std::make_unique<TableElement>(historyTableTitle, historyHeaderDatas, historyInnerDatas, historyFooterDatas, historyWidthRatio);
    table2->setElementId(materialComponentNames[1]);
    addElementBelow(std::move(table2), materialComponentNames[0], 5);
    ////////





    ////////
    // // 큰 테이블 - 행별 분할 허용
    // auto largeTable = std::make_unique<TableElement>(generateLargeTableData());
    // largeTable->setElementId("large_table");
    // largeTable->setPageBreakPolicy(TableElement::PageBreakPolicy::SplitRows);
    // largeTable->setRepeatHeaderOnNewPage(true);  // 새 페이지에서 헤더 반복
    // addElementBelow(std::move(largeTable), "title", 20);

    // // 작은 테이블 - 전체를 새 페이지로 이동
    // auto smallTable = std::make_unique<TableElement>(generateSmallTableData());
    // smallTable->setElementId("small_table");
    // smallTable->setPageBreakPolicy(TableElement::PageBreakPolicy::MoveToNewPage);
    // addElementBelow(std::move(smallTable), "large_table", 15);
    ////////
}
