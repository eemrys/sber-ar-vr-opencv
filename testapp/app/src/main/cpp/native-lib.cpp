#include <jni.h>
#include <android/log.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "camera-calibration.h"

using namespace std;
using namespace cv;


extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_CvCameraViewListener_identifyChessboard(
            JNIEnv *env,jobject instance,jlong matAddr) {

        Mat &frame = *(Mat *) matAddr;
        CameraCalibration::identifyChessboard(frame);
    }

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_CvCameraViewListener_setSizes(
        JNIEnv *env,jobject instance,jlong matAddr,
        jint boardWidth, jint boardHeight, jint squareSize) {

    Mat &frame = *(Mat *) matAddr;
    Size boardSize(boardWidth, boardHeight);
    CameraCalibration::setSizes(boardSize, frame.size(), squareSize);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_CvCameraViewListener_calibrate(
        JNIEnv *env,jobject instance, jlong matrix_addr, jlong dist_addr){

    Mat &matrix = *(Mat *) matrix_addr;
    Mat &dist = *(Mat *) dist_addr;

    vector<Mat> results = CameraCalibration::calibrate();

    matrix = results[0];
    dist = results[1];
}