package com.example.testapp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.WindowManager
import org.opencv.android.OpenCVLoader

class MainActivity : AppCompatActivity(R.layout.activity_main) {

    init {
        initOpenCv()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
    }

    private fun initOpenCv() {
        val isLoadOpenCVSuccess = OpenCVLoader.initDebug()
        if(isLoadOpenCVSuccess) initMyNativeLib()
    }

    private fun initMyNativeLib() {
        System.loadLibrary("native-lib")
    }
}