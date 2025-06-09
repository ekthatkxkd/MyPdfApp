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
                textAreaSize = getCalculatedCellTextArea(cellText, painter.font(), cellData.isVerticalDir, QTextOption::WordWrap, (pxCellWidths[colIndex] - (2 * cellTextMargins)));
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
                    remainingCellHeight = dividedHeight;
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
    QTextLayout textLayout(text, font);
    QTextOption option;
    option.setWrapMode(wrapMode);
    textLayout.setTextOption(option);

    textLayout.beginLayout();
    qreal totalHeight = 0;
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

    if (isVertical) {
        QFontMetrics metrics(font);
        fixedWidth = metrics.horizontalAdvance("가");
    }

    return QSizeF(fixedWidth, totalHeight);
}

QRectF PdfExporter::drawTemplateTitle(QPainter &painter, QQuickItem *textItem) {
    QString titleText = textItem->property("text").toString();

    setFont(painter, 20, true);

    QFontMetrics metrics(painter.font());
    QRectF boundingBox(QPointF(0, 0), QSizeF(pxContentsFullSize.width(), metrics.height()));

    Qt::AlignmentFlag align = Qt::AlignHCenter;

    painter.drawText(boundingBox, align, titleText);

    return boundingBox;
}

QRectF PdfExporter::drawTemplateTable(QPainter &painter, QPdfWriter &pdfWriter, QQuickItem *tableItem, QPointF &cursorPoint, const qreal &pxTableFullWidthSize, const std::vector<qreal> &tableWidthRatio) {
    QRectF tableArea(QPointF(0, 0), QSizeF(0, 0));

    int tableRowCount = tableItem->property("dividedRowCount").toInt();
    int innerTableColCount = tableItem->property("dividedInnerColCount").toInt();


    QString tableTitle = tableItem->property("tableTextValue").toString();

    QList<QList<TableCellData>> headerData = getCellDatas(tableItem, "headerData");
    QList<QList<TableCellData>> innerData = getCellDatas(tableItem, "innerDatas");
    QList<QList<TableCellData>> footerData = getCellDatas(tableItem, "footerData");

    std::vector<double> pxCellWidths(tableRowCount, 0);
    std::vector<double> pxCellHeights(innerTableColCount, 0);

    initTableCellSizes(painter, pxCellWidths, pxCellHeights, innerData, pxTableFullWidthSize, tableWidthRatio);

    //////// innerTable 그리기
    ///
    ///

    // 1. pxCellWidths, pxCellHeights 객체에 값을 initializing 하기
    // 2. n 번째 col 에 위치한 cell data 들을 QRectF boundingBox 계산해서
    //    가장 큰 height 값을 구하고 이 값을 pxCellHeights 객체의 n 번째 col 에 해당하는 index 에 값 변경하기.

    // * newPage 고려하기


    // QTextOption textOption;
    // textOption.setAlignment(alignment);
    // textOption.setWrapMode(QTextOption::WrapAnywhere);

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

    setFont(painter);
    ///
    ///
    ////////

    //////// informTable 그리기
    ///
    ///
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

