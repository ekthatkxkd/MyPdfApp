#include "include/docElement/TableElement.h"

TableElement::TableElement(const QString &title,
                           const QVector<CellData> &headerDatas,
                           const QVector<QVector<CellData>>& innerDatas,
                           const QVector<CellData> &footerDatas,
                           const QVector<qreal>& colWidths)
    : title(title),
    headerDatas(headerDatas), innerDatas(innerDatas), footerDatas(footerDatas),
    columnWidths(colWidths) {
}

QRectF TableElement::render(QPainter& painter, const QPointF& startPos,
                            const QRectF& availableRect, int& currentPage) {
    return QRectF();
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
