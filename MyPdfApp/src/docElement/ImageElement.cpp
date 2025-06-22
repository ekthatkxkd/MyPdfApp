#include "include/docElement/ImageElement.h"


ImageElement::ImageElement(const QPixmap& img, const QSizeF& maxSz,
                           Qt::AspectRatioMode mode,
                           Qt::Alignment align)
    : image(img), maxSize(maxSz), aspectRatioMode(mode), alignment(align) {

}

QRectF ImageElement::render(QPainter& painter, const QPointF& startPos,
                            const QSizeF &pxContentSize,
                            std::function<void()> newPageCallback) {
    return QRectF();
}

QSizeF ImageElement::calculateSize(const QFontMetrics& fontMetrics,
                                   const QRectF& availableRect) {
    return QSizeF();
}

bool ImageElement::needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) {
    return false;
}
