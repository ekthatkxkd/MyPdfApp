#include "include/docElement/TableElement.h"

TableElement::TableElement(const QString &title,
                           const QVector<QString> &headerDatas,
                           const QVector<QVector<QString>>& innerDatas,
                           const QVector<QString> &footerDatas,
                           const QVector<qreal>& colWidths)
    : title(title),
    headerDatas(headerDatas), innerDatas(innerDatas), footerDatas(footerDatas),
    columnWidths(colWidths), useDatabase(false), dataCached(true) {
}

// db 용 생성자
TableElement::TableElement(const QString &title,
                           std::shared_ptr<IDataProvider> provider,
                           const QString &tableIdentifier,
                           const QVector<qreal>& colWidths)
    : title(title),
    dataProvider(provider), tableId(tableIdentifier), columnWidths(colWidths),
    useDatabase(true), dataCached(false) {

}

// db 용 생성자. 커스텀 쿼리 생성자
TableElement::TableElement(const QString &title,
                           std::shared_ptr<IDataProvider> provider,
                           const QString& query,
                           bool isQuery, // 구분용 파라미터
                           const QVector<qreal>& colWidths)
    : title(title),
    dataProvider(provider), customQuery(query), columnWidths(colWidths),
    useDatabase(true), dataCached(false) {

}

QRectF TableElement::render(QPainter& painter, const QPointF& startPos,
                            const QRectF& availableRect, int& currentPage,
                            std::function<void()> newPageCallback) {
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

const TableData& TableElement::getData() const {
    if (!useDatabase) {
        static TableData tableData;

        tableData.title = title;
        tableData.headers = headerDatas;
        tableData.rows = innerDatas;
        tableData.footers = footerDatas;

        return tableData;
    }

    ensureDataLoaded();
    return cachedData;
}

void TableElement::ensureDataLoaded() const {
    if (dataCached || !dataProvider) return;

    if (!customQuery.isEmpty()) {
        cachedData = dataProvider->executeQuery(customQuery);
    } else if (!tableId.isEmpty()) {
        cachedData = dataProvider->getTableData(tableId);
    }

    dataCached = true;
}

void TableElement::refreshData() {
    dataCached = false;
    cachedData = TableData(); // 캐시 초기화
}

void TableElement::clearCache() {
    refreshData();
}
