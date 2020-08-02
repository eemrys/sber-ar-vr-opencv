#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "custom_stitcher.hpp"

// copying images onto a canvas with bigger height
// (this way we can see the edges of the images after stitching)
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
    //todo: enums
    const int target_height = 800,
    stitching_mode = 1,
    detector = 0;
    const cv::Mat3b left = put_on_canvas(cv::imread("image1.jpg"), target_height);
    const cv::Mat3b middle = put_on_canvas(cv::imread("image2.jpg"), target_height);
    const cv::Mat3b right = put_on_canvas(cv::imread("image3.jpg"), target_height);
    
    ThreeImagesStitcher stitcher = ThreeImagesStitcher(detector);
    stitcher.stitch(left, middle, right, stitching_mode);
    
    return 0;
}
