#include "camera_calibration.h"

void CameraCalibration::set_sizes(const cv::Size& board, const cv::Size& image, const int square) {
    board_size = board;
    image_size = image;
    square_size = square;
}

int CameraCalibration::identify_chessboard(cv::Mat& frame, const bool mode_take_snapshot) {

    std::vector<cv::Point2f> corners;
    corners.clear();
    cv::Mat gray;
    cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    bool pattern_found = findChessboardCorners(gray, board_size, corners,
                                               cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

    if (pattern_found) {
        cornerSubPix(gray, corners, cv::Size(11, 11),
                     cv::Size(-1, -1),
                     cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
        if (mode_take_snapshot)
        {
            if (image_points.size() < 20) {
                image_points.push_back(corners);
            }
        }
    }
    drawChessboardCorners(frame, board_size, cv::Mat(corners), pattern_found);

    return image_points.size();
}

void CameraCalibration::calc_board_corner_positions(std::vector<cv::Point3f>& obj) {
    obj.clear();
    for (int i = 0; i < board_size.height; ++i)
        for (int j = 0; j < board_size.width; ++j)
            obj.emplace_back(j * square_size, i * square_size, 0);
}

std::vector<cv::Mat> CameraCalibration::calibrate() {
    float grid_width = (float)square_size * (board_size.width - 1.f);

    std::vector<std::vector<cv::Point3f> > object_points(1);
    calc_board_corner_positions(object_points[0]);
    object_points[0][board_size.width - 1].x = object_points[0][0].x + grid_width;
    object_points.resize(image_points.size(), object_points[0]);

    cv::Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    cv::Mat dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);

    std::vector<cv::Mat> r_vecs, t_vecs;
    calibrateCamera(object_points, image_points, image_size,
                    camera_matrix, dist_coeffs, r_vecs, t_vecs);

    std::vector<cv::Mat> results {camera_matrix, dist_coeffs};
    return results;
}

void CameraCalibration::undistort_image(cv::Mat& frame, const cv::Mat& matrix, const cv::Mat& dist) {
    cv::Mat temp = frame.clone();
    undistort(temp, frame, matrix, dist);
}