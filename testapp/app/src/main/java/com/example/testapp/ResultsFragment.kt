package com.example.testapp

import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.View
import androidx.core.os.bundleOf
import androidx.navigation.NavOptions
import androidx.navigation.fragment.findNavController
import kotlinx.android.synthetic.main.fragment_results.*
import org.opencv.core.Mat

class ResultsFragment : Fragment(R.layout.fragment_results) {

    private val arguments by lazy {
        ResultsFragmentArgs.fromBundle(requireArguments())
    }

    private val navOptions by lazy {
        NavOptions.Builder().setEnterAnim(R.anim.slide_in_left)
            .setPopEnterAnim(R.anim.slide_in_left).build()
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        setOnClick()

        postData(arguments.results)
    }

    private fun setOnClick() {
        btnUndistort.setOnClickListener {
            navigateToUndistort(arguments.results)
        }
    }

    private fun postData(data: CameraInfo) {
        data.apply {
            val matrixMat = Mat(matrix)
            val distMat = Mat(dist)
            txtvMatrixRes.text = matrixMat.dump()
            txtvDistRes.text = distMat.dump()
        }
    }

    private fun navigateToUndistort(data: CameraInfo) {
        val bundle = bundleOf("data" to data)
        findNavController().navigate(R.id.fragmentUndistort, bundle, navOptions)
    }
}