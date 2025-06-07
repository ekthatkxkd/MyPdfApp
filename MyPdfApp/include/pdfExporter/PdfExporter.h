#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QObject>
#include <QQuickItem>

class PdfExporter : public QObject {
    Q_OBJECT
public :
    explicit PdfExporter(QObject *parent = nullptr);
    ~PdfExporter();

    Q_INVOKABLE bool exportToPdf(QQuickItem *rootItem, const QString &filePath);

};

#endif // PDFEXPORTER_H
