package com.example.testapp

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.View
import kotlinx.android.synthetic.main.fragment_results.*

class ResultsFragment : Fragment(R.layout.fragment_results) {

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val arguments = ResultsFragmentArgs.fromBundle(requireArguments())
        postData(arguments.results)
    }

    private fun postData(cameraInfo: CameraInfoString) {
        cameraInfo.apply {
            txtvMatrixRes.text = matrix
            txtvDistRes.text = dist
        }
    }
}