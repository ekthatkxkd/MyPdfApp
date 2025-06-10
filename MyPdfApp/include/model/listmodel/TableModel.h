#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QVector>
#include <QTimer>
#include <QMap>
#include <QPair>
#include <QSet>
#include <qqmlregistration.h>
#include "include/model/listmodel/CellData.h"

class TableModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int rowCount READ getRowCount NOTIFY rowCountChanged)
    Q_PROPERTY(int columnCount READ getColumnCount NOTIFY columnCountChanged)

public:
    explicit TableModel(QObject *parent = nullptr);
    ~TableModel();

    // QAbstractListModel 필수 구현
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // 속성 접근자
    int getRowCount() const { return m_tableData.size(); }
    int getColumnCount() const { return m_columnCount; }

public slots:
    // 테이블 조작 메서드
    Q_INVOKABLE void initializeTable(int rows, int cols);
    Q_INVOKABLE void clearTable();

    // 셀 데이터 조작
    Q_INVOKABLE QVariantMap getCellData(int row, int col) const;
    Q_INVOKABLE bool setCellData(int row, int col, const QVariantMap &cellData);
    Q_INVOKABLE bool updateCellProperty(int row, int col, const QString &property, const QVariant &value);

    // 행/열 조작
    // Q_INVOKABLE void insertRow(int row);
    Q_INVOKABLE void insertRow(const QVariantList &rowData);
    Q_INVOKABLE void removeRow(int row);
    Q_INVOKABLE void insertColumn(int col);
    Q_INVOKABLE void removeColumn(int col);

    // 유틸리티 메서드
    Q_INVOKABLE QVariantMap createDefaultCell(int row, int col) const;
    Q_INVOKABLE QVariantList getRowData(int row) const;
    Q_INVOKABLE bool setRowData(int row, const QVariantList &rowData);

    // TextEdit 전용 최적화된 메서드
    Q_INVOKABLE bool updateCellText(int row, int col, const QString &newText);

    Q_INVOKABLE QVariantList getAllTableData() const;

signals:
    void rowCountChanged();
    void columnCountChanged();
    void cellDataChanged(int row, int col);
    void tableStructureChanged();
    void cellTextChanged(int row, int col, const QString &newText);

private slots:
    void processPendingTextUpdates();

private:
    enum TableRoles {
        RowDataRole = Qt::UserRole + 1,
        RowIndexRole,
        StartRowRole,
        StartColRole,
        RowSpanRole,
        ColSpanRole
    };

    // 이중 배열 데이터 저장
    QVector<QVector<CellData>> m_tableData;
    int m_columnCount;

    // 디바운싱을 위한 멤버들
    QTimer* m_textUpdateTimer;
    QMap<QPair<int, int>, QString> m_pendingTextUpdates;

    // 헬퍼 메서드
    bool isValidPosition(int row, int col) const;
    CellData createCellData(int row, int col,
                            int rowSpan = 1, int colSpan = 1,
                            const QString &cellText = "",
                            bool isTextEdit = false,
                            const QColor &bgColor = Qt::white,
                            bool isBold = false,
                            int fontSize = 12,
                            const QString &alignPosition = "right",
                            bool isVerticalDir = false) const;
};

#endif // TABLEMODEL_H
