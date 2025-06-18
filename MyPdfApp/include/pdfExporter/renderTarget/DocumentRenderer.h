#ifndef DOCUMENTRENDERER_H
#define DOCUMENTRENDERER_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include "include/drawingtemplate/DocumentTemplate.h"
#include <memory>

// 문서 렌더러 (Context)
class DocumentRenderer {
private:
    std::unique_ptr<IRenderTarget> renderTarget;

public:
    DocumentRenderer(std::unique_ptr<IRenderTarget> target);

    void renderTemplate(std::unique_ptr<DocumentTemplate> docTemplate);
    void setRenderTarget(std::unique_ptr<IRenderTarget> target);
};

#endif // DOCUMENTRENDERER_H
