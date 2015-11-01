# Lane-Detection-OpenCV
This code  implements the lane detectors i.e. Right and Left Lane for highway images.

The code works in the following way:
1)All the images in the folder "images" are read sequentially.
2) For each image, the following algorithm is implementedd:
1--- Image is read in Mat.
2---After careful study, a region of interest is chosen and just that part of the image is taken for processing.
3---Gaussian Filter is applied to remove noise. Filter used is 7*7
4---Canny edge detector is applied to detect the edges in the image
5---Once the edges are detected, Probablistic Hough Transform is applied.
6---It gives all the relavant line segments in the image
7---These segments are converted into lines by calculating the slope.
8---The segment is made to intersect the top and bottom of the image.
9---The bottom intersection reveals the X-intercept.
10--This is the purpose of the image: The X-Intercept of the right and Left lane markers
11--We get many lines from hough trasform.
12--These lines are filters by looking at the slope ranges for right markers and left markers respectively.
13--The sign of the slope reveals which marker it could be.
14--For every image, a flag is maintained, which tells if a marker is found for the image within the boundries
15--If the marker is found, the boundries are updates as per the marker intercepts.
16--The servers as the boundries for the next frame
17--In case no markers are found for more than 2 frames, the boundries are reset.
