#ifndef TABLEELEMENT_H
#define TABLEELEMENT_H

#include "include/docElement/IRenderElement.h"

// 표 요소
class TableElement : public IRenderElement {
public:
    TableElement(const QString &title,
                 const QVector<CellData> &headerDatas,
                 const QVector<QVector<CellData>>& innerDatas,
                 const QVector<CellData> &footerDatas,
                 const QVector<qreal>& colWidths = QVector<qreal>());

    QRectF render(QPainter& painter, const QPointF& startPos,
                  const QRectF& availableRect, int& currentPage) override;
    QSizeF calculateSize(const QFontMetrics& fontMetrics,
                         const QRectF& availableRect) override;
    bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) override;

private:
    void renderTableRow(QPainter& painter, const QVector<QString>& rowData,
                        const QPointF& startPos, bool isHeader);

    const QString title;
    QVector<CellData> headerDatas;
    QVector<QVector<CellData>> innerDatas;
    QVector<CellData> footerDatas;

    QVector<qreal> columnWidths;
    QFont headerFont;
    QFont cellFont;
};

#endif // TABLEELEMENT_H
