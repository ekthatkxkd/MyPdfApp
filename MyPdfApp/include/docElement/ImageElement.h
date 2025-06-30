#ifndef IMAGEELEMENT_H
#define IMAGEELEMENT_H

#include "include/docElement/IRenderElement.h"

class ImageElement : public IRenderElement {
public:
    ImageElement(const QString &imgPath, const bool &isFillPageWidth = false,
                 Qt::Alignment align = Qt::AlignHCenter);

    QRectF render(QPainter& painter, const QPointF& startPos,
                  const QSizeF &pxContentSize,
                  std::function<void()> newPageCallback = nullptr) override;
    QSizeF calculateSize(const QFontMetrics& fontMetrics,
                         const QRectF& availableRect) override;
    bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) override;

    void initPixMap();

    Qt::Alignment getAlign() override;

private:
    QString imgPath;
    bool isFillPageWidth;
    Qt::Alignment alignment;
    QPixmap image;

    bool isImageLoaded = false;

};

#endif // IMAGEELEMENT_H
