#include "include/pdfExporter/renderTarget/RenderTargetFactory.h"
#include "include/pdfExporter/renderTarget/PdfRenderTarget.h"
#include "include/pdfExporter/renderTarget/ImageRenderTarget.h"

std::unique_ptr<RenderTarget> RenderTargetFactory::createPdfTarget(const QString& filename,
                                                                    const QSizeF& pageSize) {
    return std::make_unique<PdfRenderTarget>(filename);
}

std::unique_ptr<RenderTarget> RenderTargetFactory::createImageTarget(const QSizeF& pageSize) {
    return std::make_unique<ImageRenderTarget>();
}
