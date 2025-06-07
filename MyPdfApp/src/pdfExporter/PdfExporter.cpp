#include "include/pdfExporter/PdfExporter.h"
#include <QPainter>
#include <QFont>
#include <QVariant>

#include <QDebug>

PdfExporter::PdfExporter(QObject *parent) : QObject(parent) {

}

PdfExporter::~PdfExporter() {

}

bool PdfExporter::exportToPdf(QQuickItem *rootItem, const QString &filePath) {
    qDebug() << "[LLDDSS] exportToPdf";

    int myDPI = 97;
    int myMargin = 15;

    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMarginsF(myMargin, myMargin, myMargin, myMargin), QPageLayout::Millimeter);
    pdfWriter.setResolution(myDPI);

    QPainter painter;

    if (!painter.begin(&pdfWriter)) {
        qWarning() << "QPdfWriter 초기화 실패";
        return false;
    }

    QFont defaultFont = painter.font();
    defaultFont.setPointSize(9);
    defaultFont.setBold(false);

    painter.setFont(defaultFont);
    painter.setPen(QPen(Qt::green, 2));

    ////////
    /// 내가 계산해본 결과 pageRect 는 포인트 타입으로 값을 가지고 있는 것 같음.
    /// pageRect 는 margin 계산하지 않은 a4 전체 width, height 값을 가지고 있음.
    /// pageRect.width() 는 595 값을 반환함.
    /// 이 595 포인트를 픽셀 단위로 바꾸면 다음과 같다.
    /// (포인트/72 * DPI = 픽셀)  ===> 595/72 * 97 = 약 801 픽셀 width 값.
    /// 마찬가지로 pageRect.height() 는 842 포인트이고 픽셀 단위로 변경하면 약 1134 픽셀 height 값.
    ///
    /// 그리고 margins 객체 값은 약 42 포인트 이다.
    ///
    /// 따라서 contentWidth 는 595 - 42 - 42 = 511 포인트 이고 픽셀 단위로 변경하면 약 688 픽셀.
    /// contentHeight 는 842 - 42 - 42 = 758 포인트 이고 픽셀 단위로 변경하면 약 1021 픽셀.
    /// 위 값으로 drawRect(QRectF(QPointF(0, 0) , QSizeF(511, 758))) 호출하여 사각형을 그리면
    /// content 영역만큼 사각형이 그려진다.

    // QRectF pageRect = pdfWriter.pageLayout().fullRect(QPageLayout::Point);
    // QMarginsF margins = pdfWriter.pageLayout().margins(QPageLayout::Point);

    // qreal fullWidth = pageRect.width();
    // qreal fullHeight = pageRect.height();

    // qreal marginTop = margins.top();
    // qreal marginBottom = margins.bottom();
    // qreal marginLeft = margins.left();
    // qreal marginRight = margins.right();

    // qreal contentWidth = fullWidth - marginLeft - marginRight;
    // qreal contentHeight = fullHeight - marginTop - marginBottom;


    // qDebug() << "[LLDDSS] fullWidth : " << fullWidth;
    // qDebug() << "[LLDDSS] fullHeight : " << fullHeight;
    // qDebug() << "[LLDDSS] marginTop : " << marginTop;
    // qDebug() << "[LLDDSS] marginLeft : " << marginLeft;

    // painter.drawRect(QRectF(QPointF(marginTop, marginLeft), QSize(contentWidth, contentHeight)));
    // painter.drawRect(pageRect);

    ///
    ///
    ////////

    ////////
    ///
    ///
    qreal a4widthmm = 210.0;
    qreal a4heightmm = 297.0;

    qreal widthPixels = ((a4widthmm-(myMargin*2)) / 25.4) * myDPI;  // 300 : dpi
    qreal heightPixels = ((a4heightmm-(myMargin*2)) / 25.4) * myDPI;

    QRectF calculatedRect(0, 0, widthPixels, heightPixels);

    painter.drawRect(calculatedRect);

    qDebug() << "[LLDDSS] fullArea x : " << calculatedRect.x();
    qDebug() << "[LLDDSS] fullArea y : " << calculatedRect.y();
    qDebug() << "[LLDDSS] fullArea width : " << calculatedRect.width();
    qDebug() << "[LLDDSS] fullArea height : " << calculatedRect.height();

    ///
    ///
    ////////

    ////////
    ///
    ///
    // QRectF fullArea = painter.window();

    // painter.setPen(QPen(Qt::red, 2));
    // painter.setBrush(Qt::NoBrush);
    // painter.drawRect(fullArea);

    // qDebug() << "[LLDDSS] fullArea x : " << fullArea.x();
    // qDebug() << "[LLDDSS] fullArea y : " << fullArea.y();
    // qDebug() << "[LLDDSS] fullArea width : " << fullArea.width();
    // qDebug() << "[LLDDSS] fullArea height : " << fullArea.height();

    ///
    ///
    ////////



    painter.end();

    return true;
}
