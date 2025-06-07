#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "include/pdfExporter/PdfExporter.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    //////// PdfExporter 객체 생성.
    PdfExporter pdfExporter;
    engine.rootContext()->setContextProperty("pdfExporter", &pdfExporter);

    const QUrl url(QStringLiteral("qrc:/MyPdfApp/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
