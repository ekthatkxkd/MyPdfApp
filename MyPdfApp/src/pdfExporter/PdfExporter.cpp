#include "include/pdfExporter/PdfExporter.h"

#include <QDebug>

PdfExporter::PdfExporter(QObject *parent) : QObject(parent) {

}

PdfExporter::~PdfExporter() {

}

bool PdfExporter::exportToPdf(QQuickItem *rootItem, const QString &filePath) {
    qDebug() << "[LLDDSS] exportToPdf";
    return true;
}
