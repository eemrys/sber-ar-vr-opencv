package com.example.testapp

import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.*

object CvCameraViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    private const val boardWidth = 10
    private const val boardHeight = 7
    private const val squareSize = 50
    private var sizesSet = false

    private var cameraInfo: CameraInfo = CameraInfo()

    override fun onCameraViewStarted(width: Int, height: Int) {}

    override fun onCameraViewStopped() {}

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()

        if (!sizesSet) {
            setSizes(frame.nativeObjAddr, boardWidth, boardHeight, squareSize)
            sizesSet = true
        }

        identifyChessboard(frame.nativeObjAddr)

        return frame
    }

    fun calibrateCamera(): CameraInfo {
        cameraInfo.apply {
            calibrate(matrix.nativeObjAddr, dist.nativeObjAddr)
        }
        return cameraInfo
    }

    private external fun identifyChessboard(matAddr: Long)
    private external fun setSizes(matAddr: Long, boardWidth: Int, boardHeight: Int, squareSize: Int)
    private external fun calibrate(matrixAddr: Long, distAddr: Long)
}