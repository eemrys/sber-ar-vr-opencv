#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "custom_stitcher.hpp"

cv::Mat GetSquareImage(const cv::Mat& img)
{
    int width = img.cols,
       height = img.rows;

    int target_width = 500;
    
    cv::Mat square = cv::Mat::zeros( target_width, target_width, img.type() );

    int max_dim = ( width >= height ) ? width : height;
    float scale = ( ( float ) target_width ) / max_dim;
    cv::Rect roi;
    if ( width >= height )
    {
        roi.width = target_width;
        roi.x = 0;
        roi.height = height * scale;
        roi.y = ( target_width - roi.height ) / 2;
    }
    else
    {
        roi.y = 0;
        roi.height = target_width;
        roi.width = width * scale;
        roi.x = ( target_width - roi.width ) / 2;
    }

    cv::resize( img, square( roi ), roi.size() );

    return square;
}

int main(int argc, char **argv) {
    cv::Mat left = GetSquareImage(cv::imread("image1.jpg"));
    cv::Mat middle = GetSquareImage(cv::imread("image2.jpg"));
    cv::Mat right = GetSquareImage(cv::imread("image3.jpg"));
    ThreeImagesStitcher stitcher = ThreeImagesStitcher();
    stitcher.stitch(left, middle, right);
    return 0;
}
