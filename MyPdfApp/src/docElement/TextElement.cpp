#include "include/docElement/TextElement.h"

TextElement::TextElement(const QString& text, const QFont& font,
                         Qt::Alignment align)
    : text(text), font(font), alignment(align) {

}

QRectF TextElement::render(QPainter& painter, const QPointF& startPos,
                           const QSizeF &pxContentSize,
                           std::function<void()> newPageCallback) {

    QString titleText = text;

    painter.setFont(font);

    QFontMetrics metrics(painter.font());
    QRectF boundingBox(startPos, QSizeF(pxContentSize.width(), metrics.height()));

    painter.drawText(boundingBox, alignment, titleText);

    return boundingBox;
}

QSizeF TextElement::calculateSize(const QFontMetrics& fontMetrics,
                                  const QRectF& availableRect) {

    return QSizeF();
}

bool TextElement::needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) {

    return false;
}

Qt::Alignment TextElement::getAlign() {
    return alignment;
}
