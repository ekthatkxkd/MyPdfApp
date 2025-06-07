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

        Repeater {
            model : 1
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
                        text : "품목명 추가"
                    }

                    MouseArea {
                        anchors.fill : parent
                        onClicked : {
                            // loadTemplateForm(index)

                            informTable.addInnerRowData([
                                                informTable.initCell(0, 3, 1, 1, "구매담당자", false, informTable.headerBgColor, true, 12, "center"),
                                                informTable.initCell(1, 3, 1, 1, "", true, "white", false, 12, "left"),
                                                informTable.initCell(2, 3, 1, 1, "TEL", false, informTable.headerBgColor, true, 12, "center"),
                                                informTable.initCell(3, 3, 1, 1, "TEL", false, informTable.headerBgColor, true, 12, "center"),
                                                informTable.initCell(4, 3, 1, 1, "", true, "white", false, 12, "left")
                                            ])
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

                    // dividedInnerColCount : 3
                    dividedRowCount : 5

                    tableTextValue : "테스트 타이틀"

                    Component.onCompleted: {
                        initDividedWidths([0.15, 0.3, 0.1, 0.15, 0.3])

                        initHeaderData([
                                           initCell(0, 0, 1, 1, "테스트1", false, headerBgColor, true, 12, "center"),
                                           initCell(1, 0, 2, 1, "테스트2", false, headerBgColor, true, 12, "center", false),
                                           initCell(3, 0, 2, 1, "테스트3", false, headerBgColor, true, 12, "center", false)
                                       ])

                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "구매번호", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 0, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(2, 0, 1, 3, "구매처", false, headerBgColor, true, 12, "center", true),
                                            initCell(3, 0, 1, 1, "상호", false, headerBgColor, true, 12, "center"),
                                            initCell(4, 0, 1, 1, "", true, "white", false, 12, "left")
                                        ])


                        addInnerRowData([
                                            initCell(0, 1, 1, 1, "구매창고", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(3, 1, 1, 1, "주소", false, headerBgColor, true, 12, "center"),
                                            initCell(4, 1, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(0, 2, 1, 1, "구매담당자", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 2, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(3, 2, 1, 1, "TEL", false, headerBgColor, true, 12, "center"),
                                            initCell(4, 2, 1, 1, "", true, "white", false, 12, "left")
                                        ])
                    }
                }
            }
        }
    }
}
