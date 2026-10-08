pragma Singleton
import QtQuick

QtObject {
    // Backgrounds & Surface Tones
    readonly property color bgBase: "#07080C"       // OLED-black base
    readonly property color panelSurface: "#11131A" // Dark gaming panel surface
    readonly property color panelHover: "#181B24"   // Subtle interactive surface highlight
    readonly property color borderDim: "#222634"    // Inactive thin border

    // Neon Accents
    readonly property color electricViolet: "#7C5CFF" // Primary gaming violet
    readonly property color plasmaCyan: "#19E3FF"    // High-visibility status cyan
    readonly property color successLime: "#B6FF3B"   // Low latency (<60ms) / Online indicator
    readonly property color alertOrange: "#FF6B35"   // High latency (>100ms) / Warning indicator
    readonly property color textPrimary: "#FFFFFF"
    readonly property color textSecondary: "#8E929E"

    // Geometry & Motion
    readonly property int cornerRadius: 12
    readonly property int animDurationFast: 120
    readonly property int animDurationNormal: 180

    // Font Family Names
    readonly property string fontDisplay: "Rajdhani, Orbitron, sans-serif"
    readonly property string fontBody: "Inter, Roboto, sans-serif"
}
