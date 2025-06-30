#ifndef RENDERTARGETFACTORY_H
#define RENDERTARGETFACTORY_H

#include "include/docRender/renderTarget/RenderTarget.h"
#include <memory>

class RenderTargetFactory {
public:
    static std::shared_ptr<RenderTarget> createPdfTarget(const QString& filename);

    static std::shared_ptr<RenderTarget> createImageTarget();
};

#endif // RENDERTARGETFACTORY_H
