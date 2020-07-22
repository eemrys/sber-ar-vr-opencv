package com.example.testapp.screencamera

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import com.example.testapp.models.CameraInfo
import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.*

object CvCameraViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    private const val boardWidth = 10
    private const val boardHeight = 6
    private const val squareSize = 50
    private var sizesSet = false
    private var modeTakeSnapshot = false

    private val mutableImagePointsCount = MutableLiveData<Int>()
    val imagePointsCount: LiveData<Int>
        get() = mutableImagePointsCount

    override fun onCameraViewStarted(width: Int, height: Int) {}

    override fun onCameraViewStopped() {}

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()

        if (!sizesSet) {
            setSizes(frame.nativeObjAddr, boardWidth, boardHeight, squareSize)
            sizesSet = true
        }

        mutableImagePointsCount.postValue(identifyChessboard(frame.nativeObjAddr, modeTakeSnapshot))
        modeTakeSnapshot = false

        return frame
    }

    fun takeSnapshot() {
        modeTakeSnapshot = true
    }

    fun calibrateCamera(): CameraInfo {

        val matrixMat = Mat()
        val distMat = Mat()

        calibrate(matrixMat.nativeObjAddr, distMat.nativeObjAddr)

        return CameraInfo(
            matrixMat.nativeObjAddr,
            distMat.nativeObjAddr,
            matrixMat.dump(),
            distMat.dump())
    }

    private external fun identifyChessboard(matAddr: Long, modeTakeSnapshot: Boolean): Int
    private external fun setSizes(matAddr: Long, boardWidth: Int, boardHeight: Int, squareSize: Int)
    private external fun calibrate(matrixAddr: Long, distAddr: Long)
}