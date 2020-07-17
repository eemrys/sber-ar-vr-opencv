package com.example.testapp

import android.os.Bundle
import android.view.SurfaceView
import android.view.View
import androidx.fragment.app.Fragment
import kotlinx.android.synthetic.main.fragment_undistort.*

class UndistortFragment : Fragment(R.layout.fragment_undistort) {

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        undistort_surface.apply {
            visibility = SurfaceView.VISIBLE
            val arguments = UndistortFragmentArgs.fromBundle(requireArguments())
            setCvCameraViewListener(UndistortViewListener(arguments.data))
        }
    }

    override fun onResume() {
        super.onResume()
        undistort_surface.enableView()
    }

    override fun onPause() {
        super.onPause()
        undistort_surface.disableView()
    }

    override fun onDestroy() {
        super.onDestroy()
        undistort_surface.disableView()
    }
}