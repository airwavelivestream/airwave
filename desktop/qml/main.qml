import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Airwave 1.0
import "components"

ApplicationWindow {
    id: appWindow
    visible: true
    width: 1366
    height: 768
    minimumWidth: 960
    minimumHeight: 600
    title: qsTr("Airwave - Your phone. Your big screen. Zero lag.")
    color: "#07080C"
    icon.source: "qrc:/qml/assets/airwave_logo.svg"

    property bool isStageActive: false
    property bool isFullscreen: false

    // App Hotkeys (Navigation & Controls)
    Shortcut {
        sequence: "F11"
        onActivated: toggleFullscreen()
    }
    Shortcut {
        sequence: "Ctrl+H"
        onActivated: stageHud.visible = !stageHud.visible
    }
    Shortcut {
        sequence: "Ctrl+R"
        onActivated: controlRail.recordClicked()
    }

    function toggleFullscreen() {
        if (isFullscreen) {
            appWindow.showNormal();
            isFullscreen = false;
        } else {
            appWindow.showFullScreen();
            isFullscreen = true;
        }
    }

    // 1. Device Radar View (Home)
    Item {
        id: radarView
        anchors.fill: parent
        visible: !appWindow.isStageActive

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 40
            spacing: 24

            // Brand Header
            RowLayout {
                spacing: 16
                Image {
                    width: 48
                    height: 48
                    source: "qrc:/qml/assets/airwave_logo.svg"
                    sourceSize.width: 48
                    sourceSize.height: 48
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
                Column {
                    Text {
                        text: "AIRWAVE RADAR"
                        color: "#FFFFFF"
                        font.pixelSize: 22
                        font.bold: true
                        font.letterSpacing: 2
                    }
                    Text {
                        text: "Searching for mobile gaming devices over USB tethering and 5GHz Wi-Fi..."
                        color: "#8E929E"
                        font.pixelSize: 13
                    }
                }
            }

            // Radar Device Cards Grid
            Flow {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 20

                DeviceRadarCard {
                    deviceName: "OnePlus 11 5G (USB)"
                    connectionType: "USB 3.0 Tethered"
                    ipAddress: "192.168.42.129"
                    latencyMs: 38.2
                    onCastClicked: appWindow.isStageActive = true
                }

                DeviceRadarCard {
                    deviceName: "iPhone 15 Pro (AirPlay)"
                    connectionType: "5GHz Wi-Fi"
                    ipAddress: "192.168.1.145"
                    latencyMs: 58.7
                    onCastClicked: appWindow.isStageActive = true
                }
            }
        }
    }

    // 2. Stage Mirroring View
    Item {
        id: stageView
        anchors.fill: parent
        visible: appWindow.isStageActive

        // Video Frame Surface
        VideoRendererItem {
            id: videoSurface
            anchors.fill: parent
        }

        // Edge Hover Area for Auto-Hiding Slim Control Rail
        MouseArea {
            id: edgeHoverTrigger
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: controlRail.width + 60
            height: 90
            hoverEnabled: true

            StageControlRail {
                id: controlRail
                anchors.bottom: parent.bottom
                anchors.bottomMargin: edgeHoverTrigger.containsMouse ? 20 : -60
                anchors.horizontalCenter: parent.horizontalCenter
                hudVisible: stageHud.visible

                Behavior on anchors.bottomMargin {
                    NumberAnimation { duration: 160; easing.type: Easing.OutQuad }
                }

                onRotateClicked: videoSurface.rotation = (videoSurface.rotation + 90) % 360
                onHudToggled: stageHud.visible = !stageHud.visible
                onFullscreenClicked: toggleFullscreen()
            }
        }

        // Draggable Gaming Signal HUD Overlay
        Rectangle {
            id: stageHud
            width: 250
            height: 140
            radius: 12
            x: parent.width - width - 24
            y: 24
            color: "#E611131A"
            border.color: latencyTracker.glassToGlassMs < 60 ? "#B6FF3B" : (latencyTracker.glassToGlassMs < 100 ? "#19E3FF" : "#FF6B35")
            border.width: 1

            Drag.active: hudMouseArea.drag.active
            Drag.target: stageHud

            MouseArea {
                id: hudMouseArea
                anchors.fill: parent
                drag.target: stageHud
                drag.minimumX: 0
                drag.maximumX: appWindow.width - stageHud.width
                drag.minimumY: 0
                drag.maximumY: appWindow.height - stageHud.height

                Column {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 6

                    Row {
                        spacing: 8
                        Text { text: "LATENCY:"; color: "#8E929E"; font.pixelSize: 11; font.bold: true }
                        Text {
                            text: latencyTracker.glassToGlassMs.toFixed(1) + " ms"
                            color: stageHud.border.color
                            font.pixelSize: 13
                            font.bold: true
                        }
                    }

                    Row {
                        spacing: 8
                        Text { text: "FPS:"; color: "#8E929E"; font.pixelSize: 11; font.bold: true }
                        Text { text: latencyTracker.fps.toFixed(0); color: "#19E3FF"; font.pixelSize: 13; font.bold: true }
                    }

                    // Live Waveform stability line
                    WaveformVisualizer {
                        width: parent.width - 28
                        height: 38
                        waveColor: stageHud.border.color
                    }
                }
            }
        }
    }
}
