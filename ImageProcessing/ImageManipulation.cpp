
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
//#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/utils/filesystem.hpp>
#include "libxl.h"

using namespace libxl;
using namespace cv;
using namespace std;


#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include <fstream>



class ImageManipulation {
public:
	int ImageSize;
	int Rows;
	int Cols;
	int BlockWidth;
	int BlockHeight;
	int TotalBlocks;
	cv::Mat Image;
	string ImageName;
	string FolderName;



public:ImageManipulation(int imageSize, int blockWidth, int blockHeight, cv::Mat image, string folderName = "", string imageName = "") {
	ImageName = imageName;
	FolderName = folderName;
	ImageSize = imageSize;
	BlockWidth = blockWidth;
	BlockHeight = blockHeight;
	Rows = image.rows;
	Cols = image.cols;
	Image = image;
	TotalBlocks = (imageSize / blockWidth) * (imageSize / blockHeight);
}
public:ImageManipulation() {
	ImageName = "";
	FolderName = "";
	ImageSize = 0;
	BlockWidth = 0;
	BlockHeight = 0;
	Rows = 0;
	Cols = 0;
	Image = cv::Mat::zeros(cv::Size(0, 0), CV_64FC1);
	TotalBlocks = 0;
}

public:int divideImage(std::vector<cv::Mat>& blocks)
{
	// Checking if the image was passed correctly
	if (!Image.data || Image.empty())
	{
		std::wcout << "Image Error: Cannot load image to divide." << std::endl;
		return EXIT_FAILURE;
	}

	// init image dimensions
	int imgWidth = Image.cols;
	int imgHeight = Image.rows;
	std::wcout << "IMAGE SIZE: " << "(" << imgWidth << "," << imgHeight << ")" << std::endl;

	// init block dimensions
	int bwSize;
	int bhSize;

	int y0 = 0;
	while (y0 < imgHeight)
	{
		// compute the block height
		bhSize = ((y0 + BlockHeight) > imgHeight) * (BlockHeight - (y0 + BlockHeight - imgHeight)) + ((y0 + BlockHeight) <= imgHeight) * BlockHeight;

		int x0 = 0;
		while (x0 < imgWidth)
		{
			// compute the block height
			bwSize = ((x0 + BlockWidth) > imgWidth) * (BlockWidth - (x0 + BlockWidth - imgWidth)) + ((x0 + BlockWidth) <= imgWidth) * BlockWidth;

			// crop block
			blocks.push_back(Image(cv::Rect(x0, y0, bwSize, bhSize)).clone());

			// update x-coordinate
			x0 = x0 + BlockWidth;
		}

		// update y-coordinate
		y0 = y0 + BlockHeight;
	}
	return EXIT_SUCCESS;
}


public:int getLaplacian(int index) {


	cv::Mat img_gray;
	
	// Blur the image for better edge detection
	cv::Mat img_blur;

	int channels = Image.channels();

	if (channels != 1)
	{
		//Remove noise by applying a Gaussian blur and then convert the original image to grayscale
		GaussianBlur(Image, img_blur, Size(3, 3), 0);

		// Convert the image to grayscale
		cvtColor(img_blur, img_gray, cv::ColorConversionCodes::COLOR_BGR2GRAY);
	}
	else {
		//cvtColor(Image, img_gray, cv::ColorConversionCodes::COLOR_BGR2GRAY);	
		img_gray = Image;
	}

	cv::Mat edges;

	//	Desired depth of the destination image(the output image depth)
	//when ddepth=-1, the output image will have the same depth as the source.
	// CV_16S = 3 RGB
	int ddepth ;
	int kernel_size;
	if (channels == 1) {
		ddepth= CV_16S;
		kernel_size = 3;
	}
	else {
		ddepth = CV_16S;
		kernel_size = 3;
	}
	//Aperture size used to compute the second-derivative filters. See getDerivKernels for details. The size must be positive and odd
	//size of the filter used to convolve an image 
	
	
	//Optional scale factor for the computed Laplacian values. By default, no scaling is applied
	int scale = 1;

	//Optional delta value that is added to the results prior to storing them in dst .
	int delta = 0;


	//if (img_gray.empty())
		//std::cout << "failed to open img.jpg" << std::endl;
	//else
		//std::cout << "img.jpg loaded OK" << std::endl;
	
	// laplacian perform second order deviation, and internally it does Sobel operator (1st deviation)
	Laplacian(img_gray, edges, ddepth, kernel_size, scale, delta, cv::BorderTypes::BORDER_DEFAULT);
	//Sobel(img_blur, edges, ddepth, 1, 0, kernel_size, scale, delta, BORDER_DEFAULT);

	int edgeCount = getEdgesCount(edges);

	std::string blockImgName;
	if (FolderName != "" && ImageName != "")
	{
		blockImgName = FolderName + "/" + ImageName + "_EDGE_" + std::to_string(index)+"_"+ std::to_string(edgeCount) + ".bmp";
		//imwrite(blockImgName, edges);
	}


	return edgeCount;
}

private:int getEdgesCount(cv::Mat edges) {
		  int r_value = 0;
		  int g_value = 0;
		  int b_value = 0;
		  int non_black = 0;
		  for (int i = 0; i < edges.rows; i++)       // rows, not cols
		  {
			  Vec3b* ptr = edges.ptr<Vec3b>(i);
			  for (int j = 0; j < edges.cols; j++)  // cols, not rows
			  {
				  Vec3b pixel = ptr[j];
				  r_value = (int)pixel.val[0];
				  g_value = (int)pixel.val[1];
				  b_value = (int)pixel.val[2];
				  if (r_value != 0 && g_value != 0 && b_value != 0) {
					  non_black++;
				  }

			  }
		  }
		  return non_black;
	  }

public:int getMSB(int index) {
	int r_value = 0;
	int g_value = 0;
	int b_value = 0;
	int msb = 0;
	for (int i = 0; i < Rows; i++)       // rows, not cols
	{
		Vec3b* ptr = Image.ptr<Vec3b>(i);
		for (int j = 0; j < Cols; j++)  // cols, not rows
		{
			Vec3b pixel = ptr[j];
			r_value = (int)pixel.val[0];
			g_value = (int)pixel.val[1];
			b_value = (int)pixel.val[2];
			if (r_value > 127 && g_value > 127 && b_value > 127) {
				msb++;
			}

		}
	}

	if (ImageName != "" && FolderName != "")
	{
		//std::string blockImgName = FolderName + "/" + ImageName + "_MSB_" + std::to_string(index) + "_" + std::to_string(msb) + ".bmp";
		//imwrite(blockImgName, Image);
				
		//Mat bgr[3];
		//split(Image, bgr);
		//std::string blockImgName = FolderName + "/" + ImageName + "_0_" + std::to_string(index) +".bmp";		
		//imwrite(blockImgName, bgr[0]);
		//blockImgName = FolderName + "/" + ImageName + "_1_" + std::to_string(index) + ".bmp";
		//imwrite(blockImgName, bgr[1]);
		//blockImgName = FolderName + "/" + ImageName + "_2_" + std::to_string(index) + ".bmp";
		//imwrite(blockImgName, bgr[2]);

	}
	return msb;
}

public:cv::Mat gatherImage(std::vector<cv::Mat>& blocks)
{
	// colour  CV_8UC3
	Mat gatheredImage(Cols, Rows, CV_8UC3, Scalar(0, 0, 0));

	Mat img(Cols, Rows, CV_8UC3, Scalar(0, 0, 0));
	cv::Mat dst;
	cv::Mat firstBlock;
	int count = 1;

	cv::Size mat_size(64, 8);
	std::vector<cv::Mat> hBlocks;

	for (cv::Mat block : blocks) {

		if (count % 8 == 1) {
			firstBlock = block;
			count++;
			continue;
		}
		if (count % 8 == 2) {
			cv::hconcat(firstBlock, block, dst);
		}
		else {
			cv::hconcat(dst, block, dst);
		}

		if (count % 8 == 0 && count != 1) {
			//std::string blockImgName = "LenaRGB/newImage"+std::to_string(count) + ".bmp";
			//imwrite(blockImgName, dst);
			hBlocks.push_back(dst);
		}

		count++;
	}
	count = 1;
	cv::Mat finalImage;

	for (cv::Mat block : hBlocks) {
		if (count % 8 == 1) {
			firstBlock = block;
			count++;
			continue;
		}

		if (count % 8 == 2) {
			cv::vconcat(firstBlock, block, dst);
		}
		else {
			cv::vconcat(dst, block, dst);
		}
		finalImage = dst;
		count++;
	}	


	return finalImage;
}

};