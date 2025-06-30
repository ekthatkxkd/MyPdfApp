import QtQuick
import QtQuick.Controls
import pdfExporter 1.0
import "qrc:/templateItems"

Item {
    id : rootArea
    objectName : "OrderForm"

    property alias pdfExporter : pdfExporter
    property alias templateItemArea : templateItemArea

    PdfExporter {
        id : pdfExporter
    }

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
                        text : "발주내용 추가"
                    }

                    MouseArea {
                        anchors.fill : parent
                        onClicked : {
                            orderDetailsTable.addInnerDefaultRowData()
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
                objectName : "orderFormItemArea"

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

                    text : "발 주 서"
                }

                Item {
                    id : orderDateItem
                    objectName : "orderDataItem"

                    property string orderDateItemText : dateTitle.text + orderDateText.text
                    property int maxTextWidth : parent.width / 2

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.right : parent.right

                    width : dateTitle.implicitWidth + orderDateText.width
                    height : dateTitle.height

                    Row {
                        spacing: 0
                        Text {
                            id : dateTitle

                            font.bold : true
                            font.pixelSize: 12

                            text : "발주일자 : "
                        }

                        TextInput {
                            id : orderDateText

                            readonly property int minWidth : 50

                            property string previousText: ""
                            property bool suppressSignal: false

                            readOnly: false

                            width : Math.max(implicitWidth, minWidth)

                            horizontalAlignment : TextInput.AlignRight

                            onTextChanged: {
                                if (suppressSignal)
                                    return

                                if ((metrics.advanceWidth + dateTitle.width) > orderDateItem.maxTextWidth) {
                                    if (previousText.length > text.length) {
                                        return
                                    }

                                    suppressSignal = true
                                    text = previousText // 복원
                                    suppressSignal = false
                                } else {
                                    previousText = text // 저장
                                }
                            }

                            TextMetrics {
                                id: metrics
                                font: orderDateText.font
                                text: orderDateText.text
                            }
                        }
                    }
                }

                TemplateTable {
                    id : recipientInformTable
                    objectName : "recipientInformTable"

                    anchors.top : templateTitle.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left
                    anchors.right : parent.right

                    dividedRowCount : 5
                    dividedColCount : 4

                    tableTextValue : "■ 수신자 정보"

                    Component.onCompleted: {
                        initDividedWidths([0.2, 0.3, 0.2, 0.3])
                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "사업자등록번호", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(0, 2, 1, 1, "거래담당자/TEL", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 3, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(1, 0, 1, 1, "회사명/대표", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(1, 2, 1, 1, "TEL/FAX", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 3, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(2, 0, 1, 1, "주  소", false, headerBgColor, true, 12, "center"),
                                            initCell(2, 1, 1, 3, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(3, 0, 1, 1, "납기희망일", false, headerBgColor, true, 12, "center"),
                                            initCell(3, 1, 1, 3, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(4, 0, 1, 1, "참고사항", false, headerBgColor, true, 12, "center"),
                                            initCell(4, 1, 1, 3, "", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////

                    }
                }

                TemplateTable {
                    id : senderInformTable
                    objectName : "senderInformTable"

                    anchors.top : recipientInformTable.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left
                    anchors.right : parent.right

                    dividedRowCount : 4
                    dividedColCount : 4

                    tableTextValue : "■ 발신자 정보"

                    Component.onCompleted: {
                        initDividedWidths([0.2, 0.3, 0.2, 0.3])
                        tableModel.setTableSize(dividedRowCount, dividedColCount)

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "발주번호", false, headerBgColor, true, 12, "center"),
                                            initCell(0, 1, 1, 3, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(1, 0, 1, 1, "사업자등록번호", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(1, 2, 1, 1, "담당자", false, headerBgColor, true, 12, "center"),
                                            initCell(1, 3, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(2, 0, 1, 1, "회사명/대표", false, headerBgColor, true, 12, "center"),
                                            initCell(2, 1, 1, 1, "", true, "white", false, 12, "left"),
                                            initCell(2, 2, 1, 1, "TEL/FAX", false, headerBgColor, true, 12, "center"),
                                            initCell(2, 3, 1, 1, "", true, "white", false, 12, "left")
                                        ])

                        addInnerRowData([
                                            initCell(3, 0, 1, 1, "주  소", false, headerBgColor, true, 12, "center"),
                                            initCell(3, 1, 1, 3, "", true, "white", false, 12, "left")
                                        ])
                        ///
                        ///
                        ////////

                    }
                }

                TemplateTable {
                    id : orderDetailsTable
                    objectName : "orderDetailsTable"

                    anchors.top : senderInformTable.bottom
                    anchors.topMargin : 5
                    anchors.left : parent.left
                    anchors.right : parent.right

                    dividedRowCount : 1
                    dividedColCount : 9

                    tableTextValue : "■ 발주 내용"

                    innerDefaultData : [
                        initCell(0, 0, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 1, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 2, 1, 1, "", true, "white", false, 12, "left"),
                        initCell(0, 3, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 4, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 5, 1, 1, "", true, "white", false, 12, "right"),
                        initCell(0, 6, 1, 1, "", true, "white", false, 12, "right"),
                        initCell(0, 7, 1, 1, "", true, "white", false, 12, "center"),
                        initCell(0, 8, 1, 1, "", true, "white", false, 12, "right")
                    ]

                    Component.onCompleted: {
                        initDividedWidths([0.05, 0.1, 0.15, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1])

                        headerTableModel.setTableSize(1, dividedColCount)
                        tableModel.setTableSize(dividedRowCount, dividedColCount)
                        footerTableModel.setTableSize(1, dividedColCount)

                        /////// header data
                        ///
                        ///
                        initHeaderData([
                                           initCell(0, 0, 1, 1, "NO", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 1, 1, 1, "품목코드", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 2, 1, 1, "품목명[규격]", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 3, 1, 1, "수량", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 4, 1, 1, "단가", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 5, 1, 1, "공급가액", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 6, 1, 1, "부가세", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 7, 1, 1, "합계", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 8, 1, 1, "적요", true, headerBgColor, true, 12, "center")
                                       ])
                        ///
                        ///
                        ////////

                        /////// inner data
                        ///
                        ///
                        addInnerRowData([
                                            initCell(0, 0, 1, 1, "1", true, "white", false, 12, "center"),
                                            initCell(0, 1, 1, 1, "L005-0061", true, "white", false, 12, "center"),
                                            initCell(0, 2, 1, 1, "이태리 전시회 리플렛 [210X297, 아트지 150g, 양면인쇄]", true, "white", false, 12, "left"),
                                            initCell(0, 3, 1, 1, "500.00", true, "white", false, 12, "center"),
                                            initCell(0, 4, 1, 1, "320.00", true, "white", false, 12, "center"),
                                            initCell(0, 5, 1, 1, "160,000.00", true, "white", false, 12, "right"),
                                            initCell(0, 6, 1, 1, "16,000.00", true, "white", false, 12, "right"),
                                            initCell(0, 7, 1, 1, "176,000.00", true, "white", false, 12, "center"),
                                            initCell(0, 8, 1, 1, "", true, "white", false, 12, "right")
                                        ])
                        ///
                        ///
                        ////////


                        /////// footer data
                        ///
                        ///
                        initFooterData([
                                           initCell(0, 0, 1, 3, "합계(VAT포함)", false, headerBgColor, true, 12, "center"),
                                           initCell(0, 3, 1, 1, "500.00", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 4, 1, 1, "320.00", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 5, 1, 1, "160,000.00", true, headerBgColor, true, 12, "right"),
                                           initCell(0, 6, 1, 1, "16,000.00", true, headerBgColor, true, 12, "right"),
                                           initCell(0, 7, 1, 1, "176,000.00", true, headerBgColor, true, 12, "center"),
                                           initCell(0, 8, 1, 1, "", true, headerBgColor, true, 12, "right")
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
