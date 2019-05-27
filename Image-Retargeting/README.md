# Image Re-targeting
Image retargeting is the process of pasting the content of an image of certain size to a canvas of a different size. This implmentation used Seam- carving algorithm and Dynamic programming.

### Seam Carving
This algorithm uses vertical or horizontal seams to reduce the size iteratively. The seam removal leads to reduction of image size by one unit. The seams are detected using Dynamic Programming. Following is an example of these seams:

![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/abc.png)

## Demo:
The program takes an input image, the new width and height required and creates an reduced output file. Following are some samples:


Input            |  Output
:-------------------------:|:-------------------------:
![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/mian.jpg)  |  ![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/mian200215.jpg)
![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/castle.jpg)  |  ![](https://github.com/DhwaniSondhi/Image-Segmentation-Retargeting/blob/master/Image-Retargeting/code/build/castle900800.jpg)


