#include <algorithm>
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
//#include "ImageManipulation.cpp"
#include "ImageEncryption.cpp"

using namespace libxl;
using namespace cv;
using namespace std;
using namespace std::chrono;



#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>
#include "QualityAssessment.cpp"
#include "ExportExcel.cpp"
#include "ImageDecryption.cpp"


using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;

const int image_size = 512;
const int blockw = 64;
const int blockh = 64;
const int totalBlocks = (image_size / blockw) * (image_size / blockh);
const int IV = 234;
int Results[3][totalBlocks][2]; // (MSB,Edge) x3
cv::Mat IV_img;
int** FisherRandomNumbers[64];


double diffclock(clock_t end, clock_t start) {

	double diffticks = end - start;
	double diffms = diffticks / (CLOCKS_PER_SEC / 1000);

	return diffms;
}

string generateImageName(string folderName, string encryptionCriteria, double* psnr_mse_value, Scalar SSIMValue, float cc) {

	return folderName + "/" + encryptionCriteria + ".bmp";

			//return folderName + "/"+ encryptionCriteria + "_MSE_" + std::to_string(psnr_mse_value[1]) +"_PSNR_"+ std::to_string(psnr_mse_value[0]) +
		//"_CC_"+ std::to_string(cc)+	
		//"_RGB_" + std::to_string(SSIMValue.val[2] * 100) + "%_" + std::to_string(SSIMValue.val[1] * 100) + "%_" + std::to_string(SSIMValue.val[0] * 100) + +"%_" + ".bmp";
}
string generateEncryptedImageName(bool withMSB, bool withChannelSwap, bool withFisher, bool withBytesXOR, bool cbcMode) {
	string eim = "";
	if (withMSB) {
		eim += "M";
	}
	if (withChannelSwap) {
		eim += "C";
	}
	if (withFisher) {
		eim += "P";
	}
	if (withBytesXOR) {
		eim += "B";
	}
	if (cbcMode) {
		eim += "-CBC";
	}
	return eim;
}
cv::Mat decryptImage(cv::Mat encryptedImage, string folderName, string imageName,string prng,
	bool withBytesXOR, bool withFisher, bool withMSB , bool  withChannelSwap, bool cbcMode, double edge_percentage) {
	ImageManipulation image(512, 64, 64, encryptedImage, folderName, imageName);


	std::vector<cv::Mat> blocks;
	image.divideImage(blocks);
	
	int msb_sum = 0;
	int edge_sum = 0;
	int msb_average;
	int edge_average;
	int result[totalBlocks][2]; // (MSB,Edge) x3

	
	for (int j = 0; j < blocks.size(); j++)
	{
		
		// definitions
		std::string blockId = std::to_string(j);
		ImageManipulation block(64, 8, 8, blocks[j]);// , image.FolderName, imageName);

		// Get MSB count for each block
		int msb = block.getMSB(j);

		// Get the edge count for each block
		int laplacian = block.getLaplacian(j);
		//int laplacian = block.getLaplacian(j);
		// save results into an array
		result[j][0] = msb;
		result[j][1] = laplacian;
		//get the sum value of the msb of all blocks
		msb_sum += msb;
		//get the sum of the laplacian edge values of all blocks
		edge_sum += laplacian;
	}
	edge_average = edge_sum / totalBlocks;
	msb_average = msb_sum / totalBlocks;
	
	//Book* edgeMSBBook = xlCreateBook(); // xlCreateXMLBook() for xlsx
	//ExportExcel edgeMSBExcelExport(edgeMSBBook);
	//
	//Sheet* sheet = edgeMSBBook->addSheet(L"MSB");

	//sheet->writeStr(1, 1, L"MSB");
	//sheet->writeStr(1, 2, L"Edge");

	//int k = 0;

	string encryptedBlocks = "";
	int previously_encrypted_index = -1;
	std::vector<cv::Mat>  decryptedBlocks = std::vector<cv::Mat>();
	
	for (int j = 0; j < blocks.size(); j++)
	{
		cv::Mat xoredBlock = blocks[j].clone();
		ImageDecryption decryptionBlock(blocks[j].clone(), prng);
		if (result[j][1] >= (edge_average* edge_percentage) 
			|| 1 == 1
			) {

			

			encryptedBlocks+=std::to_string(j)+"-";
		
		if (withBytesXOR)
		{
			decryptionBlock.decryptImage_alter_fisher();
		}

		if (withFisher)
		{
			//string bock_index = std::to_string(j);
			//std::string decryptedImgName = folderName + "/" + bock_index + "_decrypted.bmp";
			//imwrite(decryptedImgName, decryptionBlock.Image);
			//continue;

			decryptionBlock.decryptFisherYates(FisherRandomNumbers[j]);			
		}

		if (withMSB || withChannelSwap)
		{
			decryptionBlock.decryptImageMSB(withChannelSwap, withMSB);
		}

		if (cbcMode)
		{
			cv::Mat previous_block;
			cv::Mat current_block;
			current_block = decryptionBlock.Image;

			if (previously_encrypted_index != -1) {
				previous_block = blocks[previously_encrypted_index].clone();
			}
			else {
				previous_block = IV_img.clone();
			}
			decryptionBlock.xorWithPreviousBlock(previous_block, current_block);
			previously_encrypted_index = j;


		}
		/*
		if (cbcMode)
		{
			cv::Mat previous_block;
			cv::Mat current_block;
			current_block = decryptionBlock.Image;

			if (previously_encrypted_index != -1) {
				previous_block = blocks[previously_encrypted_index].clone();
			}
			else {
				previous_block = IV_img.clone();
			}
			xoredBlock = decryptionBlock.xorWithPreviousBlock(previous_block, current_block);
			previously_encrypted_index = j;
		}
		else {
			xoredBlock = decryptionBlock.Image;
		}
		*/
		}
		//std::string decryptedblockName = folderName + "/" + std::to_string(j)+".bmp";

		//imwrite(decryptedblockName, xoredBlock.clone());

		decryptedBlocks.push_back(decryptionBlock.Image);

	}
	
	//edgeMSBBook->save(L"Output/HighEncrypted.xls");
	//edgeMSBBook->release();

	cv::Mat decryptedImage = image.gatherImage(decryptedBlocks);
	std::string decryptedImgName = folderName + "/decrypteEdge.bmp";
	//imwrite(decryptedImgName, decryptedImage);

	return encryptedImage;
}

