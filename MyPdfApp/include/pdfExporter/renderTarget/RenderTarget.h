#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <QPainter>
#include <memory>

// 렌더링 대상 인터페이스
class IRenderTarget {
public:
    virtual ~IRenderTarget() = default;
    virtual QPainter* getPainter() = 0;
    virtual void newPage() = 0;
    virtual void finalize() = 0;
};

#endif // RENDERTARGET_H
