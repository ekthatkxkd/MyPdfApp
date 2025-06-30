#include "include/docRender/DocumentRenderer.h"

DocumentRenderer::DocumentRenderer(std::shared_ptr<RenderTarget> target)
    : renderTarget(target) {

}

bool DocumentRenderer::renderTemplate(std::unique_ptr<DocTemplate> docTemplate) {
    if (!renderTarget || !docTemplate) {
        return false;
    }

    QPainter* painter = renderTarget->getPainter();
    if (!painter) {
        return false;
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

    return true;
}

void DocumentRenderer::setRenderTarget(std::unique_ptr<RenderTarget> target) {
    renderTarget = std::move(target);
}
