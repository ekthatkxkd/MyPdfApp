import QtQuick
import QtQuick.Controls
import TableModel 1.0

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

    property var innerDefaultData : []

    height : column.implicitHeight

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
        if (datas.length <= 0) {
            return
        }

        addDividedInnerHeights()

        let rowDatas = []

        for (let index = 0; index < datas.length; index++) {
            rowDatas.push(datas[index])
        }

        innerDatas.push(rowDatas)

        // innerDatas = innerDatas
        innerDatasChanged()
    }

    function addInnerDefaultRowData() {
        let defaultRowData = innerDefaultData

        for (let i = 0; i < innerDefaultData.length; i++) {
            defaultRowData[i].startCol = (dividedInnerHeights.length)
        }

        addInnerRowData(defaultRowData)
    }

    function initFooterData(datas) {
        for (let index = 0; index < datas.length; index++) {
            footerData.push(datas[index])
        }

        footerDataChanged()
    }

    Component {
        id : textEditComp
        TextEdit {
            id : cellTextEdit
            objectName : "cellText"

            signal textModified(string newText)

            anchors.fill : parent

            horizontalAlignment : {
                if (currentModelData.alignPosition === "center") {
                    Text.AlignHCenter
                } else if (currentModelData.alignPosition === "left") {
                    Text.AlignLeft
                } else {
                    Text.AlignRight
                }
            }
            verticalAlignment : Text.AlignVCenter

            wrapMode : TextEdit.Wrap

            font.pixelSize : currentModelData.fontSize
            font.bold : currentModelData.isBold

            text : currentModelData.cellText

            onTextChanged: {
                textModified(cellTextEdit.text)
            }
        }
    }

    Component {
        id : textComp

        Text {
            objectName : "cellText"

            anchors.fill : parent

            horizontalAlignment : {
                if (currentModelData.alignPosition === "center") {
                    Text.AlignHCenter
                } else if (currentModelData.alignPosition === "left") {
                    Text.AlignLeft
                } else {
                    Text.AlignRight
                }
            }

            verticalAlignment : Text.AlignVCenter

            wrapMode : Text.Wrap

            font.pixelSize : currentModelData.fontSize
            font.bold : currentModelData.isBold

            text : {
                if (currentModelData.isVerticalDir) {
                    currentModelData.cellText.split("").join("\n")
                } else {
                    currentModelData.cellText
                }
            }
        }
    }

    TableModel {
        id: tableModel

        // 모델 변경 시그널 처리
        onCellDataChanged: function(row, col) {
            console.log(`셀 데이터 변경: (${row}, ${col})`);
        }

        onTableStructureChanged: {
            console.log("테이블 구조 변경됨");
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

                width : parent.width

                Repeater {
                    model : headerData

                    Rectangle {
                        id : headerCell
                        objectName : "cell"

                        readonly property int headerTextMargin : 5

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
                            anchors.margins : headerTextMargin

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
                id : innerColRep
                model : innerDatas.length


                Repeater {
                    id : innerRowRep
                    readonly property int innerColIndex : index

                    model : innerDatas[innerColIndex]

                    Rectangle {
                        id : cellArea
                        readonly property int innerRowIndex : index
                        readonly property int innerTextMargin : 5

                        readonly property int textContentHeight : (innerTextTypeLoader.status === Loader.Ready &&
                                                                   innerTextTypeLoader.item !== null &&
                                                                   innerTextTypeLoader.item.contentHeight !== undefined
                                                                   ) ? innerTextTypeLoader.item.contentHeight
                                                                    : 0

                        x : {
                            let basicXpos = 0  // dividedWidths[modelData.startRow]

                            for (let index = 0; index < (modelData.startRow); index++) {
                                basicXpos += dividedWidths[index];
                            }

                            return basicXpos
                        }

                        y : {
                            let basicYpos = 0  // dividedInnerHeights[modelData.startCol]

                            for (let index = 0; index < (modelData.startCol); index++) {
                                basicYpos += dividedInnerHeights[index];
                            }

                            return basicYpos
                        }

                        width : {
                            let sumWidth = 0

                            for (let index = modelData.startRow; index < (modelData.startRow + modelData.rowSpan); index++) {
                                sumWidth += dividedWidths[index]
                            }

                            return sumWidth
                        }

                        height : {
                            let sumHeight = 0

                            for (let index = modelData.startCol; index < (modelData.startCol + modelData.colSpan); index++) {
                                sumHeight += dividedInnerHeights[index]
                            }
                            return sumHeight
                        }

                        color : modelData.bgColor
                        border.color : "black"

                        Loader {
                            id : innerTextTypeLoader

                            property var currentModelData : modelData

                            anchors.fill : parent

                            anchors.margins: innerTextMargin

                            sourceComponent: {
                                if (modelData.isTextEdit) {
                                    textEditComp
                                } else {
                                    textComp
                                }
                            }
                        }

                        Connections {
                            target: innerTextTypeLoader.item

                            enabled: ((innerTextTypeLoader.status === Loader.Ready) &&
                                      (innerTextTypeLoader.item !== null))

                            function onContentHeightChanged() {
                                let colHeight = dividedInnerHeights[innerRowRep.innerColIndex]

                                let contentTextHeight = 0

                                if (innerTextTypeLoader.item) {
                                    contentTextHeight = innerTextTypeLoader.item.contentHeight
                                }

                                if (contentTextHeight > colHeight) {
                                    dividedInnerHeights[innerRowRep.innerColIndex] = contentTextHeight + (2*innerTextMargin)

                                    dividedInnerHeights = dividedInnerHeights
                                }
                            }

                            function onTextModified(newText) {
                                let colIndex = innerRowRep.innerColIndex   // outer index
                                let rowIndex = cellArea.innerRowIndex   // inner index

                                // 방어 코드 및 디버그 로그
                                if (!root.innerDatas || root.innerDatas.length <= colIndex) {
                                    console.warn("[ERROR] Invalid column index:", colIndex)
                                    return
                                }

                                let column = root.innerDatas[colIndex]
                                if (!column || column.length <= rowIndex) {
                                    console.warn("[ERROR] Invalid row index:", rowIndex)
                                    return
                                }

                                // 안전하게 셀 복사 및 수정
                                let cell = Object.assign({}, column[rowIndex])
                                cell.cellText = newText

                                // 내부 배열도 복사 후 수정 → QML이 감지 가능하게
                                let newColumn = column.slice(0)
                                newColumn[rowIndex] = cell
                                root.innerDatas[colIndex] = newColumn

                                console.log("[UPDATED] cellText:", root.innerDatas[colIndex][rowIndex].cellText)

                                /*
                                let rowIndex = cellArea.innerRowIndex  // root.selectedRow
                                let colIndex = innerRowRep.innerColIndex // root.selectedCol
                                console.log("[LLDDSS] rowIndex : ", rowIndex)
                                console.log("[LLDDSS] colIndex : ", colIndex)

                                // 방어 코드 및 디버깅 로그
                                if (!root.innerDatas || root.innerDatas.length <= colIndex) {
                                    console.warn("[ERROR] Invalid row index:", colIndex)
                                    return
                                }

                                let row = root.innerDatas[colIndex]
                                if (!row || row.length <= colIndex) {
                                    console.warn("[ERROR] Invalid column index:", colIndex)
                                    return
                                }

                                // 안전하게 복사하고 값 수정
                                let cell = Object.assign({}, row[rowIndex])
                                cell.cellText = newText

                                // 배열을 복사해서 다시 대입 (QML이 감지할 수 있도록)
                                let newRow = row.slice(0) // row 자체도 복사
                                newRow[rowIndex] = cell
                                root.innerDatas[colIndex] = newRow

                                console.log("[UPDATED] cellText:", root.innerDatas[colIndex][rowIndex].cellText)
                                */
                            }

                            function onTextChanged() {
                                // console.log("[LLDDSS] onTextChanged")
                                // console.log("[LLDDSS] onTextChanged index : ", innerDatas[innerRowRep.innerColIndex][index])
                                // console.log("[LLDDSS] onTextChanged startRow : ", innerDatas[innerRowRep.innerColIndex][index].startRow)
                                // console.log("[LLDDSS] onTextChanged startCol : ", innerDatas[innerRowRep.innerColIndex][index].startCol)
                                // innerDatas[innerRowRep.innerColIndex][index].cellText = innerTextTypeLoader.item.text;
                                // console.log("[LLDDSS] onTextChanged text : ", innerDatas[innerRowRep.innerColIndex][index].cellText)
                            }
                        }
                    }
                }
            }
        }

        Item {
            id : footerTableArea

            width : root.width
            height : footerHeight

            visible : (footerData.length > 0) ? true : false

            Row {
                id : footerRow
                objectName : "footerRow"

                width : parent.width

                Repeater {
                    model : footerData

                    Rectangle {
                        id : footerCell
                        objectName : "cell"

                        readonly property int footerTextMargin : 5

                        width : {
                            let getStartRow = modelData.startRow
                            let getRowSpan = modelData.rowSpan

                            let actualWidth = 0

                            for (let i = getStartRow; i < (getStartRow + getRowSpan); i++) {
                                actualWidth += dividedWidths[i]
                            }

                            return actualWidth

                        }

                        height : footerHeight

                        color : modelData.bgColor
                        border.color : "black"

                        Text {
                            anchors.fill : parent
                            anchors.margins : footerTextMargin


                            verticalAlignment : Text.AlignVCenter
                            horizontalAlignment : {
                                if (modelData.alignPosition === "center") {
                                    Text.AlignHCenter
                                } else if (modelData.alignPosition === "left") {
                                    Text.AlignLeft
                                } else {
                                    Text.AlignRight
                                }
                            }

                            font.pixelSize : modelData.fontSize
                            font.bold : modelData.isBold

                            text : modelData.cellText
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
    }
}
