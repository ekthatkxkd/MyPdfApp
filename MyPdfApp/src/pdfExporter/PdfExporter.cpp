#include "include/pdfExporter/PdfExporter.h"
#include "include/model/listmodel/TableModel.h"
#include <QPainter>
#include <QFont>
#include <QVariant>
#include <QResource>
#include <QPixmap>

#include <QDebug>

const qreal PdfExporter::A4_WIDTH_PTS = 595.27559;
const qreal PdfExporter::A4_HEIGHT_PTS = 841.88976;
const qreal PdfExporter::MARGIN_PTS = 15;
const qreal PdfExporter::CELL_PADDING_PTS = 5;

PreviewImageProvider::PreviewImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
    QImage defaultPage(PdfExporter::PREVIEW_PIXEL_WIDTH,
                       PdfExporter::PREVIEW_PIXEL_HEIGHT,
                       QImage::Format_ARGB32);
    defaultPage.fill(Qt::white);
    m_previewPages.append(defaultPage);
}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id.startsWith("page_")) {
        bool ok;
        int pageIndex = id.mid(5).toInt(&ok);
        if (ok && pageIndex >= 0 && pageIndex < m_previewPages.size()) {
            if (size) {
                *size = m_previewPages[pageIndex].size();
            }
            if (requestedSize.isValid()) {
                // return m_previewPages[pageIndex].scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                // return m_previewPages[pageIndex].scaled(QSize(600, 800), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            // mm to pixel with dpi
            return m_previewPages[pageIndex].scaled(QSize((210*97)/25.4, (297*97)/25.4), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            // return m_previewPages[pageIndex];
        }
    }

    QImage emptyPage(PdfExporter::PREVIEW_PIXEL_WIDTH,
                     PdfExporter::PREVIEW_PIXEL_HEIGHT,
                     QImage::Format_ARGB32);
    emptyPage.fill(Qt::white);
    return emptyPage;
}

void PreviewImageProvider::updatePreviewImages(const QList<QImage> &images)
{
    m_previewPages = images;
}


/////////////////////////////////////////////////////////////////////////

PdfExporter::PdfExporter(PreviewImageProvider* provider, QObject *parent) : QObject(parent), m_imageProvider(provider) {
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

bool PdfExporter::hasPreview() const { return m_hasPreview; }
int PdfExporter::pageCount() const { return m_pageCount; }

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

// TableCellData PdfExporter::convertToTableCellData(const QVariantMap &map) {
//     TableCellData data;

//     data.startRow = map.value("startRow", 0).toInt();
//     data.startCol = map.value("startCol", 0).toInt();
//     data.rowSpan = map.value("rowSpan", 1).toInt();
//     data.colSpan = map.value("colSpan", 1).toInt();
//     data.cellText = map.value("cellText", "").toString();
//     data.isTextEdit = map.value("isTextEdit", false).toBool();
//     data.bgColor = QColor(map.value("bgColor", "white").toString());
//     data.isBold = map.value("isBold", false).toBool();
//     data.fontSize = map.value("fontSize", 12).toInt();
//     data.alignPosition = map.value("alignPosition", "right").toString();
//     data.isVerticalDir = map.value("isVerticalDir", false).toBool();

//     return data;
// }

// QList<TableCellData> PdfExporter::convertToRowData(const QVariantList &rowList) {
//     QList<TableCellData> cellDatas;

//     for (const QVariant &cellVariant : rowList) {
//         if (cellVariant.canConvert<QVariantMap>()) {
//             QVariantMap cellMap = cellVariant.toMap();

//             TableCellData cellData = convertToTableCellData(cellMap);

//             if (cellData.isVerticalDir && !cellData.cellText.contains("\n")) {
//                 cellData.cellText = cellData.cellText.split("").join("\n");

//                 // 앞뒤 개행문자 제거
//                 if (cellData.cellText.startsWith('\n')) {
//                     cellData.cellText = cellData.cellText.mid(1);
//                 }
//                 if (cellData.cellText.endsWith('\n')) {
//                     cellData.cellText.chop(1); // 마지막 문자 제거
//                 }
//             }

//             cellDatas.append(cellData);
//         }
//     }

//     return cellDatas;
// }

// QList<QList<TableCellData>> PdfExporter::getCellDatas(QQuickItem *tableItem, const QString &propertyName) {
//     QList<QList<TableCellData>> cellDatas;

//     QVariantList varCellDatas = tableItem->property(propertyName.toLocal8Bit().data()).toList();

//     if (!varCellDatas.isEmpty()) {
//         const QVariant &firstElement = varCellDatas.first();

//         bool isDoubleArray = firstElement.canConvert<QVariantList>();

//         if (isDoubleArray) {
//             // innerData 와 같이 이차 배열의 cellData 를 읽어오기
//             for (int i = 0; i < varCellDatas.size(); i++) {
//                 const QVariant &rowVariant = varCellDatas[i];

//                 QVariantList rowList = rowVariant.toList();
//                 QList<TableCellData> rowData = convertToRowData(rowList);

//                 cellDatas.append(rowData);
//             }

//         } else {
//             // headerData 와 같이 일차 배열의 cellData 를 읽어오기
//             QList<TableCellData> rowData = convertToRowData(varCellDatas);

//             cellDatas.append(rowData);
//         }
//     }

//     return cellDatas;
// }

QHash<QString, QQuickItem*> PdfExporter::getChildItems(QQuickItem *rootItem, const QList<QString> childObjNames, const bool isInside) {
    QHash<QString, QQuickItem*> childItems;

    if (rootItem != nullptr) {
        for (QString childObjName : childObjNames) {
            if (isInside) {
                childItems[childObjName] = getInnerItem(rootItem, childObjName);
            } else {
                childItems[childObjName] = getChildItem(rootItem, childObjName);
            }
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

void PdfExporter::initTableCellSizes(QPainter &painter, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights, const QVector<QVector<CellData>> &cellDatas,
                                     const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio) {
    //////// cell widths init
    ///
    ///
    qreal remainingWidthValue = pxTableFullWidthSize;
    int maxWidth = 0;
    int maxWidthIndex = 0;
    for (int index = 0; index < pxCellWidths.size(); index++) {
        qreal calculatedWidth = tableWidthRatio[index] * pxTableFullWidthSize;
        pxCellWidths[index] = calculatedWidth;
        remainingWidthValue -= calculatedWidth;

        if (calculatedWidth > maxWidth) {
            maxWidth = calculatedWidth;
            maxWidthIndex = index;
        }
    }

    if (remainingWidthValue > 0) {
        pxCellWidths[maxWidthIndex] += remainingWidthValue;
    }
    ///
    ///
    ////////

    //////// inner cell height init
    ///
    ///

    for (int colIndex = 0; colIndex < cellDatas.size(); colIndex++) {
        std::vector<std::pair<int, qreal>> verticalCell;

        for (int rowIndex = 0; rowIndex < cellDatas[colIndex].size(); rowIndex++) {
            QSizeF textAreaSize;
            const auto &cellData = cellDatas[colIndex][rowIndex];

            QString cellText = cellData.cellText;

            if (cellData.isVerticalDir) {
                if (!cellText.contains("\n")) {
                    cellText = cellText.split("").join("\n");  // qml 내에서 개행문자 추가 안되어 있을 경우 대비.
                }

                textAreaSize = getCalculatedCellTextArea(cellText, painter.font(), cellData.isVerticalDir);

                verticalCell.push_back(std::make_pair(rowIndex, textAreaSize.height()));
            } else {
                qreal actualCellWidth = 0;
                for (int widthIndex = cellData.startRow; widthIndex < (cellData.startRow + cellData.rowSpan); widthIndex++) {
                    actualCellWidth += pxCellWidths[widthIndex];
                }

                textAreaSize = getCalculatedCellTextArea(cellText, painter.font(), cellData.isVerticalDir, QTextOption::WrapAnywhere, (actualCellWidth - (2 * cellTextMargins)));
                if ((textAreaSize.height() + (2 * cellTextMargins)) > pxCellHeights[colIndex]) {
                    pxCellHeights[colIndex] = textAreaSize.height() + (2 * cellTextMargins);
                }
            }
        }

        for (int verticalCellIndex = 0; verticalCellIndex < verticalCell.size(); verticalCellIndex++) {
            const auto &cellData = cellDatas[colIndex][verticalCell[verticalCellIndex].first];

            qreal cellHeight = verticalCell[verticalCellIndex].second + (2 * cellTextMargins);

            int cellColSpan = cellData.colSpan;


            qreal remainingCellHeight = cellHeight - pxCellHeights[colIndex];

            for (int cellStartCol = (cellData.startCol+1); cellStartCol < (cellData.startCol + cellColSpan); cellStartCol++) {
                qreal dividedHeight = remainingCellHeight / ((cellData.startCol + cellColSpan) - cellStartCol);

                if (dividedHeight > pxCellHeights[cellStartCol]) {
                    pxCellHeights[cellStartCol] = dividedHeight;
                    remainingCellHeight -= dividedHeight;
                } else {
                    remainingCellHeight -= pxCellHeights[cellStartCol];
                }
            }
        }
    }

    ///
    ///
    ////////
}

QSizeF PdfExporter::getCalculatedCellTextArea(const QString &text, const QFont &font, const bool isVertical, const QTextOption::WrapMode &wrapMode, qreal fixedWidth) {
    qreal totalHeight = 0;

    if (isVertical) {
        int explicitLineCount = text.count('\n') + 1;
        QFontMetrics fm(font);
        totalHeight = explicitLineCount * fm.height();
        fixedWidth = fm.horizontalAdvance("가");
    } else {
        QTextLayout textLayout(text, font);
        QTextOption option;
        option.setWrapMode(wrapMode);
        textLayout.setTextOption(option);

        textLayout.beginLayout();
        QList<QTextLine> lines;

        while(true) {
            QTextLine line = textLayout.createLine();
            if (!line.isValid()) break;

            line.setLineWidth(fixedWidth);
            // line.setPosition(QPointF(0, totalHeight));
            // lines.append(line);
            totalHeight += line.height();
        }

        textLayout.endLayout();
    }

    return QSizeF(fixedWidth, totalHeight);
}

QRectF PdfExporter::drawTemplateTitle(QPainter &painter, QQuickItem *textItem) {
    QString titleText = textItem->property("text").toString();

    setFont(painter, 20, true);

    QFontMetrics metrics(painter.font());
    QRectF boundingBox(QPointF(0, 0), QSizeF(pxContentsFullSize.width(), metrics.height()));

    Qt::Alignment align = Qt::AlignHCenter;

    painter.drawText(boundingBox, align, titleText);

    return boundingBox;
}

QRectF PdfExporter::drawTemplateTable(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *tableItem, QPointF &cursorPoint, const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio) {
    QRectF tableArea(QPointF(0, 0), QSizeF(0, 0));

    const int titleTableMargin = 5;
    const qreal basicCellHeight = 50;

    QPointF tableCursorPoint = cursorPoint;

    QString tableTitle = tableItem->property("tableTextValue").toString();

    QVector<QVector<CellData>> headerDatas = getCellDatas(tableItem, "headerColRep");
    QVector<QVector<CellData>> innerDatas = getCellDatas(tableItem, "innerColRep");
    QVector<QVector<CellData>> footerDatas = getCellDatas(tableItem, "footerColRep");

    int tableRowCount = tableItem->property("dividedRowCount").toInt();
    int innerTableColCount = innerDatas.size();

    std::vector<double> pxCellWidths(tableRowCount, 0);
    std::vector<double> pxCellHeights(innerTableColCount, 0);


    //////// innerTableCell height 값 초기화
    ///
    ///
    initTableCellSizes(painter, pxCellWidths, pxCellHeights, innerDatas, pxTableFullWidthSize, tableWidthRatio);
    ///
    ///
    ////////

    double pxSumTableWidths = 0;
    for (const auto &pxCellWidth : pxCellWidths) {
        pxSumTableWidths += pxCellWidth;
    }

    //////// table title 그리기
    ///
    ///
    if (tableTitle != "") {
        setFont(painter, 10, true);

        QFontMetrics metrics(painter.font());
        QRectF boundingBox(tableCursorPoint, QSizeF(pxTableFullWidthSize, metrics.height()));

        Qt::AlignmentFlag align = Qt::AlignLeft;

        painter.drawText(boundingBox, align, tableTitle);

        tableCursorPoint = QPointF(cursorPoint.x(), tableCursorPoint.y() + boundingBox.height() + titleTableMargin);
    }


    ///
    ///
    ////////


    //////// header list 그리기
    ///
    ///

    // header list 는 오직 하나의 열만 가지고 있고 text 는 오직 한 줄인 것으로 가정.
    if (headerDatas.size() != 0) {
        setFont(painter, 8, true);

        QPointF headerCursorPoint = tableCursorPoint;

        std::vector<double> pxHeaderCellWidths(tableRowCount, 0);
        std::vector<double> pxHeaderCellHeights(1, 0);

        initTableCellSizes(painter, pxHeaderCellWidths, pxHeaderCellHeights, headerDatas,
                           pxTableFullWidthSize, tableWidthRatio);

        for (const auto &headerData : headerDatas) {
            for (int rowIndex = 0; rowIndex < headerData.size(); rowIndex++) {
                QString cellText = headerData[rowIndex].cellText;
                int cellStartRow = headerData[rowIndex].startRow;
                int cellRowSpan = headerData[rowIndex].rowSpan;
                QColor cellBgColor = headerData[rowIndex].bgColor;
                QString cellAlign = headerData[rowIndex].alignPosition;

                qreal actualWidth = 0;

                for (int widthIndex = cellStartRow; widthIndex < (cellStartRow + cellRowSpan); widthIndex++) {
                    actualWidth += pxCellWidths[widthIndex];
                }

                QRectF boundingBox(headerCursorPoint, QSizeF(actualWidth, pxHeaderCellHeights[0]));

                painter.setPen(QPen(Qt::black, 1.0));
                painter.fillRect(boundingBox,
                                 QColor(cellBgColor));
                painter.drawRect(boundingBox);

                QTextOption textOption;
                Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;

                if (cellAlign == "center") {
                    align |= Qt::AlignmentFlag::AlignHCenter;
                } else if (cellAlign == "right") {
                    align |= Qt::AlignmentFlag::AlignRight;
                } else {
                    align |= Qt::AlignmentFlag::AlignLeft;
                }

                textOption.setAlignment(align);
                textOption.setWrapMode(QTextOption::WrapAnywhere);

                QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

                painter.drawText(adjustedTextRect, cellText, textOption);

                headerCursorPoint = QPointF(headerCursorPoint.x() + actualWidth,
                                            headerCursorPoint.y());
            }
        }

        tableCursorPoint = QPointF(tableCursorPoint.x(),
                                   tableCursorPoint.y() + pxHeaderCellHeights[0]);
    }

    ///
    ///
    ////////


    //////// innerTable 그리기
    ///
    ///

    // * newPage 고려하기

    if (innerDatas.size() != 0) {
        QPointF innerCursorPoint = tableCursorPoint;

        qreal cellRectYPos = innerCursorPoint.y();

        for (int colIndex = 0; colIndex < innerDatas.size(); colIndex++) {

            if (innerCursorPoint.y() + pxCellHeights[colIndex] > pxContentsFullSize.height()) {
                pdfWriter.newPage();

                cursorPoint = QPointF(0, 0);
                tableCursorPoint = cursorPoint;
                innerCursorPoint = tableCursorPoint;
                cellRectYPos = 0;
            }

            for (int rowIndex = 0; rowIndex < innerDatas[colIndex].size(); rowIndex++) {
                QString cellText = innerDatas[colIndex][rowIndex].cellText;

                int cellStartRow = innerDatas[colIndex][rowIndex].startRow;
                int cellRowSpan = innerDatas[colIndex][rowIndex].rowSpan;
                int cellStartCol = innerDatas[colIndex][rowIndex].startCol;
                int cellColSpan = innerDatas[colIndex][rowIndex].colSpan;

                qreal cellRectXPos = tableCursorPoint.x();

                for (int widthIndex = 0; widthIndex < cellStartRow; widthIndex++) {
                    cellRectXPos += pxCellWidths[widthIndex];
                }

                qreal cellRectWidth = 0;
                qreal cellRectHeight = 0;

                for (int widthIndex = cellStartRow; widthIndex < (cellStartRow + cellRowSpan); widthIndex++) {
                    cellRectWidth += pxCellWidths[widthIndex];
                }

                for (int heightIndex = cellStartCol; heightIndex < (cellStartCol + cellColSpan); heightIndex++) {
                    cellRectHeight += pxCellHeights[heightIndex];
                }

                QColor cellRectBgColor = innerDatas[colIndex][rowIndex].bgColor;

                QRectF boundingBox(QPointF(cellRectXPos, cellRectYPos),
                                   QSizeF(cellRectWidth, cellRectHeight));

                painter.setPen(QPen(Qt::black, 1.0));
                painter.fillRect(boundingBox,
                                 QColor(cellRectBgColor));
                painter.drawRect(boundingBox);

                QString cellTextAlign = innerDatas[colIndex][rowIndex].alignPosition;
                bool cellTextBold = innerDatas[colIndex][rowIndex].isBold;

                setFont(painter, 8, cellTextBold);

                QTextOption textOption;
                Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;

                if (cellTextAlign == "center") {
                    align |= Qt::AlignmentFlag::AlignHCenter;
                } else if (cellTextAlign == "right") {
                    align |= Qt::AlignmentFlag::AlignRight;
                } else {
                    align |= Qt::AlignmentFlag::AlignLeft;
                }

                textOption.setAlignment(align);
                textOption.setWrapMode(QTextOption::WrapAnywhere);

                QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

                painter.drawText(adjustedTextRect, cellText, textOption);
            }

            innerCursorPoint = QPointF(innerCursorPoint.x(),
                                       innerCursorPoint.y() + pxCellHeights[colIndex]);

            cellRectYPos += pxCellHeights[colIndex];
        }

        tableCursorPoint = QPointF(tableCursorPoint.x(),
                                   innerCursorPoint.y());
    }

    ///
    ///
    ////////

    //////// footer 그리기
    ///
    ///

    if (footerDatas.size() != 0) {
        QPointF innerCursorPoint = tableCursorPoint;

        qreal cellRectYPos = innerCursorPoint.y();

        for (int colIndex = 0; colIndex < footerDatas.size(); colIndex++) {

            if (innerCursorPoint.y() + pxCellHeights[colIndex] > pxContentsFullSize.height()) {
                pdfWriter.newPage();

                cursorPoint = QPointF(0, 0);
                tableCursorPoint = cursorPoint;
                innerCursorPoint = tableCursorPoint;
                cellRectYPos = 0;
            }

            for (int rowIndex = 0; rowIndex < footerDatas[colIndex].size(); rowIndex++) {
                QString cellText = footerDatas[colIndex][rowIndex].cellText;

                int cellStartRow = footerDatas[colIndex][rowIndex].startRow;
                int cellRowSpan = footerDatas[colIndex][rowIndex].rowSpan;
                int cellStartCol = footerDatas[colIndex][rowIndex].startCol;
                int cellColSpan = footerDatas[colIndex][rowIndex].colSpan;

                qreal cellRectXPos = tableCursorPoint.x();

                for (int widthIndex = 0; widthIndex < cellStartRow; widthIndex++) {
                    cellRectXPos += pxCellWidths[widthIndex];
                }

                qreal cellRectWidth = 0;
                qreal cellRectHeight = 0;

                for (int widthIndex = cellStartRow; widthIndex < (cellStartRow + cellRowSpan); widthIndex++) {
                    cellRectWidth += pxCellWidths[widthIndex];
                }

                for (int heightIndex = cellStartCol; heightIndex < (cellStartCol + cellColSpan); heightIndex++) {
                    cellRectHeight += pxCellHeights[heightIndex];
                }

                QColor cellRectBgColor = footerDatas[colIndex][rowIndex].bgColor;

                QRectF boundingBox(QPointF(cellRectXPos, cellRectYPos),
                                   QSizeF(cellRectWidth, cellRectHeight));

                painter.setPen(QPen(Qt::black, 1.0));
                painter.fillRect(boundingBox,
                                 QColor(cellRectBgColor));
                painter.drawRect(boundingBox);

                QString cellTextAlign = footerDatas[colIndex][rowIndex].alignPosition;
                bool cellTextBold = footerDatas[colIndex][rowIndex].isBold;

                setFont(painter, 8, cellTextBold);

                QTextOption textOption;
                Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;

                if (cellTextAlign == "center") {
                    align |= Qt::AlignmentFlag::AlignHCenter;
                } else if (cellTextAlign == "right") {
                    align |= Qt::AlignmentFlag::AlignRight;
                } else {
                    align |= Qt::AlignmentFlag::AlignLeft;
                }

                textOption.setAlignment(align);
                textOption.setWrapMode(QTextOption::WrapAnywhere);

                QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

                painter.drawText(adjustedTextRect, cellText, textOption);
            }

            innerCursorPoint = QPointF(innerCursorPoint.x(),
                                       innerCursorPoint.y() + pxCellHeights[colIndex]);

            cellRectYPos += pxCellHeights[colIndex];
        }

        tableCursorPoint = QPointF(tableCursorPoint.x(),
                                   innerCursorPoint.y());
    }

    ///
    ///
    ////////

    tableArea = QRectF(cursorPoint, QSizeF(pxSumTableWidths, tableCursorPoint.y() - cursorPoint.y()));

    return tableArea;
}

QRectF PdfExporter::drawTemplateImage(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *imageItem, QPointF &cursorPoint, const int settingPxImageWidth, const bool isHCenter) {
    QPointF imgCursorPoint = cursorPoint;

    QPixmap scaledPixmap(0, 0);

    QString qrcPath = imageItem->property("source").toString();

     // "qrc:" 제거.
    if (qrcPath.startsWith("qrc:")) {
        qrcPath = qrcPath.mid(3);
    }

    QResource resource(qrcPath);

    if (!resource.isValid() || resource.size() == 0) {
        qDebug() << "drawTemplateImage, could not found qrc resource...";

        return QRectF(QPointF(0, 0), QSizeF(0, 0));
    }

    QPixmap originPixmap(qrcPath);
    if (!originPixmap.loadFromData(reinterpret_cast<const uchar*>(resource.data()), resource.size())) {
        qDebug() << "drawTemplateImage, could not load image data to QPixmap...";
        return QRectF(QPointF(0, 0), QSizeF(0, 0));
    }

    if (!originPixmap.isNull()) {
        // QRect pageRect = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());  // pdf 페이지의 실제 그리기 영역.


        if (settingPxImageWidth > 0) {
            int targetImgWidth = settingPxImageWidth;

            double aspectRatio = static_cast<double>(originPixmap.height()) / originPixmap.width();
            int targetImgHeight = static_cast<int>(targetImgWidth * aspectRatio);

            if (targetImgWidth > pxContentsFullSize.width() || targetImgHeight > pxContentsFullSize.height()) {
                scaledPixmap = originPixmap.scaled(pxContentsFullSize.width(), pxContentsFullSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                scaledPixmap = originPixmap.scaled(targetImgWidth, targetImgHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        } else {
            scaledPixmap = originPixmap.scaled(pxContentsFullSize.width(), pxContentsFullSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        // newPage
        if ((imgCursorPoint.y() + scaledPixmap.height()) > pxContentsFullSize.height()) {
            pdfWriter.newPage();

            cursorPoint = QPointF(0, 0);
            imgCursorPoint = cursorPoint;
        }

        if (isHCenter) {
            imgCursorPoint = QPointF((pxContentsFullSize.width() - scaledPixmap.width()) / 2,
                                     imgCursorPoint.y());
        }

        painter.drawPixmap(imgCursorPoint, scaledPixmap);
    } else {
        qDebug() << "drawTemplateImage, loaded QPixmap is null...";
        return QRectF(QPointF(0, 0), QSizeF(0, 0));
    }

    return QRectF(imgCursorPoint, QSizeF(scaledPixmap.width(), scaledPixmap.height()));
}

void PdfExporter::drawMaterialTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(materialObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, materialObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawMaterialTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[materialObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[materialObjNames[0]]);
    }
    ///
    ///
    ////////

    //////// informTable 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);

    if (templateQuickItems[materialObjNames[1]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.15, 0.3, 0.05, 0.1, 0.4};

        childItemRect[1] = drawTemplateTable(painter, pdfWriter, templateQuickItems[materialObjNames[1]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 자재구매 내역 리스트 그리기
    ///
    ///
    setFont(painter);
    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[1].height() + templateItemSpacing);

    if (templateQuickItems[materialObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.1, 0.6, 0.1, 0.2};

        childItemRect[2] = drawTemplateTable(painter, pdfWriter, templateQuickItems[materialObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////
}

void PdfExporter::defectReportTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(defectReportObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, defectReportObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawMaterialTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[defectReportObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[defectReportObjNames[0]]);
    }
    ///
    ///
    ////////

    int gapMarginTwoUpperTable = 20;  // 문서 테이블, 장소 테이블 사이의 margin

    //////// 상단의 문서 번호 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);
    if (templateQuickItems[defectReportObjNames[1]] != nullptr) {
        qreal pxTableFullWidthSize = (pxContentsFullSize.width() - gapMarginTwoUpperTable) / 2;

        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[1] = drawTemplateTable(painter, pdfWriter, templateQuickItems[defectReportObjNames[1]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 상단의 장소 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(childItemRect[1].x() + childItemRect[1].width() + gapMarginTwoUpperTable, childItemRect[1].y());
    if (templateQuickItems[defectReportObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = (pxContentsFullSize.width() - gapMarginTwoUpperTable) / 2;

        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[2] = drawTemplateTable(painter, pdfWriter, templateQuickItems[defectReportObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 품목 리스트 그리기
    ///
    ///
    setFont(painter);

    if (templateQuickItems[defectReportObjNames[3]] != nullptr) {
        int maxUpperTableHeight = (childItemRect[1].height() > childItemRect[2].height()) ? childItemRect[1].height() : childItemRect[2].height();

        cursorPoint = QPointF(0, childItemRect[1].y() + maxUpperTableHeight + templateItemSpacing);

        qreal pxTableFullWidthSize = pxContentsFullSize.width();

        std::vector<qreal> tableWidthRatio{0.05, 0.25 ,0.36, 0.12, 0.12, 0.1};

        childItemRect[3] = drawTemplateTable(painter, pdfWriter, templateQuickItems[defectReportObjNames[3]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 총 수량 테이블
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[3].y() + childItemRect[3].height() + templateItemSpacing);
    if (templateQuickItems[defectReportObjNames[4]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.1, 0.25, 0.1, 0.25, 0.1, 0.2};

        childItemRect[4] = drawTemplateTable(painter, pdfWriter, templateQuickItems[defectReportObjNames[4]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }

    ///
    ///
    ////////
}

void PdfExporter::orderFormTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(orderFromObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, orderFromObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawMaterialTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[orderFromObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[orderFromObjNames[0]]);
    }
    ///
    ///
    ////////

    //////// 발주일자 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[1]] != nullptr) {
        QQuickItem * orderDateItem = templateQuickItems[orderFromObjNames[1]];

        QString orderDateText = orderDateItem->property("orderDateItemText").toString();

        QFontMetrics metrics(painter.font());
        QRectF boundingBox(QPointF(pxContentsFullSize.width() - metrics.horizontalAdvance(orderDateText), cursorPoint.y()),
                           QSizeF(metrics.horizontalAdvance(orderDateText), metrics.height()));

        Qt::Alignment align = Qt::AlignVCenter;

        painter.drawText(boundingBox, align, orderDateText);

        childItemRect[1] = boundingBox;
    }
    ///
    ///
    ////////

    //////// 수신자 정보 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[0].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.2, 0.3, 0.2, 0.3};

        childItemRect[2] = drawTemplateTable(painter, pdfWriter, templateQuickItems[orderFromObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 발신자 정보 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[2].y() + childItemRect[2].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[3]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.2, 0.3, 0.2, 0.3};

        childItemRect[3] = drawTemplateTable(painter, pdfWriter, templateQuickItems[orderFromObjNames[3]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }

    ///
    ///
    ////////

    //////// 발주 내용 테이블 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[3].y() + childItemRect[3].height() + templateItemSpacing);
    if (templateQuickItems[orderFromObjNames[4]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.05, 0.1, 0.15, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

        childItemRect[4] = drawTemplateTable(painter, pdfWriter, templateQuickItems[orderFromObjNames[4]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////
}

void PdfExporter::drawReceiptVoucherTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(receiptVouchrObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, receiptVouchrObjNames, true);

    if (templateQuickItems.size() == 0) {
        qDebug() << "drawReceiptVoucherTemplate, could not fount childitems...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[receiptVouchrObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[receiptVouchrObjNames[0]]);
    }
    ///
    ///
    ////////

    int gapMarginTwoUpperTable = 20;  // 전표 번호, 출입고처 테이블 사이의 margin

    //////// 전표 번호 table 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + childItemRect[0].height() + templateItemSpacing);

    if (templateQuickItems[receiptVouchrObjNames[1]] != nullptr) {
        qreal pxTableFullWidthSize = (pxContentsFullSize.width() - gapMarginTwoUpperTable) / 2;

        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[1] = drawTemplateTable(painter, pdfWriter, templateQuickItems[receiptVouchrObjNames[1]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 출입고처 table 그리기
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(childItemRect[1].x() + childItemRect[1].width() + gapMarginTwoUpperTable, childItemRect[1].y());

    if (templateQuickItems[receiptVouchrObjNames[2]] != nullptr) {
        qreal pxTableFullWidthSize = (pxContentsFullSize.width() - gapMarginTwoUpperTable) / 2;
        std::vector<qreal> tableWidthRatio{0.2, 0.8};

        childItemRect[2] = drawTemplateTable(painter, pdfWriter, templateQuickItems[receiptVouchrObjNames[2]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }
    ///
    ///
    ////////

    //////// 제품 리스트 테이블 그리기
    ///
    ///
    {
        setFont(painter);

        int maxUpperTableHeight = (childItemRect[1].height() > childItemRect[2].height()) ? childItemRect[1].height() : childItemRect[2].height();
        cursorPoint = QPointF(0, childItemRect[1].y() + maxUpperTableHeight + templateItemSpacing);

        qreal pxTableFullWidthSize = pxContentsFullSize.width();

        std::vector<qreal> tableWidthRatio{0.05, 0.15, 0.5, 0.1, 0.2};

        // 생산 테이블
        if (templateQuickItems[receiptVouchrObjNames[3]] != nullptr) {
            childItemRect[3] = drawTemplateTable(painter, pdfWriter, templateQuickItems[receiptVouchrObjNames[3]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
        }


        cursorPoint = QPointF(0, childItemRect[3].y() + childItemRect[3].height());
        // 소모 테이블
        if (templateQuickItems[receiptVouchrObjNames[4]] != nullptr) {
            childItemRect[4] = drawTemplateTable(painter, pdfWriter, templateQuickItems[receiptVouchrObjNames[4]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
        }
    }
    ///
    ///
    ////////

    //////// 총 수량 테이블
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[4].y() + childItemRect[4].height() + templateItemSpacing);

    if (templateQuickItems[receiptVouchrObjNames[5]] != nullptr) {
        qreal pxTableFullWidthSize = pxContentsFullSize.width();
        std::vector<qreal> tableWidthRatio{0.1, 0.25, 0.1, 0.25, 0.1, 0.2};

        childItemRect[5] = drawTemplateTable(painter, pdfWriter, templateQuickItems[receiptVouchrObjNames[5]], cursorPoint, pxTableFullWidthSize, tableWidthRatio);
    }

    ///
    ///
    ////////

    //////// 하단 이미지
    ///
    ///
    setFont(painter);

    cursorPoint = QPointF(0, childItemRect[5].y() + childItemRect[5].height() + templateItemSpacing);

    if (templateQuickItems[receiptVouchrObjNames[6]] != nullptr) {
        childItemRect[6] = drawTemplateImage(painter, pdfWriter, templateQuickItems[receiptVouchrObjNames[6]], cursorPoint, pxContentsFullSize.width(), true);
    }
    ///
    ///
    ////////
}

bool PdfExporter::exportToPdf(QQuickItem *rootItem, const QString &filePath) {
    qDebug() << "[LLDDSS] exportToPdf";


    // QPdfWriter pdfWriter(filePath);
    // pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    // pdfWriter.setResolution(72);

    // QPainter painter(&pdfWriter);

    // painter.setPen(Qt::red);
    // painter.drawRect(141.732, 141.732, 283.465, 283.465);  //(5, 5, 10, 10) dpi 72 : point 단위로 전달해야 함.
    // // painter.drawRect(590.55, 590.55, 1181.1, 1181.1);  //(5, 5, 10, 10) dpi 300 : pixel 단위로 전달해야 함.





    // painter.end();



    // return true;

    //////////////////////////

    QPdfWriter pdfWriter(filePath);

    setDefaultPdfEnvironment(pdfWriter);

    QPainter painter;

    if (!painter.begin(&pdfWriter)) {
        qWarning() << "QPdfWriter 초기화 실패";
        return false;
    }

    setFont(painter);

    if (rootItem->objectName() == templateObjNames[0]) {
        drawMaterialTemplate(painter, pdfWriter, rootItem);
    } else if (rootItem->objectName() == templateObjNames[1]) {
        defectReportTemplate(painter, pdfWriter, rootItem);
    } else if (rootItem->objectName() == templateObjNames[2]) {
        orderFormTemplate(painter, pdfWriter, rootItem);
    } else if (rootItem->objectName() == templateObjNames[3]) {
        drawReceiptVoucherTemplate(painter, pdfWriter, rootItem);
    }

    painter.end();

    return true;
}

bool PdfExporter::generatePreview(QQuickItem *rootItem) {
    qDebug() << "[LLDDSS] generatePreview";

    // m_hasPreview = false;
    // emit hasPreviewChanged();

    // QList<QImage> previewPages;

    // // QSize pageSize(2481, 3507);  // dpi 300 일 때 a4 크기의 픽셀 값. (210mm/25.4 * 300), (297mm/25.4 * 300)
    // QSize pageSize(595, 841);  // dpi 72 일 때 a4 크기의 픽셀 값. (210mm/25.4 * 72), (297mm/25.4 * 72)

    // QImage image(pageSize, QImage::Format_ARGB32);
    // image.fill(Qt::white); // 배경색 지정

    // // image.setDotsPerMeterX(11811);  // dpi 300 : 1/0.0254 * dpi
    // // image.setDotsPerMeterY(11811);  // dpi 300

    // image.setDotsPerMeterX(2835);  // dpi 72
    // image.setDotsPerMeterY(2835);  // dpi 72

    // QPainter painter(&image);



    // painter.setPen(Qt::red);
    // // painter.drawRect(590.551, 590.551, 1181.102, 1181.102);  //(5, 5, 10, 10) dpi 300 : cm/2.54*dpi
    // painter.drawRect(141.732, 141.732, 283.464, 283.464);  //(5, 5, 10, 10) dpi 72

    // painter.end();

    // previewPages.append(image);

    // m_pageCount = previewPages.size();
    // emit pageCountChanged();

    // m_imageProvider->updatePreviewImages(previewPages);

    // m_hasPreview = true;
    // emit hasPreviewChanged();
    // emit previewUpdated();

    // return true;

    //////////////////////////////////////////////////////////////////

    m_hasPreview = false;
    emit hasPreviewChanged();


    // ★★★ 핵심 수정: 안전한 페이지별 렌더링 ★★★
    QList<QImage> previewPages;

    qreal scaleFactor = (qreal)PREVIEW_PIXEL_WIDTH / A4_WIDTH_PTS;
    qreal pageContentHeight = A4_HEIGHT_PTS - 2 * MARGIN_PTS;

    qDebug() << "페이지 콘텐츠 높이:" << pageContentHeight;
    qDebug() << "스케일 팩터:" << scaleFactor;

    int currentRow = 0;
    int pageNumber = 1;

    ////////
    // // 새 페이지 이미지 생성
    // QImage pageImage(PREVIEW_PIXEL_WIDTH, PREVIEW_PIXEL_HEIGHT, QImage::Format_ARGB32);
    // pageImage.fill(Qt::white);
    ////////
    // PDF 설정에 맞는 페이지 이미지 생성
    PdfPageSettings settings = calculatePdfPageSettings();
    QImage pageImage = createPdfSizedImage();
    ////////



    // ★★★ 각 페이지마다 새로운 QPainter 인스턴스 사용 (스코프 기반 관리) ★★★
    QPainter painter(&pageImage);
    if (!painter.isActive()) {
        qDebug() << "QPainter 초기화 실패";
        return false;
    }

    ////////
    // painter.scale(scaleFactor, scaleFactor);
    // drawPageFrame(&painter);
    // painter.translate(MARGIN_PTS, MARGIN_PTS);
    ////////

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(settings.marginPixels, settings.marginPixels);
    painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
    painter.drawRect(0, 0, settings.contentSize.width(), settings.contentSize.height());
    ////////

    qreal currentY = 0;
    int rowsInThisPage = 0;

    //////// PdfExporter::drawMaterialTemplate 함수 복사해옴.
    if (rootItem->objectName() == templateObjNames[0]) {
        ////////  MY SOURCE
        ///
        ///
        // QPointF cursorPoint(0, 0);
        // std::vector<QRectF> childItemRect(materialObjNames.length());
        // QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, materialObjNames);
        // if (templateQuickItems.size() == 0) {
        //     qDebug() << "drawMaterialTemplate, could not fount childitems...";
        //     return false;
        // }
        // if (templateQuickItems[materialObjNames[0]] != nullptr) {
        //     childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[materialObjNames[0]]);
        // }
        QPdfWriter pdfWriter("");
        drawMaterialTemplate(painter, pdfWriter, rootItem);
        ///
        ///
        ////////

        ////////  EXAMPLE
        ///
        ///
        // QRectF cellRect(50, 50, 200, 200);
        // painter.fillRect(cellRect, Qt::darkYellow);
        // painter.setPen(QPen(QColor("#cccccc"), 2));
        // painter.drawRect(cellRect);

        // painter.setPen(Qt::black);
        // QFont font("Arial", 12);
        // painter.setFont(font);
        // painter.drawText(0, 0,
        //                  QString("A4 Page: %1×%2px, %3 DPI, Margin: %4px")
        //                      .arg(settings.imageSize.width())
        //                      .arg(settings.imageSize.height())
        //                      .arg(settings.resolution)
        //                      .arg(settings.marginPixels));
        ///
        ///
        ////////

        //////// EXAMPLE TEXT
        ///
        ///
        // QRectF cellRect(0, 0, 200, 200);
        // painter.fillRect(cellRect, Qt::darkYellow);
        // painter.setPen(QPen(QColor("#cccccc"), 1));
        // painter.drawRect(cellRect);

        // setFont(painter, 20, true);
        // QFontMetrics metrics(painter.font());
        // QRectF boundingBox(QPointF(0, 0), QSizeF(PREVIEW_PIXEL_WIDTH, metrics.height()));

        // Qt::Alignment align = Qt::AlignHCenter;

        // painter.drawText(boundingBox, align, "test test test");
        ///
        ///
        ////////


        ////////  EXAMPLE RECT
        ///
        ///
        // QRectF cellRect(0, 0, 200, 200);
        // painter.fillRect(cellRect, Qt::darkYellow);
        // painter.setPen(QPen(QColor("#cccccc"), 1));
        // painter.drawRect(cellRect);
        ///
        ///
        ////////




        // 완성된 페이지를 리스트에 추가
        previewPages.append(pageImage);
        pageNumber++;
    }
    ////////

    qDebug() << "총 생성된 페이지 수:" << previewPages.size();

    // ★★★ 안전한 결과 설정 ★★★
    m_pageCount = previewPages.size();
    emit pageCountChanged();

    m_imageProvider->updatePreviewImages(previewPages);

    m_hasPreview = true;
    emit hasPreviewChanged();
    emit previewUpdated();

    qDebug() << "안전한 미리보기 생성 완료 - 총" << m_pageCount << "페이지";

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

void PdfExporter::testOutputCellDatas(const CellData &cellData) {
    qDebug() << "[LLDDSS] --------[CellDatas]--------";
    qDebug() << "[LLDDSS] startRow : " << cellData.startRow << ", startCol : " << cellData.startCol;
    qDebug() << "[LLDDSS] rowSpan : " << cellData.rowSpan << ", colSpan : " << cellData.colSpan;
    qDebug() << "[LLDDSS] cellText : " << cellData.cellText;
    qDebug() << "[LLDDSS] isTextEdit : " << cellData.isTextEdit;
    qDebug() << "[LLDDSS] bgColor : " << cellData.bgColor;
    qDebug() << "[LLDDSS] isBold : " << cellData.isBold;
    qDebug() << "[LLDDSS] fontSize : " << cellData.fontSize;
    qDebug() << "[LLDDSS] alignPosition : " << cellData.alignPosition;
    qDebug() << "[LLDDSS] isVerticalDir : " << cellData.isVerticalDir;
    qDebug() << "[LLDDSS] ---------------------------";
}

void PdfExporter::testDrawTable(QPainter &painter, QPointF &cursorPoint,
                                std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights) {
    QPointF innerTableStartPoint = cursorPoint;
    for (const auto &cellHeight : pxCellHeights) {
        for (const auto &cellWidth : pxCellWidths) {
            painter.drawRect(QRectF(innerTableStartPoint,
                                    QSize(cellWidth, cellHeight)));

            innerTableStartPoint = QPointF((innerTableStartPoint.x() + cellWidth),
                                           innerTableStartPoint.y());
        }

        innerTableStartPoint = QPointF(cursorPoint.x(),
                                       innerTableStartPoint.y() + cellHeight);
    }
}

void PdfExporter::testDrawContent(QPainter& painter, const QSize& size) {
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 14));
    painter.drawText(QRect(0, 0, size.width(), size.height()), Qt::AlignCenter, "PDF & Image Test");
    painter.drawRect(100, 100, 300, 200);
}

QVector<QVector<CellData>> PdfExporter::getCellDatas(QQuickItem *tableItem, const QString &repeaterObjName) {
    QVector<QVector<CellData>> m_processedTableData;

    QQuickItem * innerRepQuickItem = getInnerItem(tableItem, repeaterObjName);

    if (innerRepQuickItem) {
        QVariant modelProperty = innerRepQuickItem->property("model");
        QObject* modelObject = modelProperty.value<QObject*>();

        if (modelObject) {
            TableModel* tableModel = qobject_cast<TableModel*>(modelObject);

            if (tableModel) {
                QVariantList allData = tableModel->getAllTableData();

                for (int i = 0; i < allData.size(); ++i) {
                    QVariantList rowData = allData[i].toList();
                    QVector<CellData> processedRow;

                    for (int j = 0; j < rowData.size(); ++j) {
                        QVariantMap cellMap = rowData[j].toMap();
                        CellData cellData = CellData::fromVariantMap(cellMap);

                        // vertical text 일 경우 개행문자 추가.
                        if (cellData.isVerticalDir && !cellData.cellText.contains("\n")) {
                            cellData.cellText = cellData.cellText.split("").join("\n");

                            // 앞뒤 개행문자 제거
                            if (cellData.cellText.startsWith("\n")) {
                                cellData.cellText = cellData.cellText.mid(1);
                            }

                            if (cellData.cellText.endsWith("\n")) {
                                cellData.cellText.chop(1);
                            }
                            //
                        }
                        //

                        processedRow.append(cellData);
                    }

                    m_processedTableData.append(processedRow);
                }
            }
        }
    }

    return m_processedTableData;
}

void PdfExporter::drawPageFrame(QPainter *painter) const {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    painter->fillRect(0, 0, A4_WIDTH_PTS, A4_HEIGHT_PTS, Qt::white);
    painter->setPen(QPen(QColor("#e0e0e0"), 1));
    painter->drawRect(0, 0, A4_WIDTH_PTS, A4_HEIGHT_PTS);
}

PdfPageSettings PdfExporter::calculatePdfPageSettings() const {
    PdfPageSettings settings;

    // A4 페이지 크기 설정
    QPageSize pageSize(QPageSize::A4);
    settings.pageSizeInMM = pageSize.size(QPageSize::Millimeter);

    // 여백 설정 (QPdfWriter와 동일)
    settings.marginsInMM = QMarginsF(15, 15, 15, 15);
    settings.resolution = PDF_RESOLUTION;

    // 전체 페이지 크기를 픽셀로 변환
    double pageWidthInInches = settings.pageSizeInMM.width() * MM_TO_INCH;
    double pageHeightInInches = settings.pageSizeInMM.height() * MM_TO_INCH;

    // A4 SIZE 전체 크기
    settings.imageSize = QSize(
        qRound(pageWidthInInches * settings.resolution),
        qRound(pageHeightInInches * settings.resolution)
        );

    // 여백을 픽셀로 변환
    settings.marginPixels = qRound(settings.marginsInMM.left() * MM_TO_INCH * settings.resolution);

    // 내용 영역 크기 계산 (여백 제외)
    settings.contentSize = QSize(
        settings.imageSize.width() - (2 * settings.marginPixels),
        settings.imageSize.height() - (2 * settings.marginPixels)
        );

    return settings;
}

QImage PdfExporter::createPdfSizedImage() const {
    PdfPageSettings settings = calculatePdfPageSettings();

    // PDF와 동일한 크기의 QImage 생성
    QImage pageImage(settings.imageSize, QImage::Format_ARGB32);
    pageImage.fill(Qt::white);

    // DPI 정보 설정 (QPainter 스케일링 정확성을 위해)
    double dotsPerMeter = settings.resolution * 39.3701; // 1인치 = 39.3701 dots/meter
    pageImage.setDotsPerMeterX(qRound(dotsPerMeter));
    pageImage.setDotsPerMeterY(qRound(dotsPerMeter));

    return pageImage;
}
