#ifndef DRAWINGMATERIALTEMPLATE_H
#define DRAWINGMATERIALTEMPLATE_H

#include "include/drawingtemplate/DrawingTemplate.h"

class DrawingMaterialTemplate : public DrawingTemplate {
public :
    DrawingMaterialTemplate();

    virtual void drawTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf) override;
};

#endif // DRAWINGMATERIALTEMPLATE_H
