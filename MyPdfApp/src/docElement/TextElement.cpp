#include "include/docElement/TextElement.h"

TextElement::TextElement(const QString& text, const QFont& font,
                         Qt::Alignment align) : text(text), font(font), alignment(align) {

}

QRectF TextElement::render(QPainter& painter, const QPointF& startPos,
                           const QRectF& availableRect, int& currentPage) {

    return QRectF();
}

QSizeF TextElement::calculateSize(const QFontMetrics& fontMetrics,
                                  const QRectF& availableRect) {

    return QSizeF();
}

bool TextElement::needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) {

    return false;
}
