#include "camera-calibration.h"

void CameraCalibration::setSizes(const Size& boardSize, const Size& imageSize, int& squareSize) {
    _boardSize = boardSize;
    _imageSize = imageSize;
    _squareSize = squareSize;
}

int CameraCalibration::identifyChessboard(Mat &frame, bool &modeTakeSnapshot) {

    vector<Point2f> corners;
    corners.clear();
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    bool patternFound = findChessboardCorners(gray, _boardSize, corners,
                                              CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

    if(patternFound) {
        cornerSubPix(gray, corners, Size(11, 11),
                     Size(-1, -1),
                     TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
        if (modeTakeSnapshot)
        {
            if (_imagePoints.size() > 15) {
                _imagePoints.front() = move(_imagePoints.back());
                _imagePoints.pop_back();
            }
            _imagePoints.push_back(corners);
        }
    }
    drawChessboardCorners(frame, _boardSize, Mat(corners), patternFound);

    return _imagePoints.size();
}

void CameraCalibration::calcBoardCornerPositions(vector<Point3f> &obj) {
    obj.clear();
    for( int i = 0; i < _boardSize.height; ++i )
        for( int j = 0; j < _boardSize.width; ++j )
            obj.emplace_back(j*_squareSize, i*_squareSize, 0);
}

vector<Mat> CameraCalibration::calibrate() {
    float grid_width = (float)_squareSize * (_boardSize.width - 1.f);
    vector<vector<Point3f>> objectPoints(1);
    vector<Point3f> newObjPoints;
    calcBoardCornerPositions(objectPoints[0]);
    objectPoints[0][_boardSize.width - 1].x = objectPoints[0][0].x + grid_width;
    newObjPoints = objectPoints[0];
    objectPoints.resize(_imagePoints.size(),objectPoints[0]);

    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    Mat distortion = Mat::zeros(8, 1, CV_64F);

    vector<Mat> rvecs, tvecs;
    calibrateCamera(objectPoints, _imagePoints, _imageSize,
                            cameraMatrix, distortion, rvecs, tvecs);

    vector<Mat> results {cameraMatrix, distortion};
    return results;
}

void CameraCalibration::undistortImage(Mat& frame, Mat& matrix, Mat& dist) {
    Mat temp = frame.clone();
    undistort(temp, frame, matrix, dist);
}
