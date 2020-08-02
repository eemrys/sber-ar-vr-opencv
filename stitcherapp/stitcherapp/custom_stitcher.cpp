#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/stitching.hpp>

#include "custom_stitcher.hpp"

cv::Mat1d ThreeImagesStitcher::surf_detector(const cv::Mat3b& first_image, const cv::Mat3b& second_image) {
   
    const int min_hessian = 400;
    cv::Ptr<cv::xfeatures2d::SURF> surf_detector = cv::xfeatures2d::SURF::create(min_hessian);
    
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat1f descriptors1, descriptors2;
    surf_detector->detectAndCompute(first_image, cv::Mat3b(), keypoints1, descriptors1);
    surf_detector->detectAndCompute(second_image, cv::Mat3b(), keypoints2, descriptors2);
    
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector<std::vector<cv::DMatch> > knn_matches;
    matcher->knnMatch(descriptors1, descriptors2, knn_matches, 2);
   
    // filter matches using the Lowe's ratio test
    // todo : two-way filer
    const float ratio_thresh = 0.7f;
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
        points1.push_back(keypoints1[good_matches[i].queryIdx].pt);
        points2.push_back(keypoints2[good_matches[i].trainIdx].pt);
    }

    return cv::findHomography(points1, points2, cv::RANSAC);
}

cv::Mat3b ThreeImagesStitcher::stitch_left(const cv::Mat3b& left, const cv::Mat3b& right) {
    cv::Mat1d move_into_frame = cv::Mat::eye(3,3,CV_64F);
    // move image to the right (into frame)
    move_into_frame.at<double>(0,2) += left.cols;
    
    const cv::Mat1d homography_matrix = surf_detector(left, right);
    const cv::Mat1d translation_matrix = move_into_frame * homography_matrix;

    cv::Mat3b image_stitch;
    warpPerspective(left, image_stitch, translation_matrix, cv::Size(left.cols + right.cols, left.rows));
    cv::Mat3b half = image_stitch(cv::Rect(image_stitch.cols - right.cols, 0, right.cols, right.rows));
    right.copyTo(half);
    
    return image_stitch;
}

cv::Mat3b ThreeImagesStitcher::stitch_right(const cv::Mat3b& left, const cv::Mat3b& right, const bool side_image) {
    const cv::Mat1d homography_matrix = surf_detector(right, left);
    
    int width;
    //expand frame to show complete image if needed
    if (side_image) {
        width = right.cols * 2;
    } else {
        width = right.cols;
    }
    
    cv::Mat3b image_stitch;
    warpPerspective(right, image_stitch, homography_matrix, cv::Size(width + left.cols, right.rows));
    cv::Mat3b half = image_stitch(cv::Rect(0, 0, left.cols, left.rows));
    left.copyTo(half);
    
    return image_stitch;
}


void ThreeImagesStitcher::stitch(const cv::Mat3b& image_left, const cv::Mat3b& image_middle, const cv::Mat3b& image_right) {
    
    cv::Mat3b first_stitch = stitch_left(image_middle, image_right);
    //cv::Rect clean_cut = cv::Rect(0, 0, first_stitch.cols*0.9, first_stitch.rows);
    cv::Mat3b second_stitch = stitch_left(image_left, first_stitch);
    cv::imwrite("resulting_image.jpg", second_stitch);
}
