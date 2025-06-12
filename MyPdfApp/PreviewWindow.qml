import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    width: 800
    height: 900
    visible: true
    title: "테이블 PDF 생성기 - Qt 6.8.3"




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
                            width: Math.min(600, previewColumn.width - 40)
                            height: pageContainer.height + 40
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
                                    fillMode: Image.PreserveAspectFit
                                    asynchronous: true
                                    cache: false

                                    // 로딩 인디케이터
                                    BusyIndicator {
                                        anchors.centerIn: parent
                                        running: pageImage.status === Image.Loading
                                        visible: running
                                    }

                                    // 페이지 테두리 및 그림자 효과
                                    Rectangle {
                                        anchors.fill: parent
                                        anchors.margins: -3
                                        color: "transparent"
                                        border.color: "#adb5bd"
                                        border.width: 2
                                        radius: 6
                                        z: -1

                                        // 그림자 효과
                                        Rectangle {
                                            anchors.fill: parent
                                            anchors.topMargin: 6
                                            anchors.leftMargin: 6
                                            color: "#00000020"
                                            radius: 6
                                            z: -1
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















    /*
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
                width: 600// Math.max(600, previewScrollView.width - 40)


                // 미리보기가 있을 때만 페이지들 표시
                Repeater {
                    model: 3// pdfExporter.hasPreview ? pdfExporter.pageCount : 0

                    Rectangle {
                        width : 100
                        height : 100

                        color : "red"
                        border.color : "black"

                        Component.onCompleted: {
                            console.log("[LLDDSS] ASDFASDF")
                        }
                    }


                    // Rectangle {
                    //     width: Math.min(600, previewColumn.width - 40)
                    //     height: pageContainer.height + 40
                    //     color: "transparent"
                    //     anchors.horizontalCenter: parent.horizontalCenter


                    //     Column {
                    //         id: pageContainer
                    //         anchors.horizontalCenter: parent.horizontalCenter
                    //         spacing: 15


                    //         // 페이지 이미지
                    //         Image {
                    //             id: pageImage
                    //             source: "image://preview/page_" + index
                    //             width: Math.min(600, previewColumn.width - 40)
                    //             height : 800
                    //             // fillMode: Image.PreserveAspectFit
                    //             asynchronous: true
                    //             cache: false

                    //             Component.onCompleted: {
                    //                 console.log("[LLDDSS] PreviewWindow.qml, pageImage, onCompleted source : " + source)
                    //             }

                    //             // 로딩 인디케이터
                    //             BusyIndicator {
                    //                 anchors.centerIn: parent
                    //                 running: pageImage.status === Image.Loading
                    //                 visible: running
                    //             }

                    //             // 페이지 테두리 및 그림자 효과
                    //             Rectangle {
                    //                 anchors.fill: parent
                    //                 anchors.margins: -3
                    //                 color: "transparent"
                    //                 border.color: "#adb5bd"
                    //                 border.width: 2
                    //                 radius: 6
                    //                 z: -1

                    //                 // 그림자 효과
                    //                 Rectangle {
                    //                     anchors.fill: parent
                    //                     anchors.topMargin: 6
                    //                     anchors.leftMargin: 6
                    //                     color: "#00000020"
                    //                     radius: 6
                    //                     z: -1
                    //                 }
                    //             }
                    //         }
                    //     }
                    // }
                }
            }
        }
    }
    */
}
