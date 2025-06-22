#include "include/pdfExporter/renderTarget/DocumentRenderer.h"

DocumentRenderer::DocumentRenderer(std::unique_ptr<RenderTarget> target)
    : renderTarget(std::move(target)) {

}

void DocumentRenderer::renderTemplate(std::unique_ptr<DocumentTemplate> docTemplate) {
    if (!renderTarget || !docTemplate) {
        return;
    }

    QPainter* painter = renderTarget->getPainter();
    if (!painter) {
        return;
    }

    QSizeF pxContentSize = renderTarget->getPxContentSize();

    // ★ 콜백으로 newPage 전달
    auto newPageCallback = [this]() {
        this->renderTarget->newPage();
    };

    // 템플릿 렌더링 실행
    docTemplate->renderDocument(*painter, pxContentSize, newPageCallback);

    // 렌더링 완료
    renderTarget->finalize();
}

void DocumentRenderer::setRenderTarget(std::unique_ptr<RenderTarget> target) {
    renderTarget = std::move(target);
}
