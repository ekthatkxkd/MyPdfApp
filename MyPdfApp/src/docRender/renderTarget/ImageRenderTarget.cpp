#include "include/docRender/renderTarget/ImageRenderTarget.h"
#include <QDebug>

ImageRenderTarget::ImageRenderTarget(QImage::Format format)
    : RenderTarget() {
    initDefaultImage(format);

    initPainter();
}

ImageRenderTarget::~ImageRenderTarget() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}

void ImageRenderTarget::initDefaultImage(QImage::Format format) {
    image = std::make_shared<QImage>(pageSettings.pxImageSize, format);
    image->fill(Qt::white);

    // DPI 정보 설정 (QPainter 스케일링 정확성을 위해)
    double dotsPerMeter = pageSettings.resolution * 39.3701; // 1인치 = 39.3701 dots/meter
    image->setDotsPerMeterX(qRound(dotsPerMeter));
    image->setDotsPerMeterY(qRound(dotsPerMeter));

    previewImages.append(image);
}

void ImageRenderTarget::initPainter() {
    if (image != nullptr) {
        if (painter != nullptr) {
            painter->begin(image.get());
        } else {
            painter = std::make_unique<QPainter>(image.get());
        }

        painter->setRenderHint(QPainter::Antialiasing);
        painter->translate(pageSettings.pxMargins, pageSettings.pxMargins);
    }
}

void ImageRenderTarget::newPage() {
    painter->end();

    initDefaultImage();

    initPainter();
}

void ImageRenderTarget::finalize() {
    // previewImages.clear();

    if (painter && painter->isActive()) {
        painter->end();
    }
}

bool ImageRenderTarget::saveImage(const QString& filename, const char* format) {
    if (!image) return false;
    return image->save(filename, format);
}
