#ifndef IMAGERENDERTARGET_H
#define IMAGERENDERTARGET_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include <QImage>

class ImageRenderTarget : public IRenderTarget {
public:
    ImageRenderTarget(const QSizeF& pageSize, QImage::Format format = QImage::Format_ARGB32);
    ~ImageRenderTarget();

    QPainter* getPainter() override { return painter.get(); }
    void newPage() override {} // 이미지는 단일 페이지
    void finalize() override;

    QImage getImage() const { return *image; }
    bool saveImage(const QString& filename, const char* format = nullptr);

private:
    std::unique_ptr<QImage> image;
    std::unique_ptr<QPainter> painter;
    QSizeF pageSize;
};

#endif // IMAGERENDERTARGET_H
