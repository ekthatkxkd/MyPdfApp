import QtQuick

Item {
    id : root
    readonly property int minCellHeight : 30
    readonly property string headerBgColor : "#f0f0f0"

    property string tableTextValue : ""
    property var headerData : []
    property list<var> innerData : []
    property var footerData : []

    property int dividedColCount : 0
    property int dividedRowCount : 0

    property var dividedWidths : []
    property var dividedHeights : []  // header, inner, footer

    function initDividedWidths(widthRatios) {
        let remainWidth = root.width

        for (let index = 0; index < dividedRowCount; index++) {
            let calculatedWidth = Math.round(root.width * widthRatios[index])

            dividedWidths.push(calculatedWidth)
            remainWidth -= calculatedWidth
        }

        dividedWidths[dividedWidths.length - 1] += remainWidth

        dividedWidths = dividedWidths
    }

    function initDividedHeights() {
        for (let index = 0; index < dividedColCount; index++) {
            dividedHeights.push(minCellHeight)
        }

        dividedHeights = dividedHeights
    }

    function initCell(startRow, startCol, rowSpan, colSpan,
                      cellText, isTextEdit, bgColor, isBold, fontSize, alignPosition, isVerticalDir) : var {
        let newCell = {
            startRow : startRow || 0,
            startCol : startCol || 0,
            rowSpan : rowSpan || 1,
            colSpan : colSpan || 1,
            cellText : cellText || "",
            isTextEdit : isTextEdit || false,
            bgColor : bgColor || "white",
            isBold : isBold || false,
            fontSize : fontSize || 12,
            alignPosition : alignPosition || "right",
            isVerticalDir : isVerticalDir || false
        }

        return newCell
    }

    function initHeaderData(datas) {
        for (let index = 0; index < datas.length; index++) {
            headerData.push(datas[index])
        }

        headerData = headerData
    }

    Component {
        id : textEditComp
        TextEdit {

        }
    }
    Component {
        id : textComp
        Text {

        }
    }

    Column {
        id : column

        width : root.width

        spacing : 0

        Item {
            id : textArea
            objectName : "tableTitle"

            width : root.width
            height : tableText.height

            visible : (tableTextValue !== "") ? true : false

            Text {
                id : tableText
                objectName : "tableText"

                height : visible ? contentHeight : 0

                font.pixelSize : 15
                font.bold : true

                text : tableTextValue
            }
        }

        Item {
            id : headerTableArea

            width : root.width
            height : dividedHeights[0]

            visible : (headerData.length > 0) ? true : false

            Row {
                id : headerRow
                objectName : "headerRow"

                width : root.width

                Repeater {
                    model : headerData

                    Rectangle {
                        id : headerCell
                        objectName : "cell"

                        width : {
                            let getRowSpan = modelData.rowSpan
                            let actualWidth = 0

                            let getStartRow = modelData.startRow
                            let getIndex = index

                            for (let i = getStartRow; i < (getStartRow + getRowSpan); i++) {
                                actualWidth += dividedWidths[i]
                            }

                            return actualWidth
                        }

                        height : dividedHeights[0]

                        color : modelData.bgColor
                        border.color : "black"

                        Text {
                            anchors.fill : parent
                            anchors.margins : 5
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter

                            font.pixelSize : modelData.fontSize
                            font.bold : modelData.isBold

                            text : modelData.cellText
                        }
                    }
                }
            }
        }

        Item {
            id : innerTableArea
            width : root.width







        }
    }


    Rectangle {
        width : column.width
        height : column.height
        color : "transparent"
        border.color : "red"
        border.width : 2
    }

    Component.onCompleted: {
        initDividedHeights()
    }
}
