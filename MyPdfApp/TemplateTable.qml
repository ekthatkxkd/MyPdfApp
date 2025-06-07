import QtQuick

Item {
    id : root
    readonly property int minCellHeight : 30
    readonly property string headerBgColor : "#f0f0f0"

    property string tableTextValue : ""

    property var headerData : []
    property list<var> innerDatas : []
    property var footerData : []

    property int dividedInnerColCount : 0
    property int dividedRowCount : 0

    property var dividedWidths : []

    property int headerHeight : minCellHeight
    property var dividedInnerHeights : []
    property int footerHeight : minCellHeight

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

    function addDividedInnerHeights() {
        dividedInnerHeights.push(minCellHeight)
        dividedInnerColCount = dividedInnerHeights.length
        dividedInnerHeights = dividedInnerHeights
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

        // headerData = headerData
        headerDataChanged()
    }

    function addInnerRowData(datas) {
        addDividedInnerHeights()

        let rowDatas = []

        for (let index = 0; index < datas.length; index++) {
            rowDatas.push(datas[index])
        }

        innerDatas.push(rowDatas)

        // innerDatas = innerDatas
        innerDatasChanged()
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
            height : headerHeight  // dividedHeights[0]

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
                            // let getIndex = index

                            for (let i = getStartRow; i < (getStartRow + getRowSpan); i++) {
                                actualWidth += dividedWidths[i]
                            }

                            return actualWidth
                        }

                        height : headerHeight  // dividedHeights[0]

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

            height : {
                let sumHeight = 0;

                for (let index = 0; index < dividedInnerHeights.length; index++) {
                    sumHeight += dividedInnerHeights[index]
                }

                return sumHeight
            }

            Repeater {
                model : innerDatas.length

                Repeater {
                    readonly property int innerColIndex : index

                    model : innerDatas[innerColIndex]

                    Rectangle {
                        readonly property int innerRowIndex : index
                        x : {
                            let testinnerdata = innerDatas
                            let testInnerDataslength = innerDatas.length
                            let testInnerDatasindexdata = innerDatas[innerColIndex]
                            let testInnerDatasindexdatalength = innerDatas[innerColIndex].length
                            let testInnerDatasindexdatalengthdata = innerDatas[innerColIndex][innerRowIndex]

                            let testModelData = modelData
                            let testStartRow = modelData.startRow

                            let basicXpos = 0  // dividedWidths[modelData.startRow]

                            for (let index = 0; index < (modelData.startRow); index++) {
                                basicXpos += dividedWidths[index];
                            }

                            return basicXpos
                        }
                        y : {
                            let testinnerdata = innerDatas
                            let testInnerDataslength = innerDatas.length
                            let testInnerDatasindexdata = innerDatas[innerColIndex]
                            let testInnerDatasindexdatalength = innerDatas[innerColIndex].length
                            let testInnerDatasindexdatalengthdata = innerDatas[innerColIndex][innerRowIndex]

                            let testModelData = modelData
                            let testStartRow = modelData.startRow


                            let basicYpos = 0  // dividedInnerHeights[modelData.startCol]

                            for (let index = 0; index < (modelData.startCol); index++) {
                                basicYpos += dividedInnerHeights[index];
                            }

                            return basicYpos
                        }

                        width : {
                            let testinnerdata = innerDatas
                            let testInnerDataslength = innerDatas.length
                            let testInnerDatasindexdata = innerDatas[innerColIndex]
                            let testInnerDatasindexdatalength = innerDatas[innerColIndex].length
                            let testInnerDatasindexdatalengthdata = innerDatas[innerColIndex][innerRowIndex]

                            let testModelData = modelData
                            let testStartRow = modelData.startRow


                            let sumWidth = 0

                            for (let index = modelData.startRow; index < (modelData.startRow + modelData.rowSpan); index++) {
                                sumWidth += dividedWidths[index]
                            }

                            return sumWidth
                        }

                        height : {
                            let testinnerdata = innerDatas
                            let testInnerDataslength = innerDatas.length
                            let testInnerDatasindexdata = innerDatas[innerColIndex]
                            let testInnerDatasindexdatalength = innerDatas[innerColIndex].length
                            let testInnerDatasindexdatalengthdata = innerDatas[innerColIndex][innerRowIndex]

                            let testModelData = modelData
                            let testStartRow = modelData.startRow


                            let sumHeight = 0

                            for (let index = modelData.startCol; index < (modelData.startCol + modelData.colSpan); index++) {
                                sumHeight += dividedInnerHeights[index]
                            }

                            return sumHeight
                        }

                        color : modelData.bgColor
                        border.color : "blue"
                    }
                }
            }
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
    }
}
