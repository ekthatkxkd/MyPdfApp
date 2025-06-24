#ifndef TEMPLATEFACTORY_H
#define TEMPLATEFACTORY_H

#include "include/drawingtemplate/DocumentTemplate.h"
#include <memory>

// 템플릿 팩토리
class TemplateFactory {
public:
    enum TemplateType {
        MATERIAL,
        DEFECTREPORT,
        ORDER,
        RECEIPTVOUCHER
    };

    static std::unique_ptr<DocumentTemplate> createTemplate(TemplateType type);
};

#endif // TEMPLATEFACTORY_H
