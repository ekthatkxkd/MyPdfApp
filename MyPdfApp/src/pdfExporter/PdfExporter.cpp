#include "include/pdfExporter/PdfExporter.h"
#include "include/docTemplate/DocTemplateFactory.h"
#include "include/docRender/renderTarget/RenderTargetFactory.h"
#include "include/docRender/DocumentRenderer.h"
#include "include/docRender/renderTarget/ImageRenderTarget.h"
#include <QDebug>

PdfExporter *PdfExporter::s_instance = nullptr;

PreviewImageProvider::PreviewImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage PreviewImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize) {
    QImage retImage(600, 848, QImage::Format_ARGB32);
    retImage.fill(Qt::white);

    PdfExporter *pdfExporter = PdfExporter::getInstance();

    if (pdfExporter) {
        if (id.startsWith("page_")) {
            bool isConverted = false;
            int pageIndex = id.mid(5).toInt(&isConverted);

            if (isConverted) {
                std::shared_ptr<QImage> previewImage = pdfExporter->getPreviewImage(pageIndex);
                if (size) {
                    *size = previewImage->size();
                }

                // mm to pixel with dpi
                retImage = previewImage->scaled(QSize((210*97)/25.4, (297*97)/25.4), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        } else {
            qDebug() << "requestImage, id does not start with page_";
        }
    } else {
        qDebug() << "requestImage, pdfExporter is nullptr...";
    }

    return retImage;
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

    std::unique_ptr<DocTemplate> docTemplate;

    if (formObjectName == "MaterialForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::MATERIAL);
    } else if (formObjectName == "DefectReportForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::DEFECTREPORT);
    } else if (formObjectName == "OrderForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::ORDER);
    } else if (formObjectName == "ReceiptVoucherForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::RECEIPTVOUCHER);
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
    previewImages.clear();

    std::unique_ptr<DocTemplate> docTemplate;

    if (formObjectName == "MaterialForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::MATERIAL);
    } else if (formObjectName == "DefectReportForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::DEFECTREPORT);
    } else if (formObjectName == "OrderForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::ORDER);
    } else if (formObjectName == "ReceiptVoucherForm") {
        docTemplate = DocTemplateFactory::createTemplate(DocTemplateFactory::RECEIPTVOUCHER);
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
