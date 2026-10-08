import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: 520
    height: 380

    signal finished()

    property int currentStep: 1

    Rectangle {
        anchors.fill: parent
        radius: 12
        color: "#11131A"
        border.color: "#19E3FF"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            Text {
                text: "AIRWAVE QUICK SETUP (" + root.currentStep + "/3)"
                color: "#19E3FF"
                font.bold: true
                font.pixelSize: 16
            }

            // Step 1: Device Type
            Column {
                visible: root.currentStep === 1
                spacing: 12
                Text { text: "1. Select your phone platform:"; color: "#FFFFFF"; font.bold: true }
                Button { text: "Android (Samsung, OnePlus, Xiaomi, ROG, etc.)"; width: 400; onClicked: root.currentStep = 2 }
                Button { text: "Apple iOS (iPhone / iPad - Zero app needed via AirPlay)"; width: 400; onClicked: root.currentStep = 2 }
            }

            // Step 2: Connection Type
            Column {
                visible: root.currentStep === 2
                spacing: 12
                Text { text: "2. Choose connection mode:"; color: "#FFFFFF"; font.bold: true }
                Button { text: "🔌 USB Cable (Recommended for PUBG: <40ms latency)"; width: 400; onClicked: root.currentStep = 3 }
                Button { text: "📶 5GHz Wi-Fi (Wireless: <65ms latency)"; width: 400; onClicked: root.currentStep = 3 }
            }

            // Step 3: USB Tethering Checklist
            Column {
                visible: root.currentStep === 3
                spacing: 10
                Text { text: "3. Live Connection Checklist:"; color: "#FFFFFF"; font.bold: true }
                Text { text: "✔ USB Cable Connected"; color: "#B6FF3B" }
                Text { text: "✔ USB Tethering Enabled in Android Settings"; color: "#B6FF3B" }
                Text { text: "✔ PC RNDIS Interface Detected (192.168.42.x)"; color: "#B6FF3B" }

                Spacer { height: 10 }
                Button {
                    text: "LAUNCH RADAR & CAST"
                    width: 400
                    onClicked: root.finished()
                }
            }
        }
    }
}
