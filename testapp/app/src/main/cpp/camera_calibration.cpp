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

std::vector<double> CameraCalibration::detect_aruco_marker(cv::Mat& frame, const cv::Mat& matrix, const cv::Mat& dist) {
    double distance_marker = 0;
    double distance_surface = 0;
    float marker_length = 0.05f;

    std::vector<int> marker_ids;
    std::vector<std::vector<cv::Point2f> > marker_corners;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

    cvtColor(frame, frame, cv::COLOR_RGBA2BGR);

    cv::aruco::detectMarkers(frame, dictionary, marker_corners, marker_ids, parameters);

    if (!marker_ids.empty()) {
        cv::aruco::drawDetectedMarkers(frame, marker_corners, marker_ids);

        std::vector<cv::Vec3d> r_vecs, t_vecs;
        cv::aruco::estimatePoseSingleMarkers(marker_corners, marker_length, matrix, dist, r_vecs, t_vecs);
        cv::aruco::drawAxis(frame, matrix, dist, r_vecs[0], t_vecs[0], marker_length * 2.f);

        distance_marker = norm(t_vecs[0]);

        cv::Mat rotation_matrix;
        Rodrigues(r_vecs[0], rotation_matrix);
        cv::Mat camera_translation_vector = -rotation_matrix.t()*t_vecs[0];

        distance_surface = camera_translation_vector.at<double>(0,2);
    }

    cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    std::vector<double> results {distance_marker, distance_surface};
    return results;
}