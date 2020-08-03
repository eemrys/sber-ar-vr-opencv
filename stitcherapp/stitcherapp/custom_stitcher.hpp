#ifndef custom_stitcher_hpp
#define custom_stitcher_hpp

#include <stdio.h>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

class ThreeImagesStitcher {
private:
    const int detector_type;
    cv::Mat1d get_homography(const cv::Mat3b& first_image, const cv::Mat3b& second_image);
    cv::Mat3b stitch_left(const cv::Mat3b& left, const cv::Mat3b& right);
    cv::Mat3b stitch_right(const cv::Mat3b& left, const cv::Mat3b& right, const bool side_image);
public:
    ThreeImagesStitcher(const int detector): detector_type(detector) {};
    cv::Mat3b stitch(const cv::Mat3b& image_left,
                const cv::Mat3b& image_middle,
                const cv::Mat3b& image_right,
                const int mode);
};

#endif
