#include "include/docTemplate/DefectReportDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"

DefectReportDocTemplate::DefectReportDocTemplate() : DocTemplate("DefectReport", "불량발견 보고서") {
    // 데이터베이스 연결 설정
    dataProvider = std::make_shared<SqliteDataProvider>("database.db", "template_a_conn");

    if (dataProvider->connect()) {
        // 미리 정의된 쿼리들 등록
        dataProvider->registerTableQuery("inform_table",
                                         "SELECT product_name, quantity, price, total FROM sales_summary ORDER BY total DESC");
    }
}

void DefectReportDocTemplate::setupTemplate(const QSizeF &pxContentSize) {
    if (templateTitle != "") {
        QFont font;
        font.setPointSize(20);
        font.setBold(true);

        auto title = std::make_unique<TextElement>(templateTitle,
                                                   font,
                                                   Qt::AlignHCenter);
        title->setElementId(elementIds[0]);
        addElement(std::move(title));
    }

    if (dataProvider && dataProvider->isConnected()) {
        {
            //////// numberTable
            ///
            ///
            TableData numberTableData;

            numberTableData.innerDatas = numberTableInnerDatas;

            qreal tableFullWidth = (pxContentSize.width() - 10) / 2;  // 10 : 오른쪽에 배치될 table 과의 margin

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();

            //////// [LLDDSS] 테스트 코드.
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("dateNumber", "2025/03/25 - 7"));
            readDataFromDB.push_back(QPair<QString, QString>("processing", "품목대체"));
            ////////

            int dataCount = 0;
            while (dataCount < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[dataCount];

                for (auto &rowDatas : numberTableData.innerDatas) {
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

                dataCount++;
            }

            auto numberTable = std::make_unique<TableElement>("", numberTableData, tableFullWidth, numberTableWidthRatio, Qt::AlignLeft);
            numberTable->setElementId(elementIds[1]);
            addElementBelow(std::move(numberTable), QVector<QString>{elementIds[0]}, 10);
            ///
            ///
            ////////
        }

        {
            //////// factoryTable
            ///
            ///
            TableData factoryTableData;

            factoryTableData.innerDatas = factoryTableInnerDatas;
            qreal tableFullWidth = (pxContentSize.width() - 10) / 2;  // 10 : 왼쪽에 배치된 table 과의 margin

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();

            //////// [LLDDSS] 테스트 코드.
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("manager", "()"));
            readDataFromDB.push_back(QPair<QString, QString>("factory", "Rework창고(00186)"));
            ////////

            int dataCount = 0;
            while (dataCount < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[dataCount];

                for (auto &rowDatas : factoryTableData.innerDatas) {
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

                dataCount++;
            }

            auto factoryTable = std::make_unique<TableElement>("", factoryTableData, tableFullWidth, factoryTableWidthRatio, Qt::AlignLeft);
            factoryTable->setElementId(elementIds[2]);
            addElementRightOf(std::move(factoryTable), QVector<QString>{elementIds[1]}, 10);
            ///
            ///
            ////////
        }


        double reduceTotal = 0.0;
        double increseTotal = 0.0;

        {
            //////// product list table
            ///
            ///
            TableData productListTableData;

            productListTableData.headerDatas = productListHeaderDatas;

            qreal tableFullWidth = pxContentSize.width();

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();

            //////// [LLDDSS] 테스트 코드.
            QList<QVector<QString>> readDataFromDB;
            readDataFromDB.push_back(QVector<QString>{"", "P001-0162", "[EFFECT_V2_WHITE_GRAY_SKY] PRODUCT", "102.00", "", "테스트 코드 테스트 코드 테스트 코드"});
            readDataFromDB.push_back(QVector<QString>{"", "E002-0002", "TPS61088 [TI, SMD,VQFN-20, 4.5Vto12.6V, 11.4A, Reel250]", "", "102.00", ""});
            readDataFromDB.push_back(QVector<QString>{"", "E002-0003", "STM32F030CC_LQFP48 [ST, SMD, LQFP-48, 2.4Vto3.6V, 메모리 크기: 256kB, RAM크기:32kB, Reel 2K4]", "", "102.00", ""});
            readDataFromDB.push_back(QVector<QString>{"", "E002-0004", "TPS55340RTE [TI, SMD, WQFN-16, 2.9Vto32V, 2.9Vto38V, 6.6A, Reel3K]", "", "102.00", ""});
            readDataFromDB.push_back(QVector<QString>{"", "E002-0006", "SN74AHC4066RGYR((VQFN) [TI, SMD, VQFN-14, 4 Switch, 2Vto5.5V, Reel 3K])", "", "102.00", ""});
            ////////

            int dataRowIndex = productListTableData.innerDatas.size();

            for (auto &item : readDataFromDB) {
                QVector<CellData> rowDatas = defaultProductListInnerRowDatas;

                for (int index = 0; index < rowDatas.size(); index++) {
                    rowDatas[index].startRow = dataRowIndex;

                    if (rowDatas[index].cellId == "number") {
                        rowDatas[index].cellText = QString::number(dataRowIndex + 1);
                    } else {
                        rowDatas[index].cellText = item[index];

                        if (rowDatas[index].cellId == "reducedQuantity" && item[index] != "") {
                            QString cleanedStr = item[index];
                            cleanedStr.remove(',');

                            reduceTotal += cleanedStr.toDouble();
                        } else if (rowDatas[index].cellId == "increaseQuantity" && item[index] != "") {
                            QString cleanedStr = item[index];
                            cleanedStr.remove(',');

                            increseTotal += cleanedStr.toDouble();
                        }
                    }
                }
                productListTableData.innerDatas.push_back(rowDatas);
                dataRowIndex++;
            }

            auto productListTable = std::make_unique<TableElement>("", productListTableData, tableFullWidth, productListWidthRatio, Qt::AlignLeft);
            productListTable->setElementId(elementIds[3]);
            addElementBelow(std::move(productListTable), QVector<QString>{elementIds[1], elementIds[2]}, 10);
            ///
            ///
            ////////
        }


        int sumInteger = (int)(reduceTotal);
        int sumDecimal = (int)std::round((reduceTotal - sumInteger) * 100);

        QLocale locale(QLocale::Korean, QLocale::SouthKorea);
        QString formattedSumInteger = locale.toString(sumInteger);
        QString formattedSumDecimal = (sumDecimal < 10) ? ("0"+QString::number(sumDecimal))
                                                        : QString::number(sumDecimal);
        QString reduceTotalString = formattedSumInteger + "." + formattedSumDecimal;

        sumInteger = (int)(increseTotal);
        sumDecimal = (int)std::round((increseTotal - sumInteger) * 100);

        formattedSumInteger = locale.toString(sumInteger);
        formattedSumDecimal = (sumDecimal < 10) ? ("0"+QString::number(sumDecimal))
                                                : QString::number(sumDecimal);
        QString increseTotalString = formattedSumInteger + "." + formattedSumDecimal;

        {
            //////// sum table
            ///
            ///
            TableData sumTableData;

            sumTableData.innerDatas = sumTableInnerDatas;
            qreal tableFullWidth = pxContentSize.width();

            for (auto &cellData : sumTableData.innerDatas[0]) {
                if (cellData.cellId == "reduceTotal") {
                    cellData.cellText = reduceTotalString;
                } else if (cellData.cellId == "increaseTotal") {
                    cellData.cellText = increseTotalString;
                }
            }

            auto sumTable = std::make_unique<TableElement>("", sumTableData, tableFullWidth, sumTableWidthRatio, Qt::AlignLeft);
            sumTable->setElementId(elementIds[4]);
            addElementBelow(std::move(sumTable), QVector<QString>{elementIds[3]}, 10);
            ///
            ///
            ////////
        }
    }
}
