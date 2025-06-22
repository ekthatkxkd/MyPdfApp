#ifndef DOCUMENTTEMPLATE_H
#define DOCUMENTTEMPLATE_H

#include "include/docElement/IRenderElement.h"
#include "include/drawingtemplate/RelativePositionManager.h"
// #include <QVector>
#include <vector>
#include <memory>
#include <QPainter>
#include <QQuickItem>
#include <QMap>

class DocumentTemplate {
public:
    DocumentTemplate(const QString& name, const QString &title = "", const QSizeF& size = QSizeF(595, 842));
    virtual ~DocumentTemplate() = default;

    void renderDocument(QPainter& painter, const QSizeF &pxContentSize, std::function<void()> newPageCb);

    // virtual void setupTemplate(const QQuickItem *rootItem) = 0;
    //////// [LLDDSS] ex) QMap<"informTable", QList<QPair<"company", QList("TIMATEC")>>>
    ///                   QMap<"history", QList<QPair<"row", QList("250101","Deco","1,232","its my items")>>>
    virtual void setupTemplate(const QSizeF &pxContentSize) = 0;

    // 요소 추가 메서드들
    void addElement(std::unique_ptr<IRenderElement> element);
    void addElementBelow(std::unique_ptr<IRenderElement> element,
                         const QVector<QString>& referenceId, qreal spacing = 5);
    void addElementRightOf(std::unique_ptr<IRenderElement> element,
                           const QVector<QString>& referenceId, qreal spacing = 5);
    void addElementSameRow(std::unique_ptr<IRenderElement> element,
                           const QVector<QString>& referenceId, qreal spacing = 5);

    // const QList<QString> materialComponentNames = {"informTable", "history"};

protected:
    const qreal belowSpacing = 5;
    const qreal sideSpacing = 5;

    virtual void renderHeader(QPainter& painter, int pageNumber) {}
    virtual void renderFooter(QPainter& painter, int pageNumber) {}

    QRectF getContentRect() const;

    QHash<QString, QQuickItem*> templateQuickItems;

    std::vector<std::unique_ptr<IRenderElement>> elements;
    QSizeF pageSize;
    QMarginsF pageMargins;
    QString templateName;
    const QString templateTitle;
    std::function<void()> newPageCallback;
};

#endif // DOCUMENTTEMPLATE_H
