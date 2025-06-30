#include "include/docTemplate/DocTemplateFactory.h"

#include "include/docTemplate/MaterialDocTemplate.h"
#include "include/docTemplate/DefectReportDocTemplate.h"
#include "include/docTemplate/OrderDocTemplate.h"
#include "include/docTemplate/ReceiptVoucherDocTemplate.h"

std::unique_ptr<DocTemplate> DocTemplateFactory::createTemplate(DocTemplateType type) {
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
