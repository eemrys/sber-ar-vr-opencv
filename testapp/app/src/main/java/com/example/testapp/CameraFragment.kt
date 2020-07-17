package com.example.testapp

import android.os.Bundle
import android.view.*
import androidx.core.os.bundleOf
import androidx.fragment.app.Fragment
import androidx.navigation.NavOptions
import androidx.navigation.fragment.findNavController
import kotlinx.android.synthetic.main.fragment_camera.*

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

    private fun setOnClickListeners() {
        btnCalibrate.setOnClickListener {
            val results = camera.calibrateCamera()
            navigateToResults(results)
        }
        btnTakeSnapshot.setOnClickListener {
            camera.takeSnapshot()
        }
    }

    private fun navigateToResults(results: CameraInfo) {
        val bundle = bundleOf("results" to results)
        findNavController().navigate(R.id.fragmentResults, bundle, navOptions)
    }
}