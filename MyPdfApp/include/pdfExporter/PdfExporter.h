#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QQuickItem>
#include <QPdfWriter>

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

    const QList<QString> templateObjNames = {"materialItemArea"};
    const QList<QString> materialObjNames = {"templateTitle", "informTable"};

    const double mmA4Width = 210.0;  // a4 full size
    const double mmA4Height = 297.0;  // a4 full size

    const int pdfDefaultDPI = 300;
    const int mmPdfDefaultMargin = 15;

private :
    TableCellData convertToTableCellData(const QVariantMap &map);
    QList<TableCellData> convertToRowData(const QVariantList &rowList);
    QList<QList<TableCellData>> getCellDatas(QQuickItem *tableItem, const QString &propertyName);

    QHash<QString, QQuickItem*> getChildItems(QQuickItem *rootItem, const QList<QString> childObjNames);
    QQuickItem* getChildItem(QQuickItem *rootItem, const QString &childObjName);
    QQuickItem* getInnerItem(QQuickItem *rootItem, const QString &objNameToFound);

    void setDefaultPdfEnvironment(QPdfWriter &pdfWriter);
    void setFont(QPainter &painter, int fontSize = 12, bool isBold = false);

    QRectF drawTemplateTitle(QPainter &painter, QQuickItem *textItem);
    QRectF drawTemplateTable(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *tableItem, QPointF &cursorPoint);

    void drawMaterialTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem);

    void testDrawFullRectWithRect(QPainter &painter);
    void testDrawFullRectWithWindow(QPainter &painter);
    void testOutputCellDatas(const TableCellData &cellData);

    QSizeF pxContentsFullSize{0, 0};  // A4 전체 pixel size 가 아닌 margin 이 반영된 pixel size.
};

#endif // PDFEXPORTER_H
