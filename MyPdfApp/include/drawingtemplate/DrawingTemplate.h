#ifndef DRAWINGTEMPLATE_H
#define DRAWINGTEMPLATE_H

#include <QQuickItem>
#include <QTextLayout>
#include <QPdfWriter>
#include "include/model/listmodel/CellData.h"

struct PageSettings {
    QSizeF pageSizeInMM;      // 페이지 크기 (밀리미터)
    QMarginsF marginsInMM;    // 여백 (밀리미터)
    int resolution;           // DPI
    QSize imageSize;          // 전체 이미지 크기 (픽셀)
    QSize contentSize;        // 여백 제외한 내용 영역 크기 (픽셀)
    int marginPixels;         // 여백 픽셀 크기
};

class DrawingTemplate {
public :
    DrawingTemplate();
    bool exportToPdf(QQuickItem *rootItem, const QString &filePath);
    QList<std::shared_ptr<QImage>> generatePreview(QQuickItem *rootItem);

    virtual void drawTemplate(QPainter &painter, QQuickItem *rootItem, const bool &isPdf) = 0;

protected :
    // 기본 font 값 설정.
    void setFont(QPainter &painter, int fontSize = 10, bool isBold = false);

    QImage createPdfSizedImage();

    PageSettings pageSettings;

    QList<std::shared_ptr<QImage>> previewPages;
    std::shared_ptr<QImage> imagePtr;

    std::shared_ptr<QPdfWriter> pdfWriter;

private :
    void initPageSettings();
    void setPdfWriterEnvironment();
};

#endif // DRAWINGTEMPLATE_H
