#include "include/docElement/TableElement.h"
#include <QTextLayout>

TableElement::TableElement(const QString &title,
                           const TableData tableData,
                           const qreal tableFullWidth,
                           const QVector<qreal>& widthRatio,
                           Qt::Alignment align)
    : title(title), tableData(tableData), tableFullWidth(tableFullWidth), widthRatio(widthRatio), alignment(align) {
    initCellWidth();
}

void TableElement::initCellWidth() {
    qreal remainFullWidth = tableFullWidth;
    // QVector<qreal> cellWidths;

    for (auto &ratio : widthRatio) {
        qreal setWidth = tableFullWidth * ratio;
        cellWidths.push_back(setWidth);
        remainFullWidth -= setWidth;
    }

    if (remainFullWidth > 0) {
        cellWidths[cellWidths.size()-1] += remainFullWidth;
    }
}

QRectF TableElement::render(QPainter& painter, const QPointF& startPos,
                            const QSizeF &pxContentSize,
                            std::function<void()> newPageCallback) {
    QPointF tableStartPos = startPos;

    QPointF cursorPoint = startPos;

    if (title != "") {
        const qreal titleTableMargin = 5;
        QFont font;
        font.setPointSize(10);
        font.setBold(true);

        painter.setFont(font);
        painter.setPen(QPen(Qt::black, 1));

        QFontMetrics metrics(painter.font());
        // QRectF boundingBox(cursorPoint, QSizeF(pxContentSize.width(), metrics.height()));
        QRectF boundingBox(cursorPoint, QSizeF(metrics.horizontalAdvance(title), metrics.height()));

        Qt::AlignmentFlag align = Qt::AlignLeft;

        painter.drawText(boundingBox, align, title);

        cursorPoint = QPointF(tableStartPos.x(), cursorPoint.y() + boundingBox.height() + titleTableMargin);
    }

    //////// table header 그리기
    ///
    ///
    if (tableData.headerDatas.size() != 0) {
        QPointF headerCursorPoint = cursorPoint;
        qreal rowHeight = calculateOneRowHeight(tableData.headerDatas);

        if ((headerCursorPoint.y() + rowHeight) > pxContentSize.height()) {
            newPageCallback();

            tableStartPos = QPointF(startPos.x(), 0);
            cursorPoint = tableStartPos;
            headerCursorPoint = tableStartPos;
        }

        for (auto &cellData : tableData.headerDatas) {
            //////// rectangle 그리기
            ///
            ///
            qreal cellWidth = 0;
            for (int colIndex = cellData.startCol; colIndex < (cellData.startCol + cellData.colSpan); colIndex++) {
                cellWidth += cellWidths[colIndex];
            }

            QRectF boundingBox(headerCursorPoint, QSizeF(cellWidth, rowHeight));
            painter.setPen(QPen(Qt::black, 1.0));
            painter.fillRect(boundingBox, cellData.bgColor);
            painter.drawRect(boundingBox);
            ///
            ///
            ////////

            //////// text 그리기
            ///
            ///
            QString cellText = cellData.cellText;
            QFont font = painter.font();
            font.setPointSize(cellData.fontSize);
            font.setBold(cellData.isBold);
            painter.setFont(font);

            QString cellAlign = cellData.alignPosition;

            Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;
            if (cellAlign == "center") {
                align |= Qt::AlignmentFlag::AlignHCenter;
            } else if (cellAlign == "right") {
                align |= Qt::AlignmentFlag::AlignRight;
            } else {
                align |= Qt::AlignmentFlag::AlignLeft;
            }

            QTextOption textOption;
            textOption.setAlignment(align);
            textOption.setWrapMode(QTextOption::WrapAnywhere);

            QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

            painter.drawText(adjustedTextRect, cellText, textOption);
            ///
            ///
            ////////

            headerCursorPoint = QPointF(boundingBox.right(), headerCursorPoint.y());
        }

        cursorPoint = QPointF(tableStartPos.x(), cursorPoint.y() + rowHeight);
    }
    ///
    ///
    ////////

    //////// table inner 그리기
    ///
    ///
    if (tableData.innerDatas.size() != 0) {
        QPointF innerCursorPoint = cursorPoint;
        QVector<qreal> rowHeights = calculateRowHeights(tableData.innerDatas);

        for (int rowIndex = 0; rowIndex < tableData.innerDatas.size(); rowIndex++) {
            QVector<CellData> rowDatas = tableData.innerDatas[rowIndex];

            if ((innerCursorPoint.y() + rowHeights[rowIndex]) > pxContentSize.height()) {
                newPageCallback();

                tableStartPos = QPointF(startPos.x(), 0);
                cursorPoint = tableStartPos;
                innerCursorPoint = tableStartPos;
            }

            for (int colIndex = 0; colIndex < rowDatas.size(); colIndex++) {
                CellData cellData = rowDatas[colIndex];
                //////// rectangle 그리기
                ///
                ///
                qreal cellWidth = 0;

                for (int widthIndex = cellData.startCol; widthIndex < (cellData.startCol + cellData.colSpan); widthIndex++) {
                    cellWidth += cellWidths[widthIndex];
                }

                qreal cellHeight = 0;

                for (int heightIndex = cellData.startRow; heightIndex < (cellData.startRow + cellData.rowSpan); heightIndex++) {
                    cellHeight += rowHeights[heightIndex];
                }

                QRectF boundingBox(innerCursorPoint, QSizeF(cellWidth, cellHeight));
                painter.setPen(QPen(Qt::black, 1.0));
                painter.fillRect(boundingBox, cellData.bgColor);
                painter.drawRect(boundingBox);
                ///
                ///
                ////////

                //////// text 그리기
                ///
                ///
                QString cellText = cellData.cellText;
                QFont font = painter.font();
                font.setPointSize(cellData.fontSize);
                font.setBold(cellData.isBold);
                painter.setFont(font);

                QString cellAlign = cellData.alignPosition;

                Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;
                if (cellAlign == "center") {
                    align |= Qt::AlignmentFlag::AlignHCenter;
                } else if (cellAlign == "right") {
                    align |= Qt::AlignmentFlag::AlignRight;
                } else {
                    align |= Qt::AlignmentFlag::AlignLeft;
                }

                QTextOption textOption;
                textOption.setAlignment(align);
                textOption.setWrapMode(QTextOption::WrapAnywhere);

                QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

                painter.drawText(adjustedTextRect, cellText, textOption);
                ///
                ///
                ////////

                innerCursorPoint = QPointF(boundingBox.right(), innerCursorPoint.y());
            }

            innerCursorPoint = QPointF(cursorPoint.x(), innerCursorPoint.y() + rowHeights[rowIndex]);
        }

        cursorPoint = innerCursorPoint;
    }
    ///
    ///
    ////////

    //////// footer 그리기
    ///
    ///
    if (tableData.footerDatas.size() != 0) {
        QPointF footerCursorPoint = cursorPoint;
        qreal rowHeight = calculateOneRowHeight(tableData.footerDatas);

        if ((footerCursorPoint.y() + rowHeight) > pxContentSize.height()) {
            newPageCallback();

            tableStartPos = QPointF(startPos.x(), 0);
            cursorPoint = tableStartPos;
            footerCursorPoint = tableStartPos;
        }

        for (auto &cellData : tableData.footerDatas) {
            //////// rectangle 그리기
            ///
            ///
            qreal cellWidth = 0;
            for (int colIndex = cellData.startCol; colIndex < (cellData.startCol + cellData.colSpan); colIndex++) {
                cellWidth += cellWidths[colIndex];
            }

            QRectF boundingBox(footerCursorPoint, QSizeF(cellWidth, rowHeight));
            painter.setPen(QPen(Qt::black, 1.0));
            painter.fillRect(boundingBox, cellData.bgColor);
            painter.drawRect(boundingBox);
            ///
            ///
            ////////

            //////// text 그리기
            ///
            ///
            QString cellText = cellData.cellText;
            QFont font = painter.font();
            font.setPointSize(cellData.fontSize);
            font.setBold(cellData.isBold);
            painter.setFont(font);

            QString cellAlign = cellData.alignPosition;

            Qt::Alignment align = Qt::AlignmentFlag::AlignVCenter;
            if (cellAlign == "center") {
                align |= Qt::AlignmentFlag::AlignHCenter;
            } else if (cellAlign == "right") {
                align |= Qt::AlignmentFlag::AlignRight;
            } else {
                align |= Qt::AlignmentFlag::AlignLeft;
            }

            QTextOption textOption;
            textOption.setAlignment(align);
            textOption.setWrapMode(QTextOption::WrapAnywhere);

            QRectF adjustedTextRect = boundingBox.adjusted(cellTextMargins, cellTextMargins, -cellTextMargins, -cellTextMargins);

            painter.drawText(adjustedTextRect, cellText, textOption);
            ///
            ///
            ////////

            footerCursorPoint = QPointF(boundingBox.right(), footerCursorPoint.y());
        }

        cursorPoint = QPointF(tableStartPos.x(), cursorPoint.y() + rowHeight);
    }
    ///
    ///
    ////////

    return QRectF(tableStartPos, QSizeF(tableFullWidth, cursorPoint.y()));
}

