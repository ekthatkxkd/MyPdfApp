#ifndef DOCUMENTRENDERER_H
#define DOCUMENTRENDERER_H

#include "include/docRender/renderTarget/RenderTarget.h"
#include "include/docTemplate/DocTemplate.h"
#include <memory>

class DocumentRenderer {
public:
    DocumentRenderer(std::shared_ptr<RenderTarget> target);

    bool renderTemplate(std::unique_ptr<DocTemplate> docTemplate);
    void setRenderTarget(std::unique_ptr<RenderTarget> target);

private:
    std::shared_ptr<RenderTarget> renderTarget;
};

#endif // DOCUMENTRENDERER_H
