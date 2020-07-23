package com.example.testapp.screenundistort

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.SurfaceView
import android.view.View
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.lifecycle.Observer
import com.example.testapp.R
import kotlinx.android.synthetic.main.fragment_undistort.*

private const val CAMERA_PERMISSION_REQUEST = 1

class UndistortFragment : Fragment(R.layout.fragment_undistort) {

    private val camera by lazy {
        UndistortViewListener
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        undistort_surface.apply {
            visibility = SurfaceView.VISIBLE
            setCvCameraViewListener(camera)
        }

        val arguments = UndistortFragmentArgs.fromBundle(requireArguments()).data
        camera.cameraInfo = arguments

        requestPermissions(arrayOf(Manifest.permission.CAMERA), CAMERA_PERMISSION_REQUEST)

        addObserver()
    }

    override fun onResume() {
        super.onResume()
        undistort_surface?.enableView()
    }

    override fun onPause() {
        super.onPause()
        undistort_surface?.disableView()
    }

    override fun onDestroy() {
        super.onDestroy()
        undistort_surface?.disableView()
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
                    undistort_surface.setCameraPermissionGranted()
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

    private fun addObserver() {
        camera.distance.observe(viewLifecycleOwner, Observer {
            if (it > 0) {
                txtvDistance.text = it.toString()
            }
        })
    }
}