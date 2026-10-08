import QtQuick
import Airwave 1.0

Item {
    id: root
    width: 240
    height: 60

    property var latencySamples: [38, 41, 39, 42, 40, 39, 45, 41, 39, 38, 40, 42, 39, 41, 40, 39]
    property color waveColor: "#B6FF3B"

    function pushLatency(val) {
        var copy = latencySamples.slice(1);
        copy.push(val);
        latencySamples = copy;
        canvas.requestPaint();
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        antialiasing: true

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            ctx.lineWidth = 2;
            ctx.strokeStyle = root.waveColor;
            ctx.lineJoin = "round";

            ctx.beginPath();
            var step = width / (root.latencySamples.length - 1);
            for (var i = 0; i < root.latencySamples.length; i++) {
                var val = root.latencySamples[i];
                // Map latency 0-100ms into height
                var y = height - (val / 100.0) * height;
                if (i === 0) {
                    ctx.moveTo(0, y);
                } else {
                    ctx.lineTo(i * step, y);
                }
            }
            ctx.stroke();

            // Glow gradient under line
            ctx.lineTo(width, height);
            ctx.lineTo(0, height);
            ctx.closePath();
            var grad = ctx.createLinearGradient(0, 0, 0, height);
            grad.addColorStop(0, Qt.rgba(root.waveColor.r, root.waveColor.g, root.waveColor.b, 0.25));
            grad.addColorStop(1, Qt.rgba(root.waveColor.r, root.waveColor.g, root.waveColor.b, 0.0));
            ctx.fillStyle = grad;
            ctx.fill();
        }
    }
}
