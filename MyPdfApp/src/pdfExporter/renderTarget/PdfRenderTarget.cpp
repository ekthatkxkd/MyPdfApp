#include "include/pdfExporter/renderTarget/PdfRenderTarget.h"
#include <QDebug>

// PdfRenderTarget 구현
PdfRenderTarget::PdfRenderTarget(const QString& filename) : RenderTarget() {
    initDefaultPdf(filename);

    painter = std::make_unique<QPainter>(pdfWriter.get());
}

PdfRenderTarget::~PdfRenderTarget() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}

void PdfRenderTarget::initDefaultPdf(const QString& filename) {
    pdfWriter = std::make_unique<QPdfWriter>(filename);
    // pdfWriter->setPageSize(QPageSize(pageSize, QPageSize::Point));
    pdfWriter->setPageSize(QPageSize(QPageSize::A4));
    pdfWriter->setPageMargins(pageSettings.mmMargins, QPageLayout::Millimeter);
    pdfWriter->setResolution(pageSettings.resolution);
}

void PdfRenderTarget::newPage() {
    if (pdfWriter) {
        pdfWriter->newPage();
    }
}

void PdfRenderTarget::finalize() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}
