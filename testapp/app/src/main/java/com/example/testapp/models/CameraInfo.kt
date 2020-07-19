package com.example.testapp.models

import android.os.Parcelable
import kotlinx.android.parcel.Parcelize

@Parcelize data class CameraInfo(
    val matrix: Long,
    val dist: Long,
    val matDump: String,
    val distDump: String) : Parcelable