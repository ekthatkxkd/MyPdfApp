#ifndef TEXTELEMENT_H
#define TEXTELEMENT_H

#include "include/docElement/IRenderElement.h"

// 텍스트 요소
class TextElement : public IRenderElement {
public:
    TextElement(const QString& text, const QFont& font = QFont(),
                Qt::Alignment align = Qt::AlignLeft);

    QRectF render(QPainter& painter, const QPointF& startPos,
                  const QSizeF &pxContentSize,
                  std::function<void()> newPageCallback = nullptr) override;

    QSizeF calculateSize(const QFontMetrics& fontMetrics,
                         const QRectF& availableRect) override;

    bool needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) override;

    Qt::Alignment getAlign() override;

private:
    QString text;
    QFont font;
    Qt::Alignment alignment;
};

#endif // TEXTELEMENT_H
