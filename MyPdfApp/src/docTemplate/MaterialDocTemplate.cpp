#include "include/docTemplate/MaterialDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"
// #include <QPixmap>
#include <QDate>
#include <algorithm>

MaterialDocTemplate::MaterialDocTemplate() : DocTemplate("Material", "자재구매 확인서") {
    // 데이터베이스 연결 설정
    dataProvider = std::make_shared<SqliteDataProvider>("database.db", "template_a_conn");

    if (dataProvider->connect()) {
        // 미리 정의된 쿼리들 등록
        dataProvider->registerTableQuery("inform_table",
                                         "SELECT number, company, storage, address, manager, tel FROM material_inform");

        dataProvider->registerTableQuery("history_table",
                                         "SELECT date, item_name, quantity, briefs FROM material_history ORDER BY date");
    }
}

void MaterialDocTemplate::setupTemplate(const QSizeF &pxContentSize) {
    if (templateTitle != "") {
        setupTitle();
    }

    if (dataProvider && dataProvider->isConnected()) {
        setupInformTable(pxContentSize);
        setupHistoryTable(pxContentSize);
    }
}

void MaterialDocTemplate::setupTitle() {
    QFont font;
    font.setPointSize(20);
    font.setBold(true);

    auto title = std::make_unique<TextElement>(templateTitle, font, Qt::AlignHCenter);
    title->setElementId(elementIds[0]);
    addElement(std::move(title));
}

void MaterialDocTemplate::setupInformTable(const QSizeF &pxContentSize) {
    TableData informTableData;
    informTableData.title = "";
    informTableData.headerDatas = QVector<CellData>();
    informTableData.innerDatas = informTableInnerDatas;
    informTableData.footerDatas = QVector<CellData>();

    qreal tableFullWidth = pxContentSize.width();

    // 실제 데이터베이스에서 데이터 가져오기
    QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
    
    if (tableQuery.isActive() && tableQuery.next()) {
        // 실제 데이터베이스 데이터 사용
        populateInformTableFromDB(informTableData, tableQuery);
    } else {
        // 테스트 데이터 사용 (데이터베이스 연결 실패 시)
        populateInformTableWithTestData(informTableData);
    }

    auto informTable = std::make_unique<TableElement>("", informTableData, tableFullWidth, informTableWidthRatio, Qt::AlignLeft);
    informTable->setElementId(elementIds[1]);
    addElementBelow(std::move(informTable), QVector<QString>{elementIds[0]}, 10);
}

void MaterialDocTemplate::populateInformTableFromDB(TableData &informTableData, QSqlQuery &query) {
    QSqlRecord record = query.record();
    
    // 데이터베이스에서 가져온 데이터를 테이블에 매핑
    QMap<QString, QString> dbData;
    for (int i = 0; i < record.count(); ++i) {
        QString fieldName = record.fieldName(i);
        QString value = query.value(i).toString();
        dbData[fieldName] = value;
    }

    // 테이블 셀에 데이터 적용
    for (auto &rowDatas : informTableData.innerDatas) {
        for (auto &cellData : rowDatas) {
            if (dbData.contains(cellData.cellId)) {
                cellData.cellText = dbData[cellData.cellId];
            }
        }
    }
}

void MaterialDocTemplate::populateInformTableWithTestData(TableData &informTableData) {
    QList<QPair<QString, QString>> testData = {
        {"number", "2025/03/13 - 8"},
        {"company", "(주) 테스트"},
        {"storage", "수입검사(IQC)"},
        {"address", "경기도 부천시 OO구 OO로 000번길 00 (내동)"},
        {"manager", "홍길동"},
        {"tel", "032-000-0000"}
    };

    for (const auto &data : testData) {
        for (auto &rowDatas : informTableData.innerDatas) {
            for (auto &cellData : rowDatas) {
                if (cellData.cellId == data.first) {
                    cellData.cellText = data.second;
                    break;
                }
            }
        }
    }
}

