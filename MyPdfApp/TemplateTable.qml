import QtQuick
import QtQuick.Controls
import TableModel 1.0

Item {
    id : root

    property alias headerTableModel : headerTableModel
    property alias tableModel : tableModel
    property alias footerTableModel : footerTableModel

    readonly property int minCellHeight : 30
    readonly property string headerBgColor : "#f0f0f0"

    property string tableTextValue : ""

    property int dividedColCount : 0
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
        headerTableModel.insertRow(datas);
    }

    function addInnerRowData(datas) {
        if (datas.length <= 0) {
            return
        }

        addDividedInnerHeights()

        tableModel.insertRow(datas);
    }

    function addInnerDefaultRowData() {
        let defaultRowData = innerDefaultData

        for (let i = 0; i < innerDefaultData.length; i++) {
            defaultRowData[i].startCol = (dividedInnerHeights.length)
        }

        addInnerRowData(defaultRowData)
    }

    function initFooterData(datas) {
        footerTableModel.insertRow(datas);
    }

    Component {
        id : textEditComp
        TextEdit {
            objectName : "cellText"

            signal textModified(string newText)

            property var tableModelRef: null

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
                textModified(text)
            }
        }
    }

    Component {
        id : textComp
        Text {
            objectName : "cellText"

            property var tableModelRef: null

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
        id: headerTableModel

        function setTableSize(row, col) {
            headerTableModel.initializeTable(row, col);
        }

        Component.onCompleted: {
            console.log("TableModel 인스턴스 생성 완료");
        }

        // C++ 시그널 처리
        onCellDataChanged: function(row, col) {
            console.log(`셀 데이터 변경: (${row}, ${col})`);
        }

        onTableStructureChanged: {
            console.log("테이블 구조 변경됨");
        }

        onCellTextChanged: function(row, col, newText) {
            console.log(`셀 텍스트 변경: (${row}, ${col}) -> "${newText}"`);
        }
    }

    TableModel {
        id: tableModel

        function setTableSize(row, col) {
            tableModel.initializeTable(row, col);
        }

        Component.onCompleted: {
            console.log("TableModel 인스턴스 생성 완료");
            // initializeTable(0, 0);
        }

        // C++ 시그널 처리
        onCellDataChanged: function(row, col) {
            console.log(`셀 데이터 변경: (${row}, ${col})`);
        }

        onTableStructureChanged: {
            console.log("테이블 구조 변경됨");
        }

        onCellTextChanged: function(row, col, newText) {
            console.log(`셀 텍스트 변경: (${row}, ${col}) -> "${newText}"`);
        }
    }

    TableModel {
        id: footerTableModel

        function setTableSize(row, col) {
            footerTableModel.initializeTable(row, col);
        }

        Component.onCompleted: {
            console.log("TableModel 인스턴스 생성 완료");
        }

        // C++ 시그널 처리
        onCellDataChanged: function(row, col) {
            console.log(`셀 데이터 변경: (${row}, ${col})`);
        }

        onTableStructureChanged: {
            console.log("테이블 구조 변경됨");
        }

        onCellTextChanged: function(row, col, newText) {
            console.log(`셀 텍스트 변경: (${row}, ${col}) -> "${newText}"`);
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

            visible : headerTableModel.rowCount > 0 ? true : false // (headerData.length > 0) ? true : false

            Row {
                id : headerRow
                objectName : "headerRow"

                width : parent.width

                Repeater {
                    objectName : "headerColRep"
                    model : headerTableModel  // headerData

                    Repeater {
                        id : headerRowRep

                        model : rowData

                        Rectangle {
                            id : headerCell
                            objectName : "cell"

                            readonly property int headerTextMargin : 5

                            width : {
                                let getRowSpan = modelData.rowSpan
                                let actualWidth = 0

                                let getStartRow = modelData.startRow

                                for (let i = getStartRow; i < (getStartRow + getRowSpan); i++) {
                                    actualWidth += dividedWidths[i]
                                }

                                return actualWidth
                            }

                            height : headerHeight  // dividedHeights[0]

                            color : modelData.bgColor
                            border.color : "black"

                            Text {
                                id : headerText
                                anchors.fill : parent
                                anchors.margins : headerCell.headerTextMargin

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
        }

        Item {
            id : innerTableArea

            width : root.width
            height : childrenRect.height

            Repeater {
                id : innerColRep
                objectName : "innerColRep"

                model : tableModel // innerDatas.length

                Repeater {
                    id : innerRowRep
                    readonly property int innerColIndex : index

                    model : rowData  // innerDatas[innerColIndex]

                    Rectangle {
                        id : cellArea

                        property var cellData : modelData

                        readonly property int innerRowIndex : index
                        readonly property int innerTextMargin : 5

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

                            anchors.margins: cellArea.innerTextMargin

                            sourceComponent: {
                                if (modelData.isTextEdit) {
                                    textEditComp
                                } else {
                                    textComp
                                }
                            }

                            onStatusChanged : {
                                if (status === Loader.Ready) {
                                    item.tableModelRef = tableModel

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


                            Component.onCompleted: {
                                if (innerTextTypeLoader.item && innerTextTypeLoader.item.textModified) {
                                    innerTextTypeLoader.item.textModified.connect(function(newText) {
                                        tableModel.updateCellText(innerRowRep.innerColIndex, cellArea.innerRowIndex, newText);
                                    })
                                }
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

            visible : footerTableModel.rowCount > 0 ? true : false   // (footerData.length > 0) ? true : false

            Row {
                id : footerRow
                objectName : "footerRow"

                width : parent.width

                Repeater {
                    objectName : "footerColRep"
                    model : footerTableModel  // footerData

                    Repeater {
                        id : footerRowRep
                        readonly property int footerColIndex : index

                        model : rowData

                        Rectangle {
                            id : footerCell
                            objectName : "cell"

                            property var cellData : modelData

                            readonly property int footerRowIndex : index
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

                            Loader {
                                id : footerTextTypeLoader

                                property var currentModelData : modelData

                                anchors.fill : parent
                                anchors.margins : footerCell.footerTextMargin

                                sourceComponent: {
                                    if (modelData.isTextEdit) {
                                        textEditComp
                                    } else {
                                        textComp
                                    }
                                }

                                onStatusChanged : {
                                    if (status === Loader.Ready) {
                                        item.tableModelRef = footerTableModel
                                    }
                                }
                            }

                            Connections {
                                target: footerTextTypeLoader.item

                                enabled: ((footerTextTypeLoader.status === Loader.Ready) &&
                                          (footerTextTypeLoader.item !== null))

                                function onContentHeightChanged() {
                                    let colHeight = footerHeight

                                    let contentTextHeight = 0

                                    if (footerTextTypeLoader.item) {
                                        contentTextHeight = footerTextTypeLoader.item.contentHeight
                                    }

                                    if (contentTextHeight > colHeight) {
                                        footerHeight = contentTextHeight + (2*footerCell.footerTextMargin)

                                        footerHeight = footerHeight
                                    }
                                }

                                Component.onCompleted: {
                                    if (footerTextTypeLoader.item && footerTextTypeLoader.item.textModified) {
                                        footerTextTypeLoader.item.textModified.connect(function(newText) {
                                            footerTableModel.updateCellText(footerRowRep.footerColIndex, footerCell.footerRowIndex, newText);
                                        })
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
