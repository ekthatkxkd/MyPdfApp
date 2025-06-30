#ifndef TABLEELEMENT_H
#define TABLEELEMENT_H

#include "include/docElement/IRenderElement.h"
#include "include/dataProvider/IDataProvider.h"

// 표 요소
class TableElement : public IRenderElement {
public:
    TableElement(const QString &title,
                 const TableData tableData,
                 const qreal tableFullWidth,
                 const QVector<qreal>& widthRatio = QVector<qreal>(),
                 Qt::Alignment align = Qt::AlignLeft);

    void refreshData(); // db 용 데이터 새로고침
    void clearCache();  // db 용 캐시 초기화

    QRectF render(QPainter& painter, const QPointF& startPos,
                  const QSizeF &pxContentSize,
                  std::function<void()> newPageCallback = nullptr) override;
    QSizeF calculateSize(const QFontMetrics& fontMetrics,
                         const QRectF& availableRect) override;
    bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) override;

    Qt::Alignment getAlign() override;

    qreal calculateOneRowHeight(QPainter& painter, QVector<CellData> &rowDatas);
    QVector<qreal> calculateRowHeights(QPainter& painter, QVector<QVector<CellData>> &datas);

    QSizeF calculatedCellTextArea(const QString &text, const QFont &font,
                                  const bool isVertical, const QTextOption::WrapMode &wrapMode, qreal fixedWidth);


private:
    void initCellWidth();
    void renderTableRow(QPainter& painter, const QVector<QString>& rowData,
                        const QPointF& startPos, bool isHeader);

    const QString title;
    const int cellTextMargins = 10;

    TableData tableData;

    Qt::Alignment alignment;

    qreal tableFullWidth;
    QVector<qreal> widthRatio;
    QVector<qreal> cellWidths;
    QFont headerFont;
    QFont cellFont;

    QVector<TableData> headerDatas;
    QVector<QVector<TableData>> innerDatas;
    QVector<TableData> footerDatas;

    std::shared_ptr<IDataProvider> dataProvider;
    QString tableId;                         // 테이블 식별자
    QString customQuery;                     // 커스텀 쿼리
    bool useDatabase;                        // 데이터베이스 사용 여부
    mutable TableData cachedData;            // 캐시된 데이터
    mutable bool dataCached;                 // 캐시 상태
    void ensureDataLoaded() const;          // 데이터 로딩 보장
};

#endif // TABLEELEMENT_H
