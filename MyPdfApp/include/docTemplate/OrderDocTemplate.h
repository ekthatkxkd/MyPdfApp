#ifndef ORDERDOCTEMPLATE_H
#define ORDERDOCTEMPLATE_H

#include "include/docTemplate/DocTemplate.h"

class OrderDocTemplate : public DocTemplate {
public :
    OrderDocTemplate();

    const QVector<QString> elementIds{"title", "orderDate", "recipientInform", "senderInform", "orderDetails"};

    virtual void setupTemplate(const QSizeF &pxContentSize) override;

private :
    std::shared_ptr<IDataProvider> dataProvider;

    const QString recipientInformTableTitle = "■ 수신자 정보";
    QVector<qreal> recipientInformTableWidthRatio{0.2, 0.3, 0.2, 0.3};
    QVector<QVector<CellData>> recipientInformTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "사업자등록번호", Qt::lightGray, true, 10, "center"),
            CellData("registrationNumber", 0, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 0, 2, 1, 1, "거래담당자/TEL", Qt::lightGray, true, 10, "center"),
            CellData("dealerInform", 0, 3, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "회사명/대표", Qt::lightGray, true, 10, "center"),
            CellData("companyInfrom", 1, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 1, 2, 1, 1, "TEL/FAX", Qt::lightGray, true, 10, "center"),
            CellData("telFax", 1, 3, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 2, 0, 1, 1, "주  소", Qt::lightGray, true, 10, "center"),
            CellData("address", 2, 1, 1, 3, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 3, 0, 1, 1, "납기희망일", Qt::lightGray, true, 10, "center"),
            CellData("hopeDay", 3, 1, 1, 3, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 4, 0, 1, 1, "참고사항", Qt::lightGray, true, 10, "center"),
            CellData("note", 4, 1, 1, 3, "", Qt::white, false, 10, "left")
        },
    };

    const QString senderInformTableTitle = "■ 발신자 정보";
    QVector<qreal> senderInformTableWidthRatio{0.2, 0.3, 0.2, 0.3};
    QVector<QVector<CellData>> senderInformTableInnerDatas {
        {
            CellData("", 0, 0, 1, 1, "발주번호", Qt::lightGray, true, 10, "center"),
            CellData("orderNumber", 0, 1, 1, 3, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 1, 0, 1, 1, "사업자등록번호", Qt::lightGray, true, 10, "center"),
            CellData("registrationNumber", 1, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 1, 2, 1, 1, "거래담당자", Qt::lightGray, true, 10, "center"),
            CellData("dealerInform", 1, 3, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 2, 0, 1, 1, "회사명/대표", Qt::lightGray, true, 10, "center"),
            CellData("companyInfrom", 2, 1, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("", 2, 2, 1, 1, "TEL/FAX", Qt::lightGray, true, 10, "center"),
            CellData("telFax", 2, 3, 1, 1, "", Qt::white, false, 10, "left")
        },
        {
            CellData("", 3, 0, 1, 1, "주  소", Qt::lightGray, true, 10, "center"),
            CellData("address", 3, 1, 1, 3, "", Qt::white, false, 10, "left")
        },
    };

    const QString orderDetailsTableTitle = "■ 발주 내용";
    QVector<qreal> orderDetailsTableWidthRatio{0.05, 0.1, 0.15, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    QVector<CellData> orderDetailsHeaderDatas {
        {
            CellData("", 0, 0, 1, 1, "NO", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 1, 1, 1, "품목코드", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 2, 1, 1, "품목명[규격]", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 3, 1, 1, "수량", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 4, 1, 1, "단가", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 5, 1, 1, "공급가액", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 6, 1, 1, "부가세", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 7, 1, 1, "합계", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 8, 1, 1, "적요", Qt::lightGray, true, 10, "center")
        },
    };
    QVector<CellData> defaultOrderDetailsRowDatas {
        {
            CellData("number", 0, 0, 1, 1, "", Qt::white, false, 10, "center"),
            CellData("code", 0, 1, 1, 1, "", Qt::white, false, 10, "center"),
            CellData("name", 0, 2, 1, 1, "", Qt::white, false, 10, "left"),
            CellData("quantity", 0, 3, 1, 1, "", Qt::white, false, 10, "center"),
            CellData("unitPrice", 0, 4, 1, 1, "", Qt::white, false, 10, "center"),
            CellData("publicMoney", 0, 5, 1, 1, "", Qt::white, false, 10, "right"),
            CellData("surtax", 0, 6, 1, 1, "", Qt::white, false, 10, "right"),
            CellData("allSum", 0, 7, 1, 1, "", Qt::white, false, 10, "center"),
            CellData("details", 0, 8, 1, 1, "", Qt::white, false, 10, "right")
        },
    };
    QVector<QVector<CellData>> orderDetailsTableInnerDatas {
    };
    QVector<CellData> orderDetailsFooterDatas {
        {
            CellData("", 0, 0, 1, 3, "합계(VAT포함)", Qt::lightGray, true, 10, "center"),
            CellData("quantity", 0, 3, 1, 1, "", Qt::lightGray, true, 10, "center"),
            CellData("unitPrice", 0, 4, 1, 1, "", Qt::lightGray, true, 10, "center"),
            CellData("publicMoney", 0, 5, 1, 1, "", Qt::lightGray, true, 10, "center"),
            CellData("surtax", 0, 6, 1, 1, "", Qt::lightGray, true, 10, "center"),
            CellData("sum", 0, 7, 1, 1, "", Qt::lightGray, true, 10, "center"),
            CellData("", 0, 8, 1, 1, "", Qt::lightGray, true, 10, "center")
        },
    };
};

#endif // ORDERDOCTEMPLATE_H
