import QtQuick
import QtQuick.Controls

Item {
    id: root
    width: 280
    height: 180

    property string deviceName: "OnePlus 11 5G"
    property string connectionType: "USB 3.0 Tethered"
    property string ipAddress: "192.168.42.129"
    property real latencyMs: 38.5
    property bool isWireless: false

    signal castClicked()

    Rectangle {
        id: cardBg
        anchors.fill: parent
        radius: 12
        color: mouseArea.containsMouse ? "#181B24" : "#11131A"
        border.color: mouseArea.containsMouse ? "#19E3FF" : "#222634"
        border.width: 1

        Behavior on color { ColorAnimation { duration: 120 } }
        Behavior on border.color { ColorAnimation { duration: 120 } }

        Column {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            Row {
                width: parent.width
                spacing: 8

                // Device indicator dot
                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    color: root.latencyMs < 60 ? "#B6FF3B" : "#FF6B35"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: root.deviceName
                    color: "#FFFFFF"
                    font.pixelSize: 16
                    font.bold: true
                }
            }

            Text {
                text: root.connectionType + " • " + root.ipAddress
                color: "#8E929E"
                font.pixelSize: 12
            }

            Row {
                spacing: 6
                Text {
                    text: "PING:"
                    color: "#8E929E"
                    font.pixelSize: 11
                    font.bold: true
                }
                Text {
                    text: root.latencyMs.toFixed(1) + " ms"
                    color: root.latencyMs < 60 ? "#B6FF3B" : (root.latencyMs < 100 ? "#19E3FF" : "#FF6B35")
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Spacer { height: 4 }

            Button {
                id: castBtn
                text: "START CASTING"
                width: parent.width
                height: 38

                background: Rectangle {
                    radius: 8
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#7C5CFF" }
                        GradientStop { position: 1.0; color: "#19E3FF" }
                    }
                }
                contentItem: Text {
                    text: castBtn.text
                    color: "#FFFFFF"
                    font.bold: true
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: root.castClicked()
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.NoButton
        }
    }
}
