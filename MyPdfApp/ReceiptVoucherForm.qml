import QtQuick
import QtQuick.Controls
import "qrc:/templateItems"

Item {
    id : rootArea

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
                        text : "생산 추가"
                    }

                    MouseArea {
                        anchors.fill : parent
                        onClicked : {
                            productionTable.addInnerDefaultRowData()
                        }
                    }
                }
            }

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
                        text : "소모 추가"
                    }

                    MouseArea {
                        anchors.fill : parent
                        onClicked : {
                            consumptionTable.addInnerDefaultRowData()
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
                objectName : "receiptVoucherItemArea"

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

                    text : "생산 입고 전표"
                }

                TemplateTable {
                    id : voucherNumTable
                    objectName : "voucherNumTable"

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left

                    width : (templateItemArea.width/2) - 3

                    dividedColCount : 2
                    dividedRowCount : 2

                    Component.onCompleted: {
                        initDividedWidths([0.2, 0.8])
                        tableModel.setTableSize(dividedColCount, dividedRowCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "전표번호", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 0, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(0, 1, 1, 1, "DATE", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }

                TemplateTable {
                    id : entranceTable
                    objectName : "entranceTable"

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.right : parent.right

                    width : (templateItemArea.width/2) - 3

                    dividedColCount : 2
                    dividedRowCount : 2

                    Component.onCompleted: {
                        initDividedWidths([0.2, 0.8])
                        tableModel.setTableSize(dividedColCount, dividedRowCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "출고처", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 0, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(0, 1, 1, 1, "입고처", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }

                Item {
                    id : listArea
                    objectName : "listArea"

                    property var dividedWidths : [0.05, 0.15, 0.5, 0.1, 0.2]
                    property int dividedRowCount : 5

                    anchors.top : (voucherNumTable.height > entranceTable.height) ? voucherNumTable.bottom : entranceTable.bottom
                    anchors.topMargin : 5

                    width : parent.width

                    height : {
                        let sumHeight = 0;
                        for (let i = 0; i < children.length; i++) {
                            var child = children[i]
                            if (child.visible) {
                                sumHeight += child.height;
                            }
                        }
                        return sumHeight
                    }

                    TemplateTable {
                        id : productionTable
                        objectName : "productionTable"

                        anchors.top : parent.top

                        width : parent.width

                        dividedColCount : 5
                        dividedRowCount : listArea.dividedRowCount

                        innerDefaultData : [
                            initCell(0, 0, 1, 1, "생산", false, "white", false, 12, "left"),
                            initCell(1, 0, 1, 1, "", true, "white", false, 12, "left"),
                            initCell(2, 0, 1, 1, "", true, "white", false, 12, "left"),
                            initCell(3, 0, 1, 1, "", true, "white", false, 12, "right"),
                            initCell(4, 0, 1, 1, "", true, "white", false, 12, "left")
                        ]

                        Component.onCompleted: {
                            initDividedWidths(listArea.dividedWidths)  // initDividedWidths([0.05, 0.15, 0.5, 0.1, 0.2])

                            headerTableModel.setTableSize(1, dividedRowCount)
                            tableModel.setTableSize(dividedColCount, dividedRowCount)
                            footerTableModel.setTableSize(1, dividedRowCount)

                            /////// header data
                            ///
                            ///
                            initHeaderData([
                                               initCell(0, 0, 1, 1, "구분", false, headerBgColor, true, 12, "center"),
                                               initCell(1, 0, 1, 1, "품목코드", false, headerBgColor, true, 12, "center"),
                                               initCell(2, 0, 1, 1, "품목명[규격]", false, headerBgColor, true, 12, "center"),
                                               initCell(3, 0, 1, 1, "수량", false, headerBgColor, true, 12, "center"),
                                               initCell(4, 0, 1, 1, "적요", false, headerBgColor, true, 12, "center")
                                           ])
                            ///
                            ///
                            ////////

                            /////// inner data
                            ///
                            ///
                            addInnerRowData([
                                                initCell(0, 0, 1, 1, "생산", false, "white", false, 12, "left"),
                                                initCell(1, 0, 1, 1, "E001-0051", true, "white", false, 12, "left"),
                                                initCell(2, 0, 1, 1, "EFFECT MAIN PCB Assy [EFFECT MAIN PCB Assy]", true, "white", false, 12, "left"),
                                                initCell(3, 0, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 0, 1, 1, "", true, "white", false, 12, "left")
                                            ])
                            addInnerRowData([
                                                initCell(0, 1, 1, 1, "생산", false, "white", false, 12, "left"),
                                                initCell(1, 1, 1, 1, "E001-0053", true, "white", false, 12, "left"),
                                                initCell(2, 1, 1, 1, "EFFECT SHOT BUTTON PCB Assy [EFFECT SHOT BUTTON PCB Assy]", true, "white", false, 12, "left"),
                                                initCell(3, 1, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 1, 1, 1, "", true, "white", false, 12, "left")
                                            ])
                            addInnerRowData([
                                                initCell(0, 2, 1, 1, "생산", false, "white", false, 12, "left"),
                                                initCell(1, 2, 1, 1, "E001-0052", true, "white", false, 12, "left"),
                                                initCell(2, 2, 1, 1, "EFFECT SUB PCB Assy [EFFECT SUB PCB Assy]", true, "white", false, 12, "left"),
                                                initCell(3, 2, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 2, 1, 1, "", true, "white", false, 12, "left")
                                            ])
                            addInnerRowData([
                                                initCell(0, 3, 1, 1, "생산", false, "white", false, 12, "left"),
                                                initCell(1, 3, 1, 1, "E001-0054", true, "white", false, 12, "left"),
                                                initCell(2, 3, 1, 1, "EFFECT LED PCB Assy [EFFECT LED PCB Assy]", true, "white", false, 12, "left"),
                                                initCell(3, 3, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 3, 1, 1, "", true, "white", false, 12, "left")
                                            ])
                            addInnerRowData([
                                                initCell(0, 4, 1, 1, "생산", false, "white", false, 12, "left"),
                                                initCell(1, 4, 1, 1, "E001-0010", true, "white", false, 12, "left"),
                                                initCell(2, 4, 1, 1, "MAXIMUM/EFFECT IR PCB Assy[공용] [FR4, Green, White, 1.2T, 21.5*30, 2층, 외층 10Z, 금 도금]....MAXIMUM/EFFECT IR PCB Assy[공용] [FR4, Green, White, 1.2T, 21.5*30, 2층, 외층 10Z, 금 도금]", true, "white", false, 12, "left"),
                                                initCell(3, 4, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 4, 1, 1, "", true, "white", false, 12, "left")
                                            ])
                            ///
                            ///
                            ////////

                            /////// footer data
                            ///
                            ///
                            initFooterData([
                                               initCell(0, 0, 3, 1, "총합계", false, headerBgColor, true, 12, "center"),
                                               initCell(3, 0, 1, 1, "3,425.00", true, headerBgColor, true, 12, "right"),
                                               initCell(4, 0, 1, 1, "", true, headerBgColor, true, 12, "left")
                                           ])
                            ///
                            ///
                            ////////
                        }
                    }

                    TemplateTable {
                        id : consumptionTable
                        objectName : "consumptionTable"

                        anchors.top : productionTable.bottom

                        width : parent.width

                        dividedColCount : 6
                        dividedRowCount : listArea.dividedRowCount

                        innerDefaultData : [
                            initCell(0, 0, 1, 1, "소모", false, "white", false, 12, "left"),
                            initCell(1, 0, 1, 1, "", true, "white", false, 12, "left"),
                            initCell(2, 0, 1, 1, "", true, "white", false, 12, "left"),
                            initCell(3, 0, 1, 1, "", true, "white", false, 12, "right"),
                            initCell(4, 0, 1, 1, "", true, "white", false, 12, "left")
                        ]

                        Component.onCompleted: {
                            initDividedWidths([0.05, 0.15, 0.5, 0.1, 0.2])

                            tableModel.setTableSize(dividedColCount, dividedRowCount)

                            /////// inner data
                            ///
                            ///
                            addInnerRowData([
                                                initCell(0, 0, 1, 1, "소모", false, "white", false, 12, "left"),
                                                initCell(1, 0, 1, 1, "E003-0020", true, "white", false, 12, "left"),
                                                initCell(2, 0, 1, 1, "R1005_22R_J [SMD Chip Resistor, 1005, 22R, J]", true, "white", false, 12, "left"),
                                                initCell(3, 0, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 0, 1, 1, "", true, "white", false, 12, "left")
                                            ])

                            addInnerRowData([
                                                initCell(0, 1, 1, 1, "소모", false, "white", false, 12, "left"),
                                                initCell(1, 1, 1, 1, "E003-0023", true, "white", false, 12, "left"),
                                                initCell(2, 1, 1, 1, "R1005_1K_J [SMD Chip Resistor, 1005, 1K, J]", true, "white", false, 12, "left"),
                                                initCell(3, 1, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 1, 1, 1, "", true, "white", false, 12, "left")
                                            ])

                            addInnerRowData([
                                                initCell(0, 2, 1, 1, "소모", false, "white", false, 12, "left"),
                                                initCell(1, 2, 1, 1, "E003-0027", true, "white", false, 12, "left"),
                                                initCell(2, 2, 1, 1, "R1005_10K_J [SMD Chip Resistor, 1005, 1K, J]", true, "white", false, 12, "left"),
                                                initCell(3, 2, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 2, 1, 1, "", true, "white", false, 12, "left")
                                            ])

                            addInnerRowData([
                                                initCell(0, 3, 1, 1, "소모", false, "white", false, 12, "left"),
                                                initCell(1, 3, 1, 1, "E003-0038", true, "white", false, 12, "left"),
                                                initCell(2, 3, 1, 1, "R1005_150R_J [SMD Chip Resistor, 1005, 150R, J]", true, "white", false, 12, "left"),
                                                initCell(3, 3, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 3, 1, 1, "", true, "white", false, 12, "left")
                                            ])

                            addInnerRowData([
                                                initCell(0, 4, 1, 1, "소모", false, "white", false, 12, "left"),
                                                initCell(1, 4, 1, 1, "E003-0010", true, "white", false, 12, "left"),
                                                initCell(2, 4, 1, 1, "KPA-3010F3C [Kingbright, SMD, 3*2*1mm, 5V 90mA, Reel 2K]", true, "white", false, 12, "left"),
                                                initCell(3, 4, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 4, 1, 1, "", true, "white", false, 12, "left")
                                            ])

                            addInnerRowData([
                                                initCell(0, 5, 1, 1, "소모", false, "white", false, 12, "left"),
                                                initCell(1, 5, 1, 1, "E003-0011", true, "white", false, 12, "left"),
                                                initCell(2, 5, 1, 1, "KPA-3010P3BT [Kingbright, SMD, 3*2*1mm, 5V 100mA, Reel 2K]", true, "white", false, 12, "left"),
                                                initCell(3, 5, 1, 1, "685.00", true, "white", false, 12, "right"),
                                                initCell(4, 5, 1, 1, "", true, "white", false, 12, "left")
                                            ])
                            ///
                            ///
                            ////////
                        }
                    }
                }

                TemplateTable {
                    id : quantityTotalTable
                    objectName : "quantityTotalTable"

                    anchors.top : listArea.bottom
                    anchors.topMargin : 5
                    anchors.right : parent.right

                    width : templateItemArea.width

                    dividedColCount : 1
                    dividedRowCount : 6

                    Component.onCompleted: {
                        initDividedWidths([0.1, 0.25, 0.1, 0.25, 0.1, 0.2])
                        tableModel.setTableSize(dividedColCount, dividedRowCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "생산수량", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 0, 1, 1, "3,425.00", true, "white", false, 12, "right"),
                                            initCell(2, 0, 1, 1, "소모수량", false, headerBgColor, true, 12, "center"),
                                            initCell(3, 0, 1, 1, "4,110.00", true, "white", false, 12, "right"),
                                            initCell(4, 0, 1, 1, "인수", false, headerBgColor, true, 12, "center"),
                                            initCell(5, 0, 1, 1, "인", false, "white", false, 12, "right")
                                        ])
                        ///
                        ///
                        ////////
                    }
                }

                Image {
                    id : bottomImage
                    objectName : "bottomImage"

                    anchors.top : quantityTotalTable.bottom
                    anchors.topMargin : 10

                    anchors.horizontalCenter : parent.horizontalCenter

                    source : "qrc:/imageFiles/cfimages.jpg"
                }
            }
        }
    }
}
