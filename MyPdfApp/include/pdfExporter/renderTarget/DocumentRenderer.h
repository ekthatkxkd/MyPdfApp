#ifndef DOCUMENTRENDERER_H
#define DOCUMENTRENDERER_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include "include/drawingtemplate/DocumentTemplate.h"
#include <memory>

// 문서 렌더러 (Context)
class DocumentRenderer {
public:
    DocumentRenderer(std::unique_ptr<RenderTarget> target);

    void renderTemplate(std::unique_ptr<DocumentTemplate> docTemplate);
    void setRenderTarget(std::unique_ptr<RenderTarget> target);

private:
    std::unique_ptr<RenderTarget> renderTarget;
};

#endif // DOCUMENTRENDERER_H
