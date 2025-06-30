#include "include/pdfExporter/PdfExporter.h"
#include "include/drawingtemplate/TemplateFactory.h"
#include "include/pdfExporter/renderTarget/RenderTargetFactory.h"
#include "include/pdfExporter/renderTarget/DocumentRenderer.h"
#include "include/pdfExporter/renderTarget/ImageRenderTarget.h"
#include <QDebug>

PdfExporter *PdfExporter::s_instance = nullptr;

PreviewImageProvider::PreviewImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    PdfExporter *pdfExporter = PdfExporter::getInstance();

    if (!pdfExporter) {
        qDebug() << "requestImage, pdfExporter is nullptr...";
        return QImage();
    }

    if (id.startsWith("page_")) {
        bool isConverted = false;
        int pageIndex = id.mid(5).toInt(&isConverted);

        if (isConverted) {
            std::shared_ptr<QImage> retImage = pdfExporter->getPreviewImage(pageIndex);
            if (size) {
                *size = retImage->size();
            }

            // mm to pixel with dpi
            return retImage->scaled(QSize((210*97)/25.4, (297*97)/25.4), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }

    QImage emptyPage(600,
                     848,
                     QImage::Format_ARGB32);
    emptyPage.fill(Qt::white);

    return emptyPage;
}

//////// START PdfExporter definition.
///
PdfExporter::PdfExporter(QObject *parent) : QObject(parent){
    PdfExporter::s_instance = this;
}

PdfExporter::~PdfExporter() {
    PdfExporter::s_instance = nullptr;
}

bool PdfExporter::hasPreview() const {
    return m_hasPreview;
}
int PdfExporter::pageCount() const {
    return m_pageCount;
}

bool PdfExporter::exportToPdf(QString formObjectName, const QString &filePath) {
    qDebug() << "exportToPdf, start";

    // template 생성. - element 들을 그리는 기능들을 함.

    std::unique_ptr<DocumentTemplate> docTemplate;

    if (formObjectName == "MaterialForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::MATERIAL);
    } else if (formObjectName == "DefectReportForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::DEFECTREPORT);
    } else if (formObjectName == "OrderForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::ORDER);
    } else if (formObjectName == "ReceiptVoucherForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::RECEIPTVOUCHER);
    }

    // PDF or 이미지 렌더 타겟 생성. - pdfwriter, qimage, painter 객체 생성.
    auto pdfTarget = RenderTargetFactory::createPdfTarget(filePath);

    // 렌더러 생성. - 렌더 타겟 내의 painter property 를 template 에 전달과 동시에 element 그리기 요청.
    DocumentRenderer renderer(pdfTarget);
    // 렌더링 실행.
    renderer.renderTemplate(std::move(docTemplate));

    return true;
}

bool PdfExporter::generatePreview(QString formObjectName) {
    qDebug() << "generatePreview, start";

    if (PdfExporter::s_instance != this) {
        PdfExporter::s_instance = this;
    }

    m_hasPreview = false;
    emit hasPreviewChanged();

    m_pageCount = 0;
    // QList<std::shared_ptr<QImage>> previewImages;
    previewImages.clear();

    std::unique_ptr<DocumentTemplate> docTemplate;

    if (formObjectName == "MaterialForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::MATERIAL);
    } else if (formObjectName == "DefectReportForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::DEFECTREPORT);
    } else if (formObjectName == "OrderForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::ORDER);
    } else if (formObjectName == "ReceiptVoucherForm") {
        docTemplate = TemplateFactory::createTemplate(TemplateFactory::RECEIPTVOUCHER);
    }

    auto imageTarget = RenderTargetFactory::createImageTarget();

    DocumentRenderer renderer(imageTarget);

    bool isSucceededRender = renderer.renderTemplate(std::move(docTemplate));

    if (isSucceededRender) {
        std::shared_ptr<ImageRenderTarget> imagePtr = std::dynamic_pointer_cast<ImageRenderTarget>(imageTarget);
        previewImages = imagePtr->previewImages;

        m_pageCount = previewImages.size();
    } else {
        m_pageCount = 0;
    }

    emit pageCountChanged();

    // m_imageProvider->updatePreviewImages(previewImages);

    m_hasPreview = true;
    emit hasPreviewChanged();
    emit previewUpdated();

    return true;
}

std::shared_ptr<QImage> PdfExporter::getPreviewImage(const int &previewIndex) {
    if (previewIndex >= 0 && previewIndex < previewImages.size()) {
        return previewImages[previewIndex];
    } else {
        std::shared_ptr<QImage> defaultPage = std::make_shared<QImage>(600, 848,
                                                                       QImage::Format_ARGB32);
        defaultPage->fill(Qt::white);

        return defaultPage;
    }
}

PdfExporter* PdfExporter::getInstance() {
    return s_instance;
}
