#include "include/pdfExporter/renderTarget/RenderTarget.h"

RenderTarget::RenderTarget() {
    initPageSettings();
}

void RenderTarget::initPageSettings() {
    // A4 페이지 크기 설정
    QPageSize pageSize(QPageSize::A4);

    pageSettings.mmPageFullSize = pageSize.size(QPageSize::Millimeter);

    // 여백 설정 (QPdfWriter와 동일)
    pageSettings.mmMargins = QMarginsF(mmDefaultPageMargin, mmDefaultPageMargin, mmDefaultPageMargin, mmDefaultPageMargin);
    pageSettings.resolution = defaultDPI;

    // A4 SIZE 전체 크기 mm 을 pixel 단위로 변환.
    pageSettings.pxImageSize = QSize(
        qRound(pageSettings.mmPageFullSize.width() / 25.4 * pageSettings.resolution),
        qRound(pageSettings.mmPageFullSize.height() / 25.4 * pageSettings.resolution)
        );

    // 여백을 픽셀로 변환
    pageSettings.pxMargins = qRound(pageSettings.mmMargins.left() / 25.4 * pageSettings.resolution);

    // 내용 영역 크기 계산 (여백 제외)
    pageSettings.pxContentSize = QSize(
        pageSettings.pxImageSize.width() - (2 * pageSettings.pxMargins),
        pageSettings.pxImageSize.height() - (2 * pageSettings.pxMargins)
        );
}

QSizeF RenderTarget::getPxContentSize() {
    return pageSettings.pxContentSize;
}
