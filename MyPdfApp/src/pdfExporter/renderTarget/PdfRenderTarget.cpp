#include "include/pdfExporter/renderTarget/PdfRenderTarget.h"
#include <QDebug>

// PdfRenderTarget 구현
PdfRenderTarget::PdfRenderTarget(const QString& filename, const QSizeF& pageSize) {
    pdfWriter = std::make_unique<QPdfWriter>(filename);
    pdfWriter->setPageSize(QPageSize(pageSize, QPageSize::Point));
    pdfWriter->setPageMargins(QMarginsF(0, 0, 0, 0));

    painter = std::make_unique<QPainter>();
}

PdfRenderTarget::~PdfRenderTarget() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}

void PdfRenderTarget::newPage() {
    if (painter && painter->isActive()) {
        //////// [LLDDSS] ORIGIN IS TO ALIVE
        // painter->newPage();
    }
}

void PdfRenderTarget::finalize() {
    if (painter && painter->isActive()) {
        painter->end();
    }
}

