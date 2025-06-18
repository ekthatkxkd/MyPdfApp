#ifndef MATERIALDOCTEMPLATE_H
#define MATERIALDOCTEMPLATE_H

#include "include/drawingtemplate/DocumentTemplate.h"

// 구체적인 템플릿 클래스들
class MaterialDocTemplate : public DocumentTemplate {
public:
    MaterialDocTemplate();

    void setupTemplate() override;

private:
    QVector<QVector<QString>> generateTableData(int tableIndex);
};

#endif // MATERIALDOCTEMPLATE_H
