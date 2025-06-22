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
    QPointF cursorPoint = startPos;

    if (title != "") {
        const qreal titleTableMargin = 5;
        QFont font;
        font.setPointSize(10);
        font.setBold(true);

        painter.setFont(font);
        painter.setPen(QPen(Qt::black, 1));

        QFontMetrics metrics(painter.font());
        QRectF boundingBox(cursorPoint, QSizeF(pxContentSize.width(), metrics.height()));

        Qt::AlignmentFlag align = Qt::AlignLeft;

        painter.drawText(boundingBox, align, title);

        cursorPoint = QPointF(cursorPoint.x(), cursorPoint.y() + boundingBox.height() + titleTableMargin);
    }

    //////// table header 그리기
    ///
    ///
    if (tableData.headerDatas.size() != 0) {
        qreal rowHeight = calculateOneRowHeight(tableData.headerDatas);
    }
    ///
    ///
    ////////

    //////// table inner 그리기
    ///
    ///

    ///
    ///
    ////////



    return QRectF();
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
