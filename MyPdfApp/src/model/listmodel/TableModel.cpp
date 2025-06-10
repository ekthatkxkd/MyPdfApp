#include "include/model/listmodel/TableModel.h"
#include <QDebug>


TableModel::TableModel(QObject *parent)
    : QAbstractListModel(parent), m_columnCount(0), m_textUpdateTimer(nullptr) {
    // QTimer 초기화 및 설정
    m_textUpdateTimer = new QTimer(this);
    m_textUpdateTimer->setSingleShot(true);
    m_textUpdateTimer->setInterval(150); // 150ms 디바운싱

    // 타이머 시그널 연결
    connect(m_textUpdateTimer, &QTimer::timeout,
            this, &TableModel::processPendingTextUpdates);

    // 기본 테이블 초기화
    // initializeTable(5, 5);
    // initializeTable(0, 0);
}

TableModel::~TableModel() {
    if (m_textUpdateTimer && m_textUpdateTimer->isActive()) {
        m_textUpdateTimer->stop();
    }
}

// qml 에서 model : table 할 때 반환.
int TableModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_tableData.size();
}

QHash<int, QByteArray> TableModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[RowDataRole] = "rowData";
    roles[RowIndexRole] = "rowIndex";

    // roles[StartRowRole] = "startRow";
    // roles[StartColRole] = "startCol";
    // roles[RowSpanRole] = "rowSpan";
    // roles[ColSpanRole] = "colSpan";

    return roles;
}

// qml 에서 table.rowData 와 같이 값을 얻어올 때 호출되는 함수.
// 상위 roleNames() 함수에서 매칭하는 부분과 연관.
QVariant TableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_tableData.size())
        return QVariant();

    switch (role) {
    case RowDataRole: {
        QVariantList rowCells;
        const QVector<CellData>& rowData = m_tableData[index.row()];
        for (const CellData& cell : rowData) {
            rowCells.append(cell.toVariantMap());
        }
        return rowCells;
    }
    case RowIndexRole:
        return index.row();

    // case StartRowRole :
    //     return;
    // case StartColRole :
    //     return;
    // case RowSpanRole :
    //     return;
    // case ColSpanRole :
    //     return;

    case Qt::DisplayRole:
        return QString("Row %1").arg(index.row());
    default:
        return QVariant();
    }
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void TableModel::initializeTable(int rows, int cols) {
    //////// [LLDDSS] ORIGIN IS TO ALIVE
    // if (rows <= 0 || cols <= 0) return;

    // beginResetModel();

    // m_tableData.clear();
    // m_columnCount = cols;

    // // 헤더 행 생성
    // QVector<CellData> headerRow;
    // headerRow.append(createCellData(0, 0, 1, 1, "상품명", false, Qt::lightGray, true, 14, "center"));
    // headerRow.append(createCellData(0, 1, 1, 2, "가격 정보", false, Qt::lightGray, true, 14, "center"));
    // headerRow.append(createCellData(0, 3, 1, 1, "재고", false, Qt::lightGray, true, 14, "center"));
    // headerRow.append(createCellData(0, 4, 1, 1, "비고", false, Qt::lightGray, true, 14, "center"));
    // m_tableData.append(headerRow);

    // // 데이터 행들 생성
    // for (int i = 0; i < rows; ++i) {
    //     QVector<CellData> dataRow;
    //     dataRow.append(createCellData(i, 0, 1, 1, QString("상품 %1").arg(i), false, Qt::white, false, 12, "left"));
    //     dataRow.append(createCellData(i, 1, 1, 1, QString("%1").arg(1000000 + i * 100000), false, Qt::white, false, 12, "right"));
    //     dataRow.append(createCellData(i, 2, 1, 1, "KRW", false, Qt::white, false, 12, "center"));
    //     dataRow.append(createCellData(i, 3, 1, 1, QString("%1").arg(100 + i * 50), false, Qt::white, false, 12, "right"));
    //     dataRow.append(createCellData(i, 4, 1, 1, "정상", false, Qt::white, false, 12, "center"));
    //     m_tableData.append(dataRow);
    // }

    // endResetModel();

    // emit rowCountChanged();
    // emit columnCountChanged();
    // emit tableStructureChanged();

    // qDebug() << "테이블 초기화 완료. 행:" << m_tableData.size() << "열:" << m_columnCount;
    ////////

    //////// [LLDDSS] MY MODIFIED
    m_columnCount = cols;
    ////////
}

