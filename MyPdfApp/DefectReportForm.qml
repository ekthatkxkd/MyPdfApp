import QtQuick
import QtQuick.Controls
import "qrc:/templateItems"

Item {
    id : rootArea
    objectName : "DefectReportForm"

    property alias templateItemArea : templateItemArea

    Rectangle {
        id : formTapArea
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        anchors.left : parent.left
        width : 80

        color : "#696969"

        Column {
            Item {
                width : formTapArea.width
                height : formTapArea.width

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
                        text : "품목 추가"
                    }

                    MouseArea {
                        anchors.fill : parent
                        onClicked : {
                            productionTable.addInnerDefaultRowData()
                        }
                    }
                }
            }
        }
    }


    Item {
        id : formArea

        anchors.top : parent.top
        anchors.bottom : parent.bottom
        anchors.left : formTapArea.right
        anchors.margins : 5

        width : 800

        Rectangle {
            anchors.fill : parent
            color : "white"
            border.color : "black"
        }
        ScrollView {
            id : scrollView

            anchors.fill : parent
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff  // ScrollBar.AsNeeded
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            // Binding을 통한 실시간 contentHeight 계산
            Binding {
                target: scrollView
                property: "contentHeight"
                value: {
                    var maxBottom = 0;
                    for (var i = 0; i < templateItemArea.children.length; i++) {
                        var child = templateItemArea.children[i];
                        if (child.visible) {
                            var bottom = child.y + child.height + (child.bottomMargin || 0);
                            maxBottom = Math.max(maxBottom, bottom);
                        }
                    }
                    return maxBottom + 20; // 여분의 하단 패딩
                }
            }

            Item {
                id : templateItemArea
                objectName : "defectReportItemArea"

                height : {
                    let sumHeight = 0;
                    for (let i = 0; i < children.length; i++) {
                        var child = children[i]
                        if (child.visible) {
                            sumHeight += child.height;
                        }
                    }
                    return Math.max(sumHeight, formArea.height)
                }

                width : scrollView.width

                Text {
                    id : templateTitle
                    objectName : "templateTitle"

                    anchors.horizontalCenter : parent.horizontalCenter

                    font.pixelSize : 30
                    font.bold : true

                    text : "불량 발견 보고서"
                }


                TemplateTable {
                    id : defectReportNumTable
                    objectName : "defectReportNumTable"

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left

                    width : (templateItemArea.width/2) - 3

                    dividedColCount : 2
                    dividedRowCount : 2

                    Component.onCompleted: {
                        initDividedWidths([0.2, 0.8])
                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "전표번호", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 1, 1, 1, "2025/03/25 - 7", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(1, 0, 1, 1, "처리방법", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "품목대체", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }

                TemplateTable {
                    id : placeInformTable
                    objectName : "placeInformTable"

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.right : parent.right

                    width : (templateItemArea.width/2) - 3

                    dividedColCount : 2
                    dividedRowCount : 2

                    Component.onCompleted: {
                        initDividedWidths([0.2, 0.8])
                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "담당자", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 1, 1, 1, "()", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(1, 0, 1, 1, "발견창고", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "Rework창고(00186)", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }

                TemplateTable {
                    id : productionTable
                    objectName : "productionTable"

                    anchors.top : (defectReportNumTable.height > placeInformTable.height) ? defectReportNumTable.bottom : placeInformTable.bottom
                    anchors.topMargin : 5

                    width : parent.width

                    dividedRowCount : 4
                    dividedColCount : 6

                    innerDefaultData : [
                        initCell(0, 0, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 1, 1, 1, "", true, "white", false, 12, "left"),
                        initCell(0, 2, 1, 1, "", true, "white", false, 12, "left"),
                        initCell(0, 3, 1, 1, "", true, "white", false, 12, "right"),
                        initCell(0, 4, 1, 1, "", true, "white", false, 12, "right"),
                        initCell(0, 5, 1, 1, "", true, "white", false, 12, "right")
                    ]

                    Component.onCompleted: {
                        initDividedWidths([0.05, 0.25 ,0.36, 0.12, 0.12, 0.1])  // initDividedWidths([0.05, 0.15, 0.5, 0.1, 0.2])

                        headerTableModel.setTableSize(1, dividedColCount)
                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        /////// header data
                        ///
                        ///
                        initHeaderData([
                                           initCell(0, 0, 1, 1, "순번", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 1, 1, 1, "품목코드", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 2, 1, 1, "품목명[규격]", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 3, 1, 1, "감소수량", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 4, 1, 1, "증가수량", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 5, 1, 1, "적요", false, headerBgColor, true, 12, "center")
                                       ])
                        ///
                        ///
                        ////////

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "1", true, "white", false, 12, "center"),
                                            initCell(0, 1, 1, 1, "P001-0162", true, "white", false, 12, "left"),
                                            initCell(0, 2, 1, 1, "[EFFECT_V2_WHITE_GRAY_SKY] PRODUCT", true, "white", false, 12, "left"),
                                            initCell(0, 3, 1, 1, "102.00", true, "white", false, 12, "right"),
                                            initCell(0, 4, 1, 1, "", true, "white", false, 12, "right"),
                                            initCell(0, 5, 1, 1, "", true, "white", false, 12, "right")
                                        ])
                        addInnerRowData([
                                            initCell(1, 0, 1, 1, "2", true, "white", false, 12, "center"),
                                            initCell(1, 1, 1, 1, "E002-0002", true, "white", false, 12, "left"),
                                            initCell(1, 2, 1, 1, "TPS61088 [TI, SMD,VQFN-20, 4.5Vto12.6V, 11.4A, Reel250]", true, "white", false, 12, "left"),
                                            initCell(1, 3, 1, 1, "", true, "white", false, 12, "right"),
                                            initCell(1, 4, 1, 1, "102.00", true, "white", false, 12, "right"),
                                            initCell(1, 5, 1, 1, "", true, "white", false, 12, "right")
                                        ])
                        addInnerRowData([
                                            initCell(2, 0, 1, 1, "3", true, "white", false, 12, "center"),
                                            initCell(2, 1, 1, 1, "E002-0003", true, "white", false, 12, "left"),
                                            initCell(2, 2, 1, 1, "STM32F030CC_LQFP48 [ST, SMD, LQFP-48, 2.4Vto3.6V, 메모리 크기: 256kB, RAM크기:32kB, Reel 2K4]", true, "white", false, 12, "left"),
                                            initCell(2, 3, 1, 1, "102.00", true, "white", false, 12, "right"),
                                            initCell(2, 4, 1, 1, "", true, "white", false, 12, "right"),
                                            initCell(2, 5, 1, 1, "", true, "white", false, 12, "right")
                                        ])
                        addInnerRowData([
                                            initCell(3, 0, 1, 1, "4", true, "white", false, 12, "center"),
                                            initCell(3, 1, 1, 1, "E002-0004", true, "white", false, 12, "left"),
                                            initCell(3, 2, 1, 1, "TPS55340RTE [TI, SMD, WQFN-16, 2.9Vto32V, 2.9Vto38V, 6.6A, Reel3K]", true, "white", false, 12, "left"),
                                            initCell(3, 3, 1, 1, "102.00", true, "white", false, 12, "right"),
                                            initCell(3, 4, 1, 1, "", true, "white", false, 12, "right"),
                                            initCell(3, 5, 1, 1, "", true, "white", false, 12, "right")
                                        ])
                        addInnerRowData([
                                            initCell(4, 0, 1, 1, "5", true, "white", false, 12, "center"),
                                            initCell(4, 1, 1, 1, "E002-0006", true, "white", false, 12, "left"),
                                            initCell(4, 2, 1, 1, "SN74AHC4066RGYR((VQFN) [TI, SMD, VQFN-14, 4 Switch, 2Vto5.5V, Reel 3K])", true, "white", false, 12, "left"),
                                            initCell(4, 3, 1, 1, "102.00", true, "white", false, 12, "right"),
                                            initCell(4, 4, 1, 1, "", true, "white", false, 12, "right"),
                                            initCell(4, 5, 1, 1, "", true, "white", false, 12, "right")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }

                TemplateTable {
                    id : quantityTotalTable
                    objectName : "quantityTotalTable"

                    anchors.top : productionTable.bottom
                    anchors.topMargin : 5

                    width : parent.width

                    dividedRowCount : 1
                    dividedColCount : 6


                    Component.onCompleted: {
                        initDividedWidths([0.1, 0.25, 0.1, 0.25, 0.1, 0.2])

                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "감소수량", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 1, 1, 1, "102.00", true, "white", false, 12, "right"),
                                            initCell(0, 2, 1, 1, "증가수량", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 3, 1, 1, "47,532.00", true, "white", false, 12, "right"),
                                            initCell(0, 4, 1, 1, "인수", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 5, 1, 1, "인", false, "white", false, 12, "right")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }
            }
        }
    }
}
