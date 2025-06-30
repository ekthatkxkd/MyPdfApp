#ifndef DOCTEMPLATE_H
#define DOCTEMPLATE_H

#include "include/docElement/IRenderElement.h"
#include "include/dataProvider/SqliteDataProvider.h"
#include <vector>
#include <memory>
#include <QPainter>
#include <QMap>
#include <QLocale>

class DocTemplate {
public:
    DocTemplate(const QString& name, const QString &title = "", const QSizeF& size = QSizeF(595, 842));
    virtual ~DocTemplate() = default;

    void renderDocument(QPainter& painter, const QSizeF &pxContentSize, std::function<void()> newPageCb);

    virtual void setupTemplate(const QSizeF &pxContentSize) = 0;

    // 요소 추가 메서드들
    void addElement(std::unique_ptr<IRenderElement> element);
    void addElementBelow(std::unique_ptr<IRenderElement> element,
                         const QVector<QString>& referenceId, qreal spacing = 5);
    void addElementRightOf(std::unique_ptr<IRenderElement> element,
                           const QVector<QString>& referenceId, qreal spacing = 5);
    void addElementSameRow(std::unique_ptr<IRenderElement> element,
                           const QVector<QString>& referenceId, qreal spacing = 5);

protected:
    const qreal belowSpacing = 5;
    const qreal sideSpacing = 5;

    QRectF getContentRect() const;

    std::vector<std::unique_ptr<IRenderElement>> elements;
    QSizeF pageSize;
    QMarginsF pageMargins;
    QString templateName;
    const QString templateTitle;
    std::function<void()> newPageCallback;
};

#endif // DOCTEMPLATE_H
