#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QVector>
#include <QString>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <memory>

// 테이블 데이터 구조체
struct TableData {
    QString title;                              // 테이블 제목 (옵션)
    QVector<QString> headers;                    // 컬럼 헤더
    QVector<QVector<QString>> rows;             // 데이터 행들
    QVector<QString> footers;                    // 푸터
    // QVector<qreal> columnWidths;                // 컬럼 너비 (옵션)

    TableData() = default;
    TableData(const QVector<QString>& hdrs, const QVector<QVector<QString>>& data)
        : headers(hdrs), rows(data) {}
};

// 데이터 제공자 인터페이스
class IDataProvider {
public:
    virtual ~IDataProvider() = default;

    // 테이블 데이터 조회
    virtual TableData getTableData(const QString& tableId) = 0;

    // 쿼리로 테이블 데이터 조회
    virtual TableData executeQuery(const QString& query) = 0;

    // 데이터베이스 연결
    virtual bool connect() = 0;

    // 연결 상태 확인
    virtual bool isConnected() const = 0;

    // 연결 해제
    virtual void disconnect() = 0;

    // 테이블별 쿼리 등록
    virtual void registerTableQuery(const QString& tableId, const QString& query) = 0;
};

#endif // DATAPROVIDER_H