QVariantMap TableModel::getCellData(int row, int col) const {
    if (!isValidPosition(row, col))
        return QVariantMap();

    return m_tableData[row][col].toVariantMap();
}

bool TableModel::setCellData(int row, int col, const QVariantMap &cellData) {
    if (!isValidPosition(row, col))
        return false;

    m_tableData[row][col] = CellData::fromVariantMap(cellData);

    QModelIndex modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex, {RowDataRole});
    emit cellDataChanged(row, col);

    return true;
}

// qml 에서 특정 위치의 cell data 를 변경하고 싶을 때 호출.
// tableModel.updateCellProperty(1, 0, "isBold", true);
bool TableModel::updateCellProperty(int row, int col, const QString &property, const QVariant &value) {
    if (!isValidPosition(row, col))
        return false;

    CellData& cell = m_tableData[row][col];
    bool changed = false;

    if (property == "cellText" && cell.cellText != value.toString()) {
        cell.cellText = value.toString();
        changed = true;
    } else if (property == "isTextEdit" && cell.isTextEdit != value.toBool()) {
        cell.isTextEdit = value.toBool();
        changed = true;
    } else if (property == "bgColor" && cell.bgColor != value.value<QColor>()) {
        cell.bgColor = value.value<QColor>();
        changed = true;
    } else if (property == "isBold" && cell.isBold != value.toBool()) {
        cell.isBold = value.toBool();
        changed = true;
    } else if (property == "fontSize" && cell.fontSize != value.toInt()) {
        cell.fontSize = value.toInt();
        changed = true;
    } else if (property == "alignPosition" && cell.alignPosition != value.toString()) {
        cell.alignPosition = value.toString();
        changed = true;
    } else if (property == "isVerticalDir" && cell.isVerticalDir != value.toBool()) {
        cell.isVerticalDir = value.toBool();
        changed = true;
    } else if (property == "rowSpan" && cell.rowSpan != value.toInt()) {
        cell.rowSpan = value.toInt();
        changed = true;
    } else if (property == "colSpan" && cell.colSpan != value.toInt()) {
        cell.colSpan = value.toInt();
        changed = true;
    }

    if (changed) {
        QModelIndex modelIndex = index(row);
        emit dataChanged(modelIndex, modelIndex, {RowDataRole});
        emit cellDataChanged(row, col);
        return true;
    }

    return false;
}

bool TableModel::updateCellText(int row, int col, const QString &newText) {
    if (!isValidPosition(row, col)) {
        qWarning() << "잘못된 셀 위치:" << row << col;
        return false;
    }

    CellData& cell = m_tableData[row][col];

    if (cell.cellText == newText) {
        return true;
    }

    QPair<int, int> position(row, col);
    m_pendingTextUpdates[position] = newText;

    if (m_textUpdateTimer) {
        m_textUpdateTimer->start();
    }

    return true;
}

// m_textUpdateTimer 가 timeout 되었을 때 호출되는 함수.
void TableModel::processPendingTextUpdates() {
    if (m_pendingTextUpdates.isEmpty()) {
        return;
    }

    QSet<int> changedRows;

    for (auto it = m_pendingTextUpdates.begin(); it != m_pendingTextUpdates.end(); ++it) {
        int row = it.key().first;
        int col = it.key().second;
        const QString& newText = it.value();

        if (isValidPosition(row, col)) {
            QString oldText = m_tableData[row][col].cellText;

            if (oldText != newText) {
                m_tableData[row][col].cellText = newText;
                changedRows.insert(row);

                emit cellTextChanged(row, col, newText);
                qDebug() << "텍스트 업데이트:" << row << col << "이전:" << oldText << "새값:" << newText;
            }
        }
    }

    for (int row : changedRows) {
        QModelIndex modelIndex = index(row);
        emit dataChanged(modelIndex, modelIndex, {RowDataRole});
    }

    m_pendingTextUpdates.clear();
}

