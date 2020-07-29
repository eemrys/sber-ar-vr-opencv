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
#include <opencv2/aruco.hpp>

using namespace cv;
using namespace std;

class CameraCalibration {
private:
    Size board_size;
    Size image_size;
    int square_size;
    vector<vector<Point2f>> image_points;
public:
    CameraCalibration();
    void set_sizes(const Size& board, const Size& image, int square);
    int identify_chessboard(Mat& frame, bool mode_take_snapshot);
    void calc_board_corner_positions(vector<Point3f>& obj);
    vector<Mat> calibrate();
    static vector<double> detect_aruco_marker(Mat& frame, const Mat& matrix, const Mat& dist);
};


#endif //TESTAPP_CAMERA_CALIBRATION_H