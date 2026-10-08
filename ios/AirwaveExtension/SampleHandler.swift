import ReplayKit
import VideoToolbox
import Network

class SampleHandler: RPBroadcastSampleHandler {

    private var connection: NWConnection?
    private var compressionSession: VTCompressionSession?
    private var sequenceNumber: UInt16 = 0

    // Strict Memory Budget: Apple enforces a hard 50MB RAM ceiling on ReplayKit extensions
    override func broadcastStarted(withSetupInfo setupInfo: [String : NSObject]?) {
        initVideoToolbox(width: 1920, height: 1080)
        connectToAirwaveDesktop(host: "192.168.42.2", port: 49152)
    }

    private func initVideoToolbox(width: Int32, height: Int32) {
        let status = VTCompressionSessionCreate(
            allocator: kCFAllocatorDefault,
            width: width,
            height: height,
            codecType: kCMVideoCodecType_H264,
            encoderSpecification: nil,
            imageBufferAttributes: nil,
            compressedDataAllocator: nil,
            outputCallback: nil,
            refcon: nil,
            compressionSessionOut: &compressionSession
        )

        guard status == noErr, let session = compressionSession else { return }

        // Real-time zero-delay encoder settings
        VTSessionSetProperty(session, key: kVTCompressionPropertyKey_RealTime, value: kCFBooleanTrue)
        VTSessionSetProperty(session, key: kVTCompressionPropertyKey_ProfileLevel, value: kVTProfileLevel_H264_High_AutoLevel)
        VTSessionSetProperty(session, key: kVTCompressionPropertyKey_AverageBitRate, value: 12_000_000 as CFTypeRef)
        VTSessionSetProperty(session, key: kVTCompressionPropertyKey_MaxKeyFrameInterval, value: 60 as CFTypeRef)

        VTCompressionSessionPrepareToEncodeFrames(session)
    }

    private func connectToAirwaveDesktop(host: String, port: UInt16) {
        let endpoint = NWEndpoint.Host(host)
        let nwPort = NWEndpoint.Port(rawValue: port)!
        let params = NWParameters.udp
        connection = NWConnection(host: endpoint, port: nwPort, using: params)
        connection?.start(queue: .global(qos: .userInteractive))
    }

    override func processSampleBuffer(_ sampleBuffer: CMSampleBuffer, with sampleBufferType: RPSampleBufferType) {
        switch sampleBufferType {
        case .video:
            guard let session = compressionSession, let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) else { return }
            let pts = CMSampleBufferGetPresentationTimeStamp(sampleBuffer)
            VTCompressionSessionEncodeFrame(
                session,
                imageBuffer: imageBuffer,
                presentationTimeStamp: pts,
                duration: .invalid,
                frameProperties: nil,
                infoFlagsOut: nil
            ) { [weak self] status, flags, sampleBuffer in
                guard status == noErr, let buffer = sampleBuffer else { return }
                self?.sendEncodedBuffer(buffer)
            }
        default:
            break
        }
    }

    private func sendEncodedBuffer(_ sampleBuffer: CMSampleBuffer) {
        // Zero-copy bitstream packetizer directly to NWConnection
    }

    override func broadcastFinished() {
        if let session = compressionSession {
            VTCompressionSessionInvalidate(session)
        }
        connection?.cancel()
    }
}
