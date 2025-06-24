#include "include/drawingtemplate/MaterialDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"
#include <QPixmap>
#include <QDate>
#include <algorithm>
#include <QLocale>

MaterialDocTemplate::MaterialDocTemplate() : DocumentTemplate("Material", "자재구매 확인서") {
    // 데이터베이스 연결 설정
    dataProvider = std::make_shared<SqliteDataProvider>("database.db", "template_a_conn");

    if (dataProvider->connect()) {
        // 미리 정의된 쿼리들 등록
        dataProvider->registerTableQuery("inform_table",
                                         "SELECT product_name, quantity, price, total FROM sales_summary ORDER BY total DESC");

        // dataProvider->registerTableQuery("monthly_report",
        //                                  "SELECT month, revenue, expenses, profit FROM monthly_report WHERE year = 2024");

        // dataProvider->registerTableQuery("top_customers",
        //                                  "SELECT customer_name, total_orders, total_amount FROM customers ORDER BY total_amount DESC LIMIT 10");
    }
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

void MaterialDocTemplate::setupTemplate(const QSizeF &pxContentSize) {
    if (templateTitle != "") {
        //////// 제목 (첫 번째 요소 - 기본 위치)
        QFont font;
        font.setPointSize(20);
        font.setBold(true);

        auto title = std::make_unique<TextElement>(templateTitle,
                                                   font,
                                                   Qt::AlignHCenter);
        title->setElementId(elementIds[0]);
        addElement(std::move(title));
    }

    //////// [LLDDSS] ORIGIN IS TO ALIVE
    ///
    ///
    // // 첫 번째 표 (제목 아래에 배치)
    // initInformTableData(elementDatas[materialComponentNames[0]]);

    // auto table1 = std::make_unique<TableElement>("", QVector<QString>(), informTableInnerDatas, QVector<QString>(),informTableWidthRatio);
    // table1->setElementId(materialComponentNames[0]);
    // addElementBelow(std::move(table1), "title", 5);
    ////////


    // // 두 번째 표 (첫 번째 표 아래에 배치)
    // initHistoryTableData(elementDatas[materialComponentNames[1]]);

    // auto table2 = std::make_unique<TableElement>(historyTableTitle, historyHeaderDatas, historyInnerDatas, historyFooterDatas, historyWidthRatio);
    // table2->setElementId(materialComponentNames[1]);
    // addElementBelow(std::move(table2), materialComponentNames[0], 5);
    ///
    ///
    ////////

    ///*
    //////// db 에서 data 가져오기.
    ///
    ///
    if (dataProvider && dataProvider->isConnected()) {

        {
            TableData informTableData;

            informTableData.title = "";
            informTableData.headerDatas = QVector<CellData>();
            informTableData.innerDatas = informTableInnerDatas;
            informTableData.footerDatas = QVector<CellData>();

            qreal tableFullWidth = pxContentSize.width();

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");

            QSqlRecord record = tableQuery.record();
            // // 헤더 추출
            // for (int i = 0; i < record.count(); ++i) {
            //     informTableData.headers.append(record.fieldName(i));
            // }

            //////// 데이터 행 추출
            // while (tableQuery.next()) {
            //     QVector<CellData> row;
            //     for (int i = 0; i < record.count(); ++i) {
            //         CellData cellData;
            //         row.append(tableQuery.value(i).toString());
            //     }
            //     informTableData.rowDatas.append(row);
            // }
            //////// [LLDDSS] 테스트 코드.
            ///
            ///
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("number", "2025/03/13 - 8"));
            readDataFromDB.push_back(QPair<QString, QString>("company", "(주) 테스트"));
            readDataFromDB.push_back(QPair<QString, QString>("storage", "수입검사(IQC)"));
            readDataFromDB.push_back(QPair<QString, QString>("address", "경기도 부천시 OO구 OO로 000번길 00 (내동)"));
            readDataFromDB.push_back(QPair<QString, QString>("manager", "홍길동"));
            readDataFromDB.push_back(QPair<QString, QString>("tel", "032-000-0000"));

            int count = 0;
            while (count < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[count];

                for (auto &rowDatas : informTableData.innerDatas) {
                    bool isFound = false;
                    for (auto &cellData : rowDatas) {
                        if (cellData.cellId == readData.first) {
                            cellData.cellText = readData.second;

                            isFound = true;
                            break;
                        }
                    }
                    if (isFound)
                        break;
                }

                count++;
            }

            auto informTable = std::make_unique<TableElement>("", informTableData, tableFullWidth, informTableWidthRatio, Qt::AlignLeft);
            informTable->setElementId(elementIds[1]);
            addElementBelow(std::move(informTable), QVector<QString>{elementIds[0]}, 10);

            ///
            ///
            ////////
        }

        {
            TableData historyTableData;

            // historyTableData.title = historyTableTitle;
            historyTableData.headerDatas = historyHeaderDatas;
            historyTableData.innerDatas = QVector<QVector<CellData>>();
            historyTableData.footerDatas = historyFooterDatas;

            qreal tableFullWidth = pxContentSize.width();

            QSqlQuery tableQuery = dataProvider->getTableData("history_table");
            QSqlRecord record = tableQuery.record();

            //////// [LLDDSS] 테스트 코드.
            ///
            ///
            QList<QVector<QString>> readDataFromDB;
            readDataFromDB.push_back(QVector<QString>{"25/03/13", "[EFFECT_V2_SKY] FRONT BUTTON DECO]", "1,232.00", "가나다라 마바사 아자차카 타파하. abcdefg hijklmn opqrstu vwxyz."});
            readDataFromDB.push_back(QVector<QString>{"25/06/20", "[EFFECT_V2_SKY] FRONT BUTTON DECO] [EFFECT_V2_SKY] FRONT BUTTON DECO]", "99.20", ""});
            readDataFromDB.push_back(QVector<QString>{"25/05/20", "[테스트 제품명]", "3,000.33", "테스트 목적으로 임의의 문자열을 나열함. 해당 문자열이 허용된 최대 width 값을 벗어날 경우 다음 줄로 넘어가야 함. 또한 cell 의 height 값도 변경되어야 함."});
            readDataFromDB.push_back(QVector<QString>{"25/05/01", "[테스트 제품명] [테스트 제품명] [테스트 제품명]", "100,000.00", ""});
            readDataFromDB.push_back(QVector<QString>{"25/04/01", "[테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명]", "1.80", ""});
            ///
            ///
            ////////

            sortReadHistoryFromDB(readDataFromDB);

            int dataRowIndex = historyTableData.innerDatas.size();
            double quantitySum = 0.0;
            for (auto &item : readDataFromDB) {
                QVector<CellData> rowDatas = defaultHistoryInnerDatas;

                for (int informIndex = 0; informIndex < rowDatas.size(); informIndex++) {
                    rowDatas[informIndex].startRow = dataRowIndex;

                    rowDatas[informIndex].cellText = item[informIndex];

                    if (rowDatas[informIndex].cellId == "quantity") {
                        QString cleanedStr = item[informIndex];
                        cleanedStr.remove(',');

                        quantitySum += cleanedStr.toDouble();
                    }
                }

                historyTableData.innerDatas.push_back(rowDatas);
                dataRowIndex++;
            }

            int sumInteger = (int)(quantitySum);
            int sumDecimal = (quantitySum - sumInteger) * 100;

            // 시스템 로케일을 기준으로 포맷 (예: 한국 로케일 사용)
            QLocale locale(QLocale::Korean, QLocale::SouthKorea);
            QString formattedSumInteger = locale.toString(sumInteger);

            QString sumString = formattedSumInteger + "." + QString::number(sumDecimal);

            for (auto &cellData : historyTableData.footerDatas) {
                if (cellData.cellId == "sum") {
                    cellData.cellText = sumString;
                }
            }

            auto historyTable = std::make_unique<TableElement>(historyTableTitle, historyTableData, tableFullWidth, historyWidthRatio, Qt::AlignLeft);
            historyTable->setElementId(elementIds[2]);
            addElementBelow(std::move(historyTable), QVector<QString>{elementIds[1]}, 10);
        }
        // // 두 번째 표 - 커스텀 쿼리 테이블
        // QString customQuery = "SELECT p.product_name, c.category_name, SUM(s.quantity) as total_sold "
        //                       "FROM sales s "
        //                       "JOIN products p ON s.product_id = p.id "
        //                       "JOIN categories c ON p.category_id = c.id "
        //                       "GROUP BY p.product_name, c.category_name "
        //                       "ORDER BY total_sold DESC LIMIT 20";
        // std::unique_ptr<TableElement> materialListTable = std::make_unique<TableElement>("", dataProvider, customQuery, true);
        // materialListTable->setElementId("list_table");
        // addElementBelow(std::move(materialListTable), "inform_table", 5);
    } else {

    }
    ///
    ///
    ////////
    //*/



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

void MaterialDocTemplate::sortReadHistoryFromDB(QList<QVector<QString>> &readDataFromDB) {
    std::sort(readDataFromDB.begin(), readDataFromDB.end(),
              [](const QVector<QString>& a, const QVector<QString>& b) {
                  if (a.isEmpty() || b.isEmpty())
                      return false;

                  // "25/03/13" 포맷을 QDate로 변환
                  QDate dateA = QDate::fromString(a[0], "yy/MM/dd");
                  QDate dateB = QDate::fromString(b[0], "yy/MM/dd");

                  return dateA < dateB;
              });
}
