QT += quick widgets printsupport core qml gui

SOURCES += \
        main.cpp \
        src/drawingtemplate/DrawingMaterialTemplate.cpp \
        src/drawingtemplate/DrawingTemplate.cpp \
        src/model/listmodel/TableModel.cpp \
        src/pdfExporter/PdfExporter.cpp

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
    include/drawingtemplate/DrawingMaterialTemplate.h \
    include/drawingtemplate/DrawingTemplate.h \
    include/model/listmodel/CellData.h \
    include/model/listmodel/TableModel.h \
    include/pdfExporter/PdfExporter.h
