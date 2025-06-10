#ifndef CELLDATA_H
#define CELLDATA_H

#include <QObject>
#include <QVariantMap>
#include <QColor>

struct CellData {
    int startRow;
    int startCol;
    int rowSpan;
    int colSpan;
    QString cellText;
    bool isTextEdit;
    QColor bgColor;
    bool isBold;
    int fontSize;
    QString alignPosition; // "left", "center", "right"
    bool isVerticalDir;

    // 기본 생성자
    CellData() :
        startRow(0), startCol(0), rowSpan(1), colSpan(1),
        cellText(""), isTextEdit(false), bgColor(Qt::white),
        isBold(false), fontSize(12), alignPosition("right"),
        isVerticalDir(false) {}

    // 매개변수 생성자
    CellData(int r, int c, int rs, int cs, const QString& text, bool isEdit,
             const QColor& bg, bool bold, int fSize, const QString& align, bool isVDir) :
        startRow(r), startCol(c), rowSpan(rs), colSpan(cs),
        cellText(text), isTextEdit(isEdit), bgColor(bg),
        isBold(bold), fontSize(fSize), alignPosition(align),
        isVerticalDir(isVDir) {}

    // QML 호환성을 위한 QVariantMap 변환
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["startRow"] = startRow;
        map["startCol"] = startCol;
        map["rowSpan"] = rowSpan;
        map["colSpan"] = colSpan;
        map["cellText"] = cellText;
        map["isTextEdit"] = isTextEdit;
        map["bgColor"] = bgColor;
        map["isBold"] = isBold;
        map["fontSize"] = fontSize;
        map["alignPosition"] = alignPosition;
        map["isVerticalDir"] = isVerticalDir;
        return map;
    }

    // QVariantMap에서 CellData로 변환
    static CellData fromVariantMap(const QVariantMap& map) {
        CellData cell;
        if (map.contains("startRow")) cell.startRow = map["startRow"].toInt();
        if (map.contains("startCol")) cell.startCol = map["startCol"].toInt();
        if (map.contains("rowSpan")) cell.rowSpan = map["rowSpan"].toInt();
        if (map.contains("colSpan")) cell.colSpan = map["colSpan"].toInt();
        if (map.contains("cellText")) cell.cellText = map["cellText"].toString();
        if (map.contains("isTextEdit")) cell.isTextEdit = map["isTextEdit"].toBool();
        if (map.contains("bgColor")) cell.bgColor = map["bgColor"].value<QColor>();
        if (map.contains("isBold")) cell.isBold = map["isBold"].toBool();
        if (map.contains("fontSize")) cell.fontSize = map["fontSize"].toInt();
        if (map.contains("alignPosition")) cell.alignPosition = map["alignPosition"].toString();
        if (map.contains("isVerticalDir")) cell.isVerticalDir = map["isVerticalDir"].toBool();
        return cell;
    }
};

#endif // CELLDATA_H
