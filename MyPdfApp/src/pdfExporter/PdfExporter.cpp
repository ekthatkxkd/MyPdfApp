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

TableCellData PdfExporter::convertToTableCellData(const QVariantMap &map) {
    TableCellData data;

    data.startRow = map.value("startRow", 0).toInt();
    data.startCol = map.value("startCol", 0).toInt();
    data.rowSpan = map.value("rowSpan", 1).toInt();
    data.colSpan = map.value("colSpan", 1).toInt();
    data.cellText = map.value("cellText", "").toString();
    data.isTextEdit = map.value("isTextEdit", false).toBool();
    data.bgColor = QColor(map.value("bgColor", "white").toString());
    data.isBold = map.value("isBold", false).toBool();
    data.fontSize = map.value("fontSize", 12).toInt();
    data.alignPosition = map.value("alignPosition", "right").toString();
    data.isVerticalDir = map.value("isVerticalDir", false).toBool();

    return data;
}

QList<TableCellData> PdfExporter::convertToRowData(const QVariantList &rowList) {
    QList<TableCellData> cellDatas;

    for (const QVariant &cellVariant : rowList) {
        if (cellVariant.canConvert<QVariantMap>()) {
            QVariantMap cellMap = cellVariant.toMap();

            TableCellData cellData = convertToTableCellData(cellMap);

            if (cellData.isVerticalDir && !cellData.cellText.contains("\n")) {
                cellData.cellText = cellData.cellText.split("").join("\n");

                // 앞뒤 개행문자 제거
                if (cellData.cellText.startsWith('\n')) {
                    cellData.cellText = cellData.cellText.mid(1);
                }
                if (cellData.cellText.endsWith('\n')) {
                    cellData.cellText.chop(1); // 마지막 문자 제거
                }
            }

            cellDatas.append(cellData);
        }
    }

    return cellDatas;
}

QList<QList<TableCellData>> PdfExporter::getCellDatas(QQuickItem *tableItem, const QString &propertyName) {
    QList<QList<TableCellData>> cellDatas;

    QVariantList varCellDatas = tableItem->property(propertyName.toLocal8Bit().data()).toList();

    if (!varCellDatas.isEmpty()) {
        const QVariant &firstElement = varCellDatas.first();

        bool isDoubleArray = firstElement.canConvert<QVariantList>();

        if (isDoubleArray) {
            // innerData 와 같이 이차 배열의 cellData 를 읽어오기
            for (int i = 0; i < varCellDatas.size(); i++) {
                const QVariant &rowVariant = varCellDatas[i];

                QVariantList rowList = rowVariant.toList();
                QList<TableCellData> rowData = convertToRowData(rowList);

                cellDatas.append(rowData);
            }

        } else {
            // headerData 와 같이 일차 배열의 cellData 를 읽어오기
            QList<TableCellData> rowData = convertToRowData(varCellDatas);

            cellDatas.append(rowData);
        }
    }

    return cellDatas;
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

void PdfExporter::initTableCellSizes(QPainter &painter, std::vector<double> &pxCellWidths, std::vector<double> &pxCellHeights, const QList<QList<TableCellData>> &cellDatas,
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

    int tableRowCount = tableItem->property("dividedRowCount").toInt();
    int innerTableColCount = tableItem->property("dividedInnerColCount").toInt();


    QString tableTitle = tableItem->property("tableTextValue").toString();

    QList<QList<TableCellData>> headerDatas = getCellDatas(tableItem, "headerData");
    QList<QList<TableCellData>> innerDatas = getCellDatas(tableItem, "innerDatas");
    QList<QList<TableCellData>> footerDatas = getCellDatas(tableItem, "footerData");

    std::vector<double> pxCellWidths(tableRowCount, 0);
    std::vector<double> pxCellHeights(innerTableColCount, 0);

    //////// innerTableCell height 값 초기화
    ///
    ///
    initTableCellSizes(painter, pxCellWidths, pxCellHeights, innerDatas, pxTableFullWidthSize, tableWidthRatio);
    ///
    ///
    ////////

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
        setFont(painter, 10, true);

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

        for (int colIndex = 0; colIndex < innerDatas.size(); colIndex++) {

            if (innerCursorPoint.y() + pxCellHeights[colIndex] > pxContentsFullSize.width()) {
                pdfWriter.newPage();

                cursorPoint = QPointF(0, 0);
                tableCursorPoint = cursorPoint;
                innerCursorPoint = tableCursorPoint;
            }

            for (int rowIndex = 0; rowIndex < innerDatas[colIndex].size(); rowIndex++) {
                QString cellText = innerDatas[colIndex][rowIndex].cellText;

                int cellStartRow = innerDatas[colIndex][rowIndex].startRow;
                int cellRowSpan = innerDatas[colIndex][rowIndex].rowSpan;
                int cellStartCol = innerDatas[colIndex][rowIndex].startCol;
                int cellColSpan = innerDatas[colIndex][rowIndex].colSpan;

                qreal cellRectXPos = tableCursorPoint.x();
                qreal cellRectYPos = tableCursorPoint.y();

                for (int widthIndex = 0; widthIndex < cellStartRow; widthIndex++) {
                    cellRectXPos += pxCellWidths[widthIndex];
                }

                for (int heightIndex = 0; heightIndex < cellStartCol; heightIndex++) {
                    cellRectYPos += pxCellHeights[heightIndex];
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

                setFont(painter, 10, cellTextBold);

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
        }

        tableCursorPoint = QPointF(tableCursorPoint.x(),
                                   innerCursorPoint.y());
    }


    ///
    ///
    ////////

    return tableArea;
}

void PdfExporter::drawMaterialTemplate(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *rootItem) {
    QPointF cursorPoint(0, 0);

    std::vector<QRectF> childItemRect(materialObjNames.length());

    QHash<QString, QQuickItem*> templateQuickItems = getChildItems(rootItem, materialObjNames);

    if (templateQuickItems.size() == 0) {
        qDebug() << "[LLDDSS] drawing failed...";
        return;
    }

    //////// template title 그리기
    ///
    ///
    if (templateQuickItems[materialObjNames[0]] != nullptr) {
        childItemRect[0] = drawTemplateTitle(painter, templateQuickItems[materialObjNames[0]]);
    }

    painter.drawRect(childItemRect[0]);  // [LLDDSS] FOR TEST DELETE
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
        drawMaterialTemplate(painter, pdfWriter, rootItem);
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

void PdfExporter::testOutputCellDatas(const TableCellData &cellData) {
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

