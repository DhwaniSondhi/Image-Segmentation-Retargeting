#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <fstream>
#include <utility>
#include <queue>
#include <vector>
#include <map>

using namespace cv;
using namespace std;

//to compare two pixels' colors
int compareVals(int v1[3], int v2[3]) {

	return abs(v1[0] - v2[0]) + abs(v1[1] - v2[1]) + abs(v1[2] - v2[2]);

}

int main(int argc, char** argv) {

	int max_weight = INT_MAX - 2;
	int min_weight = 1;

	if (argc != 4) {
		cout << "Usage: ../seg input_image initialization_file output_mask" << endl;
		return -1;
	}

	// Load the input image
	Mat input;
	input = imread(argv[1]);
	if (!input.data) {
		cout << "Could not load input image!!!" << endl;
		return -1;
	}
	// the image should be a 3 channel image
	if (input.channels() != 3) {
		cout << "Image does not have 3 channels!!! " << input.depth() << endl;
		return -1;
	}

	//get config file
	ifstream configFile(argv[2]);
	if (!configFile) {
		cout << "Could not load initial mask file!!!" << endl;
		return -1;
	}

	int colsNo = input.cols;
	int rowsNo = input.rows;

	int lines;
	configFile >> lines;

	vector<Vec3b> foreground;
	vector<Vec3b> background;
	vector<pair<int, int> > sourcePts;
	vector<pair<int, int> > targetPts;

	// get config file pixels and their colors
	for (int i = 0; i < lines; ++i) {

		int x, y, t;
		configFile >> x >> y >> t;
		//x>=width || y>=height
		if (x < 0 || x >= colsNo || y < 0 || y >= rowsNo) {

			cout << "I valid pixel mask!" << endl;
			return -1;

		}

		Vec3b pixel = input.at<Vec3b>(y, x);
		if (t == 1) {

			foreground.push_back(pixel);
			sourcePts.push_back(pair<int, int>(y, x));

		}
		else {

			background.push_back(pixel);
			targetPts.push_back(pair<int, int>(y, x));

		}

	}

	//gray scale output image 
	Mat out_image;
	/* = Mat::zeros(rowsNo, colsNo, CV_8UC1)*/
	cvtColor(input, out_image, CV_RGB2GRAY);
	map <int, int> graph_map;
	map <int, int> graph_to_image_map;

	//compare each pixel with config file vectors and assign them values 0, 255 according to the which one they are close to
	int countNodes = 0;

	for (int i = 0; i < rowsNo; i++) {

		for (int j = 0; j < colsNo; j++) {

			graph_map[(i*colsNo) + j] = ++countNodes;
			graph_to_image_map[countNodes] = (i*colsNo) + j;
			int minFore = INT_MAX, minBack = INT_MAX;
			Vec3b pixel = input.at<Vec3b>(i, j);
			int loopArr[3];
			loopArr[0] = 0;
			loopArr[1] = 0;
			loopArr[2] = 0;
			loopArr[0] = pixel[0];
			loopArr[1] = pixel[1];
			loopArr[2] = pixel[2];
			for (vector<Vec3b>::iterator it = foreground.begin(); it != foreground.end(); ++it) {

				int itArr[3];
				itArr[0] = (*it)[0];
				itArr[1] = (*it)[1];
				itArr[2] = (*it)[2];
				int loopVal = compareVals(loopArr, itArr);
				if (loopVal < minFore)				minFore = loopVal;

			}
			for (vector<Vec3b>::iterator it = background.begin(); it != background.end(); ++it) {

				int itArr[3];
				itArr[0] = (*it)[0];
				itArr[1] = (*it)[1];
				itArr[2] = (*it)[2];
				int loopVal = compareVals(loopArr, itArr);
				if (loopVal < minBack)				minBack = loopVal;

			}
			if (minFore > minBack)							out_image.at<uchar>(i, j) = 0;
			else									out_image.at<uchar>(i, j) = 255;

		}

	}

	//to create graph
	//source and target node added
	countNodes += 2;

	//for source Nodes
	vector<vector<pair<int, int> > > graph(countNodes);
	vector<pair<int, int> > st_nodes_vector;
	for (int i = 0; i < sourcePts.size(); i++) {

		int i_row = sourcePts[i].first;
		int i_col = sourcePts[i].second;
		if (graph_map.count((i_row*colsNo) + i_col) > 0) {

			st_nodes_vector.push_back(pair<int, int>(graph_map.at((i_row*colsNo) + i_col), max_weight));

		}

	}
	graph[0] = st_nodes_vector;

	//for other nodes
	int loopNode = 0;
	for (int i = 0; i < rowsNo; i++) {

		for (int j = 0; j < colsNo; j++) {

			int pixelVal = (int)out_image.at<uchar>(i, j);
			if (graph_map.count((i*colsNo) + j) > 0) {

				vector<pair<int, int> > inner_vector;
				if ((i - 1) >= 0 && graph_map.count(((i - 1)*colsNo) + j) > 0) {

					int inner_pixel_val = (int)out_image.at<uchar>(i - 1, j);
					int weight = max_weight;
					if (pixelVal != inner_pixel_val)			weight = min_weight;
					inner_vector.push_back(pair<int, int>(graph_map.at(((i - 1)*colsNo) + j), weight));

				}
				if ((i + 1) < rowsNo	&&		graph_map.count(((i + 1)*colsNo) + j) > 0) {

					int inner_pixel_val = (int)out_image.at<uchar>(i + 1, j);
					int weight = max_weight;
					if (pixelVal != inner_pixel_val)			weight = min_weight;
					inner_vector.push_back(pair<int, int>(graph_map.at(((i + 1)*colsNo) + j), weight));

				}
				if ((j - 1) >= 0 && graph_map.count((i*colsNo) + j - 1) > 0) {

					int inner_pixel_val = (int)out_image.at<uchar>(i, j - 1);
					int weight = max_weight;
					if (pixelVal != inner_pixel_val)			weight = min_weight;
					inner_vector.push_back(pair<int, int>(graph_map.at((i*colsNo) + j - 1), weight));

				}
				if ((j + 1) < colsNo		&&		graph_map.count((i*colsNo) + j + 1) > 0) {

					int inner_pixel_val = (int)out_image.at<uchar>(i, j + 1);
					int weight = max_weight;
					if (pixelVal != inner_pixel_val)			weight = min_weight;
					inner_vector.push_back(pair<int, int>(graph_map.at((i*colsNo) + j + 1), weight));

				}
				graph[graph_map.at((i*colsNo) + j)] = inner_vector;

			}

		}

	}

	//for target nodes
	st_nodes_vector.clear();
	for (int i = 0; i < targetPts.size(); i++) {

		int i_row = targetPts[i].first;
		int i_col = targetPts[i].second;
		if (graph_map.count((i_row*colsNo) + i_col) > 0) {

			int node = graph_map.at((i_row*colsNo) + i_col);
			vector<pair<int, int> >  target_pts_nei = graph[node];
			target_pts_nei.push_back(pair<int, int>(countNodes - 1, max_weight));
			graph[node] = target_pts_nei;

		}

	}
	graph[countNodes - 1] = st_nodes_vector;

	//max flow implementation
	bool pathFound = true;
	int maxFlow = 0;
	int source = 0;
	int dest = countNodes - 1;
	while (pathFound) {

		pathFound = false;
		queue<int> que;
		que.push(source);
		bool visited_nodes[countNodes];
		int previous[countNodes];
		for (int i = 0; i < countNodes; i++) {

			visited_nodes[i] = false;
			previous[i] = -1;

		}
		//find path if exists
		while (!que.empty()) {

			int val = que.front();
			que.pop();
			if (!visited_nodes[val]) {
				visited_nodes[val] = true;
				for (int j = 0; j < graph[val].size(); j++) {

					int loop = graph[val][j].first;
					if ((!visited_nodes[loop]) && graph[val][j].second > 0) {

						que.push(loop);
						previous[loop] = val;
						if (loop == dest) {
							pathFound = true;
							break;
						}

					}
				}
				if (pathFound)				break;
			}

		}
		if (pathFound) {

			int getMin = INT_MAX;
			int loop = dest;
			//get minimun flow
			while (loop != source) {

				int parent_val;
				if (previous[loop] != -1) {

					parent_val = previous[loop];
					int j = 0;
					for (; j < graph[parent_val].size(); j++) {
						if (graph[parent_val][j].first == loop)						break;
					}
					if (graph[parent_val][j].second < getMin)			getMin = graph[parent_val][j].second;
					loop = parent_val;

				}

			}
			maxFlow = maxFlow + getMin;
			loop = dest;
			//update residual graph according to the minimum flow
			while (loop != source) {

				int parent_val;
				if (previous[loop] != -1) {
					parent_val = previous[loop];
					int j = 0;
					for (; j < graph[parent_val].size(); j++) {

						if (graph[parent_val][j].first == loop)						break;
					}
					graph[parent_val][j].second = graph[parent_val][j].second - getMin;
					j = 0;
					for (; j < graph[loop].size(); j++) {

						if (graph[loop][j].first == parent_val)						break;
					}
					if (j < graph[loop].size()) {

						if (graph[loop][j].second + getMin <= INT_MAX)						graph[loop][j].second = graph[loop][j].second + getMin;
					}
					else {

						if (getMin <= INT_MAX)									graph[loop].push_back(pair<int, int>(parent_val, getMin));
					}
					loop = previous[loop];
				}
			}

		}

	}


	//to put min cut over the graph
	vector<int> lastColoring;
	queue<int> end_q;
	source = 0;
	end_q.push(source);
	bool visited_nodes[countNodes];
	for (int i = 0; i < countNodes; i++)						visited_nodes[i] = false;
	//collect nodes connected to source
	while (!end_q.empty()) {

		int val = end_q.front();
		end_q.pop();
		if (!visited_nodes[val]) {

			visited_nodes[val] = true;
			lastColoring.push_back(val);
			for (int j = 0; j < graph[val].size(); j++) {

				int loop = graph[val][j].first;
				if ((!visited_nodes[loop]) && graph[val][j].second > 0)					end_q.push(loop);

			}

		}

	}

	//convert whole image to black
	for (int i = 0; i < rowsNo; i++) {

		for (int j = 0; j < colsNo; j++)				out_image.at<uchar>(i, j) = 0;

	}

	//turn foreground image white
	for (int i = 0; i < lastColoring.size(); i++) {

		if (graph_to_image_map.count(lastColoring[i]) > 0) {

			int val = graph_to_image_map[lastColoring[i]];
			int i_row = val / colsNo;
			int j_col = val % colsNo;
			out_image.at<uchar>(i_row, j_col) = 255;

		}

	}

	//get binary output image
	Mat out_image_bin;
	threshold(out_image, out_image_bin, 100, 255, THRESH_BINARY);

	// write it on disk
	imwrite(argv[3], out_image_bin);

	namedWindow("Original image", WINDOW_AUTOSIZE);

	namedWindow("Show Marked Pixels", WINDOW_AUTOSIZE);

	imshow("Original image", input);

	imshow("Show Marked Pixels", out_image_bin);

	waitKey(0);
	return 0;
}
