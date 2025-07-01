#ifndef MATERIALDOCTEMPLATE_H
#define MATERIALDOCTEMPLATE_H

#include "include/docTemplate/DocTemplate.h"

// 구체적인 템플릿 클래스들
class MaterialDocTemplate : public DocTemplate {
public:
    MaterialDocTemplate();

    const QVector<QString> elementIds{"title", "informTable", "history"};

    virtual void setupTemplate(const QSizeF &pxContentSize) override;

private:
    // 제목 설정
    void setupTitle();
    
    // 정보 테이블 설정
    void setupInformTable(const QSizeF &pxContentSize);
    void populateInformTableFromDB(TableData &informTableData, QSqlQuery &query);
    void populateInformTableWithTestData(TableData &informTableData);
    
    // 히스토리 테이블 설정
    void setupHistoryTable(const QSizeF &pxContentSize);
    void populateHistoryDataFromDB(QList<QVector<QString>> &historyData, QSqlQuery &query);
    void populateHistoryDataWithTestData(QList<QVector<QString>> &historyData);
    void populateHistoryTableData(TableData &historyTableData, const QList<QVector<QString>> &historyData);
    void updateHistoryTableFooter(TableData &historyTableData, double quantitySum);
    
    // db 로부터 전달 받은 자재구매 내역 리스트들을 일자 기준으로 오름차순 정렬.
    void sortReadHistoryFromDB(QList<QVector<QString>> &readDataFromDB);

    const QString historyTableTitle = "▶자재구매 내역";

    std::shared_ptr<IDataProvider> dataProvider;

    QVector<qreal> informTableWidthRatio{0.15, 0.3, 0.05, 0.1, 0.4};
    QVector<QVector<CellData>> informTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "구매번호", Qt::lightGray, true, 10, "center"),
            CellData("number", 0, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 0, 2, 3, 1, "구매처", Qt::lightGray, true, 10, "center", true),
            CellData("", 0, 3, 1, 1, "상호", Qt::lightGray, true, 10, "center"),
            CellData("company", 0, 4, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "구매창고", Qt::lightGray, true, 10, "center"),
            CellData("storage", 1, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 1, 3, 1, 1, "주소", Qt::lightGray, true, 10, "center"),
            CellData("address", 1, 4, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 2, 0, 1, 1, "구매담당자", Qt::lightGray, true, 10, "center"),
            CellData("manager", 2, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 2, 3, 1, 1, "TEL", Qt::lightGray, true, 10, "center"),
            CellData("tel", 2, 4, 1, 1, "", Qt::white, false, 10, "left")
        }
    };

    QVector<qreal> historyWidthRatio{0.1, 0.6, 0.1, 0.2};
    QVector<CellData> historyHeaderDatas {
        CellData("", 0, 0, 1, 1, "일자", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 1, 1, 1, "품목명[규격명]", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 2, 1, 1, "수량", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 3, 1, 1, "적요", Qt::lightGray, true, 10, "center")
    };
    QVector<CellData> defaultHistoryInnerRowDatas {
        CellData("date", 0, 0, 1, 1, "", Qt::white, false, 10, "center"),
        CellData("itemName", 0, 1, 1, 1, "", Qt::white, false, 10, "left"),
        CellData("quantity", 0, 2, 1, 1, "", Qt::white, false, 10, "right"),
        CellData("briefs", 0, 3, 1, 1, "", Qt::white, false, 10, "right")
    };
    QVector<QVector<CellData>> historyInnerDatas{
    };
    QVector<CellData> historyFooterDatas {
        CellData("", 0, 0, 1, 2, "총합계", Qt::lightGray, true, 10, "center"),
        CellData("sum", 0, 2, 1, 1, "", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 3, 1, 1, "", Qt::lightGray, true, 10, "center"),
    };
};

#endif // MATERIALDOCTEMPLATE_H
