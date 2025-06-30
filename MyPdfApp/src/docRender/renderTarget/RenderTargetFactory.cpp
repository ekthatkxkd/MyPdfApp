#include "include/docrender/renderTarget/RenderTargetFactory.h"
#include "include/docRender/renderTarget/PdfRenderTarget.h"
#include "include/docRender/renderTarget/ImageRenderTarget.h"

std::shared_ptr<RenderTarget> RenderTargetFactory::createPdfTarget(const QString& filename) {
    return std::make_shared<PdfRenderTarget>(filename);
}

std::shared_ptr<RenderTarget> RenderTargetFactory::createImageTarget() {
    return std::make_shared<ImageRenderTarget>();
}