qreal TableElement::calculateOneRowHeight(QVector<CellData> &rowDatas) {
    qreal rowHeight = (cellTextMargins * 2);

    for (int colIndex = 0; colIndex < rowDatas.size(); colIndex++) {
        CellData cellData = rowDatas[colIndex];
        QString text = cellData.cellText;
        QFont font;
        font.setPointSize(cellData.fontSize);
        font.setBold(cellData.isBold);

        qreal cellWidth = 0;
        for (int widthIndex = cellData.startCol; widthIndex < cellData.colSpan; widthIndex++) {
            cellWidth += cellWidths[widthIndex];
        }

        QSizeF textSize = calculatedCellTextArea(text, font, false, QTextOption::WrapAnywhere, (cellWidth - (cellTextMargins * 2)));
        int actualCellHeight = (textSize.height() + (cellTextMargins * 2));

        if (actualCellHeight > rowHeight) {
            rowHeight = actualCellHeight;
        }
    }

    return rowHeight;
}

QVector<qreal> TableElement::calculateRowHeights(const QVector<QVector<CellData>> &datas) {
    QVector<qreal> rowHeights(datas.size(), cellTextMargins * 2);  // row 개수만큼 상하 마진이 반영된 값으로 초기화.

    QVector<QPair<int, int>> catchedVerticalCells;

    for (int rowIndex = 0; rowIndex < datas.size(); rowIndex++) {
        qreal maxHeight = 0;

        for (int colIndex = 0; colIndex < datas[rowIndex].size(); colIndex++) {
            CellData cellData = datas[rowIndex][colIndex];
            QString text = cellData.cellText;

            if (cellData.isVerticalDir) {
                catchedVerticalCells.push_back(QPair<int, int>(rowIndex, colIndex));
            } else {
                QFont font;
                font.setPointSize(cellData.fontSize);
                font.setBold(cellData.isBold);

                qreal cellWidth = 0;
                for (int widthIndex = cellData.startCol; widthIndex < cellData.colSpan; widthIndex++) {
                    cellWidth += cellWidths[widthIndex];
                }

                QSizeF textSize = calculatedCellTextArea(text, font, false, QTextOption::WrapAnywhere, (cellWidth - (cellTextMargins * 2)));
                int actualCellHeight = (textSize.height() + (cellTextMargins * 2));

                if (actualCellHeight > rowHeights[rowIndex]) {
                    rowHeights[rowIndex] = actualCellHeight;
                }
            }
        }
    }

    if (catchedVerticalCells.size() != 0) {
        for (auto cellPos : catchedVerticalCells) {
            CellData cellData = datas[cellPos.first][cellPos.second];

            QString text = cellData.cellText;
            QFont font;
            font.setPointSize(cellData.fontSize);
            font.setBold(cellData.isBold);

            if (!text.contains("\n")) {
                cellData.cellText = text.split("").join("\n");  // 개행문자 추가 안되어 있을 경우 대비.
            }


            int sumCellHeight = 0;
            for (int cellStartRow = cellData.startRow; cellStartRow < (cellData.startRow + cellData.rowSpan); cellStartRow++) {
                sumCellHeight += rowHeights[cellStartRow];
            }

            QSizeF textSize = calculatedCellTextArea(text, font, true, QTextOption::NoWrap, 0);
            int actualCellHeight = (textSize.height() + (cellTextMargins * 2));

            if (actualCellHeight > sumCellHeight) {
                int dividedHeight = (textSize.height() + (cellTextMargins * 2)) / cellData.rowSpan;
                for (int cellStartRow = cellData.startRow; cellStartRow < (cellData.startRow + cellData.rowSpan); cellStartRow++) {
                    rowHeights[cellStartRow] += dividedHeight;
                }

                rowHeights[cellData.startRow] += dividedHeight % cellData.rowSpan;
            }
        }
    }

    return rowHeights;
}

