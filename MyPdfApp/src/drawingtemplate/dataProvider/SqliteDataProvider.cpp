#include "include/drawingtemplate/dataProvider/SqliteDataProvider.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QFontMetrics>
#include <QFont>

SqliteDataProvider::SqliteDataProvider(const QString& dbPath, const QString& connName)
    : connectionName(connName) {
    database = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    database.setDatabaseName(dbPath);
}

SqliteDataProvider::~SqliteDataProvider() {
    disconnect();
}

bool SqliteDataProvider::connect() {
    if (!database.open()) {
        qWarning() << "Database connection failed:" << database.lastError().text();
        return false;
    }
    return true;
}

void SqliteDataProvider::registerTableQuery(const QString& tableId, const QString& query) {
    tableQueries[tableId] = query;
}

TableData SqliteDataProvider::getTableData(const QString& tableId) {
    if (!tableQueries.contains(tableId)) {
        qWarning() << "Table query not registered for ID:" << tableId;
        return TableData();
    }

    return executeQuery(tableQueries[tableId]);
}

TableData SqliteDataProvider::executeQuery(const QString& query) {
    if (!isConnected()) {
        qWarning() << "Database not connected";
        return TableData();
    }

    QSqlQuery sqlQuery(database);
    if (!sqlQuery.exec(query)) {
        qWarning() << "Query execution failed:" << sqlQuery.lastError().text();
        return TableData();
    }

    return processQueryResult(sqlQuery);
}

bool SqliteDataProvider::isConnected() const {
    return database.isOpen();
}

void SqliteDataProvider::disconnect() {
    if (database.isOpen()) {
        database.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
}

TableData SqliteDataProvider::processQueryResult(QSqlQuery& query) {
    TableData tableData;

    // 헤더 추출
    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        tableData.headers.append(record.fieldName(i));
    }

    // 데이터 행 추출
    while (query.next()) {
        QVector<QString> row;
        for (int i = 0; i < record.count(); ++i) {
            row.append(query.value(i).toString());
        }
        tableData.rows.append(row);
    }

    return tableData;
}

QVector<qreal> SqliteDataProvider::calculateColumnWidths(const TableData& data, int totalWidth) {
    QVector<qreal> widths;
    if (data.headers.isEmpty()) return widths;

    QFont font("Arial", 9);
    QFontMetrics fm(font);

    // 각 컬럼의 최대 텍스트 너비 계산
    for (int col = 0; col < data.headers.size(); ++col) {
        qreal maxWidth = fm.horizontalAdvance(data.headers[col]); // 헤더 너비

        // 데이터 행들의 너비 확인
        for (const auto& row : data.rows) {
            if (col < row.size()) {
                qreal cellWidth = fm.horizontalAdvance(row[col]);
                maxWidth = qMax(maxWidth, cellWidth);
            }
        }

        widths.append(maxWidth + 20); // 패딩 추가
    }

    // 전체 너비에 맞게 비례 조정
    qreal currentTotal = 0;
    for (qreal width : widths) {
        currentTotal += width;
    }

    if (currentTotal > 0) {
        qreal ratio = totalWidth / currentTotal;
        for (int i = 0; i < widths.size(); ++i) {
            widths[i] *= ratio;
        }
    }

    return widths;
}
