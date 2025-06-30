#ifndef RECEIPTVOUCHERDOCTEMPLATE_H
#define RECEIPTVOUCHERDOCTEMPLATE_H

#include "include/docTemplate/DocTemplate.h"

class ReceiptVoucherDocTemplate : public DocTemplate {
public :
    ReceiptVoucherDocTemplate();

    const QVector<QString> elementIds{"title", "templateInform", "wareHouseInform", "productionItems", "consumedItems", "sumTable", "bottomImage"};

    virtual void setupTemplate(const QSizeF &pxContentSize) override;

private :
    std::shared_ptr<IDataProvider> dataProvider;

    QVector<qreal> templateInformTableWidthRatio{0.2, 0.8};
    QVector<qreal> wareHouseInformTableWidthRatio{0.2, 0.8};
    QVector<qreal> itemsTableWidthRatio{0.05, 0.15, 0.5, 0.1, 0.2};
    QVector<qreal> sumTableWidthRatio{0.1, 0.25, 0.1, 0.25, 0.1, 0.2};

    QVector<QVector<CellData>> templateInformTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "전표번호", Qt::lightGray, true, 10, "center"),
            CellData("templateNum", 0, 1, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "DATE", Qt::lightGray, true, 10, "center"),
            CellData("date", 1, 1, 1, 1, "", Qt::white, false, 10, "left")
        }
    };

    QVector<QVector<CellData>> wareHouseInformTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "출고처", Qt::lightGray, true, 10, "center"),
            CellData("delivery", 0, 1, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "입고처", Qt::lightGray, true, 10, "center"),
            CellData("store", 1, 1, 1, 1, "", Qt::white, false, 10, "left")
        }
    };

    QVector<CellData> itemTableHeaderDatas {
        CellData("", 0, 0, 1, 1, "구분", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 1, 1, 1, "품목코드", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 2, 1, 1, "품목명[규격]", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 3, 1, 1, "수량", Qt::lightGray, true, 10, "center"),
        CellData("", 0, 4, 1, 1, "적요", Qt::lightGray, true, 10, "center"),
    };

    // 생산, 소모 공통 사용
    QVector<CellData> defaultItemTableRowDatas {
        CellData("division", 0, 0, 1, 1, "", Qt::white, false, 10, "left"),
        CellData("productCode", 0, 1, 1, 1, "", Qt::white, false, 10, "left"),
        CellData("productName", 0, 2, 1, 1, "", Qt::white, false, 10, "left"),
        CellData("quantity", 0, 3, 1, 1, "", Qt::white, false, 10, "right"),
        CellData("delivery", 0, 4, 1, 1, "", Qt::white, false, 10, "right")
    };
    // 생산, 소모 공통 사용
    QVector<QVector<CellData>> itemTableInnerDatas {
    };

    // 생산, 소모 공통 사용
    QVector<CellData> defaultItemTableFooterDatas {
        CellData("", 0, 0, 1, 3, "총합계", Qt::lightGray, true, 10, "center"),
        CellData("sum", 0, 3, 1, 1, "", Qt::lightGray, true, 10, "right"),
        CellData("", 0, 4, 1, 1, "", Qt::lightGray, true, 10, "right"),
    };

    QVector<QVector<CellData>> sumTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "생산수량", Qt::lightGray, true, 10, "center"),
            CellData("productionQuantity", 0, 1, 1, 1, "", Qt::white, false, 10, "right"),
            CellData("", 0, 2, 1, 1, "소모수량", Qt::lightGray, true, 10, "center"),
            CellData("consumedQuantity", 0, 3, 1, 1, "", Qt::white, false, 10, "right"),
            CellData("", 0, 4, 1, 1, "인수", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 5, 1, 1, "인", Qt::white, false, 10, "right")
        }
    };
};

#endif // RECEIPTVOUCHERDOCTEMPLATE_H
