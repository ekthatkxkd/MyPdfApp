#ifndef DEFECTREPORTDOCTEMPLATE_H
#define DEFECTREPORTDOCTEMPLATE_H

#include "include/drawingtemplate/DocumentTemplate.h"

// 구체적인 템플릿 클래스들
class DefectReportDocTemplate : public DocumentTemplate {
public:
    DefectReportDocTemplate();

    const QVector<QString> elementIds{"title", "dateNumberTable", "factoryTable", "productList", "sumTable"};

    virtual void setupTemplate(const QSizeF &pxContentSize) override;

private:
    std::shared_ptr<IDataProvider> dataProvider;

    QVector<qreal> numberTableWidthRatio{0.2, 0.8};
    QVector<qreal> factoryTableWidthRatio{0.2, 0.8};
    QVector<qreal> productListWidthRatio{0.05, 0.25 ,0.36, 0.12, 0.12, 0.1};
    QVector<qreal> sumTableWidthRatio{0.1, 0.25, 0.1, 0.25, 0.1, 0.2};

    QVector<QVector<CellData>> numberTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "전표번호", Qt::lightGray, true, 10, "center"),
            CellData("dateNumber", 0, 1, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "처리방법", Qt::lightGray, true, 10, "center"),
            CellData("processing", 1, 1, 1, 1, "", Qt::white, false, 10, "left")
        }
    };

    QVector<QVector<CellData>> factoryTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "담당자", Qt::lightGray, true, 10, "center"),
            CellData("manager", 0, 1, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "발견창고", Qt::lightGray, true, 10, "center"),
            CellData("factory", 1, 1, 1, 1, "", Qt::white, false, 10, "left")
        }
    };

    QVector<CellData> productListHeaderDatas {
        CellData("", 0, 0, 1, 1, "순번", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 1, 1, 1, "품목코드", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 2, 1, 1, "품목명[규격]", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 3, 1, 1, "감소수량", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 4, 1, 1, "증가수량", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 5, 1, 1, "적요", Qt::lightGray, true, 10, "center")
    };

    QVector<CellData> defaultProductListInnerRowDatas {
        CellData("number", 0, 0, 1, 1, "", Qt::white, false, 10, "center"),
        CellData("productCode", 0, 1, 1, 1, "", Qt::white, false, 10, "left"),
        CellData("productName", 0, 2, 1, 1, "", Qt::white, false, 10, "left"),
        CellData("reducedQuantity", 0, 3, 1, 1, "", Qt::white, false, 10, "right"),
        CellData("increaseQuantity", 0, 4, 1, 1, "", Qt::white, false, 10, "right"),
        CellData("briefs", 0, 5, 1, 1, "", Qt::white, false, 10, "right")
    };
    QVector<QVector<CellData>> productListInnerDatas{
    };

    QVector<QVector<CellData>> sumTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "감소수량", Qt::lightGray, true, 10, "center"),
            CellData("reduceTotal", 0, 1, 1, 1, "", Qt::white, false, 10, "right"),
            CellData("", 0, 2, 1, 1, "증가수량", Qt::lightGray, true, 10, "center"),
            CellData("increaseTotal", 0, 3, 1, 1, "", Qt::white, false, 10, "right"),
            CellData("", 0, 4, 1, 1, "인수", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 5, 1, 1, "인", Qt::white, false, 10, "right")
        }
    };
};

#endif // DEFECTREPORTDOCTEMPLATE_H
