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

using namespace std;
using namespace cv;

extern "C" {
    void JNICALL
    Java_com_example_testapp_CvCameraViewListener_identifyChessboard(
            JNIEnv *env,jobject instance,jlong matAddr,
            jint boardWidth, jint boardHeight) {

        vector<Point2f> corners;

        Mat &frame = *(Mat *) matAddr;

        Size patternSize(boardWidth,boardHeight);

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        bool patternFound = findChessboardCorners(gray, patternSize, corners,
                CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

        if(patternFound) {
            cornerSubPix(gray, corners, Size(11, 11),
                         Size(-1, -1),
                         TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
        }
        drawChessboardCorners(frame, patternSize, Mat(corners), patternFound);
    }
}