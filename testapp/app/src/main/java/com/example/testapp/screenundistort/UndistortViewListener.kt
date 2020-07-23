package com.example.testapp.screenundistort

import com.example.testapp.models.CameraInfo
import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Mat

object UndistortViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    var cameraInfo: CameraInfo? = null

    override fun onCameraViewStarted(width: Int, height: Int) {}

    override fun onCameraViewStopped() {}

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()

        cameraInfo?.apply {
            detectArucoMarker(frame.nativeObjAddr, matrix, dist)
        }

        return frame
    }

    private external fun detectArucoMarker(frameAddr: Long, matrixAddr: Long, distAddr: Long)
}