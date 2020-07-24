package com.example.testapp.screenundistort

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.example.testapp.models.CameraInfo
import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Mat

object UndistortViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    var cameraInfo: CameraInfo? = null

    private val mutableDistance = MutableLiveData<List<Double>>()
    val distance: LiveData<List<Double>>
        get() = mutableDistance

    override fun onCameraViewStarted(width: Int, height: Int) {}

    override fun onCameraViewStopped() {}

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()
        val distanceMarker = 0.0
        val distanceSurface = 0.0

        cameraInfo?.apply {
            detectArucoMarker(frame.nativeObjAddr, matrix, dist, distanceMarker, distanceSurface)
        }

        mutableDistance.value = listOf(distanceMarker, distanceSurface)
        return frame
    }

    private external fun detectArucoMarker(frameAddr: Long,
                                           matrixAddr: Long,
                                           distAddr: Long,
                                           distanceMarker: Double,
                                           distanceSurface: Double)
}