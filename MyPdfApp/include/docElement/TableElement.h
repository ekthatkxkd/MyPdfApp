#ifndef TABLEELEMENT_H
#define TABLEELEMENT_H

#include "include/docElement/IRenderElement.h"
#include "include/drawingtemplate/dataProvider/DataProvider.h"

// 표 요소
class TableElement : public IRenderElement {
public:
    TableElement(const QString &title,
                 const QVector<QString> &headerDatas,
                 const QVector<QVector<QString>>& innerDatas,
                 const QVector<QString> &footerDatas,
                 const QVector<qreal>& colWidths = QVector<qreal>());

    // db 용 생성자 - 데이터베이스 테이블 ID용 생성자
    TableElement(const QString &title,
                 std::shared_ptr<IDataProvider> provider,
                 const QString &tableIdentifier,
                 const QVector<qreal>& colWidths = QVector<qreal>());

    // db 용 생성자 - 커스텀 쿼리용 생성자
    TableElement(const QString &title,
                 std::shared_ptr<IDataProvider> provider,
                 const QString& query,
                 bool isQuery, // 단순히 상위 db 생성자와 구분하기 위한 파라미터
                 const QVector<qreal>& colWidths = QVector<qreal>());

    void refreshData(); // db 용 데이터 새로고침
    void clearCache();  // db 용 캐시 초기화

    QRectF render(QPainter& painter, const QPointF& startPos,
                  const QRectF& availableRect, int& currentPage,
                  std::function<void()> newPageCallback = nullptr) override;
    QSizeF calculateSize(const QFontMetrics& fontMetrics,
                         const QRectF& availableRect) override;
    bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) override;

private:
    void renderTableRow(QPainter& painter, const QVector<QString>& rowData,
                        const QPointF& startPos, bool isHeader);

    const QString title;

    QVector<qreal> columnWidths;
    QFont headerFont;
    QFont cellFont;

    QVector<QString> headerDatas;
    QVector<QVector<QString>> innerDatas;
    QVector<QString> footerDatas;

    std::shared_ptr<IDataProvider> dataProvider;
    QString tableId;                         // 테이블 식별자
    QString customQuery;                     // 커스텀 쿼리
    bool useDatabase;                        // 데이터베이스 사용 여부
    mutable TableData cachedData;            // 캐시된 데이터
    mutable bool dataCached;                 // 캐시 상태
    const TableData& getData() const;        // 데이터 조회 (캐시 포함)
    void ensureDataLoaded() const;          // 데이터 로딩 보장
};

#endif // TABLEELEMENT_H
