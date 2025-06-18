#include "include/pdfExporter/renderTarget/DocumentRenderer.h"

DocumentRenderer::DocumentRenderer(std::unique_ptr<IRenderTarget> target)
    : renderTarget(std::move(target)) {}

void DocumentRenderer::renderTemplate(std::unique_ptr<DocumentTemplate> docTemplate) {
    if (!renderTarget || !docTemplate) {
        return;
    }

    QPainter* painter = renderTarget->getPainter();
    if (!painter) {
        return;
    }

    // 템플릿 렌더링 실행
    docTemplate->renderDocument(*painter);

    // 렌더링 완료
    renderTarget->finalize();
}

void DocumentRenderer::setRenderTarget(std::unique_ptr<IRenderTarget> target) {
    renderTarget = std::move(target);
}
