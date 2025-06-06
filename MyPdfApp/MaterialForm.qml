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

                width : scrollView.width/2

                Rectangle {
                    width : scrollView.width / 2
                    height : 1000
                    color : "skyblue"
                    border.color : "red"

                    Text {
                        anchors.centerIn: parent
                        text : "TEST rectangle.."
                        font.pixelSize: 20
                    }
                }
            }
        }
    }
}
