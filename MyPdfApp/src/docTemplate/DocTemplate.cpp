#include "include/docTemplate/DocTemplate.h"
#include <QFontMetrics>

DocTemplate::DocTemplate(const QString& name, const QString &title, const QSizeF& size)
    : templateName(name), templateTitle(title), pageSize(size), pageMargins(20, 20, 20, 20) {
}

void DocTemplate::renderDocument(QPainter& painter, const QSizeF &pxContentSize, std::function<void()> newPageCb) {
    setupTemplate(pxContentSize);

    newPageCallback = newPageCb;

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

    painter.end();
}

void DocTemplate::addElement(std::unique_ptr<IRenderElement> element) {
    elements.push_back(std::move(element));
}

void DocTemplate::addElementBelow(std::unique_ptr<IRenderElement> element,
                                       const QVector<QString>& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::below(referenceId, spacing));
    elements.push_back(std::move(element));
}

void DocTemplate::addElementRightOf(std::unique_ptr<IRenderElement> element,
                                         const QVector<QString>& referenceId, qreal spacing) {
    element->setPositionReference(PositionReference::rightOf(referenceId, spacing));
    elements.push_back(std::move(element));
}

QRectF DocTemplate::getContentRect() const {
    return QRectF(QPointF(0, 0), pageSize).marginsRemoved(pageMargins);
}
