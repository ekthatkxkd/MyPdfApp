#include "include/drawingtemplate/TemplateFactory.h"
#include "include/drawingtemplate/MaterialDocTemplate.h"
#include "include/drawingtemplate/DefectReportDocTemplate.h"
#include "include/drawingtemplate/OrderDocTemplate.h"
#include "include/drawingtemplate/ReceiptVoucherDocTemplate.h"

std::unique_ptr<DocumentTemplate> TemplateFactory::createTemplate(TemplateType type) {
    switch(type) {
    case MATERIAL:
        return std::make_unique<MaterialDocTemplate>();
    case DEFECTREPORT:
        return std::make_unique<DefectReportDocTemplate>();
    case ORDER:
        return std::make_unique<OrderDocTemplate>();
    case RECEIPTVOUCHER:
        return std::make_unique<ReceiptVoucherDocTemplate>();
    default:
        return nullptr;
    }
}
