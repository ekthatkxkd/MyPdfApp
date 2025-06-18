#ifndef PDFRENDERTARGET_H
#define PDFRENDERTARGET_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include <QPdfWriter>

class PdfRenderTarget : public IRenderTarget {
public:
    PdfRenderTarget(const QString& filename, const QSizeF& pageSize);
    ~PdfRenderTarget();

    QPainter* getPainter() override { return painter.get(); }
    void newPage() override;
    void finalize() override;

private:
    std::unique_ptr<QPdfWriter> pdfWriter;
    std::unique_ptr<QPainter> painter;
};

#endif // PDFRENDERTARGET_H
