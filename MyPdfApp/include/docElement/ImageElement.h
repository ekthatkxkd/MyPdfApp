#ifndef IMAGEELEMENT_H
#define IMAGEELEMENT_H

#include "include/docElement/IRenderElement.h"

// 이미지 요소
class ImageElement : public IRenderElement {
public:
    ImageElement(const QPixmap& img, const QSizeF& maxSz = QSizeF(),
                 Qt::AspectRatioMode mode = Qt::KeepAspectRatio,
                 Qt::Alignment align = Qt::AlignCenter);

    QRectF render(QPainter& painter, const QPointF& startPos,
                  const QRectF& availableRect, int& currentPage,
                  std::function<void()> newPageCallback = nullptr) override;
    QSizeF calculateSize(const QFontMetrics& fontMetrics,
                         const QRectF& availableRect) override;
    bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) override;

private:
    QPixmap image;
    QSizeF maxSize;
    Qt::AspectRatioMode aspectRatioMode;
    Qt::Alignment alignment;
};

#endif // IMAGEELEMENT_H
