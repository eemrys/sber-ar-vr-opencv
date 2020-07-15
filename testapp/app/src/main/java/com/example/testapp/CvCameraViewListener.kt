package com.example.testapp

import org.opencv.android.CameraBridgeViewBase
import org.opencv.calib3d.Calib3d
import org.opencv.core.*
import org.opencv.imgproc.Imgproc


object CvCameraViewListener : CameraBridgeViewBase.CvCameraViewListener2 {

    private val objectPoints = mutableListOf<Mat>()
    private val imagePoints = mutableListOf<Mat>()
    private const val boardWidth = 10.0
    private const val boardHeight = 7.0

    private var imageSize: Size? = null

    var cameraInfo = CameraInfo()

    override fun onCameraViewStarted(width: Int, height: Int) {
        cameraInfo = CameraInfo()
    }

    override fun onCameraViewStopped() {
        val matrixRes = Mat(3, 3, CvType.CV_32FC1)
        val distRes = Mat()
        val rvecsRes: List<Mat> = ArrayList()
        val tvecsRes: List<Mat> = ArrayList()
        imageSize?.apply {
            Calib3d.calibrateCamera(objectPoints, imagePoints, this,
                matrixRes, distRes, rvecsRes, tvecsRes
            )
        }
        cameraInfo.apply {
            matrix = matrixRes.toString()
            dist = distRes.toString()
            rvecs = rvecsRes.toString()
            tvecs = tvecsRes.toString()
        }
    }

    override fun onCameraFrame(inputFrame: CameraBridgeViewBase.CvCameraViewFrame): Mat {

        val frame = inputFrame.rgba()
        imageSize = frame.size()

        val boardSize = Size(boardWidth, boardHeight)
        val grayImage = Mat()
        val corners = MatOfPoint2f()

        Imgproc.cvtColor(frame, grayImage, Imgproc.COLOR_BGR2GRAY)

        val found = Calib3d.findChessboardCorners(
            grayImage, boardSize, corners,
            Calib3d.CALIB_CB_ADAPTIVE_THRESH + Calib3d.CALIB_CB_NORMALIZE_IMAGE + Calib3d.CALIB_CB_FAST_CHECK)

        if (found) {
            val term = TermCriteria(TermCriteria.EPS or TermCriteria.MAX_ITER, 30, 0.1)
            Imgproc.cornerSubPix(grayImage, corners, Size(11.0, 11.0),
                Size(-1.0, -1.0), term)

            Calib3d.drawChessboardCorners(frame, boardSize, corners, found)

            if (objectPoints.size < 15) {
                objectPoints.add(createObjPoints())
                imagePoints.add(corners)
            }
        }
        return frame
    }

    //private external fun identifyChessboard(matAddr: Long, boardWidth: Int, boardHeight: Int)

    private fun createObjPoints(): MatOfPoint3f {
        val obj = MatOfPoint3f()
        val numSquares = (boardWidth * boardHeight).toInt()
        for (j in 0 until numSquares) {
            obj.push_back(MatOfPoint3f(Point3((j / boardWidth), (j % boardHeight), 0.0)))
        }
        return obj
    }
}