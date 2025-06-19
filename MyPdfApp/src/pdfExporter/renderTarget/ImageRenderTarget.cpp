#include "include/pdfExporter/renderTarget/ImageRenderTarget.h"

ImageRenderTarget::ImageRenderTarget(const QSizeF& pageSize, QImage::Format format)
    : RenderTarget(), pageSize(pageSize) {
    int width = static_cast<int>(pageSize.width());
    int height = static_cast<int>(pageSize.height());

    image = std::make_unique<QImage>(width, height, format);
    image->fill(Qt::white);

    painter = std::make_unique<QPainter>(image.get());
}

ImageRenderTarget::~ImageRenderTarget() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}

void ImageRenderTarget::finalize() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}

bool ImageRenderTarget::saveImage(const QString& filename, const char* format) {
    if (!image) return false;
    return image->save(filename, format);
}
