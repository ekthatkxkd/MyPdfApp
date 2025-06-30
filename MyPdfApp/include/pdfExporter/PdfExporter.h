#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QQuickItem>
#include <QPdfWriter>
#include <QQuickImageProvider>
#include <QPainter>
#include <QImage>
#include <QList>

class PreviewImageProvider : public QQuickImageProvider {
public:
    PreviewImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
    // void updatePreviewImages(QList<std::shared_ptr<QImage>> &images);
    // int getPageCount() const { return m_previewPages.size(); }

private:
    const int PREVIEW_PIXEL_WIDTH = 600;
    const int PREVIEW_PIXEL_HEIGHT = 848;
    // QList<std::shared_ptr<QImage>> m_previewPages;
};

/////////////////////////////////////////////////////////////////

class PdfExporter : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasPreview READ hasPreview NOTIFY hasPreviewChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

public :
    explicit PdfExporter(QObject *parent = nullptr);
    ~PdfExporter();

    bool hasPreview() const;
    int pageCount() const;
    std::shared_ptr<QImage> getPreviewImage(const int &previewIndex);

    Q_INVOKABLE bool exportToPdf(QString formObjectName, const QString &filePath);
    Q_INVOKABLE bool generatePreview(QString formObjectName);

    static PdfExporter *getInstance();

private :
    static PdfExporter *s_instance;

    std::shared_ptr<QPdfWriter> pdfWriter;
    std::shared_ptr<QImage> imagePtr;

    QList<std::shared_ptr<QImage>> previewImages;
    // 미리보기에 보여줄 QImage 생성 완료 여부.
    bool m_hasPreview = false;
    // 미리보기에 보여줄 page 갯수.
    int m_pageCount = 0;

signals :
    void hasPreviewChanged();
    void pageCountChanged();
    void previewUpdated();
};

#endif // PDFEXPORTER_H
