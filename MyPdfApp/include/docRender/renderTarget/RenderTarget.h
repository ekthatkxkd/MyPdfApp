#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <QPainter>
#include <memory>
#include <QPageSize>

// 렌더링 대상 인터페이스
class RenderTarget {
public:
    struct PageSettings {
        QSizeF mmPageFullSize;      // 페이지 크기 (밀리미터)
        QMarginsF mmMargins;    // 여백 (밀리미터)
        int resolution;           // DPI
        QSize pxImageSize;          // 전체 이미지 크기 (픽셀)
        QSize pxContentSize;        // 여백 제외한 내용 영역 크기 (픽셀)
        int pxMargins;         // 여백 픽셀 크기
    };

    RenderTarget();

    virtual QPainter* getPainter() = 0;
    virtual void newPage() = 0;
    virtual void finalize() = 0;

    QSizeF getPxContentSize();

protected :
    PageSettings pageSettings;

private :
    void initPageSettings();


    // 25/06/13 - 주의. 현재 DPI 값을 300 외의 다른 값으로 설정할 경우 pdf 에 content 의 위치 및 크기 값이 엉뚱하게 계산됨.
    // 그 이유는 DPI 값이 300 일 경우 pixel 단위로 값을 설정해야 하고
    // 다른 값일 경우 pixel 이 아닌 point 단위로 값을 설정해야 하기 때문.
    // 위 문제 사항을 해결하기 위해선 시간이 오래 걸릴 것으로 판단되어 기능 구현은 보류.
    const int defaultDPI = 300;

    const int mmDefaultPageMargin = 15;
};

#endif // RENDERTARGET_H
