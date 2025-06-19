#ifndef IRENDERELEMENT_H
#define IRENDERELEMENT_H

#include "include/docElement/ElementInform.h"
#include <QPainter>
#include <QFontMetrics>
#include <memory>

using namespace ElementInform;

class IRenderElement {
public:
    virtual ~IRenderElement() = default;

    // 렌더링 (시작 위치와 사용 가능한 영역 제공)
    virtual QRectF render(QPainter& painter, const QPointF& startPos,
                          const QRectF& availableRect, int& currentPage,
                          std::function<void()> newPageCallback = nullptr) = 0;

    // 크기 계산
    virtual QSizeF calculateSize(const QFontMetrics& fontMetrics,
                                 const QRectF& availableRect) = 0;

    // 새 페이지 필요 여부 확인
    virtual bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) = 0;

    // ★ 새로운 메서드: 사전 페이지 체크가 필요한지 여부
    virtual bool requiresPrePageCheck() const { return true; }

    // 위치 참조 정보
    void setPositionReference(const PositionReference& ref) { positionRef = ref; }
    PositionReference getPositionReference() const { return positionRef; }

    // 요소 ID
    void setElementId(const QString& id) { elementId = id; }
    QString getElementId() const { return elementId; }

protected:
    PositionReference positionRef;
    QString elementId;
};

#endif // IRENDERELEMENT_H
