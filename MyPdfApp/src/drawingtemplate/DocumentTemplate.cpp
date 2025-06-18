#include "include/drawingtemplate/DocumentTemplate.h"
#include <QFontMetrics>

DocumentTemplate::DocumentTemplate(const QString& name, const QSizeF& size)
    : templateName(name), pageSize(size), pageMargins(20, 20, 20, 20) {}

void DocumentTemplate::renderDocument(QPainter& painter) {
    setupTemplate();

    painter.begin(painter.device());

    RelativePositionManager positionManager(getContentRect());
    int currentPage = 1;

    renderHeader(painter, currentPage);

    for (auto& element : elements) {
        // 위치 참조가 새 페이지를 요구하는 경우
        if (element->getPositionReference().position == RelativePosition::NextPage) {
            //////// [LLDDSS] ORIGIN IS TO ALIVE
            // painter.newPage();
            // currentPage++;
            // positionManager.resetForNewPage();
            // renderHeader(painter, currentPage);
        }

        // 시작 위치 계산
        QPointF startPos = positionManager.calculatePosition(element->getPositionReference());

        // 사전 페이지 체크가 필요한 요소만 체크
        if (element->requiresPrePageCheck()) {
            QFontMetrics fm(painter.font());
            QSizeF elementSize = element->calculateSize(fm, getContentRect());

            // 위치 보정
            startPos = positionManager.adjustPositionToContentArea(startPos, elementSize);

            // 새 페이지 필요 여부 확인
            QRectF availableRect(startPos,
                                 QSizeF(getContentRect().right() - startPos.x(),
                                        getContentRect().bottom() - startPos.y()));

            if (element->needsNewPage(availableRect, elementSize)) {
                //////// [LLDDSS] ORIGIN IS TO ALIVE
                // painter.newPage();
                // currentPage++;
                // positionManager.resetForNewPage();
                // renderHeader(painter, currentPage);
                // startPos = QPointF(getContentRect().left(), getContentRect().top());
            }
        }

        // 모든 요소 렌더링
        QRectF availableRect(startPos,
                             QSizeF(getContentRect().right() - startPos.x(),
                                    getContentRect().bottom() - startPos.y()));

        //////// [LLDDSS] ORIGIN IS TO ALIVE
        // QSizeF actualSize = element->render(painter, startPos, availableRect, currentPage);
        // // 렌더링된 요소 정보 등록
        // QRectF elementRect(startPos, actualSize);
        // RenderedElementInfo renderedInfo(element->getElementId(), elementRect, currentPage);
        //////// [LLDDSS] MY ADDED SOURCE
        QRectF actualSize = element->render(painter, startPos, availableRect, currentPage);
        RenderedElementInfo renderedInfo(element->getElementId(), actualSize, currentPage);
        ////////

        positionManager.registerRenderedElement(renderedInfo);
    }

    renderFooter(painter, currentPage);
    painter.end();
}

void DocumentTemplate::addElement(std::unique_ptr<IRenderElement> element) {
    elements.push_back(std::move(element));
}

void DocumentTemplate::addElementBelow(std::unique_ptr<IRenderElement> element,
                                       const QString& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::below(referenceId, spacing));
    elements.push_back(std::move(element));
}

void DocumentTemplate::addElementRightOf(std::unique_ptr<IRenderElement> element,
                                         const QString& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::rightOf(referenceId, spacing));
    elements.push_back(std::move(element));
}

void DocumentTemplate::addElementSameRow(std::unique_ptr<IRenderElement> element,
                                         const QString& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::sameRow(referenceId, spacing));
    elements.push_back(std::move(element));
}

QRectF DocumentTemplate::getContentRect() const {
    return QRectF(QPointF(0, 0), pageSize).marginsRemoved(pageMargins);
}
