#include "include/docTemplate/ReceiptVoucherDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"
#include "include/docElement/ImageElement.h"

ReceiptVoucherDocTemplate::ReceiptVoucherDocTemplate() : DocTemplate("ReceiptVoucher", "생산입고전표") {
    // 데이터베이스 연결 설정
    dataProvider = std::make_shared<SqliteDataProvider>("database.db", "template_a_conn");

    if (dataProvider->connect()) {
        // 미리 정의된 쿼리들 등록
        dataProvider->registerTableQuery("inform_table",
                                         "SELECT product_name, quantity, price, total FROM sales_summary ORDER BY total DESC");
    }
}

void ReceiptVoucherDocTemplate::setupTemplate(const QSizeF &pxContentSize) {
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

    double productionSum = 0.0;
    double consumedSum = 0.0;

    if (dataProvider && dataProvider->isConnected()) {
        {
            //////// templateInform
            ///
            ///

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("templateNum", "2025/03/25 - 14"));
            readDataFromDB.push_back(QPair<QString, QString>("date", "2025.03.25"));
            ////////

            QVector<QVector<CellData>> innerDatas = templateInformTableInnerDatas;

            int count = 0;
            while (count < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[count];

                for (auto &rowDatas : innerDatas) {
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

            qreal tableFullWidth = (pxContentSize.width() - 10) / 2;
            TableData templateInformTableData;
            templateInformTableData.innerDatas = innerDatas;

            auto templateInformTable = std::make_unique<TableElement>("", templateInformTableData, tableFullWidth, templateInformTableWidthRatio, Qt::AlignLeft);
            templateInformTable->setElementId(elementIds[1]);
            addElementBelow(std::move(templateInformTable), QVector<QString>{elementIds[0]}, 10);
            ///
            ///
            ////////
        }

        {
            //////// wareHouseInform
            ///
            ///

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("delivery", "Rework창고(00186)"));
            readDataFromDB.push_back(QPair<QString, QString>("store", "Rework창고(00186)"));
            ////////

            QVector<QVector<CellData>> innerDatas = wareHouseInformTableInnerDatas;

            int count = 0;
            while (count < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[count];

                for (auto &rowDatas : innerDatas) {
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

            qreal tableFullWidth = (pxContentSize.width() - 10) / 2;
            TableData wareHouseInformTableData;
            wareHouseInformTableData.innerDatas = innerDatas;

            auto templateInformTable = std::make_unique<TableElement>("", wareHouseInformTableData, tableFullWidth, wareHouseInformTableWidthRatio, Qt::AlignLeft);
            templateInformTable->setElementId(elementIds[2]);
            addElementRightOf(std::move(templateInformTable), QVector<QString>{elementIds[1]}, 10);

            ///
            ///
            ////////
        }

        {
            //////// itemTable - productionItems
            ///
            ///

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QVector<QString>> readDataFromDB;
            readDataFromDB.push_back(QVector<QString>{"생산", "E001-0051", "EFFECT MAIN PCB Assy [EFFECT MAIN PCB Assy]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"생산", "E001-0053", "EFFECT SHOT BUTTON PCB Assy [EFFECT SHOT BUTTON PCB Assy]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"생산", "E001-0052", "EFFECT SUB PCB Assy [EFFECT SUB PCB Assy]", "685.00", "적요 테스트 코드."});
            readDataFromDB.push_back(QVector<QString>{"생산", "E001-0054", "EFFECT LED PCB Assy [EFFECT LED PCB Assy]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"생산", "E001-0010", "MAXIMUM/EFFECT IR PCB Assy[공용] [FR4, Green, White, 1.2T, 21.5*30, 2층, 외층 10Z, 금 도금]....MAXIMUM/EFFECT IR PCB Assy[공용] [FR4, Green, White, 1.2T, 21.5*30, 2층, 외층 10Z, 금 도금]", "685.00", ""});
            ////////

            QVector<QVector<CellData>> innerDatas = itemTableInnerDatas;

            int dataRowIndex = innerDatas.size();

            for (auto &item : readDataFromDB) {
                QVector<CellData> rowDatas = defaultItemTableRowDatas;

                for (int rowIndex = 0; rowIndex < rowDatas.size(); rowIndex++) {
                    rowDatas[rowIndex].startRow = dataRowIndex;

                    rowDatas[rowIndex].cellText = item[rowIndex];

                    if (rowDatas[rowIndex].cellId == "quantity") {
                        double value = 0.0;
                        if (item[rowIndex] != "") {
                            QString cleanedStr = item[rowIndex];
                            cleanedStr.remove(',');
                            value = cleanedStr.toDouble();
                        }

                        productionSum += value;
                    }
                }

                innerDatas.push_back(rowDatas);
                dataRowIndex++;
            }

            //////// footer 정의
            ///
            ///
            QVector<CellData> footerDatas = defaultItemTableFooterDatas;

            for (auto &cellData: footerDatas) {
                if (cellData.cellId == "sum") {
                    double sumValue = productionSum;

                    int sumInteger = (int)(sumValue);
                    int sumDecimal = (int)std::round((sumValue - sumInteger) * 100);

                    QLocale locale(QLocale::Korean, QLocale::SouthKorea);
                    QString formattedSumInteger = locale.toString(sumInteger);
                    QString formattedSumDecimal = (sumDecimal < 10) ? ("0"+QString::number(sumDecimal))
                                                                    : QString::number(sumDecimal);

                    QString sumString = formattedSumInteger + "." + formattedSumDecimal;

                    cellData.cellText = sumString;
                    break;
                }
            }

            ///
            ///
            ////////


            qreal tableFullWidth = pxContentSize.width();
            TableData productionItemsData;
            productionItemsData.headerDatas = itemTableHeaderDatas;
            productionItemsData.innerDatas = innerDatas;
            productionItemsData.footerDatas = footerDatas;

            auto productionItemsTable = std::make_unique<TableElement>("", productionItemsData, tableFullWidth, itemsTableWidthRatio, Qt::AlignLeft);
            productionItemsTable->setElementId(elementIds[3]);
            addElementBelow(std::move(productionItemsTable), QVector<QString>{elementIds[1], elementIds[2]}, 10);

            ///
            ///
            ////////
        }

        {
            //////// itemTable - consumedItems
            ///
            ///

            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QVector<QString>> readDataFromDB;
            readDataFromDB.push_back(QVector<QString>{"소모", "E003-0020", "R1005_22R_J [SMD Chip Resistor, 1005, 22R, J]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"소모", "E003-0023", "R1005_1K_J [SMD Chip Resistor, 1005, 1K, J]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"소모", "E003-0027", "R1005_10K_J [SMD Chip Resistor, 1005, 1K, J]", "685.00", "적요 테스트 코드."});
            readDataFromDB.push_back(QVector<QString>{"소모", "E003-0038", "R1005_150R_J [SMD Chip Resistor, 1005, 150R, J]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"소모", "E003-0010", "KPA-3010F3C [Kingbright, SMD, 3*2*1mm, 5V 90mA, Reel 2K]", "685.00", ""});
            readDataFromDB.push_back(QVector<QString>{"소모", "E003-0011", "KPA-3010P3BT [Kingbright, SMD, 3*2*1mm, 5V 100mA, Reel 2K]", "685.00", ""});
            ////////

            QVector<QVector<CellData>> innerDatas = itemTableInnerDatas;

            int dataRowIndex = innerDatas.size();

            for (auto &item : readDataFromDB) {
                QVector<CellData> rowDatas = defaultItemTableRowDatas;

                for (int rowIndex = 0; rowIndex < rowDatas.size(); rowIndex++) {
                    rowDatas[rowIndex].startRow = dataRowIndex;

                    rowDatas[rowIndex].cellText = item[rowIndex];

                    if (rowDatas[rowIndex].cellId == "quantity") {
                        double value = 0.0;
                        if (item[rowIndex] != "") {
                            QString cleanedStr = item[rowIndex];
                            cleanedStr.remove(',');
                            value = cleanedStr.toDouble();
                        }

                        consumedSum += value;
                    }
                }

                innerDatas.push_back(rowDatas);
                dataRowIndex++;
            }

            //////// footer 정의
            ///
            ///
            QVector<CellData> footerDatas = defaultItemTableFooterDatas;

            for (auto &cellData: footerDatas) {
                if (cellData.cellId == "sum") {
                    double sumValue = consumedSum;

                    int sumInteger = (int)(sumValue);
                    int sumDecimal = (int)std::round((sumValue - sumInteger) * 100);

                    QLocale locale(QLocale::Korean, QLocale::SouthKorea);
                    QString formattedSumInteger = locale.toString(sumInteger);
                    QString formattedSumDecimal = (sumDecimal < 10) ? ("0"+QString::number(sumDecimal))
                                                                    : QString::number(sumDecimal);

                    QString sumString = formattedSumInteger + "." + formattedSumDecimal;

                    cellData.cellText = sumString;
                    break;
                }
            }
            ///
            ///
            ////////

            qreal tableFullWidth = pxContentSize.width();
            TableData consumedItemsData;
            consumedItemsData.innerDatas = innerDatas;
            consumedItemsData.footerDatas = footerDatas;

            auto consumedItemsTable = std::make_unique<TableElement>("", consumedItemsData, tableFullWidth, itemsTableWidthRatio, Qt::AlignLeft);
            consumedItemsTable->setElementId(elementIds[4]);
            addElementBelow(std::move(consumedItemsTable), QVector<QString>{elementIds[3]}, 0);
            ///
            ///
            ////////
        }
    }

    //////// sumTable
    ///
    ///
    {
        QVector<QVector<CellData>> innerDatas = sumTableInnerDatas;

        for (auto &rowData : innerDatas) {
            for (auto &cellData : rowData) {
                double sumValue = 0.0;

                if (cellData.cellId == "productionQuantity") {
                    sumValue = productionSum;
                } else if (cellData.cellId == "consumedQuantity") {
                    sumValue = consumedSum;
                } else {
                    continue;
                }

                int sumInteger = (int)(sumValue);
                int sumDecimal = (int)std::round((sumValue - sumInteger) * 100);

                QLocale locale(QLocale::Korean, QLocale::SouthKorea);
                QString formattedSumInteger = locale.toString(sumInteger);
                QString formattedSumDecimal = (sumDecimal < 10) ? ("0"+QString::number(sumDecimal))
                                                                : QString::number(sumDecimal);

                QString sumString = formattedSumInteger + "." + formattedSumDecimal;

                cellData.cellText = sumString;
            }
        }

        qreal tableFullWidth = pxContentSize.width();
        TableData sumTableData;
        sumTableData.innerDatas = innerDatas;

        auto orderDetailsInformTable = std::make_unique<TableElement>("", sumTableData, tableFullWidth, sumTableWidthRatio, Qt::AlignLeft);
        orderDetailsInformTable->setElementId(elementIds[5]);
        addElementBelow(std::move(orderDetailsInformTable), QVector<QString>{elementIds[4]}, 10);
    }
    ///
    ///
    ////////


    //////// bottom image
    ///
    ///

    QString imagePath = ":/imageFiles/cfimages.jpg";  // 예시 이미지

    auto logoImage = std::make_unique<ImageElement>(imagePath, true, Qt::AlignHCenter);
    logoImage->setElementId(elementIds[6]);
    addElementBelow(std::move(logoImage), QVector<QString>{elementIds[5]}, 10);

    ///
    ///
    ////////
}
