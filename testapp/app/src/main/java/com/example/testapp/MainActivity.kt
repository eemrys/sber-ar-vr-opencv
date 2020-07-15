package com.example.testapp

import android.Manifest
import android.content.pm.PackageManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.SurfaceView
import android.view.WindowManager
import android.widget.Toast
import androidx.core.app.ActivityCompat
import kotlinx.android.synthetic.main.activity_main.*
import org.opencv.android.OpenCVLoader

private const val TAG = "MainActivity"
private const val CAMERA_PERMISSION_REQUEST = 1

class MainActivity : AppCompatActivity(R.layout.activity_main) {

    private val camera by lazy {
        CvCameraViewListener
    }

    init {
        initOpenCv()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        ActivityCompat.requestPermissions(
            this,
            arrayOf(Manifest.permission.CAMERA),
            CAMERA_PERMISSION_REQUEST
        )

        setOnClickListeners()

        main_surface.apply {
            visibility = SurfaceView.VISIBLE
            setCvCameraViewListener(camera)
        }

        btnClear.isEnabled = false
    }

    override fun onResume() {
        super.onResume()
        main_surface.enableView()
    }

    override fun onPause() {
        super.onPause()
        main_surface.disableView()
    }

    override fun onDestroy() {
        super.onDestroy()
        main_surface.disableView()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray) {
        when (requestCode) {
            CAMERA_PERMISSION_REQUEST -> {
                val isPermissionGranted = grantResults.isNotEmpty() &&
                        (grantResults[0] == PackageManager.PERMISSION_GRANTED)
                if (isPermissionGranted) {
                    main_surface.setCameraPermissionGranted()
                } else {
                    val message = "Camera permission was not granted"
                    Log.e(TAG, message)
                    Toast.makeText(this, message, Toast.LENGTH_LONG).show()
                }
            }
            else -> {
                Log.e(TAG, "Unexpected permission request")
            }
        }
    }

    private fun initOpenCv(){
        val isLoadOpenCVSuccess = OpenCVLoader.initDebug()
        if(isLoadOpenCVSuccess) initMyNativeLib()
        Log.d(TAG, "isLoadOpenCVSuccess: $isLoadOpenCVSuccess")
    }

    private fun initMyNativeLib(){
        System.loadLibrary("native-lib")
    }

    private fun setOnClickListeners() {
        btnResults.setOnClickListener {
            main_surface.disableView()
            updateData()
            it.isEnabled = false
            btnClear.isEnabled = true
        }
        btnClear.setOnClickListener {
            main_surface.enableView()
            updateData()
            it.isEnabled = false
            btnResults.isEnabled = true
        }
    }

    private fun updateData() {
        camera.cameraInfo.apply {
            txtvMatrixRes.text = matrix
            txtvDistRes.text = dist
            txtvRvecsRes.text = rvecs
            txtvTvecsRes.text = tvecs
        }
    }
}