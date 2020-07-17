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

using namespace CameraCalibration;
extern "C" JNIEXPORT jint JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_identifyChessboard(
            JNIEnv *env,jobject instance,jlong matAddr, jboolean mode_take_snapshot) {

    Mat &frame = *(Mat *) matAddr;
    return identifyChessboard(frame, reinterpret_cast<bool &>(mode_take_snapshot));
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_setSizes(
        JNIEnv *env,jobject instance,jlong matAddr,
        jint boardWidth, jint boardHeight, jint squareSize) {

    Mat &frame = *(Mat *) matAddr;
    Size boardSize(boardWidth, boardHeight);
    setSizes(boardSize, frame.size(), squareSize);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_calibrate(
        JNIEnv *env,jobject instance, jlong matrix_addr, jlong dist_addr){

    Mat &matrix = *(Mat *) matrix_addr;
    Mat &dist = *(Mat *) dist_addr;

    vector<Mat> results = calibrate();

    matrix = results[0];
    dist = results[1];
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screenundistort_UndistortViewListener_undistort(
        JNIEnv *env,jobject instance,jlong matAddr, jlong matrix_addr, jlong dist_addr) {

    Mat &frame = *(Mat *) matAddr;
    Mat &matrix = *(Mat *) matrix_addr;
    Mat &dist = *(Mat *) dist_addr;

    undistortImage(frame, matrix, dist);
}