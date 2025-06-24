import QtQuick
import QtQuick.Controls
import "qrc:/templateItems"

Item {
    id : rootArea
    objectName : "MaterialForm"

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
                        text : "구매내역 추가"
                    }

                    MouseArea {
                        anchors.fill : parent
                        onClicked : {
                            purchaseHistory.addInnerDefaultRowData()
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
                objectName : "materialItemArea"

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

                    text : "자재 구매 확인서"
                }

                TemplateTable {
                    id : informTable
                    objectName : "informTable"

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left
                    anchors.right : parent.right

                    dividedRowCount : 3
                    dividedColCount : 5

                    Component.onCompleted: {
                        initDividedWidths([0.15, 0.3, 0.1, 0.15, 0.3])

                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        //////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "구매번호", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(0, 2, 3, 1, "구매처", false, headerBgColor, true, 12, "center", true),
                                            initCell(0, 3, 1, 1, "상호", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 4, 1, 1, "", true, "white", false, 12, "left")
                                        ])


                        addInnerRowData([
                                            initCell(1, 0, 1, 1, "구매창고", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(1, 3, 1, 1, "주소", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 4, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(2, 0, 1, 1, "구매담당자", false, headerBgColor, true, 12, "center"),
                                            initCell(2, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(2, 3, 1, 1, "TEL", false, headerBgColor, true, 12, "center"),
                                            initCell(2, 4, 1, 1, "", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////
                    }

                }


                TemplateTable {
                    id : purchaseHistory
                    objectName : "purchaseHistory"

                    anchors.top : informTable.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left
                    anchors.right : parent.right

                    dividedRowCount : 5
                    dividedColCount : 4

                    tableTextValue : "▶ 자재구매 내역"

                    innerDefaultData : [
                        initCell(0, 0, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 1, 1, 1, "", true, "white", false, 12, "left"),
                        initCell(0, 2, 1, 1, "", true, "white", false, 12, "right"),
                        initCell(0, 3, 1, 1, "", true, "white", false, 12, "center")
                    ]

                    Component.onCompleted: {
                        initDividedWidths([0.1, 0.6, 0.1, 0.2])

                        headerTableModel.setTableSize(1, dividedColCount)
                        tableModel.setTableSize(dividedRowCount, dividedColCount)
                        footerTableModel.setTableSize(1, dividedColCount)

                        //////// header data
                        ///
                        ///
                        initHeaderData([
                                           initCell(0, 0, 1, 1, "일자", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 1, 1, 1, "품목명[규격명]", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 2, 1, 1, "수랑", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 3, 1, 1, "적요", false, headerBgColor, true, 12, "center"),
                                       ])
                        ///
                        ///
                        ////////

                        //////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "03/13", true, "white", false, 12, "center"),
                                            initCell(0, 1, 1, 1, "[EFFECT_V2_SKY] FRONT BUTTON DECO", true, "white", false, 12, "left"),
                                            initCell(0, 2, 1, 1, "1,232.00", true, "white", false, 12, "right"),
                                            initCell(0, 3, 1, 1, "", true, "white", false, 12, "center"),
                                        ])

                        addInnerDefaultRowData()
                        addInnerDefaultRowData()
                        addInnerDefaultRowData()
                        addInnerDefaultRowData()
                        ///
                        ///
                        ////////

                        //////// footer data
                        ///
                        ///
                        initFooterData([
                                           initCell(0, 0, 1, 2, "총합계", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 2, 1, 1, "1,232.00", false, headerBgColor, true, 12, "right"),
                                           initCell(0, 3, 1, 1, "", false, headerBgColor, true, 12, "center")
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
