# Camera calibratioin | chessboard pattern

## Overview:
1. Camera calibration: basic **theory** and definitions
2. Using **OpenCV** functions in C++
3. Communicating between Kotlin and C++ via **JNI**
4. Java camera portrait mode orientation fix
5. Results

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
    
So now we have our object points and image points we are ready perform calibration. For that we use the function
   ```cpp
   calibrateCamera(objectPoints, _imagePoints, _imageSize,
                            cameraMatrix, distortion, rvecs, tvecs);
   ```
which saves camera matrix and distortion coefficients to provided arrays:
   ```cpp
   Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
   Mat distortion = Mat::zeros(8, 1, CV_64F);
   ```
It also returns rotation ```rvecs``` and translation ```tvecs``` vectors, but we won't really need those values.

Finally, now that we got camera's matrix with the distortion coefficients we want to correct the image using undistort function, which transforms an image to compensate radial and tangential lens distortion. 
   ```cpp
   Mat temp = frame.clone();
   undistort(temp, frame, matrix, dist);
   ```
where
* ```temp``` – a copy of input (distorted) image
* ```frame``` – output (corrected) image that has the same size and type as ```temp```

## Communicating between Kotlin and C++ via JNI

Now we need to connect all these functions, that are located in our .cpp file, to our UI logic, written in Kotlin. Here is when we need JNI, the Java Native Interface. It defines a way for the bytecode that Android compiles from managed code (written in the Java or Kotlin programming languages) to interact with native code (written in C/C++).

First we create a listener object for the camera by implementing ```CameraBridgeViewBase.CvCameraViewListener2``` interface. The ```onCameraFrame``` method receives and returns each frame caputred by the camera; here we can proccess and modify each frame before returning it.
We then need to declare an enternal function, which will refer to our native code in C++, for example:
```cpp
private external fun identifyChessboard(matAddr: Long, modeTakeSnapshot: Boolean): Int
```
The compiler will look for a function with a specific name using this template:
```
Java_packageName_className_funName()
```
So in a .cpp file we define this function like this:
```cpp
extern "C" JNIEXPORT jint JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_identifyChessboard(
            JNIEnv *env,jobject instance,jlong matAddr, jboolean mode_take_snapshot) {

    Mat &frame = *(Mat *) matAddr;
    // call to function in another .cpp file
    return identifyChessboard(frame, reinterpret_cast<bool &>(mode_take_snapshot));
}
```
Explanation:
* In order for the JNI to locate out native functions automatically, they have to match the expected function signatures (the template mentioned above) exactly. C++ function names get mangled by the compiler (to support overloading and other things) -- unless you specify extern "C". If you forget the extern declaration, JNI will be unable to find the function 
* JNIEXPORT is used to make native functions appear in the dynamic table of the built binary (\*.so file). If these functions are not in the dynamic table, JNI will not be able to find the functions to call them so the RegisterNatives call will fail at runtime.
* JNICALL contains any compiler directives required to ensure that the given function is treated with the proper calling convention.

Now we can pass data between the CvCameraViewListener object and native functions in C++.
It's probably best to create a separate .cpp file for JNI functions, and keep all OpenCV logic elsewhere. In this app, we have a camera-calibration.cpp and .h files that contain all our functions, and also a native-lib.cpp file that only communicates with CvCameraViewListener:
```cpp
#include "camera-calibration.h"
```
After we included the header file, we can call these functions from our native-lib file.

```cpp
extern "C" JNIEXPORT jint JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_identifyChessboard(
            JNIEnv *env,jobject instance,jlong matAddr, jboolean mode_take_snapshot) {
    // call to a function from camera-calibration.h
}

extern "C" JNIEXPORT void JNICALL Java_com_example_testapp_screencamera_CvCameraViewListener_setSizes(
        JNIEnv *env,jobject instance,jlong matAddr,
        jint boardWidth, jint boardHeight, jint squareSize) {
    // call to a function from camera-calibration.h
}
...

```
This keeps our file relatively short and easier to understand.

## Java camera portrait mode orientation fix

