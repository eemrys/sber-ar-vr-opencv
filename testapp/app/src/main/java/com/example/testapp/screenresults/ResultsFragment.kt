package com.example.testapp.screenresults

import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment
import androidx.navigation.NavOptions
import androidx.navigation.fragment.findNavController
import com.example.testapp.R
import com.example.testapp.models.CameraInfo
import com.example.testapp.screenundistort.UndistortFragmentArgs
import kotlinx.android.synthetic.main.fragment_results.*
import org.opencv.core.Mat

class ResultsFragment : Fragment(R.layout.fragment_results) {

    private val arguments by lazy {
        ResultsFragmentArgs.fromBundle(requireArguments()).results
    }

    private val navOptions by lazy {
        NavOptions.Builder().setEnterAnim(R.anim.slide_in_left)
            .setPopEnterAnim(R.anim.slide_in_left).build()
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        setOnClick()

        postData(arguments)
    }

    private fun setOnClick() {
        btnUndistort.setOnClickListener {
            navigateToUndistort(arguments)
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
        val args = UndistortFragmentArgs.Builder(data).build().toBundle()
        findNavController().navigate(R.id.fragmentUndistort, args, navOptions)
    }
}