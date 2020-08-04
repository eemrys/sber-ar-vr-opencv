# Stitching Images With OpenCV

1. Introduction to feature detection
2. Detectors
3. Matchers
4. Warping and stitching
5. Results

# Introduction to feature detection

In many Computer Vision applications, we often need to identify interesting stable points in an image. These points are called keypoints or feature points.
Features are specific patterns in an image which are unique, and can be easily tracked and compared.

For example, to solve a jigsaw puzzle, we search for these features in an image, find them, look for the same features in other images and align them. Finding these image features is called **Feature Detection**. Once you have found it, you should be able to find the same in the other images. To do that, we take a region around the feature and describe it.

Similarly, a computer should also describe the region around the feature so that it can find it in other images. So-called description is called **Feature Description**. Once you have the features and their descriptions, you can find same features in all images and align them, or, like we will do in this program, stitch them together.

The algorithm of matching the features is pretty straightforward: 
1. We use a queryImage and find some feature points in it.
2. We take another trainImage, find the features in that image too.
3. We find the best matches among them. 
In short, we found locations of some parts of an object in another cluttered image. This information is sufficient to find the object exactly on the trainImage. 

OpenCV provides a module called "Feature Detection and Description" as part of their 2D Features Framework. There are several keypoint detectors implemented in OpenCV ( e.g. SIFT, SURF, and ORB).

# Detectors

**SIFT** and **SURF** are classical feature descriptors that are usually compared and matched using the Euclidean distance (or L2-norm).

1. **SIFT** is a class for extracting keypoints and computing descriptors using the Scale Invariant Feature Transform (SIFT) algorithm by D. Lowe. We need to construct a SIFT object using ```create()``` function. We could pass different parameters to it, but it's optional, and for now we will stick to the default parameters:
    ```cpp
    cv::Ptr<cv::SIFT> sift_detector = cv::SIFT::create();
    ```
  
2. **SURF** is a class for extracting Speeded Up Robust Features from an image. As the name suggests, it is a speeded-up version of SIFT. OpenCV provides SURF functionalities just like SIFT. You initiate a SURF object with a ```create()``` function and some optional conditions, which we will skip in this case as well:
    ```cpp
    cv::Ptr<cv::xfeatures2d::SURF> surf_detector = cv::xfeatures2d::SURF::create();
    ```
  
3. **ORB** is a binary descriptor that is matched using the Hamming distance. This distance is equivalent to count the number of different elements for binary strings (population count after applying a XOR operation). As usual, we have to create an ORB object:
    ```cpp
    cv::Ptr<cv::ORB> orb_detector = cv::ORB::create();
    ```

