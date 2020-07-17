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

class CameraCalibration {

private:
    inline static Size _boardSize = Size();
    inline static Size _imageSize = Size();
    inline static int _squareSize = 0;
    inline static vector<vector<Point2f>> _imagePoints = vector<vector<Point2f>>();
public:
    static void setSizes(const Size& boardSize, const Size& imageSize, int& squareSize);

    static void calcBoardCornerPositions(vector<Point3f>& obj);

    static void identifyChessboard(Mat& frame, bool& modeTakeSnapshot);

    static vector<Mat> calibrate();
};


#endif //TESTAPP_CAMERA_CALIBRATION_H
