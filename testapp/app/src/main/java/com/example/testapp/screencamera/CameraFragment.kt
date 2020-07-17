package com.example.testapp.screencamera

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.*
import android.widget.Toast
import androidx.core.os.bundleOf
import androidx.fragment.app.Fragment
import androidx.navigation.NavOptions
import androidx.navigation.fragment.findNavController
import com.example.testapp.models.CameraInfo
import com.example.testapp.R
import kotlinx.android.synthetic.main.fragment_camera.*

private const val CAMERA_PERMISSION_REQUEST = 1

class CameraFragment : Fragment(R.layout.fragment_camera) {

    private val camera by lazy {
        CvCameraViewListener
    }

    private val navOptions by lazy {
        NavOptions.Builder().setEnterAnim(R.anim.slide_in_left)
            .setPopEnterAnim(R.anim.slide_in_left).build()
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        setOnClickListeners()

        main_surface.apply {
            visibility = SurfaceView.VISIBLE
            setCvCameraViewListener(camera)
        }
        requestPermissions(arrayOf(Manifest.permission.CAMERA),
            CAMERA_PERMISSION_REQUEST
        )
    }

    override fun onResume() {
        super.onResume()
        main_surface?.enableView()
    }

    override fun onPause() {
        super.onPause()
        main_surface?.disableView()
    }

    override fun onDestroy() {
        super.onDestroy()
        main_surface?.disableView()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode,permissions,grantResults)
        when (requestCode) {
            CAMERA_PERMISSION_REQUEST -> {
                val isPermissionGranted = grantResults.isNotEmpty() &&
                        (grantResults[0] == PackageManager.PERMISSION_GRANTED)
                if (isPermissionGranted) {
                    main_surface.setCameraPermissionGranted()
                } else {
                    val message = "Camera permission was not granted"
                    Toast.makeText(context, message, Toast.LENGTH_LONG).show()
                }
            }
            else -> {
                val message = "Unexpected permission request"
                Toast.makeText(context, message, Toast.LENGTH_LONG).show()
            }
        }
    }

    private fun setOnClickListeners() {
        btnCalibrate.setOnClickListener {
            val results =
                CvCameraViewListener.calibrateCamera()
            navigateToResults(results)
        }
        btnTakeSnapshot.setOnClickListener {
            CvCameraViewListener.takeSnapshot()
        }
    }

    private fun navigateToResults(results: CameraInfo) {
        val bundle = bundleOf("results" to results)
        findNavController().navigate(R.id.fragmentResults, bundle, navOptions)
    }
}