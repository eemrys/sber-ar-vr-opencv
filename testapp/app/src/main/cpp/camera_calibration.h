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

class CameraCalibration {

private:
    cv::Size board_size;
    cv::Size image_size;
    int square_size;
    std::vector<std::vector<cv::Point2f> > image_points;
public:
    CameraCalibration():
            board_size(cv::Size()),
            image_size(cv::Size()),
            square_size(0),
            image_points(std::vector<std::vector<cv::Point2f> >())
            {};
    void set_sizes(const cv::Size& board, const cv::Size& image, const int square);
    int identify_chessboard(cv::Mat& frame, const bool mode_take_snapshot);
    void calc_board_corner_positions(std::vector<cv::Point3f>& obj);
    std::vector<cv::Mat> calibrate();
    static void undistort_image(cv::Mat& frame, const cv::Mat& matrix, const cv::Mat& dist);
};

#endif //TESTAPP_CAMERA_CALIBRATION_H