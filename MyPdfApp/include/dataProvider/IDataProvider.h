#ifndef IDATAPROVIDER_H
#define IDATAPROVIDER_H

#include <QVector>
#include <QString>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
// #include <memory>

// 데이터 제공자 인터페이스
class IDataProvider {
public:
    virtual ~IDataProvider() = default;

    // 테이블 데이터 조회
    virtual QSqlQuery getTableData(const QString& tableId) = 0;

    // 쿼리로 테이블 데이터 조회
    virtual QSqlQuery executeQuery(const QString& query) = 0;

    // 데이터베이스 연결
    virtual bool connect() = 0;

    // 연결 상태 확인
    virtual bool isConnected() const = 0;

    // 연결 해제
    virtual void disconnect() = 0;

    // 테이블별 쿼리 등록
    virtual void registerTableQuery(const QString& tableId, const QString& query) = 0;
};

#endif // IDATAPROVIDER_H
