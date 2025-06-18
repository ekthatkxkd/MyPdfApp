#include "include/drawingtemplate/TemplateFactory.h"

std::unique_ptr<DocumentTemplate> TemplateFactory::createTemplate(TemplateType type) {
    switch(type) {
    // case MATERIAL:
    //     return std::make_unique<TemplateA>();
    // case DEFECTREPORT:
    //     return std::make_unique<TemplateB>();
    default:
        return nullptr;
    }
}