cv::Mat encryptBlock(ImageEncryption ecnrypt, bool withMSB, bool withChannelSwap, cv::Mat block,
	bool withFisher, bool withBytesXOR, bool cbcMode, std::vector<cv::Mat> blocks, int j) {
	cv::Mat blockEncrypted = block;

	if (cbcMode)
	{
		cv::Mat previous_block;
		cv::Mat current_block;
		current_block = blockEncrypted;

		if (j - 1 >= 0) {
			previous_block = blocks[j - 1].clone();
		}
		else {
			//iv_img = cv::Mat(blocks[j].rows, edgeBlocks[j].cols, CV_8UC3);
			previous_block = IV_img.clone();
		}
		//blockEncrypted = ecnrypt.xorWithPreviousBlock(blockEncrypted, edgeBlocks[j - 1]);
		blockEncrypted = ecnrypt.xorWithPreviousBlock(previous_block, blockEncrypted);
	}

	if(withMSB || withChannelSwap)
	{
		blockEncrypted = ecnrypt.encryptImage(withMSB, withChannelSwap
			//, block.Image.clone());
			, block);
	}

	if (withFisher) {
		//blockEncrypted = ecnrypt.encryptImageFisher(block.Image.clone());
		blockEncrypted = ecnrypt.encryptImageFisher(block);
	}
	if (withBytesXOR)
	{
		blockEncrypted = ecnrypt.encryptImage_alter_fisher(blockEncrypted);
	}
	
	return blockEncrypted;
}

