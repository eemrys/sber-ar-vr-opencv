# Camera calibratioin | chessboard pattern

## Overview:
1. Camera calibration: basic **theory** and definitions
2. Using **OpenCV** functions in C++
3. Communicating between Kotlin and C++ via **JNI**
4. Results

## Camera calibration: basic theory and definitions

In many computer vision applications the camera is generally modeled with a set of intrinsic parameters (focal length, principal point, skew of axis) and its orientation is expressed by extrinsic parameters (rotation and translation). Camera calibration is the process of estimating these parameters.

The calibration of the camera is often necessary when the alignment between the lens and the optic sensors chip is not correct; the effect produced by this wrong alignment is usually more visible in low quality cameras.

Both intrinsic and extrinsic parameters are estimated using known points in the real world and their projections in the image plane. These points are generally presented as a calibration pattern with known geometry, usually a flat chessboard, which we will use in this app.

We will be using several functions from OpenCV library, which use a so-called pinhole camera mode. This model describes the mathematical relationship between the coordinates of a point in three-dimensional space and its projection onto the image plane of an ideal pinhole camera. Here is the model equation:

![model equation](https://docs.opencv.org/2.4/_images/math/803e522ec37bc5bc609c0ef08373a350a819fc15.png)

where
* (X, Y, Z) are the coordinates of a 3D point in the world coordinate space
* (u, v) are the coordinates of the projection point in pixels
* (cx, cy) is a principal point that is usually at the image center
* fx, fy are the focal lengths expressed in pixel units.

The following figure illustrates the pinhole camera model

![model illustration](https://docs.opencv.org/2.4/_images/pinhole_camera_model.png)

Real lenses usually have some distortion, mostly radial distortion and slight tangential distortion. So, the above model is extended as:

![model equation extended](https://docs.opencv.org/2.4/_images/math/331ebcd980b851f25de1979ebb67a2fed1c8477e.png)

k1, k2, k3, k4, k5, and k6 are radial distortion coefficients. p1 and p2 are tangential distortion coefficients. Higher-order coefficients are not considered in OpenCV.

## Using OpenCV functions in C++

The goal of this app is to determine
* a camera matrix, that includes the focal point and the principal point,
* distortion coefficients.

The general idea is to take several pictures of a chessboard from different angles and distances, process each image, calculate the camera matrix and distortion coefficients, and then apply this data to remove distortion from the camera.

We will use several OpenCV functions to do the following **to each image**:
1. As we're using the chessboard pattern in our app, we first need to identify this pattern on an image. To do this, we have to find the positions of internal corners of the chessboard using this function: 
   ```cpp

    bool patternFound = findChessboardCorners(gray, patternSize = _boardSize, corners,
                           CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
   ```
   where ```patternSize``` is the number of inner corners per a chessboard row and column (e.g. 10x6). There are also two operation flags present:
      * ```CALIB_CB_ADAPTIVE_THRESH``` uses adaptive thresholding to convert the image to black and white, rather than a fixed threshold level (computed from the average image brightness).
      * ```CALIB_CB_NORMALIZE_IMAGE``` normalizes the image gamma with equalizeHist() before applying fixed or adaptive thresholding.
      * ```CALIB_CB_FAST_CHECK Run``` a fast check on the image that looks for chessboard corners, and shortcut the call if none is found. This can drastically speed up the call in the degenerate condition when no chessboard is observed.

2. Once we find the corners, we can increase their accuracy by refining the corner locations using 
    ```cpp
    if(patternFound) {
        cornerSubPix(gray, corners, winsize = Size(11, 11),
                     zeroZone = Size(-1, -1),
                     criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));
    }
    ```
    where
    * ```patternFound``` is the return value of the previous function.
    * ```winSize``` – Half of the side length of the search window. For example, if ```winSize=Size(5,5)``` , then a 5*2+1 x 5*2+1 = 11 x 11 search window is used.
    * ```zeroZone``` – Half of the size of the dead region in the middle of the search zone over which the summation in the formula below is not done. It is used sometimes to avoid possible singularities of the autocorrelation matrix. The value of (-1,-1) indicates that there is no such a size.
    * ```criteria``` – Criteria for termination of the iterative process of corner refinement. So the process stops either after criteria.maxCount iterations or when the corner position moves by less than criteria.epsilon on some iteration.

3. Then, once the pattern is indentified, we want to draw them on top of the given image. To render the detected chessboard corners we use the function
    ```cpp
    drawChessboardCorners(frame, _boardSize, Mat(corners), patternFound);
    ```
4. Finally, for each recognized pattern we need to track:
    * some reference system’s 3D point where the chessboard is located (let’s assume that the Z axe is always 0):
    ```cpp
    for( int i = 0; i < _boardSize.height; ++i )
        for( int j = 0; j < _boardSize.width; ++j )
            obj.emplace_back(j*_squareSize, i*_squareSize, 0);
    ```
    * the image’s 2D points (output array of the function ```findChessboardCorners```)
    ```cpp
     _imagePoints.push_back(corners);
     ```
    So we need to save the array of chessboard corners on each iteration to ```_imageCorners```, array and array of 3D points to ```objectPoints```. Because the array of 3D points is the same on each iteration, we are basically creating the ```objectPoints``` array once (using the loop above), and then resize it to fit the ```_imagePoint``` array, as they are supposed to be the same size.
    ```cpp
    objectPoints[0][_boardSize.width - 1].x = objectPoints[0][0].x + grid_width;
    objectPoints.resize(_imagePoints.size(),objectPoints[0]);
    ```
    
So now we have our object points and image points we are ready perform calibration. For that we use the function **double calibrateCamera(InputArrayOfArrays objectPoints, InputArrayOfArrays imagePoints, Size imageSize, InputOutputArray cameraMatrix, InputOutputArray distCoeffs, OutputArrayOfArrays rvecs, OutputArrayOfArrays tvecs, int flags=0, TermCriteria criteria=TermCriteria( TermCriteria::COUNT+TermCriteria::EPS, 30, DBL_EPSILON))**, which saves camera matrix and distortion coefficients to provided arrays. It also returns rotation ```rvecs``` and translation ```tvecs``` vectors, as well as the final re-projection error, but we won't really need those values. We won't pass any flags to this function, and also use a standart termination criteria for the iterative optimization algorithm.

Finally, now that we got camera's matrix with the distortion coefficients we want to correct the image using undistort function, which transforms an image to compensate radial and tangential lens distortion. **void undistort(InputArray src, OutputArray dst, InputArray cameraMatrix, InputArray distCoeffs, InputArray newCameraMatrix=noArray() )**, where
    * ```src``` – Input (distorted) image
    * ```dst``` – Output (corrected) image that has the same size and type as src
    * ```newCameraMatrix``` – Camera matrix of the distorted image. By default, it is the same as cameraMatrix but you may additionally scale and shift the result by using a different matrix. We will leave the default value here.
