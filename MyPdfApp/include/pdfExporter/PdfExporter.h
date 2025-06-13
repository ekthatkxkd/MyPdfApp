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

struct PreviewPageSettings {
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

    // DPI 300 을 기준으로 pixel 단위의 A4 SIZE 에 margin 값이 반영된 size 를 초기화.
    void initPxContentsFullSize();

    // DPI 300 을 기준으로 mm 단위를 pixel 단위로 변환.
    qreal convertMMtoPixel(double mmValue);

    bool hasPreview() const;
    int pageCount() const;

    Q_INVOKABLE bool exportToPdf(QQuickItem *rootItem, const QString &filePath);
    Q_INVOKABLE bool generatePreview(QQuickItem *rootItem);

    // 현재 qml 로 구현된 template 종류.
    const QList<QString> templateObjNames = {"materialItemArea", "defectReportItemArea", "orderFormItemArea", "receiptVoucherItemArea"};

    // qml 로 구현된 자재구매 확인서의 주요 objectName.
    const QList<QString> materialObjNames = {"templateTitle", "informTable", "purchaseHistory"};
    // qml 로 구현된 불량발견보고서의 주요 objectName.
    const QList<QString> defectReportObjNames = {"templateTitle", "defectReportNumTable", "placeInformTable", "productionTable", "quantityTotalTable"};
    // qml 로 구현된 발주서의 주요 objectName.
    const QList<QString> orderFromObjNames = {"templateTitle", "orderDataItem", "recipientInformTable", "senderInformTable", "orderDetailsTable"};
    // qml 로 구현된 생산입고전표의 주요 objectName.
    const QList<QString> receiptVouchrObjNames = {"templateTitle", "voucherNumTable", "entranceTable", "productionTable", "consumptionTable", "quantityTotalTable", "bottomImage"};

    // A4 가로 size mm 단위.
    const double mmA4Width = 210.0;
    // A4 세로 size mm 단위.
    const double mmA4Height = 297.0;

    // 25/06/13 - 주의. 현재 DPI 값을 300 외의 다른 값으로 설정할 경우 pdf 에 content 의 위치 및 크기 값이 엉뚱하게 계산됨.
    // 그 이유는 DPI 값이 300 일 경우 pixel 단위로 값을 설정해야 하고
    // 다른 값일 경우 pixel 이 아닌 point 단위로 값을 설정해야 하기 때문.
    // 위 문제 사항을 해결하기 위해선 시간이 오래 걸릴 것으로 판단되어 기능 구현은 보류.
    const int pdfDefaultDPI = 300;

    const int mmPdfDefaultMargin = 15;

    static const int PREVIEW_PIXEL_WIDTH = 600;
    static const int PREVIEW_PIXEL_HEIGHT = 848;

private :
    // table 을 구성하고 있는 모든 cell 각각의 정보들을 저장.
    QVector<QVector<CellData>> getCellDatas(QQuickItem *tableItem, const QString &repeaterObjName);

    // qml 에 구현된 주요 template component 들 가져오기.
    QHash<QString, QQuickItem*> getChildItems(QQuickItem *rootItem, const QList<QString> childObjNames, const bool isInside = false);
    // rootItem Component 의 1 depth 범위 내에서 특정 component 가져오기.
    QQuickItem* getChildItem(QQuickItem *rootItem, const QString &childObjName);
    // rootItem Component 의 모든 depth 범위 내에서 특정 component 가져오기.
    QQuickItem* getInnerItem(QQuickItem *rootItem, const QString &objNameToFound);

    // pdf writer 의 기본 설정.
    void setDefaultPdfEnvironment(std::shared_ptr<QPdfWriter> pdfWriter);
    // 기본 font 값 설정.
    void setFont(QPainter &painter, int fontSize = 10, bool isBold = false);

    // table 을 구성하고 있는 행과 열의 size 를 계산.
    void initTableCellSizes(QPainter &painter, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights, const QVector<QVector<CellData>> &cellDatas,
                            const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio);

    // cell 내부의 존재하는 text 영역 계산.
    QSizeF getCalculatedCellTextArea(const QString &text, const QFont &font, const bool isVertical = false, const QTextOption::WrapMode &wrapMode = QTextOption::NoWrap, qreal fixedWidth = 0);

    // template 의 가장 상단의 title 그리기.
    QRectF drawTemplateTitle(QPainter &painter, QQuickItem *textItem);
    // template 의 table 그리기.
    QRectF drawTemplateTable(QPainter &painter, QQuickItem *tableItem, QPointF &cursorPoint, const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio, const bool &isPdf);
    // template 의 image 그리기. 현재는 image 를 horizontal center 위치를 고정시켜 그리도록 구현되어 있음.
    QRectF drawTemplateImage(QPainter &painter, QQuickItem *imageItem, QPointF &cursorPoint, const bool &isPdf, const int settingPxImageWidth = 0, const bool isHCenter = false);

    // 자재구매 확인서 그리기
    void drawMaterialTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);
    // 불량발견 보고서 그리기
    void defectReportTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);
    // 발주서 그리기
    void orderFormTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);
    // 생산입고전표 그리기
    void drawReceiptVoucherTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf);

    // item 을 그리던 중 A4 영역을 벗어났을 경우 새로운 page 생성.
    void createNewPage(QPainter &painter, const bool &isPdf);

    PreviewPageSettings calculatePreviewPageSettings() const;
    QImage createPdfSizedImage() const;

    const int templateItemSpacing = 20;  // 위 아래 content 사이의 margin
    const int cellTextMargins = 5;
    const int marginBetweenTables = 20;  // 테이블 사이의 margin
    const double MM_TO_INCH = 1.0 / 25.4;

    PreviewImageProvider* m_imageProvider;
    std::shared_ptr<QPdfWriter> pdfWriter;
    QList<std::shared_ptr<QImage>> previewPages;

    // A4 전체 size 에서 margin 이 반영된 size. pixel 단위이고 이 객체에 정의된 size 가 item 을 그릴 영역에 해당.
    QSizeF pxContentsFullSize{0, 0};

    // 미리보기에 보여줄 QImage 생성 완료 여부.
    bool m_hasPreview = false;
    // 미리보기에 보여줄 page 갯수.
    int m_pageCount = 0;

signals :
    void hasPreviewChanged();
    void pageCountChanged();
    void previewUpdated();
};

#endif // PDFEXPORTER_H
