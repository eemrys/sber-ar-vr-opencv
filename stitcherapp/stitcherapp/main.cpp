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
    if (argc < 7) {
        std::cout << "not enough arguments\n";
        return 0;
    }
    try {
        std::string left_path = argv[1],
            middle_path = argv[2],
            right_path = argv[3],
            result_path = argv[4];
        const int detector = atoi(argv[5]),
            stitching_mode = atoi(argv[6]);
        
        if ((detector < 1) || (stitching_mode < 1) || (detector > 3) || (stitching_mode > 3)) {
            throw std::invalid_argument("invalid argument");
        }
        if ((detector == 1) || (stitching_mode == 1)) {
            throw std::invalid_argument("ORB detector doesn't support this mode, please choose mode 2 or 3.");
        }
        
        const cv::Mat3b left = cv::imread(left_path);
        const cv::Mat3b middle = cv::imread(middle_path);
        const cv::Mat3b right = cv::imread(right_path);
        
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
