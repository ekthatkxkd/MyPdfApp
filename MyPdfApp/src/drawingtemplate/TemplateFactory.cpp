#include "include/drawingtemplate/TemplateFactory.h"
#include "include/drawingtemplate/MaterialDocTemplate.h"
#include "include/drawingtemplate/DefectReportDocTemplate.h"

std::unique_ptr<DocumentTemplate> TemplateFactory::createTemplate(TemplateType type) {
    switch(type) {
    case MATERIAL:
        return std::make_unique<MaterialDocTemplate>();
    case DEFECTREPORT:
        return std::make_unique<DefectReportDocTemplate>();
    default:
        return nullptr;
    }
}
