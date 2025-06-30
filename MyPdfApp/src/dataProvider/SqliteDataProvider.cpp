#include "include/dataProvider/SqliteDataProvider.h"
#include <QSqlQuery>
#include <QSqlError>
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

QSqlQuery SqliteDataProvider::getTableData(const QString& tableId) {
    if (!tableQueries.contains(tableId)) {
        qWarning() << "Table query not registered for ID:" << tableId;
        return QSqlQuery();
    }

    return executeQuery(tableQueries[tableId]);
}

QSqlQuery SqliteDataProvider::executeQuery(const QString& query) {
    if (!isConnected()) {
        qWarning() << "Database not connected";
        return QSqlQuery();
    }

    QSqlQuery sqlQuery(database);
    if (!sqlQuery.exec(query)) {
        qWarning() << "Query execution failed:" << sqlQuery.lastError().text();
        return QSqlQuery();
    }

    return sqlQuery;
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
