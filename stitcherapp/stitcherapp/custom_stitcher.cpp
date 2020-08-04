#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "custom_stitcher.hpp"

cv::Mat1d ThreeImagesStitcher::get_homography(const cv::Mat3b& first_image, const cv::Mat3b& second_image) {
   
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat1f descriptors1, descriptors2;
    std::vector<cv::DMatch> matches;
    
    switch (detector_type) {
        // orb
        case 1: {
            cv::Mat1b descriptors1_orb, descriptors2_orb;
            cv::Ptr<cv::ORB> orb_detector = cv::ORB::create();
            orb_detector->detectAndCompute(first_image, cv::Mat3b(), keypoints1, descriptors1_orb);
            orb_detector->detectAndCompute(second_image, cv::Mat3b(), keypoints2, descriptors2_orb);
            // different matcher for ORB
            // "crossCheck = true" is the equivalent of the symmetric test below
            cv::Ptr<cv::BFMatcher> bfmatcher = cv::BFMatcher::create(cv::NORM_HAMMING2, true);
            bfmatcher->match(descriptors1_orb, descriptors2_orb, matches, cv::Mat());
            break;
        }
        // surf
        case 2: {
            cv::Ptr<cv::xfeatures2d::SURF> surf_detector = cv::xfeatures2d::SURF::create();
            surf_detector->detectAndCompute(first_image, cv::Mat3b(), keypoints1, descriptors1);
            surf_detector->detectAndCompute(second_image, cv::Mat3b(), keypoints2, descriptors2);
            break;
        }
        // sift
        case 3: {
            cv::Ptr<cv::SIFT> sift_detector = cv::SIFT::create();
            sift_detector->detectAndCompute(first_image, cv::Mat3b(), keypoints1, descriptors1);
            sift_detector->detectAndCompute(second_image, cv::Mat3b(), keypoints2, descriptors2);
            
            // alternative matcher for SIFT (gives slightly better results in mode 2)
            // cv::Ptr<cv::BFMatcher> bfmatcher = cv::BFMatcher::create(cv::NORM_L2, true);
            // bfmatcher->match(descriptors1, descriptors2, matches, cv::Mat());
            break;
        }
    }
    
    // next steps for SIFT and SURF
    if (detector_type != 1) {
        std::vector<std::vector<cv::DMatch> > knn_matches_1, knn_matches_2;
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
        
        // matching in two directions
        matcher->knnMatch(descriptors1, descriptors2, knn_matches_1, 2);
        matcher->knnMatch(descriptors2, descriptors1, knn_matches_2, 2);

        // filter each set of matches using the Lowe's ratio test
        const float ratio_thresh = 0.7f;
        std::vector<cv::DMatch> good_matches_1, good_matches_2;
        
        for (size_t i = 0; i < knn_matches_1.size(); i++)
        {
            if (knn_matches_1[i][0].distance < ratio_thresh * knn_matches_1[i][1].distance)
            {
                good_matches_1.push_back(knn_matches_1[i][0]);
            }
        }
        
        for (size_t i = 0; i < knn_matches_2.size(); i++)
        {
            if (knn_matches_2[i][0].distance < ratio_thresh * knn_matches_2[i][1].distance)
            {
                good_matches_2.push_back(knn_matches_2[i][0]);
            }
        }
        
        // symmetric test
        for (int i = 0; i < good_matches_1.size(); i++)
        {
            for (int j = 0; j < good_matches_2.size(); j++)
            {
                // if for i-th query descriptor the j-th descriptor in the matcher's collection is the nearest and vice versa
                if (good_matches_1[i].queryIdx == good_matches_2[j].trainIdx && good_matches_2[j].queryIdx == good_matches_1[i].trainIdx)
                {
                    // then we are keeping the match
                    matches.push_back(cv::DMatch(good_matches_1[i].queryIdx, good_matches_1[i].trainIdx, good_matches_1[i].distance));
                    break;
                }
            }
        }
    }
    
    std::vector<cv::Point2f> points1, points2;
    for (size_t i = 0; i < matches.size(); i++) {
        points1.push_back(keypoints1[matches[i].queryIdx].pt);
        points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }
    
    return cv::findHomography(points1, points2, cv::RANSAC);
}

cv::Mat3b ThreeImagesStitcher::stitch_left(const cv::Mat3b& left, const cv::Mat3b& right) {
    cv::Mat1d move_into_frame = cv::Mat::eye(3,3,CV_64F);
    // move image to the right, otherwise it's partially out of frame
    move_into_frame.at<double>(0,2) += left.cols;
    
    const cv::Mat1d homography_matrix = get_homography(left, right);
    const cv::Mat1d translation_matrix = move_into_frame * homography_matrix;

    cv::Mat3b image_stitch;
    warpPerspective(left, image_stitch, translation_matrix, cv::Size(left.cols + right.cols, left.rows));
    cv::Mat3b half = image_stitch(cv::Rect(image_stitch.cols - right.cols, 0, right.cols, right.rows));
    right.copyTo(half);
    
    return image_stitch;
}

cv::Mat3b ThreeImagesStitcher::stitch_right(const cv::Mat3b& left, const cv::Mat3b& right, const bool side_image) {
    const cv::Mat1d homography_matrix = get_homography(left, right);
    
    int width;
    //expand frame to show complete image if needed
    if (side_image) {
        width = right.cols * 2;
    } else {
        width = right.cols;
    }
    
    cv::Mat3b image_stitch;
    warpPerspective(right, image_stitch, homography_matrix.inv(), cv::Size(width + left.cols, right.rows));
    cv::Mat3b half = image_stitch(cv::Rect(0, 0, left.cols, left.rows));
    left.copyTo(half);
    
    return image_stitch;
}


cv::Mat3b ThreeImagesStitcher::stitch(const cv::Mat3b& image_left,
                                 const cv::Mat3b& image_middle,
                                 const cv::Mat3b& image_right,
                                 const int mode) {
    
    cv::Mat3b first_stitch, second_stitch;
    switch (mode) {
        // 1) stitch left, 2) stitch left
        case 1: {
            first_stitch = stitch_left(image_middle, image_right);
            second_stitch = stitch_left(image_left, first_stitch);
            break;
        }
        // 1) stitch left, 2) stitch right
        case 2: {
            first_stitch = stitch_left(image_left, image_middle);
            second_stitch = stitch_right(first_stitch, image_right, true);
            break;
        }
        // 1) stitch right, 2) stitch right
        case 3: {
            first_stitch = stitch_right(image_left, image_middle, false);
            cv::Rect clean_cut = cv::Rect(0, 0, first_stitch.cols*0.9, first_stitch.rows);
            second_stitch = stitch_right(first_stitch(clean_cut), image_right, true);
            break;
        }
    }
    return second_stitch;
}
