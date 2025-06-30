QT += quick widgets printsupport core qml gui sql

SOURCES += \
        main.cpp \
        src/dataProvider/SqliteDataProvider.cpp \
        src/docElement/ImageElement.cpp \
        src/docElement/TableElement.cpp \
        src/docElement/TextElement.cpp \
        src/docRender/renderTarget/ImageRenderTarget.cpp \
        src/docRender/renderTarget/PdfRenderTarget.cpp \
        src/docRender/renderTarget/RenderTarget.cpp \
        src/docRender/renderTarget/RenderTargetFactory.cpp \
        src/docTemplate/DefectReportDocTemplate.cpp \
        src/docTemplate/DocTemplate.cpp \
        src/docTemplate/DocTemplateFactory.cpp \
        src/docTemplate/MaterialDocTemplate.cpp \
        src/docTemplate/OrderDocTemplate.cpp \
        src/docTemplate/ReceiptVoucherDocTemplate.cpp \
        src/model/listmodel/TableModel.cpp \
        src/pdfExporter/PdfExporter.cpp \
        src/docRender/DocumentRenderer.cpp

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
    include/dataProvider/IDataProvider.h \
    include/dataProvider/SqliteDataProvider.h \
    include/docElement/ElementInform.h \
    include/docElement/IRenderElement.h \
    include/docElement/ImageElement.h \
    include/docElement/TableElement.h \
    include/docElement/TextElement.h \
    include/docRender/renderTarget/ImageRenderTarget.h \
    include/docRender/renderTarget/PdfRenderTarget.h \
    include/docRender/renderTarget/RenderTarget.h \
    include/docRender/renderTarget/RenderTargetFactory.h \
    include/docTemplate/DefectReportDocTemplate.h \
    include/docTemplate/DocTemplate.h \
    include/docTemplate/DocTemplateFactory.h \
    include/docTemplate/MaterialDocTemplate.h \
    include/docTemplate/OrderDocTemplate.h \
    include/docTemplate/ReceiptVoucherDocTemplate.h \
    include/model/listmodel/CellData.h \
    include/model/listmodel/TableModel.h \
    include/pdfExporter/PdfExporter.h \
    include/docRender/DocumentRenderer.h
