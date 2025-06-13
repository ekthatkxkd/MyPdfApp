#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QQuickItem>
#include <QTextLayout>
#include <QTextOption>
#include <QPdfWriter>
#include <QQuickImageProvider>
#include <QPainter>
#include <QImage>
#include <QPageSize>
#include <QList>
#include <QtMath>

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

struct PdfPageSettings {
    QSizeF pageSizeInMM;      // 페이지 크기 (밀리미터)
    QMarginsF marginsInMM;    // 여백 (밀리미터)
    int resolution;           // DPI
    QSize imageSize;          // 전체 이미지 크기 (픽셀)
    QSize contentSize;        // 여백 제외한 내용 영역 크기 (픽셀)
    int marginPixels;         // 여백 픽셀 크기
};

class PreviewImageProvider : public QQuickImageProvider
{
public:
    PreviewImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    void updatePreviewImages(QList<std::shared_ptr<QImage>> &images);
    int getPageCount() const { return m_previewPages.size(); }

private:
    QList<std::shared_ptr<QImage>> m_previewPages;
};

class PdfExporter : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasPreview READ hasPreview NOTIFY hasPreviewChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
public :
    explicit PdfExporter(PreviewImageProvider* provider, QObject *parent = nullptr);
    ~PdfExporter();

    void initPxContentsFullSize();

    qreal convertMMtoPixel(double mmValue);

    bool hasPreview() const;
    int pageCount() const;

    Q_INVOKABLE bool exportToPdf(QQuickItem *rootItem, const QString &filePath);
    Q_INVOKABLE bool generatePreview(QQuickItem *rootItem);

    const QList<QString> templateObjNames = {"materialItemArea", "defectReportItemArea", "orderFormItemArea", "receiptVoucherItemArea"};

    const QList<QString> materialObjNames = {"templateTitle", "informTable", "purchaseHistory"};
    const QList<QString> defectReportObjNames = {"templateTitle", "defectReportNumTable", "placeInformTable", "productionTable", "quantityTotalTable"};
    const QList<QString> orderFromObjNames = {"templateTitle", "orderDataItem", "recipientInformTable", "senderInformTable", "orderDetailsTable"};
    const QList<QString> receiptVouchrObjNames = {"templateTitle", "voucherNumTable", "entranceTable", "productionTable", "consumptionTable", "quantityTotalTable", "bottomImage"};

    const double mmA4Width = 210.0;  // a4 full size
    const double mmA4Height = 297.0;  // a4 full size

    const int pdfDefaultDPI = 300;
    const int mmPdfDefaultMargin = 15;

    static const int PREVIEW_PIXEL_WIDTH = 600;
    static const int PREVIEW_PIXEL_HEIGHT = 848;
    static const qreal A4_WIDTH_PTS;
    static const qreal A4_HEIGHT_PTS;
    static const qreal MARGIN_PTS;  // == mmPdfDefaultMargin
    static const qreal CELL_PADDING_PTS;  // == cellTextMargins = 5;

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

    void setDefaultPdfEnvironment(std::shared_ptr<QPdfWriter> pdfWriter);
    void setFont(QPainter &painter, int fontSize = 10, bool isBold = false);

    void initTableCellSizes(QPainter &painter, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights, const QVector<QVector<CellData>> &cellDatas,
                            const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio);
    QSizeF getCalculatedCellTextArea(const QString &text, const QFont &font, const bool isVertical = false, const QTextOption::WrapMode &wrapMode = QTextOption::NoWrap, qreal fixedWidth = 0);

    QRectF drawTemplateTitle(QPainter &painter, QQuickItem *textItem);
    QRectF drawTemplateTable(QPainter &painter, QQuickItem *tableItem, QPointF &cursorPoint, const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio, const bool &isPdf);
    QRectF drawTemplateImage(QPainter &painter, QQuickItem *imageItem, QPointF &cursorPoint, const bool &isPdf, const int settingPxImageWidth = 0, const bool isHCenter = false);

    void drawMaterialTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);
    void defectReportTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);
    void orderFormTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);
    void drawReceiptVoucherTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);

    void testDrawFullRectWithRect(QPainter &painter);
    void testDrawFullRectWithWindow(QPainter &painter);
    void testOutputCellDatas(const CellData &cellData);
    void testDrawTable(QPainter &painter, QPointF &cursorPoint, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights);

    void testDrawContent(QPainter& painter, const QSize& size);

    void drawPageFrame(QPainter *painter) const;





    void createNewPage(QPainter &painter, const bool &isPdf);
    std::shared_ptr<QPdfWriter> pdfWriter;
    QList<std::shared_ptr<QImage>> previewPages;






    QSizeF pxContentsFullSize{0, 0};  // A4 전체 pixel size 가 아닌 margin 이 반영된 pixel size.

    PreviewImageProvider* m_imageProvider;
    PdfPageSettings calculatePdfPageSettings() const;
    QImage createPdfSizedImage() const;
    int PDF_RESOLUTION = 300;
    double MM_TO_INCH = 1.0 / 25.4;

    bool m_hasPreview = false;
    int m_pageCount = 0;

signals :
    void hasPreviewChanged();
    void pageCountChanged();
    void previewUpdated();
};

#endif // PDFEXPORTER_H
