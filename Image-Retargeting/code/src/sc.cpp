
#include "sc.h"

using namespace cv;
using namespace std;




bool seam_carving(Mat& in_image, int new_width, int new_height, Mat& out_image){

    // some sanity checks
    // Check 1 -> new_width <= in_image.cols
    if(new_width>in_image.cols){
        cout<<"Invalid request!!! new_width has to be smaller than the current size!"<<endl;
        return false;
    }
    if(new_height>in_image.rows){
        cout<<"Invalid request!!! ne_height has to be smaller than the current size!"<<endl;
        return false;
    }
    
    if(new_width<=0){
        cout<<"Invalid request!!! new_width has to be positive!"<<endl;
        return false;

    }
    
    if(new_height<=0){
        cout<<"Invalid request!!! new_height has to be positive!"<<endl;
        return false;
        
    }

    
    return seam_carving_trivial(in_image, new_width, new_height, out_image);
}


// seam carves by removing trivial seams
bool seam_carving_trivial(Mat& in_image, int new_width, int new_height, Mat& out_image){

    Mat iimage = in_image.clone();
    Mat oimage = in_image.clone();
    while(iimage.rows!=new_height || iimage.cols!=new_width){
        // horizontal seam if needed
        if(iimage.rows>new_height){
            reduce_horizontal_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
        
        if(iimage.cols>new_width){
            reduce_vertical_seam_trivial(iimage, oimage);
            iimage = oimage.clone();
        }
    }
    
    out_image = oimage.clone();
    return true;
}

// horizontl trivial seam is a seam through the center of the image
bool reduce_horizontal_seam_trivial(Mat& in_image, Mat& out_image){

	//get energies
	Mat in_gray, grad_x16, grad_y16, grad_x8, grad_y8, grad_image,inimage;
	int to_change = 1, not_change = 0, dep_16 = CV_16S;
	int channels = 3;
	GaussianBlur( in_image, inimage, Size(3, 3), 0, 0, BORDER_DEFAULT );
	cvtColor(inimage , in_gray, CV_BGR2GRAY);
	Sobel(in_gray, grad_x16, dep_16, to_change, not_change, channels, 1, 0, BORDER_DEFAULT);
	Sobel(in_gray, grad_y16, dep_16, not_change, to_change, channels, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_x16, grad_x8);
	convertScaleAbs(grad_y16, grad_y8);
	addWeighted(grad_x8, 0.5, grad_y8, 0.5, 0, grad_image);
	int rows = in_image.rows;
	int cols = in_image.cols;

	//create output image
	out_image = Mat(rows-1, cols, CV_8UC3);
	
	//get minimum seam
	vector<vector<int> > cost(rows);
	for (int i = 0; i < rows; i++) {
		vector<int> internal(cols);
		for (int j = 0; j < cols; j++)                internal[j] = INT_MAX;
		cost[i]=internal;
	}
	vector<vector<int> > path(rows);
	for (int i = 0; i < rows; i++) {
		vector<int> internal(cols);
		for (int j = 0; j < cols; j++)                internal[j] = -1;
		path[i]=internal;
	}
	for (int i = 0; i < rows; i++)			      	  cost[i][0] = grad_image.at<uchar>(i,0);
	for (int j = 0; j < cols - 1; j++) {
		for (int i = 0; i < rows; i++) {
			if (i > 0){
				int mat1=grad_image.at<uchar>(i-1,j+1);
				if(cost[i][j] + mat1 < cost[i - 1][j + 1]) {
					cost[i - 1][j + 1] = cost[i][j] + mat1;
					path[i - 1][j + 1] = i;
				}
			}
			if (i < rows - 1){
				int mat2=grad_image.at<uchar>(i+1,j+1);
				if(cost[i][j] + mat2 < cost[i + 1][j + 1]) {
					cost[i + 1][j + 1] = cost[i][j] + mat2;
					path[i + 1][j + 1] = i;
				}
			}
			int mat3=grad_image.at<uchar>(i,j+1);
			if (cost[i][j] + mat3 < cost[i][j + 1]) {
				cost[i][j + 1] = cost[i][j] + mat3;
				path[i][j + 1] = i;
			}
		}
	}

	//remove minimum seam
	int min = 0;
	for (int i = 0; i < rows; i++) {
		if (cost[min][cols-1] > cost[i][cols-1])                     min = i;
	}
	for (int i=0;i<min;i++){
		Vec3b pixel = in_image.at<Vec3b>(i, cols-1);
		out_image.at<Vec3b>(i, cols-1) = pixel;
	}
	for (int i=min+1;i<rows;i++){
		Vec3b pixel = in_image.at<Vec3b>(i, cols-1);
		out_image.at<Vec3b>(i-1, cols-1) = pixel;
	}
	int i_min=min;
	for (int j = cols-2; j >= 0; j--){
		i_min=path[i_min][j+1];
		for (int i = 0; i < i_min; i++) {
			Vec3b pixel = in_image.at<Vec3b>(i, j);
			out_image.at<Vec3b>(i,j) = pixel;
		}
		for (int i = i_min+1; i < rows; i++) {
			Vec3b pixel = in_image.at<Vec3b>(i, j);
			out_image.at<Vec3b>(i-1,j) = pixel;
		}
	}
	return true;

}

// vertical trivial seam is a seam through the center of the image
bool reduce_vertical_seam_trivial(Mat& in_image, Mat& out_image){
	
	//get energies
	Mat in_gray, grad_x16, grad_y16, grad_x8, grad_y8, grad_image,inimage;
	int to_change = 1, not_change = 0, dep_16 = CV_16S;
	int channels = 3;
	GaussianBlur( in_image, inimage, Size(3, 3), 0, 0, BORDER_DEFAULT );
	cvtColor(inimage , in_gray, CV_BGR2GRAY);
	Sobel(in_gray, grad_x16, dep_16, to_change, not_change, channels, 1, 0, BORDER_DEFAULT);
	Sobel(in_gray, grad_y16, dep_16, not_change, to_change, channels, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_x16, grad_x8);
	convertScaleAbs(grad_y16, grad_y8);
	addWeighted(grad_x8, 0.5, grad_y8, 0.5, 0, grad_image);

	
	
        int rows = in_image.rows;
	int cols = in_image.cols;

	//create output image
	out_image = Mat(rows, cols-1, CV_8UC3);
	
	//get minimum seam
	vector<vector<int> > cost(rows);
	for (int i = 0; i < rows; i++) {
		vector<int> internal(cols);
		for (int j = 0; j < cols; j++)                internal[j] = INT_MAX;
		cost[i]=internal;
	}
	vector<vector<int> > path(rows);
	for (int i = 0; i < rows; i++) {
		vector<int> internal(cols);
		for (int j = 0; j < cols; j++)                internal[j] = -1;
		path[i]=internal;
	}
	for (int j = 0; j < cols; j++)			      cost[0][j] = grad_image.at<uchar>(0,j);
	for (int i = 0; i < rows - 1; i++) {
		for (int j = 0; j < cols; j++) {
			if (j > 0){
				int mat1=grad_image.at<uchar>(i+1,j-1);
				if(cost[i][j] + mat1 < cost[i + 1][j - 1]) {
					cost[i + 1][j - 1] = cost[i][j] + mat1;
					path[i + 1][j - 1] = j;
				}
			}
			if (j < cols - 1){
				int mat2=grad_image.at<uchar>(i+1,j+1);
				if(cost[i][j] + mat2 < cost[i + 1][j + 1]) {
					cost[i + 1][j + 1] = cost[i][j] + mat2;
					path[i + 1][j + 1] = j;
				}
			}
			int mat3=grad_image.at<uchar>(i+1,j);
			if (cost[i][j] + mat3 < cost[i + 1][j]) {
				cost[i + 1][j] = cost[i][j] + mat3;
				path[i + 1][j] = j;
			}
		}
	}
	
	//remove minimum seam
	int min = 0;
	for (int j = 0; j < cols; j++) {
		if (cost[rows - 1][min] > cost[rows - 1][j])                     min = j;
	}
	for (int j=0;j<min;j++){
		Vec3b pixel = in_image.at<Vec3b>(rows-1, j);
		out_image.at<Vec3b>(rows-1,j) = pixel;
	}
	for (int j=min+1;j<cols;j++){
		Vec3b pixel = in_image.at<Vec3b>(rows-1, j);
		out_image.at<Vec3b>(rows-1,j-1) = pixel;
	}
	int j_min=min;
	for (int i = rows-2; i >= 0; i--){
		j_min=path[i+1][j_min];
		for (int j = 0; j < j_min; j++) {
			Vec3b pixel = in_image.at<Vec3b>(i, j);
			out_image.at<Vec3b>(i,j) = pixel;
		}
		for (int j = j_min+1; j < cols; j++) {
			Vec3b pixel = in_image.at<Vec3b>(i, j);
			out_image.at<Vec3b>(i,j-1) = pixel;
		}
	}
	return true;

}
