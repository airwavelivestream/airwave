import QtQuick
import QtQuick.Controls

Item {
    id: root
    height: 52
    width: railContent.width + 32

    signal recordClicked()
    signal screenshotClicked()
    signal rotateClicked()
    signal hudToggled()
    signal fullscreenClicked()

    property bool isRecording: false
    property bool hudVisible: true

    Rectangle {
        anchors.fill: parent
        radius: 26
        color: "#E611131A"
        border.color: "#3319E3FF"
        border.width: 1

        Row {
            id: railContent
            anchors.centerIn: parent
            spacing: 12

            // Record Button
            Button {
                id: recBtn
                width: 36
                height: 36
                background: Rectangle {
                    radius: 18
                    color: root.isRecording ? "#FF6B35" : "#1F232F"
                    border.color: root.isRecording ? "#FF6B35" : "#3C4254"
                }
                contentItem: Text {
                    text: root.isRecording ? "REC" : "●"
                    color: "#FFFFFF"
                    font.bold: true
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.recordClicked()
            }

            // Screenshot Button
            Button {
                width: 36
                height: 36
                background: Rectangle { radius: 18; color: "#1F232F" }
                contentItem: Text { text: "📷"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: root.screenshotClicked()
            }

            // Rotate 90
            Button {
                width: 36
                height: 36
                background: Rectangle { radius: 18; color: "#1F232F" }
                contentItem: Text { text: "⟳"; color: "#FFFFFF"; font.pixelSize: 16; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: root.rotateClicked()
            }

            // Toggle HUD
            Button {
                width: 36
                height: 36
                background: Rectangle { radius: 18; color: root.hudVisible ? "#7C5CFF" : "#1F232F" }
                contentItem: Text { text: "HUD"; color: "#FFFFFF"; font.bold: true; font.pixelSize: 10; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: root.hudToggled()
            }

            // Fullscreen
            Button {
                width: 36
                height: 36
                background: Rectangle { radius: 18; color: "#1F232F" }
                contentItem: Text { text: "⛶"; color: "#FFFFFF"; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: root.fullscreenClicked()
            }
        }
    }
}
