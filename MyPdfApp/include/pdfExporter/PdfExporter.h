#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QQuickItem>
#include <QTextLayout>
#include <QTextOption>
#include <QPdfWriter>

#include "include/model/listmodel/CellData.h"

struct TableCellData {
    int startRow, startCol;
    int rowSpan, colSpan;
    QString cellText;
    bool isTextEdit;
    QColor bgColor;
    bool isBold;
    int fontSize;
    QString alignPosition;
    bool isVerticalDir;
};

class PdfExporter : public QObject {
    Q_OBJECT
public :
    explicit PdfExporter(QObject *parent = nullptr);
    ~PdfExporter();

    void initPxContentsFullSize();

    qreal convertMMtoPixel(double mmValue);

    Q_INVOKABLE bool exportToPdf(QQuickItem *rootItem, const QString &filePath);

    const QList<QString> templateObjNames = {"materialItemArea", "defectReportItemArea", "orderFormItemArea", "receiptVoucherItemArea"};

    const QList<QString> materialObjNames = {"templateTitle", "informTable", "purchaseHistory"};
    const QList<QString> defectReportObjNames = {"templateTitle", "defectReportNumTable", "placeInformTable", "productionTable", "quantityTotalTable"};
    const QList<QString> orderFromObjNames = {"templateTitle", "orderDataItem", "recipientInformTable", "senderInformTable", "orderDetailsTable"};
    const QList<QString> receiptVouchrObjNames = {"templateTitle", "voucherNumTable", "entranceTable", "productionTable", "consumptionTable", "quantityTotalTable", "bottomImage"};

    const double mmA4Width = 210.0;  // a4 full size
    const double mmA4Height = 297.0;  // a4 full size

    const int pdfDefaultDPI = 300;
    const int mmPdfDefaultMargin = 15;

private :
    const int templateItemSpacing = 20;
    const int cellTextMargins = 5;

    QVector<QVector<CellData>> getCellDatas(QQuickItem *tableItem, const QString &repeaterObjName);

    // TableCellData convertToTableCellData(const QVariantMap &map);
    // QList<TableCellData> convertToRowData(const QVariantList &rowList);
    // QList<QList<TableCellData>> getCellDatas(QQuickItem *tableItem, const QString &propertyName);

    QHash<QString, QQuickItem*> getChildItems(QQuickItem *rootItem, const QList<QString> childObjNames, const bool isInside = false);
    QQuickItem* getChildItem(QQuickItem *rootItem, const QString &childObjName);
    QQuickItem* getInnerItem(QQuickItem *rootItem, const QString &objNameToFound);

    void setDefaultPdfEnvironment(QPdfWriter &pdfWriter);
    void setFont(QPainter &painter, int fontSize = 10, bool isBold = false);

    void initTableCellSizes(QPainter &painter, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights, const QVector<QVector<CellData>> &cellDatas,
                            const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio);
    QSizeF getCalculatedCellTextArea(const QString &text, const QFont &font, const bool isVertical = false, const QTextOption::WrapMode &wrapMode = QTextOption::NoWrap, qreal fixedWidth = 0);

    QRectF drawTemplateTitle(QPainter &painter, QQuickItem *textItem);
    QRectF drawTemplateTable(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *tableItem, QPointF &cursorPoint, const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio);
    QRectF drawTemplateImage(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *imageItem, QPointF &cursorPoint, const int settingPxImageWidth = 0, const bool isHCenter = false);

    void drawMaterialTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem);
    void defectReportTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem);
    void orderFormTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem);
    void drawReceiptVoucherTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem);

    void testDrawFullRectWithRect(QPainter &painter);
    void testDrawFullRectWithWindow(QPainter &painter);
    void testOutputCellDatas(const CellData &cellData);
    void testDrawTable(QPainter &painter, QPointF &cursorPoint, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights);

    QSizeF pxContentsFullSize{0, 0};  // A4 전체 pixel size 가 아닌 margin 이 반영된 pixel size.
};

#endif // PDFEXPORTER_H
