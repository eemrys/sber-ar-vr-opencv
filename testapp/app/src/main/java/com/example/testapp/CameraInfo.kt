package com.example.testapp

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize
import org.opencv.core.Mat

data class CameraInfo(
    var matrix: Mat = Mat(),
    var dist: Mat = Mat())

@Parcelize data class CameraInfoString(
    var matrix: String,
    var dist: String) : Parcelable