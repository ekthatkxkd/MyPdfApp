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
    pxContentsFullSize = QSizeF(convertMMtoPixel((mmA4Width - (2 * mmPdfDefaultMargin))),
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

void PdfExporter::setFont(QPainter &painter, int fontSize, bool isBold) {
    QFont font = painter.font();

    font.setPointSize(fontSize);
    font.setBold(isBold);

    painter.setFont(font);
    painter.setPen(QPen(Qt::black, 2));
}

QHash<QString, QQuickItem*> PdfExporter::getChildItems(QQuickItem *rootItem, const QList<QString> childObjNames) {
    QHash<QString, QQuickItem*> childItems;

    if (rootItem != nullptr) {
        for (QString childObjName : childObjNames) {
            childItems[childObjName] = getChildItem(rootItem, childObjName);
        }
    }

    return childItems;
}

QQuickItem* PdfExporter::getChildItem(QQuickItem *rootItem, const QString &childObjName) {
    // Searches only up to 1 depth.

    QQuickItem *retChildItem = nullptr;

    if (rootItem != nullptr) {
        const QList<QQuickItem*> childItems = rootItem->childItems();

        for (QQuickItem *childItem : childItems) {
            const QString objectName = childItem->objectName();

            if (objectName == childObjName) {
                retChildItem = childItem;
                break;
            }
        }
    }

    return retChildItem;
}

QQuickItem* PdfExporter::getInnerItem(QQuickItem *rootItem, const QString &objNameToFound) {
    // Search all depths within root.
    QQuickItem *retInnerItem = nullptr;

    if (rootItem != nullptr) {
        const QList<QQuickItem*> childItems = rootItem->childItems();

        for (QQuickItem *childItem : childItems) {
            const QString childObjName = childItem->objectName();

            if (childObjName == objNameToFound) {
                retInnerItem = childItem;
                break;
            } else {
                retInnerItem = getInnerItem(childItem, objNameToFound);

                if (retInnerItem != nullptr) {
                    break;
                }
            }
        }
    }

    return retInnerItem;
}

QRectF PdfExporter::drawTemplateTitle(QPainter &painter, QQuickItem *textItem) {
    QRectF titleArea(QPointF(0, 0), QSizeF(0, 0));
    QString titleText = textItem->property("text").toString();

    setFont(painter, 20, true);

    QFontMetrics metrics(painter.font());
    QRectF boundingBox(QPointF(0, 0), QSizeF(pxContentsFullSize.width(), metrics.height()));

    Qt::AlignmentFlag align = Qt::AlignHCenter;

    painter.drawRect(boundingBox);  // [LLDDSS] FOR TEST DELETE
    painter.drawText(boundingBox, align, titleText);

    return titleArea;
}

void PdfExporter::drawMaterialTemplate(QPainter &painter, QQuickItem *rootItem) {
    QPointF cursorPoint(0, 0);

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, materialObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "[LLDDSS] drawing failed...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[materialObjNames[0]] != nullptr) {
        setFont(painter, 20, true);
        drawTemplateTitle(painter, templateQuickItems[materialObjNames[0]]);
    }
    ///
    ///
    ////////

    setFont(painter);
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

    setFont(painter);

    if (rootItem->objectName() == templateObjNames[0]) {
        drawMaterialTemplate(painter, rootItem);
    } else {

    }

    painter.end();

    return true;
}

void PdfExporter::testDrawFullRectWithRect(QPainter &painter) {
    QRectF calculatedRect(QPointF(0, 0),
                          QSizeF(pxContentsFullSize.width(),
                                 pxContentsFullSize.height()));

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
