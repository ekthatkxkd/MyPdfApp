#ifndef DOCTEMPLATEFACTORY_H
#define DOCTEMPLATEFACTORY_H

#include "include/docTemplate/DocTemplate.h"
#include <memory>

// 템플릿 팩토리
class DocTemplateFactory {
public:
    enum DocTemplateType {
        MATERIAL,
        DEFECTREPORT,
        ORDER,
        RECEIPTVOUCHER
    };

    static std::unique_ptr<DocTemplate> createTemplate(DocTemplateType type);
};

#endif // DOCTEMPLATEFACTORY_H
