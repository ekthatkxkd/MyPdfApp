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
            // Qt::KeepAspectRatio 옵션을 사용할 경우 Qt 가 자동으로 적절한 높이를 계산함.
            resizedImage = image.scaled(pxContentSize.width(), 0, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            if (resizedImage.height() > pxContentSize.height()) {
                // 만약 변경된 height 값이 page 를 벗어났을 경우 page 의 height 값 만큼 다시 resize 진행.
                resizedImage = image.scaled(0, pxContentSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        } else {
            if (image.width() > pxContentSize.width() || image.height() > pxContentSize.height()) {
                resizedImage = image.scaled(pxContentSize.width(), pxContentSize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }

        if ((cursorPoint.y() + resizedImage.height()) > pxContentSize.height()) {
            newPageCallback();

            cursorPoint = QPointF(startPos.x(), 0);
        }

        if (alignment == Qt::AlignHCenter) {
            cursorPoint = QPointF((pxContentSize.width() - resizedImage.width() / 2),
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
