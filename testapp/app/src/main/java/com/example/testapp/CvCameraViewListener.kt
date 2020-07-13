package com.example.testapp

import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Mat

object CvCameraViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    override fun onCameraViewStarted(width: Int, height: Int) {
    }

    override fun onCameraViewStopped() {
    }

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.gray()
        adaptiveThresholdFromJNI(frame.nativeObjAddr)
        return frame
    }

    private external fun adaptiveThresholdFromJNI(matAddr: Long)
}