#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching.hpp>

#include "custom_stitcher.hpp"

cv::Mat ThreeImagesStitcher::surf_detector(const cv::Mat& first_image, const cv::Mat& second_image) {
    const int minHessian = 400;
    const float ratio_thresh = 0.7f;
    
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;
    
    cv::Ptr<cv::xfeatures2d::SURF> surf_detector = cv::xfeatures2d::SURF::create( minHessian );
    surf_detector->detectAndCompute( first_image, cv::noArray(), keypoints1, descriptors1 );
    surf_detector->detectAndCompute( second_image, cv::noArray(), keypoints2, descriptors2 );
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<cv::DMatch> > knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);
   
    //-- Filter matches using the Lowe's ratio test

    std::vector<cv::DMatch> good_matches;
    for (size_t i = 0; i < knn_matches.size(); i++)
    {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance)
        {
            good_matches.push_back(knn_matches[i][0]);
        }
    }
    
    std::vector<cv::Point2f> points1, points2;
    
    for (size_t i = 0; i < good_matches.size(); i++) {
        points1.push_back( keypoints1[ good_matches[i].queryIdx ].pt );
        points2.push_back( keypoints2[ good_matches[i].trainIdx ].pt );
    }

    return cv::findHomography( points1, points2, cv::RANSAC );
}

cv::Mat ThreeImagesStitcher::stitch_left(const cv::Mat& left, const cv::Mat& right) {
    cv::Mat image_stitch, homography_matrix;
    homography_matrix = surf_detector(left, right);
    cv::Mat A = cv::Mat::eye(3,3,CV_64F);
    A.at<double>(0,2) += left.cols;
    cv::Mat F = A * homography_matrix;
    warpPerspective(left, image_stitch, F, cv::Size(left.cols + right.cols, left.rows));
    cv::Mat half = image_stitch(cv::Rect(image_stitch.cols - right.cols, 0, right.cols, right.rows));
    right.copyTo(half);
    return image_stitch;
}

cv::Mat ThreeImagesStitcher::stitch_right(const cv::Mat& left, const cv::Mat& right) {
    cv::Mat image_stitch, homography_matrix;
    homography_matrix = surf_detector(right, left);
    warpPerspective(right, image_stitch, homography_matrix, cv::Size(right.cols + left.cols, right.rows));
    cv::Mat half = image_stitch(cv::Rect(0, 0, left.cols, left.rows));
    left.copyTo(half);
    return image_stitch;
}


void ThreeImagesStitcher::stitch(const cv::Mat& image_left, const cv::Mat& image_middle, const cv::Mat& image_right) {
    cv::Mat left_to_right, right_to_left;
    left_to_right = stitch_right(image_left, image_middle);
    right_to_left = stitch_right(left_to_right(cv::Rect(0, 0, left_to_right.cols*0.9, left_to_right.rows)), image_right);
    cv::imshow("result_left", right_to_left);
    cv::waitKey(0);
}
