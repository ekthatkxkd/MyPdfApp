#ifndef ELEMENTINFORM_H
#define ELEMENTINFORM_H

#include <QString>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QMarginsF>
#include <QColor>

namespace ElementInform{

// 상대적 위치 타입
enum class RelativePosition {
    None,           // 위치 참조 없음 (첫 번째 요소나 절대 위치)
    Below,          // 참조 요소의 아래쪽
    RightOf,        // 참조 요소의 오른쪽
    NextPage        // 새 페이지 시작
};

// 위치 참조 정보
struct PositionReference {
    QVector<QString> referenceElementId;     // 참조할 요소의 ID
    RelativePosition position;      // 상대적 위치
    qreal spacing;

    PositionReference(const QVector<QString>& refId = QVector<QString>(),
                      RelativePosition pos = RelativePosition::None,
                      const qreal spacing = 0)
        : referenceElementId(refId), position(pos), spacing(spacing) {}

    // 편의 생성자들
    static PositionReference below(const QVector<QString>& refIds, qreal spacing = 0) {
        return PositionReference(refIds, RelativePosition::Below, spacing);
    }
    static PositionReference rightOf(const QVector<QString>& refId, qreal spacing = 0) {
        return PositionReference(refId, RelativePosition::RightOf, spacing);
    }
    static PositionReference nextPage() {
        return PositionReference(QVector<QString>(), RelativePosition::NextPage);
    }
};

// 렌더링된 요소 정보
struct RenderedElementInfo {
    QString elementId;
    QRectF boundingRect;    // 실제 렌더링된 영역
    int pageNumber;         // 렌더링된 페이지 번호

    RenderedElementInfo(const QString& id = QString(),
                        const QRectF& rect = QRectF(),
                        int page = 1)
        : elementId(id), boundingRect(rect), pageNumber(page) {}
};

// Table 을 구성하는 Cell 정보
struct CellData {
    QString cellId;
    int startRow, startCol;
    int rowSpan, colSpan;
    QString cellText;
    QColor bgColor;
    bool isBold;
    int fontSize;
    QString alignPosition;
    bool isVerticalDir;


    CellData(QString id, int r, int c, int rs, int cs, const QString& text,
             const QColor& bg, bool bold, int fSize, const QString& align, bool isVDir) :
        cellId(id),
        startRow(r), startCol(c), rowSpan(rs), colSpan(cs),
        cellText(text), bgColor(bg),
        isBold(bold), fontSize(fSize), alignPosition(align),
        isVerticalDir(isVDir) {}
};

// 테이블 데이터 구조체
struct TableData {
    QString title;                              // 테이블 제목 (옵션)
    QVector<CellData> headerDatas;                    // 컬럼 헤더
    QVector<QVector<CellData>> innerDatas;             // 데이터 행들
    QVector<CellData> footerDatas;                    // 푸터
    // QVector<qreal> columnWidths;                // 컬럼 너비 (옵션)

    TableData() = default;
    TableData(const QVector<CellData>& hdrs, const QVector<QVector<CellData>>& inner, QVector<CellData>& footer)
        : headerDatas(hdrs), innerDatas(inner), footerDatas(footer) {}
};
}

#endif // ELEMENTINFORM_H