void MaterialDocTemplate::setupHistoryTable(const QSizeF &pxContentSize) {
    TableData historyTableData;
    historyTableData.headerDatas = historyHeaderDatas;
    historyTableData.innerDatas = QVector<QVector<CellData>>();
    historyTableData.footerDatas = historyFooterDatas;

    qreal tableFullWidth = pxContentSize.width();

    // 실제 데이터베이스에서 데이터 가져오기
    QSqlQuery tableQuery = dataProvider->getTableData("history_table");
    
    QList<QVector<QString>> historyData;
    
    if (tableQuery.isActive()) {
        // 실제 데이터베이스 데이터 사용
        populateHistoryDataFromDB(historyData, tableQuery);
    } else {
        // 테스트 데이터 사용 (데이터베이스 연결 실패 시)
        populateHistoryDataWithTestData(historyData);
    }

    sortReadHistoryFromDB(historyData);
    populateHistoryTableData(historyTableData, historyData);

    auto historyTable = std::make_unique<TableElement>(historyTableTitle, historyTableData, tableFullWidth, historyWidthRatio, Qt::AlignLeft);
    historyTable->setElementId(elementIds[2]);
    addElementBelow(std::move(historyTable), QVector<QString>{elementIds[1]}, 10);
}

void MaterialDocTemplate::populateHistoryDataFromDB(QList<QVector<QString>> &historyData, QSqlQuery &query) {
    QSqlRecord record = query.record();
    
    while (query.next()) {
        QVector<QString> row;
        for (int i = 0; i < record.count(); ++i) {
            row.append(query.value(i).toString());
        }
        historyData.append(row);
    }
}

void MaterialDocTemplate::populateHistoryDataWithTestData(QList<QVector<QString>> &historyData) {
    historyData = {
        {"25/03/13", "[EFFECT_V2_SKY] FRONT BUTTON DECO]", "1,232.00", "가나다라 마바사 아자차카 타파하. abcdefg hijklmn opqrstu vwxyz."},
        {"25/06/20", "[EFFECT_V2_SKY] FRONT BUTTON DECO] [EFFECT_V2_SKY] FRONT BUTTON DECO]", "99.20", ""},
        {"25/05/20", "[테스트 제품명]", "3,000.33", "테스트 목적으로 임의의 문자열을 나열함. 해당 문자열이 허용된 최대 width 값을 벗어날 경우 다음 줄로 넘어가야 함. 또한 cell 의 height 값도 변경되어야 함."},
        {"25/05/01", "[테스트 제품명] [테스트 제품명] [테스트 제품명]", "100,000.00", ""},
        {"25/04/01", "[테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명] [테스트 제품명]", "1.80", ""}
    };
}

void MaterialDocTemplate::populateHistoryTableData(TableData &historyTableData, const QList<QVector<QString>> &historyData) {
    int dataRowIndex = historyTableData.innerDatas.size();
    double quantitySum = 0.0;
    
    for (const auto &item : historyData) {
        QVector<CellData> rowDatas = defaultHistoryInnerRowDatas;

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

    // 합계 계산 및 포맷팅
    updateHistoryTableFooter(historyTableData, quantitySum);
}

void MaterialDocTemplate::updateHistoryTableFooter(TableData &historyTableData, double quantitySum) {
    int sumInteger = static_cast<int>(quantitySum);
    int sumDecimal = static_cast<int>(std::round((quantitySum - sumInteger) * 100));

    // 한국 로케일을 기준으로 포맷
    QLocale locale(QLocale::Korean, QLocale::SouthKorea);
    QString formattedSumInteger = locale.toString(sumInteger);
    QString formattedSumDecimal = (sumDecimal < 10) ? ("0" + QString::number(sumDecimal))
                                                    : QString::number(sumDecimal);

    QString sumString = formattedSumInteger + "." + formattedSumDecimal;

    for (auto &cellData : historyTableData.footerDatas) {
        if (cellData.cellId == "sum") {
            cellData.cellText = sumString;
        }
    }
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
