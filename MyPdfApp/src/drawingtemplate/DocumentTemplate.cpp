#include "include/drawingtemplate/DocumentTemplate.h"
#include <QFontMetrics>

DocumentTemplate::DocumentTemplate(const QString& name, const QString &title, const QSizeF& size)
    : templateName(name), templateTitle(title), pageSize(size), pageMargins(20, 20, 20, 20) {
}

void DocumentTemplate::renderDocument(QPainter& painter, const QSizeF &pxContentSize, std::function<void()> newPageCb) {
    setupTemplate(pxContentSize);

    newPageCallback = newPageCb;

    RelativePositionManager positionManager(getContentRect());
    int currentPage = 1;

    renderHeader(painter, currentPage);  // (옵션) 하단 페이지 번호

    QPointF startPos(0.0, 0.0);
    QMap<QString, QRectF> createdElementSize;

    // [LLDDSS] 생성된 각 element 의 size 를 저장하는 객체 초기화.
    for (auto &element : elements) {
        createdElementSize.insert(element->getElementId(),
                                  QRectF(QPointF(0, 0), QSizeF(0, 0)));
    }


    for (auto &element : elements) {

        //////// [LLDDSS] element 의 시작 position 계산
        ///
        ///
        PositionReference posRef = element->getPositionReference();

        if (posRef.referenceElementId.size() != 0) {
            if (posRef.position == RelativePosition::Below) {
                QRectF maxElement(QPointF(0, 0), QSizeF(0, 0));
                qreal maxYPos = maxElement.bottom();

                for (auto &refId : posRef.referenceElementId) {
                    QRectF refSize = createdElementSize[refId];
                    qreal refNextYPos = createdElementSize[refId].bottom();
                    if (refNextYPos > maxYPos) {
                        maxElement = refSize;
                    }
                }

                startPos = QPointF(0, (maxElement.bottom() + posRef.spacing));
            } else if (posRef.position == RelativePosition::RightOf) {
                QRectF maxElement(QPointF(0, 0), QSizeF(0, 0));
                qreal maxXPos = maxElement.right();

                for (auto &refId : posRef.referenceElementId) {
                    QRectF refSize = createdElementSize[refId];
                    qreal refNextXPos = createdElementSize[refId].right();
                    if (refNextXPos > maxXPos) {
                        maxElement = refSize;
                    }
                }

                startPos = QPointF((maxElement.right() + posRef.spacing), startPos.y());
            }
        }
        ///
        ///
        ////////

        QRectF actualSize = element->render(painter, startPos, pxContentSize, newPageCb);

        createdElementSize[element->getElementId()] = actualSize;
    }

    //////// [LLDDSS] ORIGIN IS TO ALIVE
    ///
    ///
    // for (auto& element : elements) {
    //     // 위치 참조가 새 페이지를 요구하는 경우
    //     if (element->getPositionReference().position == RelativePosition::NextPage) {
    //         if (newPageCallback) {
    //             newPageCallback();
    //         }
    //         currentPage++;
    //         positionManager.resetForNewPage();
    //         renderHeader(painter, currentPage);
    //     }

    //     // 시작 위치 계산
    //     QPointF startPos = positionManager.calculatePosition(element->getPositionReference());

    //     // 사전 페이지 체크가 필요한 요소만 체크
    //     if (element->requiresPrePageCheck()) {
    //         QFontMetrics fm(painter.font());
    //         QSizeF elementSize = element->calculateSize(fm, getContentRect());

    //         // 위치 보정
    //         startPos = positionManager.adjustPositionToContentArea(startPos, elementSize);

    //         // 새 페이지 필요 여부 확인
    //         QRectF availableRect(startPos,
    //                              QSizeF(getContentRect().right() - startPos.x(),
    //                                     getContentRect().bottom() - startPos.y()));

    //         if (element->needsNewPage(availableRect, elementSize)) {
    //             if (newPageCallback) {
    //                 newPageCallback();
    //             }
    //             currentPage++;
    //             positionManager.resetForNewPage();
    //             renderHeader(painter, currentPage);
    //             startPos = QPointF(getContentRect().left(), getContentRect().top());
    //         }
    //     }

    //     // 모든 요소 렌더링
    //     QRectF availableRect(startPos,
    //                          QSizeF(getContentRect().right() - startPos.x(),
    //                                 getContentRect().bottom() - startPos.y()));

    //     //////// [LLDDSS] ORIGIN IS TO ALIVE
    //     // QSizeF actualSize = element->render(painter, startPos, availableRect, currentPage);
    //     // // 렌더링된 요소 정보 등록
    //     // QRectF elementRect(startPos, actualSize);
    //     // RenderedElementInfo renderedInfo(element->getElementId(), elementRect, currentPage);
    //     //////// [LLDDSS] MY ADDED SOURCE
    //     QRectF actualSize = element->render(painter, startPos, availableRect, currentPage, newPageCb);
    //     RenderedElementInfo renderedInfo(element->getElementId(), actualSize, currentPage);
    //     ////////

    //     positionManager.registerRenderedElement(renderedInfo);
    // }
    // renderFooter(painter, currentPage);
    ///
    ///
    ////////

    painter.end();
}

void DocumentTemplate::addElement(std::unique_ptr<IRenderElement> element) {
    elements.push_back(std::move(element));
}

void DocumentTemplate::addElementBelow(std::unique_ptr<IRenderElement> element,
                                       const QVector<QString>& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::below(referenceId, spacing));
    elements.push_back(std::move(element));
}

void DocumentTemplate::addElementRightOf(std::unique_ptr<IRenderElement> element,
                                         const QVector<QString>& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::rightOf(referenceId, spacing));
    elements.push_back(std::move(element));
}

QRectF DocumentTemplate::getContentRect() const {
    return QRectF(QPointF(0, 0), pageSize).marginsRemoved(pageMargins);
}
