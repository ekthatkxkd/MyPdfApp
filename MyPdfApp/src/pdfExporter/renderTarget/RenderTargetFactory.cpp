#include "include/pdfExporter/renderTarget/RenderTargetFactory.h"
#include "include/pdfExporter/renderTarget/PdfRenderTarget.h"
#include "include/pdfExporter/renderTarget/ImageRenderTarget.h"

std::shared_ptr<RenderTarget> RenderTargetFactory::createPdfTarget(const QString& filename) {
    return std::make_shared<PdfRenderTarget>(filename);
}

std::shared_ptr<RenderTarget> RenderTargetFactory::createImageTarget() {
    return std::make_shared<ImageRenderTarget>();
}
