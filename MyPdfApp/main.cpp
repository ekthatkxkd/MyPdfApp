#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "include/pdfExporter/PdfExporter.h"
#include "include/model/listmodel/TableModel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;


    // 이미지 프로바이더 생성 및 등록
    PreviewImageProvider* previewProvider = new PreviewImageProvider();
    engine.addImageProvider("preview", previewProvider);

    //////// PdfExporter 객체 생성.
    PdfExporter pdfExporter(previewProvider);
    engine.rootContext()->setContextProperty("pdfExporter", &pdfExporter);

    // 명시적 QML 타입 등록
    qmlRegisterType<TableModel>("TableModel", 1, 0, "TableModel");

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
