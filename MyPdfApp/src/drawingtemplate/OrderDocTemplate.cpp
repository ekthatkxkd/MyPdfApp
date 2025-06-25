#include "include/drawingtemplate/OrderDocTemplate.h"
#include "include/docElement/TextElement.h"
#include "include/docElement/TableElement.h"

OrderDocTemplate::OrderDocTemplate() : DocumentTemplate("Order", "발 주 서") {
    // 데이터베이스 연결 설정
    dataProvider = std::make_shared<SqliteDataProvider>("database.db", "template_a_conn");

    if (dataProvider->connect()) {
        // 미리 정의된 쿼리들 등록
        dataProvider->registerTableQuery("inform_table",
                                         "SELECT product_name, quantity, price, total FROM sales_summary ORDER BY total DESC");
    }
}

void OrderDocTemplate::setupTemplate(const QSizeF &pxContentSize) {
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
            //////// order date
            ///
            ///

            QString readDataFromDB;
            readDataFromDB = "2025-03-14";
            QString orderDateText = "발주일자 : " + readDataFromDB;

            QFont font;
            font.setPointSize(10);
            font.setBold(false);

            auto orderDateTextElement = std::make_unique<TextElement>(orderDateText,
                                                                      font,
                                                                      Qt::AlignRight);
            orderDateTextElement->setElementId(elementIds[1]);
            addElementBelow(std::move(orderDateTextElement), QVector<QString>{elementIds[0]}, 10);

            ///
            ///
            ////////
        }

        {
            //////// recipient Inform Table
            ///
            ///
            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("registrationNumber", "104-81-73274"));
            readDataFromDB.push_back(QPair<QString, QString>("dealerInform", "홍길동 / 010-0000-0000"));
            readDataFromDB.push_back(QPair<QString, QString>("companyInfrom", "테스트회사(주) / 김민수"));
            readDataFromDB.push_back(QPair<QString, QString>("telFax", "031-000-0000 / 02-0000-0000"));
            readDataFromDB.push_back(QPair<QString, QString>("address", "경기도 수원시 영통구 매탄동 1170-00 1004호"));
            readDataFromDB.push_back(QPair<QString, QString>("hopeDay", "2025/01/01"));
            readDataFromDB.push_back(QPair<QString, QString>("note", "참고 사항 테스트 코드. 가나다라마바사 아자차카 타파하. ABCDEFG HIJKLMN OPQRSTU VWXYZ."));
            ////////

            QVector<QVector<CellData>> innerDatas = recipientInformTableInnerDatas;

            int dataCount = 0;
            while (dataCount < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[dataCount];

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

                dataCount++;
            }

            qreal tableFullWidth = pxContentSize.width();
            TableData recipientInformTableData;
            recipientInformTableData.title = recipientInformTableTitle;
            recipientInformTableData.innerDatas = innerDatas;

            auto recipientInformTable = std::make_unique<TableElement>(recipientInformTableTitle, recipientInformTableData, tableFullWidth, recipientInformTableWidthRatio, Qt::AlignLeft);
            recipientInformTable->setElementId(elementIds[2]);
            addElementBelow(std::move(recipientInformTable), QVector<QString>{elementIds[0]}, 10);
            ///
            ///
            ////////
        }

        {
            //////// sender Inform Table
            ///
            ///
            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QPair<QString, QString>> readDataFromDB;
            readDataFromDB.push_back(QPair<QString, QString>("orderNumber", "O2025A00340"));
            readDataFromDB.push_back(QPair<QString, QString>("registrationNumber", "690-00-00000"));
            readDataFromDB.push_back(QPair<QString, QString>("dealerInform", "이영수"));
            readDataFromDB.push_back(QPair<QString, QString>("companyInfrom", "(주)XX회사 / 김철수 21"));
            readDataFromDB.push_back(QPair<QString, QString>("telFax", "031-000-0000 / 031-9999-9999"));
            readDataFromDB.push_back(QPair<QString, QString>("address", "경기도 안양시 동안구 학의로 250, 관양벤처두산다임 101, 102, 103호"));
            ////////

            QVector<QVector<CellData>> innerDatas = senderInformTableInnerDatas;

            int dataCount = 0;
            while (dataCount < readDataFromDB.size()) {
                QPair<QString, QString> readData = readDataFromDB[dataCount];

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

                dataCount++;
            }


            qreal tableFullWidth = pxContentSize.width();
            TableData senderInformTableData;
            senderInformTableData.title = senderInformTableTitle;
            senderInformTableData.innerDatas = innerDatas;

            auto senderInformTable = std::make_unique<TableElement>(senderInformTableTitle, senderInformTableData, tableFullWidth, senderInformTableWidthRatio, Qt::AlignLeft);
            senderInformTable->setElementId(elementIds[3]);
            addElementBelow(std::move(senderInformTable), QVector<QString>{elementIds[2]}, 10);

            ///
            ///
            ////////
        }

        {
            //////// order Details Table
            ///
            ///

            //////// inner table data 정의
            ///
            ///
            QSqlQuery tableQuery = dataProvider->getTableData("inform_table");
            QSqlRecord record = tableQuery.record();
            //////// [LLDDSS] 테스트 코드.
            QList<QVector<QString>> readDataFromDB;
            readDataFromDB.push_back(QVector<QString>{"", "L005-0061", "이태리 전시회 리플렛 [210X297, 아트지 150g, 양면인쇄]", "500.00", "320.00", "160,000.00", "16,000.00", "", "테스트 코드."});
            readDataFromDB.push_back(QVector<QString>{"", "L005-0071", "테스트 제품 [1]", "10,000.00", "1,000.00", "200.00", "30,000.00", "", ""});
            readDataFromDB.push_back(QVector<QString>{"", "L005-0081", "MSI GeForce RTX 3060 VENTUS 2X 12G OC 게이밍 그래픽 카드 12GB GDDR6 192 비트 HDMI DP PCI-E 4.0 8 핀 데스크탑 풀 뉴 비디", "200.00", "1,000.00", "5.00", "2,000.00", "", "테스트 코드"});
            readDataFromDB.push_back(QVector<QString>{"", "L005-0091", "인텔 코어i5-14세대 14400F (랩터레이크 리프레시) (밸류팩 정품)", "1.10", "10,000.00", "5.00", "1,000.00", "", "참고 사항 테스트 코드. 가나다라마바사 아자차카 타파하. ABCDEFG HIJKLMN OPQRSTU VWXYZ."});
            readDataFromDB.push_back(QVector<QString>{"", "L005-0101", "AMD 라이젠7-6세대 9700X (그래니트 릿지) (정품)", "10,000.09", "100,000.00", "20,000.00", "100.00", "", ""});
            ////////

            QVector<QVector<CellData>> innerDatas = orderDetailsTableInnerDatas;

            int dataRowIndex = innerDatas.size();
            double quantitySum = 0.0;
            double unitPriceSum = 0.0;
            double publicMoneySum = 0.0;
            double surtaxSum = 0.0;
            double footerSum = 0.0;

            for (auto &item : readDataFromDB) {
                QVector<CellData> rowDatas = defaultOrderDetailsRowDatas;

                // "합계" 의 경우 db 에 저장된 값인지 코드 내부적으로 계산해야 할 값인지 파악 못함. 우선 코드 내부적으로 계산하는 방향으로 구현.
                // 따라서 readDataFromDB 에 "합계" 에 해당되는 값은 "" 이렇게 초기화 함.
                int allSumIndex = -1;
                double allSumSum = 0.0;

                for (int informIndex = 0; informIndex < rowDatas.size(); informIndex++) {

                    rowDatas[informIndex].startRow = dataRowIndex;

                    if (rowDatas[informIndex].cellId == "number") {
                        rowDatas[informIndex].cellText = QString::number(dataRowIndex + 1);
                    } else if (rowDatas[informIndex].cellId == "code" ||
                               rowDatas[informIndex].cellId == "name" ||
                               rowDatas[informIndex].cellId == "details") {
                        rowDatas[informIndex].cellText = item[informIndex];
                    } else if (rowDatas[informIndex].cellId == "allSum") {
                        allSumIndex = informIndex;
                    } else {
                        rowDatas[informIndex].cellText = item[informIndex];

                        double value = 0.0;

                        if (item[informIndex] != "") {
                            QString cleanedStr = item[informIndex];
                            cleanedStr.remove(',');
                            value = cleanedStr.toDouble();
                        }

                        if (rowDatas[informIndex].cellId == "quantity") {
                            quantitySum += value;
                        } else if (rowDatas[informIndex].cellId == "unitPrice") {
                            unitPriceSum += value;
                        } else if (rowDatas[informIndex].cellId == "publicMoney") {
                            publicMoneySum += value;
                        } else if (rowDatas[informIndex].cellId == "surtax") {
                            surtaxSum += value;
                        }

                        allSumSum += value;
                    }
                }

                footerSum += allSumSum;

                //////// allSum format 계산 후 값 대입.
                ///
                ///
                int allSumInteger = (int)(allSumSum);
                int allSumDecimal = (int)std::round((allSumSum - allSumInteger) * 100);

                QLocale locale(QLocale::Korean, QLocale::SouthKorea);
                QString formattedAllSumInteger = locale.toString(allSumInteger);
                QString formattedAllSumDecimal = (allSumDecimal < 10) ? ("0"+QString::number(allSumDecimal))
                                                                      : QString::number(allSumDecimal);
                QString allSumString = formattedAllSumInteger + "." + formattedAllSumDecimal;

                if (allSumIndex != -1) {
                    rowDatas[allSumIndex].cellText = allSumString;
                }
                ///
                ///
                ////////

                innerDatas.push_back(rowDatas);
                dataRowIndex++;
            }
            ///
            ///
            ////////


            //////// footer table data 정의
            ///
            ///
            QVector<CellData> footerDatas = orderDetailsFooterDatas;

            for (auto &cellData : footerDatas) {
                if (cellData.cellId == "") {
                    continue;
                }

                double sumValue = 0.0;

                if (cellData.cellId == "quantity") {
                    sumValue = quantitySum;
                } else if (cellData.cellId == "unitPrice") {
                    sumValue = unitPriceSum;
                } else if (cellData.cellId == "publicMoney") {
                    sumValue = publicMoneySum;
                } else if (cellData.cellId == "surtax") {
                    sumValue = surtaxSum;
                } else if (cellData.cellId == "sum") {
                    sumValue = footerSum;
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
            ///
            ///
            ////////


            qreal tableFullWidth = pxContentSize.width();
            TableData orderDetailsTableData;
            orderDetailsTableData.title = orderDetailsTableTitle;
            orderDetailsTableData.headerDatas = orderDetailsHeaderDatas;
            orderDetailsTableData.innerDatas = innerDatas;
            orderDetailsTableData.footerDatas = footerDatas;

            auto orderDetailsInformTable = std::make_unique<TableElement>(orderDetailsTableTitle, orderDetailsTableData, tableFullWidth, orderDetailsTableWidthRatio, Qt::AlignLeft);
            orderDetailsInformTable->setElementId(elementIds[4]);
            addElementBelow(std::move(orderDetailsInformTable), QVector<QString>{elementIds[3]}, 10);
        }
    }
}

