#include "include/docElement/ImageElement.h"
#include <QDebug>

ImageElement::ImageElement(const QString &imgPath, const bool &isFillPageWidth,
                           Qt::Alignment align)
    : imgPath(imgPath), isFillPageWidth(isFillPageWidth), alignment(align) {
    initPixMap();
}

void ImageElement::initPixMap() {
    isImageLoaded = image.load(imgPath);

    if (!isImageLoaded) {
        qDebug() << "initPixMap, failed image to load...";
    }

    //////// [LLDDSS] 만약 이미지 데이터를 다운로드 받아 QByteArray type 으로 메모리에 저장한 상태인 경우
    ///      QPixMap method 인 loadFromData 함수를 사용하여 메모리의 데이터를 직접 QPixmap 으로 변환할 수 있다.
    ///      database 로부터 이미지를 다운받아야 하는 상황일 경우 아래 예시 코드를 참조.
    /*
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());

    QByteArray imageData = reply->readAll();

    bool success = image.loadFromData(imageData);

    if (success) {
        qDebug() << "success !";
    } else {
        qDebug() << "failed...";
    }
    */
    ///
    ///
    ////////
}

QRectF ImageElement::render(QPainter& painter, const QPointF& startPos,
                            const QSizeF &pxContentSize,
                            std::function<void()> newPageCallback) {
    QPointF cursorPoint = startPos;
    QPixmap resizedImage = image;

    if (isImageLoaded) {
        if (isFillPageWidth) {
            // 비율 계산
            double ratio = static_cast<double>(pxContentSize.width()) / resizedImage.width();
            int resizedHeight = static_cast<int>(resizedImage.height() * ratio);

            resizedImage = image.scaled(pxContentSize.width(), resizedHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            if (image.width() > pxContentSize.width() || image.height() > pxContentSize.height()) {
                resizedImage = image.scaled(pxContentSize.width(), pxContentSize.height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }
        }

        if ((cursorPoint.y() + resizedImage.height()) > pxContentSize.height()) {
            newPageCallback();

            cursorPoint = QPointF(startPos.x(), 0);
        }

        if (alignment == Qt::AlignHCenter) {
            cursorPoint = QPointF(((pxContentSize.width() - resizedImage.width()) / 2),
                                  cursorPoint.y());
        }

        painter.drawPixmap(cursorPoint, resizedImage);
    }

    return QRectF(cursorPoint, QSizeF(resizedImage.width(), resizedImage.height()));
}

QSizeF ImageElement::calculateSize(const QFontMetrics& fontMetrics,
                                   const QRectF& availableRect) {
    return QSizeF();
}

bool ImageElement::needsNewPage(const QRectF& availableRect, const QSizeF& elementSize) {
    return false;
}

Qt::Alignment ImageElement::getAlign() {
    return alignment;
}