int main()
{
	string prng;
	prng = "00011001010001110111100010101111101110110110011000100100101010000010111000010110000000001111001010000000100000100110111111100";
	//string modifiedPRNG = "1001001000100100101000011111011100011110000011000011011000001101011101111101011010110010011010001100100111001100111001011110100001100111111000011111111001011001010110100111001001101000010101011010001111100000011101111110101011110001001010111110010100100111111010111110010000110011010011111101100011110011100011001101110101100111011110100110101110010000011010100110111011000101011100010101111011000001010100111111011010011111000111011000111001110110111000010001101111000001001011110011011001000100101011111110111011110111110011101101000100011110000100101111101111011101111101001101000110001011111010111110111110110101001001001101111101110100010010010010000100011010111110010101101110011011011000001011000010100000010010000001100001101000001100001111100100100011110001000011011110011101100111110011001100011010011101001010110101101010001011000100101011001100110110110111010110111010000001110000010101101000000010111001001101011011111101101011001101010111101110001111110010010100101111001101110011011100000100100110110100001001011101100000011111111110011011101011001001010100000111101101000100101011100010000001011001100000110100100100010001000101101000110010000000011011001000100001011101010100101001001011100101101011010000000111111100000001001110000011001010010000100111100110111001100001110110011011010011010110100111000000110100011101001001101000011111110011001100010000011100011111001100101110111110010111101010000101100010100110011010111000110100101000101100110010001101110000111100101100111100101100101100111110001000101100000000010101111010011000010000011010011101110100110000111010111000000111111110101110100010000010101011010111110000100110111111101000111100000100111111001101001010011010100010101101101001001111010111000000001010000101011100111111010001101111010010000111110110110101100001110100101001000100101010100011000011110110101100101111010100001000111010011010101011101000111010100100100011011100011000101010110010000010011100101001000111101001011000010011010011111110000010100010101001111110001001110000011110000010000011101110101001101100100000001000111011010010100001000011110110100111011001100110000011100101111110010010100100100100011110101101101101110101101101110101100011100100011001011101110101011010011110101100001100100100001000000101110100110001101001110100001100100100001011101011011110011010001000110000010100010100001000100011101111111100011011111101110110101011000110101011100100100000010000011100101101001100001101101010100100101100100100001000011010000111001101001101101111010001010101101001010111001101011101100001110001100001100011010000101111001101110110111000100000011111000000110010101000110101111000000001111000111011100111101110011101101001001011000011000100111101010000111000011010101111000101010111011000100001010111100110000001001101010111101001001000111111111010111001101111111111110010111010100111000110111000001000001001100000110111110011110101101010110000101000000011101110000011010001001000000011111001010101111011100110111110101010000110010000000101111011110101001101010000011001001011010101101011010000010100001111001000111100111100001000010010001101001111100000000001010011001010111011110111000101011100111110001010101001000000000001010111000010111000011010001010000011110100101110101010000000100111010100101000111100010100110010110001110011011111101000001101001111010110000110110000010011101100100011111010011100110010011000011110000010111000000101111011011010101101101011001100011110000010100001111011110011100110110000011100001000001010001100101111101100101110110001001001010000110001101101000010111110101101110000010100110010101100110011001000001001100001101000111111010001000110100101001000010110111100101111100011111110101001111100110101110010111111110000000010111110101010110101001000001001011000010111110110110011011110111011000000110000001001000111110010110000100110110000000011111111001100001100010110001110010101100110101100010011001001001000011010101101000011000000101110101011001101110011011100010101110100101000001011110100111110001000000001000011001111011011000001110100010110111011010011110111011110111111010010111101010110101001";
	string decryption_prng = prng;

	//string imageName[] = { "LenaRGB","barbara","Elaine512","school","people","desk","people2","people3" };
	//string imageName[] = { "barbara_color"};
	string imageName[] = { "desk"};
	//string imageName[] = { "LenaRGB"};
	string imageType = ".bmp";
	
	// true
	// false
	bool withMSB = true;			//	M
	bool withChannelSwap = true;	//	C	
	bool withFisher = true;			//	P
	bool withBytesXOR= true;		//	B
	bool cbcMode = false;			//	-CBC
	string process = generateEncryptedImageName(withMSB, withChannelSwap, withFisher, withBytesXOR, cbcMode);
	
	IV_img = cv::Mat(64, 64, CV_8UC3);
	randu(IV_img, Scalar(0, 0, 0), Scalar(255, 255, 255));

	for (int imageIndex = 0; imageIndex < (sizeof(imageName) / sizeof(imageName[0])); imageIndex++)
	{
		// definations
		std::vector<cv::Mat> blocks;
		std::vector<cv::Mat> highBlocks;
		std::vector<cv::Mat> lowBlocks;
		std::vector<cv::Mat> edgeBlocks;
		std::vector<cv::Mat> msbBlocks;
		std::vector<cv::Mat> newCriteriaBlocks;
		string folderName = imageName[imageIndex];
		string imagePath = "C:/Users/issa_/source/repos/PPU/PPU/images/bmp/" + imageName[imageIndex] + imageType;		
		ImageManipulation image(512, 64, 64, cv::imread(imagePath, IMREAD_UNCHANGED), folderName, imageName[imageIndex]);
		ImageManipulation originalImage = image;
		

		cv::Mat input_img(cv::imread(imagePath, 0));

		//std::string file_name("peppers_bit0.png");
		//for (uint32_t i(0); i < 8; ++i) {
			//cv::Mat out(((input_img / (1 << i)) & 1) * 255);
			//cv::imwrite(file_name, out);

			//file_name[11] += 1;
		//}
		
		// Divide iamge into blocks
		clock_t start = clock();
		clock_t end;
		int divideStatus = image.divideImage(blocks);
		//highBlocks= blocks;
		//lowBlocks = blocks;
		edgeBlocks = blocks;
		//msbBlocks = blocks;
		//newCriteriaBlocks = blocks;

		// generate and save the image edges using laplacian
		image.getLaplacian(imageIndex);
		
		// create image directory for saving the output
		cv::utils::fs::createDirectory(image.FolderName);
		
		// definitions
		int msb_sum = 0;
		int edge_sum = 0;
		int msb_average;
		int new_msb_average = 0;
		int edge_average;
		
		// loop through all the blocks
		for (int j = 0; j < blocks.size(); j++)
		{
			// definitions
			std::string blockId = std::to_string(j);			
			ImageManipulation block(64, 8, 8, blocks[j]);// , image.FolderName, imageName[imageIndex]);
			
			// Get MSB count for each block
			int msb = block.getMSB(j);
			
			// Get the edge count for each block
			//int laplacian = block.getLaplacian(imageName[imageIndex], blockId);
			int laplacian = block.getLaplacian(j);
			// save results into an array
			Results[imageIndex][j][0] = msb;
			Results[imageIndex][j][1] = laplacian;
			//get the sum value of the msb of all blocks
			msb_sum += msb;
			//get the sum of the laplacian edge values of all blocks
			edge_sum += laplacian;
		}
		edge_average = edge_sum / totalBlocks;
		msb_average = msb_sum / totalBlocks;
		for (int j = 0; j < blocks.size(); j++)
		{
			new_msb_average += (sqrt(Results[imageIndex][j][0]) + edge_average);
		}
		new_msb_average = new_msb_average / totalBlocks;
		int random_variables_r[4096] = {};
		int random_variables_g[4096] = {};
		int random_variables_b[4096] = {};

		Book* bookLow = xlCreateBook(); // xlCreateXMLBook() for xlsx
		Book* bookEdge = xlCreateBook(); // xlCreateXMLBook() for xlsx
		
		ExportExcel lowExcelExport(bookLow);
		ExportExcel edgeExcelExport(bookEdge);
		int visitedBlocks = 0;
		int visitedBlocksEdge = 0;
		int encryptedBlocksHigh = 0;
		int encryptedBlocksLow = 0;
		int encryptedBlocksMSB = 0;
		string encryptedBlocksEdge = "";
		int encryptedBlocksNew = 0;
		
		int previously_encrypted_index = -1;
		double edge_percentage =0.9;
		
		
			

				if (blocks[0].ptr<Vec3b>(0)[0][0] == 1)
				{
					blocks[0].ptr<Vec3b>(0)[0][0] = 0;
				}
				else {
					blocks[0].ptr<Vec3b>(0)[0][0] = 1;
				}

				if (blocks[0].ptr<Vec3b>(0)[0][1] == 1)
				{
					blocks[0].ptr<Vec3b>(0)[0][1] = 0;
				}
				else {
					blocks[0].ptr<Vec3b>(0)[0][1] = 1;
				}
		

				if (blocks[0].ptr<Vec3b>(0)[0][2] == 1)
				{
					blocks[0].ptr<Vec3b>(0)[0][2] = 0;
				}
				else {
					blocks[0].ptr<Vec3b>(0)[0][2] = 1;
				}

				

		for (int j = 0; j < blocks.size(); j++)
		{			
			std::string blockId = std::to_string(j);
			ImageManipulation block(64, 8, 8, blocks[j], image.FolderName, image.ImageName + "_Block_" + blockId);
			ImageEncryption ecnrypt(block,prng);							
			
			//encrypt the ege block
			if (Results[imageIndex][j][1] >= (edge_average*edge_percentage)				
				//||
				//(results[imageIndex][j][0] >= msb_average && results[imageIndex][j][1] >= edge_average) 
				|| 1 == 1
				) {								
				encryptedBlocksEdge += std::to_string(j) + "-";

				if (cbcMode)
				{
					cv::Mat previous_block;
					cv::Mat current_block;
					current_block = edgeBlocks[j].clone();

					if (previously_encrypted_index != -1) {
						previous_block = edgeBlocks[previously_encrypted_index].clone();
					}
					else {
						//iv_img = cv::Mat(blocks[j].rows, edgeBlocks[j].cols, CV_8UC3);	
						previous_block = IV_img.clone();
					}
					//blockEncrypted = ecnrypt.xorWithPreviousBlock(blockEncrypted, edgeBlocks[j - 1]);
					edgeBlocks[j] = ecnrypt.xorWithPreviousBlock(previous_block, edgeBlocks[j].clone());
					previously_encrypted_index = j;
				}
				
				if (withMSB || withChannelSwap)
					edgeBlocks[j] = ecnrypt.encryptImage(withMSB, withChannelSwap
						//, block.Image.clone());
						, edgeBlocks[j].clone());

				if (withFisher) {
					FisherRandomNumbers[j] = ecnrypt.encryptBlockFisher(edgeBlocks[j]);
					//FisherRandomNumbers[j]=ecnrypt.FisherRandomNumbers;
					//string bock_index = std::to_string(j);
					//std::string decryptedImgName = folderName + "/" + bock_index + "_to_encrypt_.bmp";
					//imwrite(decryptedImgName, ecnrypt.Image.Image);
					//continue;
				}
				if (withBytesXOR)
				{
					edgeBlocks[j] = ecnrypt.encryptImage_alter_fisher(edgeBlocks[j].clone());
				}
				

				if (visitedBlocksEdge == 0 && imageIndex == 0)
				{
					//edgeExcelExport.exportPostPreEncryptionToExcel(totalBlocks, j, imageName[imageIndex], block.Image.clone(), blockEncrypted);
				}
				visitedBlocksEdge++;
			}
			
			/*

			//encrypt the low block			
			if (Results[imageIndex][j][0] <= (msb_average * 1) && Results[imageIndex][j][1] <= (edge_average * 1)) {

				encryptedBlocksLow++;

				lowBlocks[j] = encryptBlock(ecnrypt, withMSB, withChannelSwap, block.Image.clone(),
					withFisher, withBytesXOR, cbcMode, lowBlocks, j);

				if (visitedBlocks == 1 && imageIndex == 0)
				{
					//lowExcelExport.exportPostPreEncryptionToExcel(totalBlocks,j, imageName[imageIndex], block.Image.clone(), blockEncrypted);
				}
				visitedBlocks++;

			}
			//encrypt the msb block			
			if (Results[imageIndex][j][0] >= msb_average ) {	
				
				encryptedBlocksMSB++;

				msbBlocks[j] = encryptBlock(ecnrypt, withMSB, withChannelSwap, block.Image.clone(),
					withFisher, withBytesXOR, cbcMode, msbBlocks, j);
			}
			//encrypt the high block
			if (Results[imageIndex][j][0] >= msb_average && Results[imageIndex][j][1] >= edge_average) {

				encryptedBlocksHigh++;

				highBlocks[j] = encryptBlock(ecnrypt, withMSB, withChannelSwap, block.Image.clone(),
					withFisher, withBytesXOR, cbcMode, highBlocks, j);
			}
			
			if ( (sqrt( Results[imageIndex][j][0])+ edge_average) >= new_msb_average) {

				encryptedBlocksNew++;
				
				newCriteriaBlocks[j] = encryptBlock(ecnrypt, withMSB, withChannelSwap, block.Image.clone(),
					withFisher, withBytesXOR, cbcMode, newCriteriaBlocks, j);
			}
			
			*/
			
			
		}
		end = clock();
		cout << diffclock(end, start) ;



		//lena image excel sheet contains all the blocks in sheets (original and encrypted)
		if (imageIndex == 0)
		{
			//bookLow->save(lowExcelExport.convertStringToTCHAR(imageName[imageIndex]));
			//bookLow->save(L"LowCriteria_LenaImage-BlockPrePostEncryptionNumbers0.xls");
			//bookLow->release();
		}

		//cv::Mat highEncryptedImage = image.gatherImage(highBlocks);		
		//cv::Mat lowEncryptedImage = image.gatherImage(lowBlocks);
		//cv::Mat msbEncryptedImage = image.gatherImage(msbBlocks);
		cv::Mat edgeEncryptedImage = image.gatherImage(edgeBlocks);
		//cv::Mat newCriteriaEncryptedImage = image.gatherImage(newCriteriaBlocks);
		
		



		QualityAssessment qa;

		//double* highPsnrMSE = qa.getPSNRMSE(originalImage.Image, highEncryptedImage);
		//Scalar highScalar = qa.getMSSIM(originalImage.Image, highEncryptedImage);

		//double* lowPsnrMSE = qa.getPSNRMSE(originalImage.Image, lowEncryptedImage);
		//Scalar lowScalar = qa.getMSSIM(originalImage.Image, lowEncryptedImage);

		//double* msbPsnrMSE = qa.getPSNRMSE(originalImage.Image, msbEncryptedImage);
		//Scalar msbScalar = qa.getMSSIM(originalImage.Image, msbEncryptedImage);

		double* edgePsnrMSE = qa.getPSNRMSE(originalImage.Image, edgeEncryptedImage);
		Scalar edgeScalar = qa.getMSSIM(originalImage.Image, edgeEncryptedImage);

		//double* newCriteriaPsnrMSE = qa.getPSNRMSE(originalImage.Image, newCriteriaEncryptedImage);
		//Scalar newCriteriaScalar = qa.getMSSIM(originalImage.Image, newCriteriaEncryptedImage);
		
		double cr = 0;
		float cc = 0;

		std::string originalImgName = originalImage.FolderName + "/original.bmp";
		cc = qa.correlationFloat(edgeEncryptedImage, originalImage.Image);
		//std::string highEncryImgName = generateImageName(originalImage.FolderName,"highEncry_" + process + "_", highPsnrMSE,highScalar);
		//std::string lowEncryImgName = generateImageName(originalImage.FolderName ,"lowEncry_" + process + "_",lowPsnrMSE,lowScalar);
		//std::string msbEncryImgName = generateImageName(originalImage.FolderName , "msbEncry" + process + "_",msbPsnrMSE,msbScalar); //swipe3MSBEncryptedGB_
		//std::string edgeEncryImgName = generateImageName(originalImage.FolderName , "edgeEncry_" + process + "_",edgePsnrMSE,edgeScalar, cc);
		std::string edgeEncryImgName = generateImageName(originalImage.FolderName , "" + process ,edgePsnrMSE,edgeScalar, cc);
		//std::string newCriteriaEncryImgName = generateImageName(originalImage.FolderName , "newCriteriaEncry_"+ process+"_", newCriteriaPsnrMSE, newCriteriaScalar);

		imwrite(edgeEncryImgName, edgeEncryptedImage);
		return 0;

		
		qa.calculateEntropyHistogram(originalImage.Image, folderName, "Original Histogram");
		
		float* histo_results;
		//histo_results = qa.calculateEntropyHistogram(originalImage.Image, folderName, process);
		histo_results = qa.calculateEntropyHistogram(edgeEncryptedImage, folderName, process);

		//Mat histogramImage =qa.histogramRGB(edgeEncryptedImage);
		//std::string histogramName = folderName + "/" +  "Histogram_" +process + ".bmp";

		//correlation is not accurate






		//cr = qa.correlation(originalImage.Image, originalImage.Image);
		//fr = qa.correlationFloat(originalImage.Image, originalImage.Image);


		//cr = qa.correlation(edgeEncryptedImage, originalImage.Image);
		
		
		//imwrite(histogramName, histogramImage);

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////// Decryption Section ////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//decryptImage(highEncryptedImage, "LenaRGB", "highEncryptedImage");
		//decryptImage(lowEncryptedImage, "LenaRGB", "lowEncryptedImage");
		//decryptImage(msbEncryptedImage, "LenaRGB", "msbEncryptedImage");
		
		//decryptImage(edgeEncryptedImage.clone(), "LenaRGB", "edgeEncryptedImage", decryption_prng, withBytesXOR,
			//withFisher, withMSB, withChannelSwap, cbcMode,edge_percentage);
		
		//decryptImage(newCriteriaEncryptedImage, "LenaRGB", "newEncryptedImage");

		
		

		//imwrite(originalImgName, originalImage.Image);
		//imwrite(highEncryImgName, highEncryptedImage);		
		//imwrite(lowEncryImgName, lowEncryptedImage);		
		//imwrite(msbEncryImgName, msbEncryptedImage);
		
		//imwrite(newCriteriaEncryImgName, newCriteriaEncryptedImage);
	


	//, edgeScalar;

	Book* book = xlCreateBook(); // xlCreateXMLBook() for xlsx
	if (book)
	{
		Sheet* sheet = book->addSheet(L"Sheet1");
		if (sheet)
		{
			sheet->writeStr(2, 1, L"Images MSB + Edge Count");
			int seperation = 0;
			int imageIndex = 0;
			
				sheet->writeStr(2 + seperation, 1, L"Cryptoanalysis results");
				sheet->writeStr(3 + seperation, 1, L"MSE#");
				sheet->writeStr(3 + seperation, 2, L"PSNR");
				sheet->writeStr(3 + seperation, 3, L"MSSIM (R)");
				sheet->writeStr(3 + seperation, 4, L"MSSIM (G)");
				sheet->writeStr(3 + seperation, 5, L"MSSIM (B)");
				sheet->writeStr(3 + seperation, 6, L"Information Entropy (Y)");
				sheet->writeStr(3 + seperation, 7, L"Information Entropy (Cr)");
				sheet->writeStr(3 + seperation, 8, L"Information Entropy (Cb)");
				sheet->writeStr(3 + seperation, 9, L"Information Entropy (Avg)");
				sheet->writeStr(3 + seperation, 10, L"Coefficient Correlation");
								
					sheet->writeNum(4 + seperation, 1, edgePsnrMSE[1]);
					sheet->writeNum(4 + seperation, 2, edgePsnrMSE[0]);
					sheet->writeNum(4 + seperation, 3, edgeScalar.val[2]);
					sheet->writeNum(4 + seperation, 4, edgeScalar.val[1]);
					sheet->writeNum(4 + seperation, 5, edgeScalar.val[0]);
					sheet->writeNum(4 + seperation, 6, histo_results[0]);
					sheet->writeNum(4 + seperation, 7, histo_results[1]);
					sheet->writeNum(4 + seperation, 8, histo_results[2]);					
					sheet->writeNum(4 + seperation, 9, ((histo_results[0] + histo_results[1] + histo_results[2])/3.00));
					sheet->writeNum(4 + seperation, 10, cc);
			
			
		}
		//book->save(L"school/M-CBC.xls");
		//book->release();
	}
	}
	// export results to excel sheet
	//Book* book = xlCreateBook(); // xlCreateXMLBook() for xlsx
	//if (book)
	//{
	//	Sheet* sheet = book->addSheet(L"Sheet1");
	//	if (sheet)
	//	{
	//		sheet->writeStr(2, 1, L"Images MSB + Edge Count" );
	//		int seperation = 0;
	//		int imageIndex = 0;
	//		for (int imageIndex = 0; imageIndex < 3; imageIndex++)
	//		{
	//			sheet->writeStr(2 + seperation, 1, L"Images MSB + Edge Count");
	//			sheet->writeStr(3 + seperation, 1, L"B#");
	//			sheet->writeStr(3 + seperation, 2, L"MSB");
	//			sheet->writeStr(3 + seperation, 3, L"Edge");
	//			for (int k = 0; k < totalBlocks; k++) {
	//				sheet->writeNum(4 + k + seperation, 1, k);
	//				sheet->writeNum(4 + k + seperation, 2, results[imageIndex][k][0]);
	//				sheet->writeNum(4 + k + seperation, 3, results[imageIndex][k][1]);
	//			}
	//			seperation += totalBlocks + 4;
	//		}
	//	}
	//	book->save(L"example.xls");
	//	book->release();
	//}
	

	return 0;

}
