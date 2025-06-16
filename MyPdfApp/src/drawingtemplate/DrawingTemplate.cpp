#include "include/drawingtemplate/DrawingTemplate.h"
#include <QPainter>

#include <QDebug>

DrawingTemplate::DrawingTemplate() {
    initPageSettings();

    // if (rootItem == nullptr) {
    //     qDebug() << "DrawingTemplate, rootItem is nullptr...";

    //     return;
    // }

    // if (isPdf && filePath == "") {
    //     qDebug() << "DrawingTemplate, filepath is empty...";

    //     return;
    // }

    // initPageSettings();

    // if (isPdf) {
    //     pdfWriter = std::make_shared<QPdfWriter>(filePath);
    //     setPdfWriterEnvironment();
    // } else {

    // }
}

void DrawingTemplate::setFont(QPainter &painter, int fontSize, bool isBold) {
    QFont font = painter.font();

    font.setPointSize(fontSize);
    font.setBold(isBold);

    painter.setFont(font);
    painter.setPen(QPen(Qt::black, 2));
}

bool DrawingTemplate::exportToPdf(QQuickItem *rootItem, const QString &filePath) {

    pdfWriter = std::make_shared<QPdfWriter>(filePath);
    setPdfWriterEnvironment();

    QPainter painter;

    if (!painter.begin(pdfWriter.get())) {
        qDebug() << "exportToPdf, Failed to initialize QPainter...";
        return false;
    }

    setFont(painter);

    drawTemplate(painter, rootItem, true);

    painter.end();

    return true;
}

QList<std::shared_ptr<QImage>> DrawingTemplate::generatePreview(QQuickItem *rootItem) {
    previewPages.clear();

    imagePtr = std::make_shared<QImage>(createPdfSizedImage());

    QPainter painter;

    if (!painter.begin(imagePtr.get())) {
        qDebug() << "generatePreview, Failed to initialize QPainter...";
        return previewPages;
    }

    previewPages.append(imagePtr);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(pageSettings.marginPixels, pageSettings.marginPixels);

    setFont(painter);

    drawTemplate(painter, rootItem, false);

    painter.end();

    return previewPages;
}

void DrawingTemplate::initPageSettings() {
    // A4 페이지 크기 설정
    QPageSize pageSize(QPageSize::A4);
    pageSettings.pageSizeInMM = pageSize.size(QPageSize::Millimeter);


    // 여백 설정
    pageSettings.marginsInMM = QMarginsF(15, 15, 15, 15);

    // dpi 설정
    pageSettings.resolution = 300;

    // 전체 페이지 크기를 픽셀로 변환 (mm 을 inch 로 변환 후 dpi 곱.)
    double pageWidthInInches = pageSettings.pageSizeInMM.width() * 1.0 / 25.4;
    double pageHeightInInches = pageSettings.pageSizeInMM.height() * 1.0 / 25.4;
    pageSettings.imageSize = QSize(
        qRound(pageWidthInInches * pageSettings.resolution),
        qRound(pageHeightInInches * pageSettings.resolution)
        );

    // 여백을 픽셀로 변환
    double pageMarginInInches = pageSettings.marginsInMM.left() * 1.0 / 25.4;
    pageSettings.marginPixels = qRound(pageMarginInInches * pageSettings.resolution);

    // 내용 영역 크기 계산 (여백 제외)
    pageSettings.contentSize = QSize(
        pageSettings.imageSize.width() - (2 * pageSettings.marginPixels),
        pageSettings.imageSize.height() - (2 * pageSettings.marginPixels)
        );
}

void DrawingTemplate::setPdfWriterEnvironment() {
    pdfWriter->setPageSize(QPageSize(QPageSize::A4));
    pdfWriter->setPageMargins(pageSettings.marginsInMM, QPageLayout::Millimeter);
    pdfWriter->setResolution(pageSettings.resolution);
}

QImage DrawingTemplate::createPdfSizedImage() {
    // PDF와 동일한 크기의 QImage 생성
    QImage pageImage(pageSettings.imageSize, QImage::Format_ARGB32);
    pageImage.fill(Qt::white);

    // DPI 정보 설정 (QPainter 스케일링 정확성을 위해)
    double dotsPerMeter = pageSettings.resolution * 39.3701; // 1인치 = 39.3701 dots/meter
    pageImage.setDotsPerMeterX(qRound(dotsPerMeter));
    pageImage.setDotsPerMeterY(qRound(dotsPerMeter));

    return pageImage;
}
