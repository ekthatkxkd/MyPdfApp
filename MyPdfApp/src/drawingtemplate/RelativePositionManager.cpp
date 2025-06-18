#include "include/drawingtemplate/RelativePositionManager.h"
#include <QDebug>

RelativePositionManager::RelativePositionManager(const QRectF& contentRect)
    : contentArea(contentRect) {
    defaultStartPosition = QPointF(contentRect.left(), contentRect.top());
}

void RelativePositionManager::registerRenderedElement(const RenderedElementInfo& info) {
    renderedElements[info.elementId] = info;
}

QPointF RelativePositionManager::calculatePosition(const PositionReference& positionRef) {
    if (positionRef.position == RelativePosition::None ||
        positionRef.referenceElementId.isEmpty()) {
        return defaultStartPosition + positionRef.offset;
    }

    // 참조 요소 찾기
    auto it = renderedElements.find(positionRef.referenceElementId);
    if (it == renderedElements.end()) {
        qWarning() << "Reference element not found:" << positionRef.referenceElementId;
        return defaultStartPosition + positionRef.offset;
    }

    const RenderedElementInfo& refElement = it.value();
    QPointF calculatedPos;

    switch (positionRef.position) {
    case RelativePosition::Below:
        calculatedPos = QPointF(refElement.boundingRect.left(),
                                refElement.boundingRect.bottom());
        break;

    case RelativePosition::RightOf:
        calculatedPos = QPointF(refElement.boundingRect.right(),
                                refElement.boundingRect.top());
        break;

    case RelativePosition::Above:
        calculatedPos = QPointF(refElement.boundingRect.left(),
                                refElement.boundingRect.top());
        break;

    case RelativePosition::LeftOf:
        calculatedPos = QPointF(refElement.boundingRect.left(),
                                refElement.boundingRect.top());
        break;

    case RelativePosition::SameRow:
        calculatedPos = QPointF(refElement.boundingRect.right(),
                                refElement.boundingRect.top());
        break;

    default:
        calculatedPos = defaultStartPosition;
        break;
    }

    // 오프셋 및 여백 적용
    calculatedPos += positionRef.offset;
    calculatedPos.setX(calculatedPos.x() + positionRef.margins.left());
    calculatedPos.setY(calculatedPos.y() + positionRef.margins.top());

    return calculatedPos;
}

QPointF RelativePositionManager::adjustPositionToContentArea(const QPointF& position,
                                                             const QSizeF& elementSize) {
    QPointF adjustedPos = position;

    // 오른쪽 경계 확인
    if (adjustedPos.x() + elementSize.width() > contentArea.right()) {
        adjustedPos.setX(contentArea.right() - elementSize.width());
    }

    // 왼쪽 경계 확인
    if (adjustedPos.x() < contentArea.left()) {
        adjustedPos.setX(contentArea.left());
    }

    return adjustedPos;
}

void RelativePositionManager::resetForNewPage() {
    // 필요에 따라 페이지별 요소 관리 로직 추가
}

void RelativePositionManager::clear() {
    renderedElements.clear();
}
