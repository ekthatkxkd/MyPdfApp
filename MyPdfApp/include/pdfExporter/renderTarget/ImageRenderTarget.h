#ifndef IMAGERENDERTARGET_H
#define IMAGERENDERTARGET_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include <QImage>

class ImageRenderTarget : public RenderTarget {
public:
    ImageRenderTarget(QImage::Format format = QImage::Format_ARGB32);
    ~ImageRenderTarget();

    QPainter* getPainter() override { return painter.get(); }
    void newPage() override; // 이미지는 단일 페이지
    void finalize() override;

    // QImage getImage() const { return *image; }
    bool saveImage(const QString& filename, const char* format = nullptr);

    QList<std::shared_ptr<QImage>> previewImages;

private:
    void initDefaultImage(QImage::Format format = QImage::Format_ARGB32);
    void initPainter();

    std::shared_ptr<QImage> image;
    std::unique_ptr<QPainter> painter;
};

#endif // IMAGERENDERTARGET_H
