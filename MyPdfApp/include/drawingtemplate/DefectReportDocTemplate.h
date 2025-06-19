#ifndef DEFECTREPORTDOCTEMPLATE_H
#define DEFECTREPORTDOCTEMPLATE_H

#include "include/drawingtemplate/DocumentTemplate.h"

// 구체적인 템플릿 클래스들
class DefectReportDocTemplate : public DocumentTemplate {
public:
    DefectReportDocTemplate();

    virtual void setupTemplate(const QMap<QString, QList<QPair<QString, QStringList>>> &elementDatas) override;

private:
    QVector<QVector<QString>> generateTableData(int tableIndex);
};

#endif // DEFECTREPORTDOCTEMPLATE_H
