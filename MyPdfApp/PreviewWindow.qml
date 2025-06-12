import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    width: 800
    height: 900
    visible: true
    title: "PDF 미리 보기"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // 미리보기 콘텐츠 영역
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#dee2e6"
            border.width: 1
            radius: 8

            ScrollView {
                id: previewScrollView
                anchors.fill: parent
                anchors.margins: 10
                clip: true

                contentHeight: previewColumn.height
                contentWidth: previewColumn.width

                Column {
                    id: previewColumn
                    spacing: 30
                    width: Math.max(600, previewScrollView.width - 40)

                    // 미리보기가 있을 때만 페이지들 표시
                    Repeater {
                        model: pdfExporter.hasPreview ? pdfExporter.pageCount : 0

                        Rectangle {
                            width: pageImage.width + 50 // Math.min(600, previewColumn.width - 40)
                            height: pageImage.height + 50// pageContainer.height + 40
                            color: "transparent"
                            anchors.horizontalCenter: parent.horizontalCenter

                            Column {
                                id: pageContainer
                                anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 15

                                // 페이지 번호 레이블
                                Rectangle {
                                    width: pageLabel.width + 20
                                    height: pageLabel.height + 10
                                    color: "#007bff"
                                    radius: 15
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    Text {
                                        id: pageLabel
                                        text: "페이지 " + (index + 1)
                                        font.pixelSize: 14
                                        font.bold: true
                                        color: "white"
                                        anchors.centerIn: parent
                                    }
                                }

                                // 페이지 이미지
                                Image {
                                    id: pageImage
                                    source: "image://preview/page_" + index
                                    width: Math.min(600, previewColumn.width - 40)
                                    // width : 600
                                    // height : 848
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                    cache: false

                                    Rectangle {
                                        anchors.fill: parent
                                        color: "transparent"
                                        border.color: "black"
                                        border.width: 1

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
