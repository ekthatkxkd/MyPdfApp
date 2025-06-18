#include "include/drawingtemplate/MaterialDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"
#include <QPixmap>

MaterialDocTemplate::MaterialDocTemplate() : DocumentTemplate("Material") {

}

void MaterialDocTemplate::setupTemplate() {
    // 제목 (첫 번째 요소 - 기본 위치)
    auto title = std::make_unique<TextElement>("Template A - 보고서",
                                               QFont("Arial", 18, QFont::Bold),
                                               Qt::AlignCenter);
    title->setElementId("title");
    addElement(std::move(title));

    // 부제목 (제목 아래에 배치)
    auto subtitle = std::make_unique<TextElement>("월간 분석 리포트",
                                                  QFont("Arial", 12));
    subtitle->setElementId("subtitle");
    addElementBelow(std::move(subtitle), "title", 15);

    // 첫 번째 표 (부제목 아래에 배치)
    auto table1 = std::make_unique<TableElement>(generateTableData(0));
    table1->setElementId("table_1");
    addElementBelow(std::move(table1), "subtitle", 20);

    // 두 번째 표 (첫 번째 표 아래에 배치)
    auto table2 = std::make_unique<TableElement>(generateTableData(1));
    table2->setElementId("table_2");
    addElementBelow(std::move(table2), "table_1", 15);

    // 세 번째와 네 번째 표 (같은 행에 배치)
    auto table3 = std::make_unique<TableElement>(generateTableData(2));
    table3->setElementId("table_3");
    addElementBelow(std::move(table3), "table_2", 20);

    auto table4 = std::make_unique<TableElement>(generateTableData(3));
    table4->setElementId("table_4");
    addElementRightOf(std::move(table4), "table_3", 10);

    // 다섯 번째 표 (새 페이지에 배치)
    auto table5 = std::make_unique<TableElement>(generateTableData(4));
    table5->setElementId("table_5");
    table5->setPositionReference(PositionReference::nextPage());
    addElement(std::move(table5));

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

QVector<QVector<QString>> MaterialDocTemplate::generateTableData(int tableIndex) {
    QVector<QVector<QString>> data;
    // 헤더
    // data.append({"항목", "값", "비고"});
    data.push_back({"항목", "값", "비고"});

    // 데이터 행들
    for (int i = 0; i < 5; ++i) {
        data.append({QString("항목 %1-%2").arg(tableIndex + 1).arg(i + 1),
                     QString("값 %1").arg(i + 1),
                     QString("비고 %1").arg(i + 1)});
    }
    return data;
}
