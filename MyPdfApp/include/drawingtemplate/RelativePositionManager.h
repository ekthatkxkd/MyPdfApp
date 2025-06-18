#ifndef RELATIVEPOSITIONMANAGER_H
#define RELATIVEPOSITIONMANAGER_H

#include "include/docElement/ElementInform.h"
#include <QMap>

using namespace ElementInform;

class RelativePositionManager {
public:
    RelativePositionManager(const QRectF& contentRect);

    // 렌더링된 요소 정보 등록
    void registerRenderedElement(const RenderedElementInfo& info);

    // 상대적 위치 계산
    QPointF calculatePosition(const PositionReference& positionRef);

    // 콘텐츠 영역 내 위치 보정
    QPointF adjustPositionToContentArea(const QPointF& position, const QSizeF& elementSize);

    // 새 페이지 시작시 초기화
    void resetForNewPage();

    void clear();

private:
    QMap<QString, RenderedElementInfo> renderedElements;
    QRectF contentArea;
    QPointF defaultStartPosition;
};

#endif // RELATIVEPOSITIONMANAGER_H
