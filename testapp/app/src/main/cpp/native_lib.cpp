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

CameraCalibration camera_calibration = CameraCalibration();

extern "C" JNIEXPORT jint JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_identifyChessboard(
        JNIEnv *env, jobject instance, jlong mat_addr, jboolean mode_take_snapshot) {

    cv::Mat& frame = *(cv::Mat *) mat_addr;
    return camera_calibration.identify_chessboard(frame, reinterpret_cast<bool&>(mode_take_snapshot));
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_setSizes(
        JNIEnv *env, jobject instance, jlong mat_addr,
        jint board_width, jint board_height, jint passed_square_size) {

    cv::Mat& frame = *(cv::Mat *) mat_addr;
    cv::Size passed_board_size(board_width, board_height);
    camera_calibration.set_sizes(passed_board_size, frame.size(), passed_square_size);
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_calibrate(
        JNIEnv *env,jobject instance, jlong matrix_addr, jlong dist_addr){

    cv::Mat& matrix = *(cv::Mat *) matrix_addr;
    cv::Mat& dist = *(cv::Mat *) dist_addr;

    std::vector<cv::Mat> results = camera_calibration.calibrate();

    matrix = results[0];
    dist = results[1];
}

extern "C" JNIEXPORT jdoubleArray JNICALL Java_com_example_testapp_screenundistort_UndistortViewListener_detectArucoMarker(
        JNIEnv *env, jobject instance, jlong mat_addr, jlong matrix_addr, jlong dist_addr) {

    cv::Mat& frame = *(cv::Mat *) mat_addr;
    cv::Mat& matrix = *(cv::Mat *) matrix_addr;
    cv::Mat& dist = *(cv::Mat *) dist_addr;

    std::vector<double> results = CameraCalibration::detect_aruco_marker(frame, matrix, dist);

    jdoubleArray output = env->NewDoubleArray(results.size());
    env->SetDoubleArrayRegion(output, 0, results.size(), &results[0]);
    return output;
}