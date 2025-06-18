#ifndef DOCUMENTTEMPLATE_H
#define DOCUMENTTEMPLATE_H

#include "include/docElement/IRenderElement.h"
#include "include/drawingtemplate/RelativePositionManager.h"
// #include <QVector>
#include <vector>
#include <memory>
#include <QPainter>

class DocumentTemplate {
public:
    DocumentTemplate(const QString& name, const QSizeF& size = QSizeF(595, 842));
    virtual ~DocumentTemplate() = default;

    void renderDocument(QPainter& painter);
    virtual void setupTemplate() = 0;

    // 요소 추가 메서드들
    void addElement(std::unique_ptr<IRenderElement> element);
    void addElementBelow(std::unique_ptr<IRenderElement> element,
                         const QString& referenceId, qreal spacing = 10);
    void addElementRightOf(std::unique_ptr<IRenderElement> element,
                           const QString& referenceId, qreal spacing = 10);
    void addElementSameRow(std::unique_ptr<IRenderElement> element,
                           const QString& referenceId, qreal spacing = 10);

protected:
    virtual void renderHeader(QPainter& painter, int pageNumber) {}
    virtual void renderFooter(QPainter& painter, int pageNumber) {}

    QRectF getContentRect() const;


    std::vector<std::unique_ptr<IRenderElement>> elements;
    QSizeF pageSize;
    QMarginsF pageMargins;
    QString templateName;
};

#endif // DOCUMENTTEMPLATE_H