QSizeF TableElement::calculatedCellTextArea(const QString &text, const QFont &font,
                                            const bool isVertical, const QTextOption::WrapMode &wrapMode, qreal fixedTextWidth) {
    qreal totalHeight = 0;

    if (isVertical) {
        int explicitLineCount = text.count('\n') + 1;
        QFontMetrics fm(font);
        totalHeight = explicitLineCount * fm.height();
        fixedTextWidth = fm.horizontalAdvance("가");
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

            line.setLineWidth(fixedTextWidth);
            totalHeight += line.height();
        }

        textLayout.endLayout();
    }

    return QSizeF(fixedTextWidth, totalHeight);
}

QSizeF TableElement::calculateSize(const QFontMetrics& fontMetrics,
                                   const QRectF& availableRect) {
    return QSizeF();
}

bool TableElement::needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) {
    return false;
}

void TableElement::renderTableRow(QPainter& painter, const QVector<QString>& rowData,
                                  const QPointF& startPos, bool isHeader) {

}

void TableElement::ensureDataLoaded() const {
    // if (dataCached || !dataProvider) return;

    // if (!customQuery.isEmpty()) {
    //     cachedData = dataProvider->executeQuery(customQuery);
    // } else if (!tableId.isEmpty()) {
    //     cachedData = dataProvider->getTableData(tableId);
    // }

    // dataCached = true;
}

void TableElement::refreshData() {
    dataCached = false;
    cachedData = TableData(); // 캐시 초기화
}

void TableElement::clearCache() {
    refreshData();
}

Qt::Alignment TableElement::getAlign() {
    return alignment;
}
