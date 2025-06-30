#ifndef SQLITEDATAPROVIDER_H
#define SQLITEDATAPROVIDER_H

#include "include/dataProvider/IDataProvider.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QMap>

class SqliteDataProvider : public IDataProvider {
public:
    SqliteDataProvider(const QString& dbPath, const QString& connName = "default");
    ~SqliteDataProvider();

    // 데이터베이스 연결
    bool connect() override;

    // 테이블별 쿼리 등록
    void registerTableQuery(const QString& tableId, const QString& query) override;

    // IDataProvider 인터페이스 구현
    QSqlQuery getTableData(const QString& tableId) override;
    QSqlQuery executeQuery(const QString& query) override;
    bool isConnected() const override;
    void disconnect() override;

private:
    // TableData processQueryResult(QSqlQuery& query);
    // QVector<qreal> calculateColumnWidths(const TableData& data, int totalWidth = 500);

    QSqlDatabase database;
    QString connectionName;
    QMap<QString, QString> tableQueries; // 미리 정의된 쿼리들
};

#endif // SQLITEDATAPROVIDER_H
