package com.example.testapp

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.View
import kotlinx.android.synthetic.main.fragment_results.*
import org.opencv.core.Mat

class ResultsFragment : Fragment(R.layout.fragment_results) {

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val arguments = ResultsFragmentArgs.fromBundle(requireArguments())
        postData(arguments.results)
    }

    private fun postData(cameraInfo: CameraInfo) {
        cameraInfo.apply {
            val matrixMat = Mat(matrix)
            val distMat = Mat(dist)
            txtvMatrixRes.text = matrixMat.dump()
            txtvDistRes.text = distMat.dump()
        }
    }
}