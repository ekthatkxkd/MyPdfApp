#include "include/pdfExporter/PdfExporter.h"
#include <QPainter>
#include <QFont>
#include <QVariant>

#include <QDebug>

PdfExporter::PdfExporter(QObject *parent) : QObject(parent) {
    initPxContentsFullSize();
}

PdfExporter::~PdfExporter() {

}

void PdfExporter::initPxContentsFullSize() {
    pxContentsFullSize = QPoint(convertMMtoPixel((mmA4Width - (2 * mmPdfDefaultMargin))),
                                convertMMtoPixel(mmA4Height - (2 * mmPdfDefaultMargin)));
}

qreal PdfExporter::convertMMtoPixel(double mmValue) {
    return (mmValue / 25.4) * pdfDefaultDPI;
}

void PdfExporter::setDefaultPdfEnvironment(QPdfWriter &pdfWriter) {
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMarginsF(mmPdfDefaultMargin, mmPdfDefaultMargin, mmPdfDefaultMargin, mmPdfDefaultMargin), QPageLayout::Millimeter);
    pdfWriter.setResolution(pdfDefaultDPI);
}

void PdfExporter::setDefaultFont(QPainter &painter) {
    QFont font = painter.font();

    font.setPointSize(9);
    font.setBold(false);

    painter.setFont(font);
    painter.setPen(QPen(Qt::black, 2));
}

bool PdfExporter::exportToPdf(QQuickItem *rootItem, const QString &filePath) {
    qDebug() << "[LLDDSS] exportToPdf";

    QPdfWriter pdfWriter(filePath);

    setDefaultPdfEnvironment(pdfWriter);

    QPainter painter;

    if (!painter.begin(&pdfWriter)) {
        qWarning() << "QPdfWriter 초기화 실패";
        return false;
    }

    setDefaultFont(painter);

    painter.end();

    return true;
}

void PdfExporter::testDrawFullRectWithRect(QPainter &painter) {
    QRectF calculatedRect(QPointF(0, 0),
                          QSizeF(pxContentsFullSize.x(),
                                 pxContentsFullSize.y()));

    painter.drawRect(calculatedRect);

    qDebug() << "[LLDDSS] fullArea x : " << calculatedRect.x();
    qDebug() << "[LLDDSS] fullArea y : " << calculatedRect.y();
    qDebug() << "[LLDDSS] fullArea width : " << calculatedRect.width();
    qDebug() << "[LLDDSS] fullArea height : " << calculatedRect.height();
}

void PdfExporter::testDrawFullRectWithWindow(QPainter &painter) {
    QRectF fullArea = painter.window();

    painter.setPen(QPen(Qt::red, 2));
    painter.drawRect(fullArea);

    qDebug() << "[LLDDSS] fullArea x : " << fullArea.x();
    qDebug() << "[LLDDSS] fullArea y : " << fullArea.y();
    qDebug() << "[LLDDSS] fullArea width : " << fullArea.width();
    qDebug() << "[LLDDSS] fullArea height : " << fullArea.height();
}
