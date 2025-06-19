#ifndef RENDERTARGETFACTORY_H
#define RENDERTARGETFACTORY_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include <memory>

class RenderTargetFactory {
public:
    static std::unique_ptr<RenderTarget> createPdfTarget(const QString& filename,
                                                          const QSizeF& pageSize);

    static std::unique_ptr<RenderTarget> createImageTarget(const QSizeF& pageSize);
};

#endif // RENDERTARGETFACTORY_H