Now that we have a detector object, we can directly find keypoints and descriptors in a single step with the function ```detectAndCompute()```.
Each keypoint is a special structure which has many attributes like its (x,y) coordinates, size of the meaningful neighbourhood, angle which specifies its orientation, etc:
```cpp
std::vector<cv::KeyPoint> keypoints1, keypoints2;
```
In SURF and SIFT, descriptors computed for keypoints are stored in a matrix of floating point values:
```cpp
cv::Mat1f descriptors1, descriptors2;
```
In ORB, however, they are stored in ```cv::Mat1b```:
```cpp
cv::Mat1b descriptors1_orb, descriptors2_orb;
```
You can also pass a mask as a second parameter if you want to search only a part of image (which we don't). 
```cpp
sift_detector->detectAndCompute(first_image, cv::Mat3b(), keypoints1, descriptors1);
sift_detector->detectAndCompute(second_image, cv::Mat3b(), keypoints2, descriptors2);
```
Next, we want to match keypoints in different images.

# Matchers

Matchers of keypoint descriptors in OpenCV have wrappers with a common interface which enables you to easily switch between different algorithms solving the same problem. We are going to use a Brute-force descriptor matcher for ORB and a Flann-based matcher for SIFT and SURF.

1. **Brute-force matcher**

    For each descriptor in the first set, this matcher finds the closest descriptor in the second set by trying each one. This descriptor matcher supports masking permissible matches of descriptor sets.
    We will be using the hamming distance as a measure of similarity between two feature descriptors.
    First we create a BFMatcher object with distance measurement ```cv.NORM_HAMMING``` or ```cv.NORM_HAMMING2``` (you can try both and see which one gives the best result) and ```crossCheck``` is switched on for better results.
    If ```crossCheck``` is true, the matcher will only return pairs (i,j) such that for i-th query descriptor the j-th descriptor in the matcher's collection is the nearest and vice versa, i.e. the BFMatcher will only return consistent pairs.
    ```cpp
    cv::Ptr<cv::BFMatcher> bfmatcher = cv::BFMatcher::create(cv::NORM_HAMMING2, true);
    ```
    Finally, we use the ```match()``` method to get the best matches in two images.
    ```cpp
    bfmatcher->match(descriptors1_orb, descriptors2_orb, matches, cv::Mat());
    ```
    The function returns a vector of ```DMatch``` objects:
    ```cpp
    std::vector<cv::DMatch> matches;
    ```
  
    The ```DMatch``` object has the following attributes:
    ```DMatch.distance``` - Distance between descriptors. The lower, the better it is.
    ```DMatch.trainIdx``` - Index of the descriptor in train descriptors
    ```DMatch.queryIdx``` - Index of the descriptor in query descriptors
    ```DMatch.imgIdx``` - Index of the train image.

2. **FLANN based Matcher**

    FLANN stands for Fast Library for Approximate Nearest Neighbors. It contains a collection of algorithms optimized for fast nearest neighbor search in large datasets and for high dimensional features. It works faster than BFMatcher for large datasets.
    Let'c create a matcher object:
    ```cpp
    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    ```
    This time, we will use ```knnMatch()``` to get k best matches. In this example, we will take k=2 so that we can apply ratio test explained by D.Lowe in his paper. We will do matching in two directions, which is the equivalent of ```crossCheck = true``` in BFMatcher:
    ```cpp
    matcher->knnMatch(descriptors1, descriptors2, knn_matches_1, 2);
    matcher->knnMatch(descriptors2, descriptors1, knn_matches_2, 2);
    ```
    Bacuse the function calculates two best batches, it returns a vector of vectors of ```DMatch```:
    ```cpp
    std::vector<std::vector<cv::DMatch> > knn_matches_1, knn_matches_2;
    ```
    Now that we have our matches, let's apply the ratio test to filter each set of values. As mentioned before, we keep the 2 best matches for each keypoint (best matches = the ones with the smallest distance measurement). Lowe's test checks that these two distances are sufficiently different. If they are not, then the keypoint is eliminated and will not be used for further calculations.
    ```cpp
    const float ratio_thresh = 0.7f;
    std::vector<cv::DMatch> good_matches_1;
    for (size_t i = 0; i < knn_matches_1.size(); i++)
    {
      if (knn_matches_1[i][0].distance < ratio_thresh * knn_matches_1[i][1].distance)
      {
        good_matches_1.push_back(knn_matches_1[i][0]);
      }
    }
    ```
    Next, we apply a symmetric test to return consistent pairs of matches from both sets:
    ```cpp
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
    ```
    
Finally, let's extract the location of the matches (this applies to all detectors):
```cpp
std::vector<cv::Point2f> points1, points2;
for (size_t i = 0; i < matches.size(); i++) {
  points1.push_back(keypoints1[matches[i].queryIdx].pt);
  points2.push_back(keypoints2[matches[i].trainIdx].pt);
}   
```

# Warping and stitching

Now that we have the location of the matches, we can use a function ```findHomography()```. If we pass the set of points from both images, it will find the perpective transformation of that object (a homography). Once we get this 3x3 transformation matrix, we use it to transform the corners of queryImage to corresponding points in trainImage.

**What is homography?**

Two images of a scene are related by a homography under two conditions.
1. The two images are that of a plane (e.g. sheet of paper, credit card etc.).
2. The two images were acquired by rotating the camera about its optical axis. We take such images while generating panoramas.

A homography is a 3×3 matrix as shown below:

![image](https://www.learnopencv.com/wp-content/ql-cache/quicklatex.com-0e501a99a732aad3f7a911294b942aa0_l3.png)

Let ```(x1,y1)``` be a point in the first image and ```(x2,y2)``` be the coordinates of the same physical point in the second image. Then, the Homography ```H``` relates them in the following way:

![image](https://www.learnopencv.com/wp-content/ql-cache/quicklatex.com-5ed4910ec273ad30626f55eefe5b0373_l3.png)

If we knew the homography, we could apply it to all the pixels of one image to obtain a warped image that is aligned with the second image.

A homography can be computed when we have 4 or more corresponding points in two images. However, feature matching that we did in the previous section does not always produce 100% accurate matches (it is not uncommon for 20-30% of the matches to be incorrect). Fortunately, the ```findHomography()``` method utilizes a robust estimation technique called Random Sample Consensus (RANSAC) which produces the right result even in the presence of large number of bad matches:
```cpp
return cv::findHomography(points1, points2, cv::RANSAC);
```
The function returns a matrix of double precision values (```Mat1d```).

Once an accurate homography has been calculated, the transformation can be applied to all pixels in one image to map it to the other image. This is done using the ```warpPerspective()``` function in OpenCV.

Because we want to stitch three images together in this program, we want to be able to stitch the left image to the right one, AND vice versa. Let's start by warping and stiching the left image.

Here is the original left image:

![image](https://i.ibb.co/QJDpRvR/image1.jpg)

And the right image:

![image](https://i.ibb.co/7X1bXPR/image2.jpg)

```cpp
// resulting image
cv::Mat3b image_stitch;

warpPerspective(left, image_stitch, translation_matrix, cv::Size(left.cols + right.cols, left.rows));
```
The second parameter is the resulting image, the last parameter is its size.

This is the result we get:

![image](https://i.ibb.co/NK9zPN6/translation-matrix-1.jpg)

As you can see the big part of the image is out of frame. To fix this, we will modify the transformation matrix to add translation values:
```cpp
// original homography matrix
const cv::Mat1d homography_matrix = get_homography(left, right);
// new matrix for translation
cv::Mat1d move_into_frame = cv::Mat::eye(3,3,CV_64F);
// move image to the right, otherwise it's partially out of frame
move_into_frame.at<double>(0,2) += left.cols;
// resulting matrix
const cv::Mat1d translation_matrix = move_into_frame * homography_matrix;
```
Now the resulting image looks like this:

![image](https://i.ibb.co/3h6sByn/translation-matrix-2.jpg)

Finally, let's copy the right (unwarped) image over to the left one, and put it in the right spot:
```cpp
cv::Mat3b half = image_stitch(cv::Rect(image_stitch.cols - right.cols, 0, right.cols, right.rows));
right.copyTo(half);
```
The result:

![image]()

Now let's try to warp and stitch the right image to the left one.

Here is the original left image:

![image](https://i.ibb.co/QJDpRvR/image2.jpg)

And the right image:

![image](https://i.ibb.co/bWCw2c2/image3.jpg)

As we're transforming the trainImage, and not the queryImage, we need to use the inverse of the homography matrix:
```cpp
cv::Mat3b image_stitch;
warpPerspective(right, image_stitch, homography_matrix.inv(), cv::Size(width + left.cols, right.rows));
```

Again we see the same problem - a small part of image is out of frame:

![image](https://i.ibb.co/Xx7ZN74/translation-matrix-3.jpg)

Fortunately, because this image is supposed to be on right, we can simple make its frame bigger to fit the rest:
```cpp
int width;
//expand frame to show complete image if it's on the far right side
if (side_image) {
  width = right.cols * 2;
} else {
  width = right.cols;
}
```
Now this is the result:

![image](https://i.ibb.co/GHb9pjn/translation-matrix-4.jpg)

And lastly, let's copy the left (unwarped) image over to the right one, and put it in the right spot:
```cpp
cv::Mat3b half = image_stitch(cv::Rect(0, 0, left.cols, left.rows));
left.copyTo(half);
```

The result:

![image]()

# Results

Finally, to combine all three images, we just call these two methods in the right order.
First we stitch the left image to the middle one:
```cpp
first_stitch = stitch_left(image_left, image_middle);
```
And then we stitch the right image to the result of the previous operation:
```cpp
second_stitch = stitch_right(first_stitch, image_right, true);
```

Here are the resulting images we got using different detectors. 

**ORB:**

![image](https://i.ibb.co/18nsySq/orb.jpg)

**SIFT:**

![image](https://i.ibb.co/NpVLLBh/sift.jpg)

**SURF:**

![image](https://i.ibb.co/RCJy2P0/surf.jpg)