// qml 에서 새로운 행 추가 요청을 할 때
void TableModel::insertRow(const QVariantList &rowData) {
    // if (row < 0 || row > m_tableData.size()) return;

    int row = m_tableData.size();
    int rowSize = rowData.size();

    beginInsertRows(QModelIndex(), row, row);

    QVector<CellData> newRow;



    //////// [LLDDSS] ORIGIN IS TO ALIVE
    // for (int j = 0; j < m_columnCount; ++j) {
    //     newRow.append(createCellData(row, j, 1, 1, QString("새 셀 (%1,%2)").arg(row).arg(j), false));
    // }
    // m_tableData.insert(row, newRow);

    // for (int i = row + 1; i < m_tableData.size(); ++i) {
    //     for (int j = 0; j < m_columnCount; ++j) {
    //         m_tableData[i][j].startRow = i;
    //     }
    // }
    ////////

    //////// [LLDDSS] MY MODIFIED
    for (int j = 0; j < rowSize; j++) {
        newRow.append(CellData::fromVariantMap(rowData[j].toMap()));
    }
    m_tableData.insert(row, newRow);
    ////////

    endInsertRows();
    emit rowCountChanged();
    emit tableStructureChanged();
}

// qml 에서 행 삭제 요청을 할 때
void TableModel::removeRow(int row) {
    if (row < 0 || row >= m_tableData.size() || m_tableData.size() <= 1) return;

    beginRemoveRows(QModelIndex(), row, row);
    m_tableData.removeAt(row);

    for (int i = row; i < m_tableData.size(); ++i) {
        for (int j = 0; j < m_columnCount; ++j) {
            m_tableData[i][j].startRow = i;
        }
    }

    endRemoveRows();
    emit rowCountChanged();
    emit tableStructureChanged();
}

// qml 에서 새로운 열을 추가할 때
void TableModel::insertColumn(int col) {
    if (col < 0 || col > m_columnCount) return;

    beginResetModel();

    for (int i = 0; i < m_tableData.size(); ++i) {
        m_tableData[i].insert(col, createCellData(i, col));

        for (int j = col + 1; j < m_tableData[i].size(); ++j) {
            m_tableData[i][j].startCol = j;
        }
    }

    m_columnCount++;
    endResetModel();

    emit columnCountChanged();
    emit tableStructureChanged();
}

void TableModel::removeColumn(int col) {
    if (col < 0 || col >= m_columnCount || m_columnCount <= 1) return;

    beginResetModel();

    for (int i = 0; i < m_tableData.size(); ++i) {
        if (col < m_tableData[i].size()) {
            m_tableData[i].removeAt(col);
        }

        for (int j = col; j < m_tableData[i].size(); ++j) {
            m_tableData[i][j].startCol = j;
        }
    }

    m_columnCount--;
    endResetModel();

    emit columnCountChanged();
    emit tableStructureChanged();
}

QVariantMap TableModel::createDefaultCell(int row, int col) const{
    return createCellData(row, col).toVariantMap();
}

// qml 에서 특정 위치의 row data 를 얻어올 때
QVariantList TableModel::getRowData(int row) const {
    QVariantList rowData;
    if (row >= 0 && row < m_tableData.size()) {
        for (const CellData& cell : m_tableData[row]) {
            rowData.append(cell.toVariantMap());
        }
    }
    return rowData;
}

bool TableModel::setRowData(int row, const QVariantList &rowData) {
    if (row < 0 || row >= m_tableData.size() || rowData.size() != m_columnCount)
        return false;

    for (int j = 0; j < m_columnCount && j < rowData.size(); ++j) {
        if (rowData[j].canConvert<QVariantMap>()) {
            m_tableData[row][j] = CellData::fromVariantMap(rowData[j].toMap());
        }
    }

    QModelIndex modelIndex = index(row);
    emit dataChanged(modelIndex, modelIndex, {RowDataRole});
    return true;
}

void TableModel::clearTable() {
    beginResetModel();

    for (int i = 0; i < m_tableData.size(); ++i) {
        for (int j = 0; j < m_tableData[i].size(); ++j) {
            m_tableData[i][j] = createCellData(i, j);
        }
    }

    endResetModel();
    emit tableStructureChanged();
}

bool TableModel::isValidPosition(int row, int col) const {
    return row >= 0 && row < m_tableData.size() &&
           col >= 0 && col < m_columnCount &&
           col < m_tableData[row].size();
}

CellData TableModel::createCellData(int row, int col, int rowSpan, int colSpan,
                                    const QString &cellText, bool isTextEdit,
                                    const QColor &bgColor, bool isBold,
                                    int fontSize, const QString &alignPosition,
                                    bool isVerticalDir) const {
    return CellData(row, col, rowSpan, colSpan, cellText, isTextEdit,
                    bgColor, isBold, fontSize, alignPosition, isVerticalDir);
}
