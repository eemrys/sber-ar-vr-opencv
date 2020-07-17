#ifndef TESTAPP_CAMERA_CALIBRATION_H
#define TESTAPP_CAMERA_CALIBRATION_H


#include <opencv2/core/types.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

namespace CameraCalibration {

    Size _boardSize = Size();

    Size _imageSize = Size();

    int _squareSize = 0;

    vector<vector<Point2f>> _imagePoints = vector<vector<Point2f>>();

    void setSizes(const Size& boardSize, const Size& imageSize, int& squareSize);

    void identifyChessboard(Mat& frame, bool& modeTakeSnapshot);

    void calcBoardCornerPositions(vector<Point3f>& obj);

    vector<Mat> calibrate();

    void undistortImage(Mat& frame, Mat& matrix, Mat& dist);
}


#endif //TESTAPP_CAMERA_CALIBRATION_H