OpenCV’s camera doesn’t handle a mobile device’s portrait mode well by default. To fix this, we need to modify the ```CameraBridgeViewBase.java``` file. There's a function called ```deliverAndDrawFrame```, that takes the camera frame, converts it to a bitmap, and renders that bitmap to an Android canvas on the screen. So before that function gets called, we need to modify the matrix into which all of those pixels get drawn. First we create our matrix:
```cpp
private final Matrix mMatrix = new Matrix();
```
We want the matrix to be updated based on various events, so we add a couple of override methods:
```cpp
 @Override
    public void layout(int l, int t, int r, int b) {
        super.layout(l, t, r, b);
        updateMatrix();
    }

 @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        updateMatrix();
    }
```
These call update the matrix when the screen is laid out, and then call to measure for screen dimension changes.
Finally let's define a function that will update the matrix:
```cpp
private void updateMatrix() {
   float hw = this.getWidth() / 2.0f; // getting basic measurements of the screen
   float hh = this.getHeight() / 2.0f;

   boolean isFrontCamera = mCameraIndex == CAMERA_ID_FRONT;
```
We want to reset the matrix from whatever manipulations occurred in the previous frame.
```cpp
   mMatrix.reset();
```
   Let's also mirror the front facing camera image:
```cpp
   if (isFrontCamera) {
      mMatrix.preScale(-1, 1, hw, hh); // this will mirror the camera
   }
```
If we were to rotate right now, OpenCV would use the top left corner of the image as its rotation point, which would send the camera image off the screen on the device. So let’s move it to the center:
```cpp
   mMatrix.preTranslate(hw, hh);
```
   Then we can rotate it, and the angle will depend on whether it's the front or rear camera.
```cpp
   if (isFrontCamera){
      mMatrix.preRotate(270);
   } else {
      mMatrix.preRotate(90);
   }
   ```
And we can now move the matrix back:
```cpp
   mMatrix.preTranslate(-hw, -hh);
}
```
For our matrix to be used, we need to put this code inside the ```deliverAndDrawFrame``` method, before it draws the bitmap:
```cpp

canvas.drawColor(0, android.graphics.PorterDuff.Mode.CLEAR);
// ....original code

int saveCount = canvas.save();
canvas.setMatrix(mMatrix); // using our matrix
```
Now we just need to scale it to fill the width of the screen:
```cpp
mScale = Math.max((float) canvas.getHeight() / mCacheBitmap.getWidth(), (float) canvas.getWidth() / mCacheBitmap.getHeight());
```
Finally, we need to restore the canvas after bitmap is drawn:
```cpp
// original code
if (mScale != 0) {
      ... 
} else { 
      ... 
}

// Restore canvas after drawing bitmap
canvas.restoreToCount(saveCount);

```
Note -- this will only work for portrait mode.

## Results

The app was tested using Android device emulator. In the first run we set emulator's rear camera to Virtual Simutation mode. As it is in portrait mode, the rotation modifications from the previous step work as expected.

### Virtual simulation test-run

Identifying the pattern:

![1-chessboard](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/virtual-simulation-testrun/1-chessboard.png)
![2-chessboard](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/virtual-simulation-testrun/2-chessboard.png)
![3-chessboard](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/virtual-simulation-testrun/3-chessboard.png)
![4-chessboard](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/virtual-simulation-testrun/4-chessboard.png)
![5-chessboard](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/virtual-simulation-testrun/5-chessboard.png)

Intrinsic parameters:

![params](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/virtual-simulation-testrun/6-params.png)

After the distortion removal:

![7-after](https://github.com/eemrys/sber-ar-vr-opencv/blob/exercise-1-camera-calibration/media/virtual-simulation-testrun/7-after.png)
![8-after](https://github.com/eemrys/sber-ar-vr-opencv/blob/exercise-1-camera-calibration/media/virtual-simulation-testrun/8-after.png)


Then, we set it to the laptop webcam. As the laptop webcam image is horizontal by default, we won't be rotating it so that it can fill the screen (otherwise the image will be too small). We also want the image to have a bit of padding, so that we can see the edges of the frame (this way we can fully see the difference after removing distortion). So we added this just below our scaling calculation in ```deliverAndDrawFrame```:
```cpp
mScale -= 0.3f
```

### Laptop webcam test-run

Identifying the pattern:

![5-demonstration](https://github.com/eemrys/sber-ar-vr-opencv/blob/exercise-1-camera-calibration/media/laptop-webcam-testrun/5-demonstration.gif?raw=true)

Intrinsic parameters:

![3-params](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/laptop-webcam-testrun/3-params.png)

Before (left) and after (right) the distortion removal:

![1-before](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/laptop-webcam-testrun/1-before.png)
![2-after](https://raw.githubusercontent.com/eemrys/sber-ar-vr-opencv/exercise-1-camera-calibration/media/laptop-webcam-testrun/2-after.png)

![4-difference](https://github.com/eemrys/sber-ar-vr-opencv/blob/exercise-1-camera-calibration/media/laptop-webcam-testrun/4-diff.gif?raw=true)
