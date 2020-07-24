# Estimating extrinsic parameters with ArUco markers

## Overview:
1. Introduction to ArUco markers
2. Detecting ArUco markers
3. Estimating camera pose
4. Calculating distance
5. Results

## Introduction to ArUco markers

The ArUco markers were primarily developed to solve the problem of camera pose estimation for various applications including augmented reality.
The process of pose estimation is based on finding correspondences between points in the real environment and their 2d image projection.

One of the most popular approaches is the use of binary square markers. The main benefit of these markers is that a single marker provides enough correspondences (=its four corners) to obtain the camera pose. Also, the inner binary codification makes them especially robust, allowing the possibility of applying error detection and correction techniques.

The aruco module is based on the ArUco library, a popular library for detection of square fiducial markers:
```cpp
#include <opencv2/aruco.hpp>
```
An ArUco marker is a synthetic square marker composed by a wide black border and an inner binary matrix which determines its id. The black border facilitates its fast detection in the image and the binary codification allows its identification. The marker size determines the size of the internal matrix. For instance a marker size of 4x4 is composed by 16 bits.

Some examples of ArUco markers:

![markers](https://docs.opencv.org/trunk/markers.jpg)

In this app we will be using this one:

![marker23](https://docs.opencv.org/trunk/marker23.png)

It must be noted that a marker can be found rotated in the environment, however, the detection process needs to be able to determine its original rotation, so that each corner is identified unequivocally. This is also done based on the binary codification. The list of binary codifications of a set of markers is called a dictionary.

## Detecting ArUco markers

Once we printed out the marker, we need to detect it and use it for further processing.

In the aruco module, The ```detectMarkers()``` function is used to detect and locate the corners of the markers. This function is the most important in the module, since all the rest of the functionality is based on the detected markers returned by ```detectMarkers()```.

Given an image containing ArUco markers, the detection process has to return a list of detected markers. Each detected marker includes:
* the position of its four corners in the image (in their original order);
* the id of the marker.

The parameters of ```detectMarkers()``` are:

* The image containing the markers to be detected. In our case it's a frame passed to native function from our camera listener. However, before we do anything with the frame we need to convert it from ```RGBA``` color system to a 3-channel system. ArUco uses ```BGR``` image encoding to draw objects, so that's what we'll convert our frame to:
     ```cpp
    cvtColor(frame, frame, COLOR_RGBA2BGR);
     ```
* The second parameter is the dictionary object used to generate the markers, in this case one of the predefined dictionaries ```(DICT_6X6_250)```, which means it contains 250 marker ids of size 6x6.
    ```cpp
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_6X6_250);
    ```
* For each successful marker detection, the four corner points of the marker are detected, in order from top left, top right, bottom right and bottom left. In C++, these 4 detected corner points are stored as a vector of points and multiple markers in the image are together stored in a vector of vector of points:
    ```cpp
    vector<vector<Point2f>> marker_corners;
    ```
* The successfully detected markers ids are stored in a vector:
    ```cpp
    vector<int> marker_ids;
    ```
* The fifth parameter is the object of type DetectionParameters. This object includes all the parameters that can be customized during the detection process.In most of the cases, the default parameter work well and OpenCV recommends to use those. So we will stick to the default parameters:
    ```cpp
    Ptr<aruco::DetectorParameters> parameters = aruco::DetectorParameters::create();
    ```
* The final parameter, ```rejected_candidates```, is a returned list of marker candidates, i.e. shapes that were found and considered but did not contain a valid marker. This parameter can be omitted and is only useful for specific purposes, so we won't need it in this app.

Finally, we can call the function and pass all our parameters:
```cpp
aruco::detectMarkers(frame, dictionary, marker_corners, marker_ids, parameters);
```

The next thing we want to do after ```detectMarkers()``` is to check that our marker has been correctly detected. The aruco module provides a function to draw the detected markers in the input image:
```cpp
if (!marker_ids.empty()) {
        aruco::drawDetectedMarkers(frame, marker_corners, marker_ids);
}
```
Earlier we converted our frame to BGR, so that the aruco module can use correct colors. After it drew the marker in our image, we can convert it back before displaying it:
```cpp
cvtColor(frame, frame, COLOR_BGR2RGB);
```

The pictures below show the marker detected by our app. Note that in some pictures the marker is rotated -- the small red square indicates the marker’s top left corner.

![screen0](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen0.png)
![screen1](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen1.png)
![screen2](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen2.png)

## Estimating camera pose

After detecting the markers we want to estimate the camera pose. To perform camera pose estimation we need to know the calibration parameters of the camera -- the camera matrix and distortion coefficients, which we obtained in the previous app.

The camera pose with respect to a marker is the 3d transformation from the marker coordinate system to the camera coordinate system. It is specified by rotation and translation vectors.
```cpp
vector<Vec3d> r_vecs, t_vecs;
```
The aruco module provides a function to estimate the poses of all the detected markers:
```cpp
aruco::estimatePoseSingleMarkers(marker_corners, marker_length, matrix, dist, r_vecs, t_vecs);
```
* The ```marker_corners``` parameter is the vector of marker corners returned by the ```detectMarkers()``` function.
* The second parameter is the size of the marker side in meters or in any other unit. Note that the translation vectors of the estimated poses will be in the same unit.
* ```matrix``` and ```dist``` are the camera calibration parameters that were obtained during the camera calibration process.
* The output parameters ```r_vecs``` and ```t_vecs``` are the rotation and translation vectors respectively, for each of the markers in ```marker_corners```.

The marker coordinate system that is assumed by this function is placed at the center of the marker with the ```Z``` axis pointing out. Axis-color correspondences are ```X```: red, ```Y```: green, ```Z```: blue. The main reason we converted our frame to BGR encoding is because we wanted to keep the correct color of axis. 

The aruco module provides a function to draw the axis, so pose estimation can be checked:
```cpp
aruco::drawAxis(frame, matrix, dist, r_vecs[0], t_vecs[0], marker_length * 2.f);
```
* ```r_vecs[0]``` and ```t_vecs[0]``` are the pose parameters for the marker whose axis is to be drawn. Since we only have one marker in this app, we just need to retrieve the first (and only) values of both these vectors. If you use several markers in your application, you will need to iterate through the ```marker_ids``` vector, and call this function on each iteration.
* The last parameter is the length of the axis, in the same unit as ```marker_length``` (usually meters).

Below images show the output of this code.

![screen3](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen3.png)
![screen4](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen4.png)
![screen5](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen5.png)

## Calculating distance

In this app we want to get two values:
* the distance between the camera and the marker;
* the distance between the camera and the **surface** of the marker.

Earlier we obtained the rotation and the translation vectors.
Norm of the translation vector is the distance between the tag frame and the camera frame.
```cpp
distance_marker = norm(t_vecs[0]);
```
As for the second value, we'll have to do a bit of math.
To get the distance between the camera and the marker surface, we need to get the position of our camera in reference to the marker, i.e. in its coordinate system. The Z coordinate of the camera is the value we're looking for.

First, we need to get the rotation matrix from the rotation vector. We can do it with ```Rodrigues``` function:
```cpp
Mat rotation_matrix;
Rodrigues(r_vecs[0], rotation_matrix);
```
Then, we need to transpose the matrix and multiply it by the inverse of the translation vector:
```cpp
Mat camera_translation_vector = -rotation_matrix.t()*t_vecs[0];
```
The resulting matrix contains camera coordinates.
Finally, we take the Z coordinate:
```cpp
distance_surface = camera_translation_vector.at<double>(0,2);
```

Here we display both of these values:

![screen6](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/screen6.png)

## Results

**Demonstration:**

![demonstration](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/demonstration.gif)

**Accuracy test using a ruler:**

![accuracy test](https://media.githubusercontent.com/media/eemrys/sber-ar-vr-opencv/exercise-2-extrinsic-parameters/media/accuracy-test.gif)
