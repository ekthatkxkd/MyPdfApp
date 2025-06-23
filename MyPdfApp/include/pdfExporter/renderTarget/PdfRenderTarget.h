#ifndef PDFRENDERTARGET_H
#define PDFRENDERTARGET_H

#include "include/pdfExporter/renderTarget/RenderTarget.h"
#include <QPdfWriter>

class PdfRenderTarget : public RenderTarget {
public:
    PdfRenderTarget(const QString& filename);
    ~PdfRenderTarget();

    QPainter* getPainter() override { return painter.get(); }
    void newPage() override;
    void finalize() override;

private:
    void initDefaultPdf(const QString& filename);

    std::unique_ptr<QPdfWriter> pdfWriter;
    std::unique_ptr<QPainter> painter;

    PageSettings pageSettings;
};

#endif // PDFRENDERTARGET_H
