#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>
#include <algorithm>

#include "custom_stitcher.hpp"

// copying images onto a canvas with bigger height
// (this way we can see more of the edges of the images after stitching)
cv::Mat3b put_on_canvas(const cv::Mat3b& img, const int target_height)
{
    const int width = img.cols,
    height = img.rows;
        
    cv::Mat3b canvas = cv::Mat::zeros(target_height, width, img.type());
    cv::Rect roi = cv::Rect(0, (target_height - height) / 2, width, height);
    cv::resize(img, canvas(roi), roi.size());

    return canvas;
}

int main(int argc, char **argv) {
    cv::String keys =
        "{@image1   |<none>| left image path}"
        "{@image2   |<none>| middle image path}"
        "{@image3   |<none>| right image path}"
        "{@result   |<none>| resulting image path}"
        "{@detector |2  | detector: 1 - ORB, 2 - SURF, 3 - SIFT}"
        "{@mode     |2  | stitching mode: 1 - to left, 2 - to middle, 3 - to right}"
        "{help ?    |   | print this message}";

    cv::CommandLineParser parser(argc, argv, keys);
    if (parser.has("help")) {
        parser.printMessage();
        return 0;
    }

    const cv::String left_path = parser.get<cv::String>(0),
        middle_path = parser.get<cv::String>(1),
        right_path = parser.get<cv::String>(2),
        result_path = parser.get<cv::String>(3);
    
    const int detector = parser.get<int>(4),
        stitching_mode = parser.get<int>(5);

    if (!parser.check()) {
        parser.printErrors();
        return -1;
    }
    
    try {
        if ((detector < 1) || (stitching_mode < 1) || (detector > 3) || (stitching_mode > 3)) {
            throw std::invalid_argument("invalid argument");
        }
        if ((detector == 1) && (stitching_mode == 1)) {
            throw std::invalid_argument("ORB detector doesn't support this mode, please choose mode 2 or 3.");
        }
        
        const cv::Mat3b left = cv::imread(left_path),
            middle = cv::imread(middle_path),
            right = cv::imread(right_path);
        
        const int target_height = std::max(std::max(left.rows, middle.rows), right.rows) * 2;
        
        ThreeImagesStitcher stitcher = ThreeImagesStitcher(detector);
        cv::Mat3b result = stitcher.stitch(put_on_canvas(left, target_height),
                                           put_on_canvas(middle, target_height),
                                           put_on_canvas(right, target_height), stitching_mode);
        
        cv::imwrite(result_path, result);
    }
    
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    
    return 0;
}
