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

#include "camera_calibration.h"

using namespace std;
using namespace cv;
using namespace cameracalibration;

extern "C" JNIEXPORT jint JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_identifyChessboard(
        JNIEnv *env, jobject instance, jlong mat_addr, jboolean mode_take_snapshot) {

    Mat& frame = *(Mat *) mat_addr;
    return identify_chessboard(frame, reinterpret_cast<bool&>(mode_take_snapshot));
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_setSizes(
        JNIEnv *env, jobject instance, jlong mat_addr,
        jint board_width, jint board_height, jint passed_square_size) {

    Mat& frame = *(Mat *) mat_addr;
    Size passed_board_size(board_width, board_height);
    set_sizes(passed_board_size, frame.size(), passed_square_size);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_calibrate(
        JNIEnv *env,jobject instance, jlong matrix_addr, jlong dist_addr){

    Mat& matrix = *(Mat *) matrix_addr;
    Mat& dist = *(Mat *) dist_addr;

    vector<Mat> results = calibrate();

    matrix = results[0];
    dist = results[1];
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_example_testapp_screenundistort_UndistortViewListener_detectArucoMarker(
        JNIEnv *env, jobject instance, jlong mat_addr, jlong matrix_addr, jlong dist_addr) {

    Mat& frame = *(Mat *) mat_addr;
    Mat& matrix = *(Mat *) matrix_addr;
    Mat& dist = *(Mat *) dist_addr;

    return detect_aruco_marker(frame, matrix, dist);
}