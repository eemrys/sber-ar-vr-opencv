package com.example.testapp.models

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize

@Parcelize data class CameraInfo(
    var matrix: Long,
    var dist: Long) : Parcelable