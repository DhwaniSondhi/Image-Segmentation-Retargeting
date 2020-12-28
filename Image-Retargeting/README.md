# Image Re-targeting
Image retargeting is the process of pasting the content of an image of a certain size to a canvas of a different size. This implementation used Seam- carving algorithm and Dynamic programming.

### Seam Carving
This algorithm uses vertical or horizontal seams to reduce the size iteratively. The seam removal leads to the reduction of image size by one unit. The seams are detected using Dynamic Programming. Following is an example of these seams:

<img src="https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/abc.png" alt="drawing" width="400"/>

## Demo:
The program takes an input image, the new width & height required and creates a reduced output file. Following are some samples:


Input            |  Output
:-------------------------:|:-------------------------:
![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/mian.jpg)  |  ![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/mian200215.jpg)
![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/castle.jpg)  |  ![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/castle900800.jpg)


