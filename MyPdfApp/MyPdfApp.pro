QT += quick widgets printsupport core qml gui sql

SOURCES += \
        main.cpp \
        src/docElement/ImageElement.cpp \
        src/docElement/TableElement.cpp \
        src/docElement/TextElement.cpp \
        src/drawingtemplate/DefectReportDocTemplate.cpp \
        src/drawingtemplate/DocumentTemplate.cpp \
        src/drawingtemplate/MaterialDocTemplate.cpp \
        src/drawingtemplate/OrderDocTemplate.cpp \
        src/drawingtemplate/RelativePositionManager.cpp \
        src/drawingtemplate/TemplateFactory.cpp \
        src/drawingtemplate/dataProvider/SqliteDataProvider.cpp \
        src/model/listmodel/TableModel.cpp \
        src/pdfExporter/PdfExporter.cpp \
        src/pdfExporter/renderTarget/DocumentRenderer.cpp \
        src/pdfExporter/renderTarget/ImageRenderTarget.cpp \
        src/pdfExporter/renderTarget/PdfRenderTarget.cpp \
        src/pdfExporter/renderTarget/RenderTarget.cpp \
        src/pdfExporter/renderTarget/RenderTargetFactory.cpp

resources.files = main.qml 
resources.prefix = /$${TARGET}
RESOURCES += resources \
    qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    include/docElement/ElementInform.h \
    include/docElement/IRenderElement.h \
    include/docElement/ImageElement.h \
    include/docElement/TableElement.h \
    include/docElement/TextElement.h \
    include/drawingtemplate/DefectReportDocTemplate.h \
    include/drawingtemplate/DocumentTemplate.h \
    include/drawingtemplate/MaterialDocTemplate.h \
    include/drawingtemplate/OrderDocTemplate.h \
    include/drawingtemplate/RelativePositionManager.h \
    include/drawingtemplate/TemplateFactory.h \
    include/drawingtemplate/dataProvider/DataProvider.h \
    include/drawingtemplate/dataProvider/SqliteDataProvider.h \
    include/model/listmodel/CellData.h \
    include/model/listmodel/TableModel.h \
    include/pdfExporter/PdfExporter.h \
    include/pdfExporter/renderTarget/DocumentRenderer.h \
    include/pdfExporter/renderTarget/ImageRenderTarget.h \
    include/pdfExporter/renderTarget/PdfRenderTarget.h \
    include/pdfExporter/renderTarget/RenderTarget.h \
    include/pdfExporter/renderTarget/RenderTargetFactory.h
