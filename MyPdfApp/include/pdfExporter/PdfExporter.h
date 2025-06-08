#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QQuickItem>
#include <QPdfWriter>

class PdfExporter : public QObject {
    Q_OBJECT
public :
    explicit PdfExporter(QObject *parent = nullptr);
    ~PdfExporter();

    Q_INVOKABLE bool exportToPdf(QQuickItem *rootItem, const QString &filePath);

    const double mmA4Width = 210.0;
    const double mmA4Height = 297.0;

    const int pdfDefaultDPI = 300;
    const int mmPdfDefaultMargin = 15;

private :
    void initPxContentsFullSize();

    qreal convertMMtoPixel(double mmValue);

    void setDefaultPdfEnvironment(QPdfWriter &pdfWriter);
    void setDefaultFont(QPainter &painter);

    void testDrawFullRectWithRect(QPainter &painter);
    void testDrawFullRectWithWindow(QPainter &painter);

    QPointF pxContentsFullSize{0, 0};
};

#endif // PDFEXPORTER_H
