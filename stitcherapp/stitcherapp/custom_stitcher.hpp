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
    cv::Mat surf_detector(const cv::Mat& first_image, const cv::Mat& second_image);
    cv::Mat stitch_left(const cv::Mat& left, const cv::Mat& right);
    cv::Mat stitch_right(const cv::Mat& left, const cv::Mat& right);
public:
    ThreeImagesStitcher() {};
    void stitch(const cv::Mat& image_left, const cv::Mat& image_middle, const cv::Mat& image_right);
};

#endif
