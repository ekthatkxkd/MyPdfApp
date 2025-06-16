import QtQuick
import QtQuick.Dialogs
import "qrc:/templateForm/"
import "qrc:/myComponents/"

Window {
    id : mainWindow

    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    minimumWidth: 640
    minimumHeight: 480

    readonly property list<string> templateList : ["자재구매 확인서", "불량 발견 보고서", "발주서", "생산 입고 전표"]

    readonly property list<string> pdfFuncText : ["파일저장", "미리보기"]

    // 미리보기 윈도우 인스턴스
    property var previewWindowInstance: null

    function loadTemplateForm(templateIndex) {
        if (templateList[templateIndex] === templateList[0]) {
            templateFormLoader.sourceComponent = materialComp
        } else if (templateList[templateIndex] === templateList[1]) {
            templateFormLoader.sourceComponent = defectReportComp
        } else if (templateList[templateIndex] === templateList[2]) {
            templateFormLoader.sourceComponent = orderFormComp
        } else if (templateList[templateIndex] === templateList[3]) {
            templateFormLoader.sourceComponent = receiptVoucherComp
        }
    }

    function bottomTapClicked(clickedIndex) {
        if (clickedIndex === 0) {
            saveDialog.open()
        } else if (clickedIndex === 1) {
            pdfExporter.generatePreview(templateFormLoader.item.templateItemArea)
        }
    }

    function callExportToPdf(templateItemArea, path) {
        console.log("[LLDDSS] callExportToPdf")
        // pdfExporter.exportToPdf(templateItemArea, "D:/savePdfResult.pdf")
        pdfExporter.exportToPdf(templateItemArea, path)
    }


    // 미리보기 윈도우를 여는 함수
    function openPreviewWindow() {
        // 기존 윈도우가 있으면 닫기
        if (previewWindowInstance) {
            previewWindowInstance.close()
            previewWindowInstance = null
        }

        // 새 미리보기 윈도우 생성
        var component = Qt.createComponent("qrc:/myComponents/PreviewWindow.qml")
        if (component.status === Component.Ready) {
            previewWindowInstance = component.createObject(mainWindow)
            if (previewWindowInstance) {
                // 윈도우가 닫힐 때의 처리를 연결
                previewWindowInstance.closing.connect(function() {
                    console.log("Preview window is closing")
                    previewWindowInstance.destroy()
                    previewWindowInstance = null
                })

                previewWindowInstance.show()
                previewWindowInstance.raise()
                previewWindowInstance.requestActivate()
            }
        } else {
            console.log("미리보기 윈도우 생성 실패:", component.errorString())
        }
    }

    // 자재구매확인서
    Component {
        id : materialComp
        MaterialForm {
        }
    }

    // 불량 발견 보고서
    Component {
        id : defectReportComp
        DefectReportForm {
        }
    }

    // 발주서
    Component {
        id : orderFormComp
        OrderForm {
        }
    }

    // 생산입고전표
    Component {
        id : receiptVoucherComp
        ReceiptVoucherForm {
        }
    }

    FileDialog {
        id: saveDialog
        fileMode: FileDialog.SaveFile
        nameFilters: ["PDF files (*.pdf)"]
        onAccepted: {
            var test = selectedFile.toString()
            var path = selectedFile.toString().replace("file:///", "")
            // exportToPdfWithPath(path)
            callExportToPdf(templateFormLoader.item.templateItemArea, path)
        }
    }

    Connections {
        target : pdfExporter

        function onPreviewUpdated() {
            console.log("메인: 미리보기 업데이트됨 - 새 창 열기")
            openPreviewWindow()
        }
    }

    Rectangle {
        id : windowBg
        anchors.fill : parent

        color : "#dcdcdc"
    }

    Rectangle {
        id : tapArea
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        anchors.left : parent.left
        width : 80

        color : "gray"

        Column {
            Repeater {
                model : templateList
                Item {
                    width : tapArea.width
                    height : tapArea.width

                    Rectangle {
                        anchors.fill : parent
                        anchors.margins : 5
                        color : "white"
                        border.color : "black"

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            width : parent.width

                            horizontalAlignment : Text.AlignHCenter
                            wrapMode : Text.WrapAnywhere

                            font.pixelSize: 15
                            text : modelData
                        }

                        MouseArea {
                            anchors.fill : parent
                            onClicked : {
                                loadTemplateForm(index)
                            }
                        }
                    }
                }
            }
        }


        Item {
            id : bottomTapArea

            anchors.bottom : parent.bottom

            height : parent.width * pdfFuncText.length
            width : parent.width

            Column {
                Repeater {
                    model : pdfFuncText
                    Item {
                        width : tapArea.width
                        height : tapArea.width

                        Rectangle {
                            anchors.fill : parent
                            anchors.margins : 5
                            color : "white"
                            border.color : "black"

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                width : parent.width

                                horizontalAlignment : Text.AlignHCenter
                                wrapMode : Text.WrapAnywhere

                                font.pixelSize: 15
                                text : modelData
                            }

                            MouseArea {
                                anchors.fill : parent
                                onClicked : {
                                    if (templateFormLoader.status === Loader.Ready) {
                                        bottomTapClicked(index)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        id : mainArea

        anchors.top : parent.top
        anchors.bottom : parent.bottom
        anchors.right : parent.right
        anchors.left : tapArea.right

        Loader {
            id : templateFormLoader

            anchors.fill : parent
        }
    }
}
