package com.example.testapp.screenundistort

import com.example.testapp.models.CameraInfo
import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Mat

class UndistortViewListener(private val cameraInfo: CameraInfo) : CameraBridgeViewBase.CvCameraViewListener2 {

    override fun onCameraViewStarted(width: Int, height: Int) {}

    override fun onCameraViewStopped() {}

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()

        cameraInfo.apply {
            val matrixMat = Mat(matrix)
            val distMat = Mat(dist)
        }

        return frame
    }

    //private external fun calibrate(matrixAddr: Long, distAddr: Long)
}