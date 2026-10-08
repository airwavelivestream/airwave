import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: 380
    height: 280

    signal presetSelected(string mode)

    property string currentPreset: "ULTRA_LOW_LATENCY"

    Rectangle {
        anchors.fill: parent
        radius: 12
        color: "#11131A"
        border.color: "#7C5CFF"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 12

            Text {
                text: "GAME MODE OPTIMIZATION"
                color: "#19E3FF"
                font.bold: true
                font.pixelSize: 14
                font.letterSpacing: 1
            }

            Text {
                text: "Select latency vs visual fidelity preset for PUBG/BGMI:"
                color: "#8E929E"
                font.pixelSize: 11
            }

            // Presets
            RadioButton {
                id: rbUltra
                text: "⚡ Ultra Low Latency (Target: <40ms, V-Sync Off, 0-frame buffer)"
                checked: root.currentPreset === "ULTRA_LOW_LATENCY"
                onClicked: root.presetSelected("ULTRA_LOW_LATENCY")
                contentItem: Text { text: rbUltra.text; color: "#FFFFFF"; font.pixelSize: 11 }
            }

            RadioButton {
                id: rbBalanced
                text: "⚖️ Balanced (Target: ~55ms, 1-frame jitter smoothing)"
                checked: root.currentPreset === "BALANCED"
                onClicked: root.presetSelected("BALANCED")
                contentItem: Text { text: rbBalanced.text; color: "#FFFFFF"; font.pixelSize: 11 }
            }

            RadioButton {
                id: rbQuality
                text: "💎 Max Quality (Target: ~75ms, 20Mbps CBR, 1440p/4K support)"
                checked: root.currentPreset === "MAX_QUALITY"
                onClicked: root.presetSelected("MAX_QUALITY")
                contentItem: Text { text: rbQuality.text; color: "#FFFFFF"; font.pixelSize: 11 }
            }
        }
    }
}
