package com.example.testapp

import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Mat

object CvCameraViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    var cameraInfo: CameraInfo? = null

    override fun onCameraViewStarted(width: Int, height: Int) {
        cameraInfo = null
    }

    override fun onCameraViewStopped() {
        cameraInfo = CameraInfo("matrix", "dist", "rvecs", "tvecs")
    }

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()
        //adaptiveThresholdFromJNI(frame.nativeObjAddr)
        return frame
    }

    private external fun adaptiveThresholdFromJNI(matAddr: Long)
}