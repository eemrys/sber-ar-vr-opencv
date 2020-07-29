#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, const char * argv[]) {
    cv::Mat image;
    image = cv::imread("septofbaelor.png");
    cv::imshow( "Display window", image );
    cv::waitKey(0);
    return 0;
}
