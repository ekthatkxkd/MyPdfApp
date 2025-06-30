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
};

/////////////////////////////////////////////////////////////////

class PdfExporter : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool hasPreview READ hasPreview NOTIFY hasPreviewChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

public :
    static PdfExporter *getInstance();

    explicit PdfExporter(QObject *parent = nullptr);
    ~PdfExporter();

    bool hasPreview() const;
    int pageCount() const;
    std::shared_ptr<QImage> getPreviewImage(const int &previewIndex);

    Q_INVOKABLE bool exportToPdf(QString formObjectName, const QString &filePath);
    Q_INVOKABLE bool generatePreview(QString formObjectName);

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
