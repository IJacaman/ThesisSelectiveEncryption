
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/utils/filesystem.hpp>
#include <libxl.h>
#include "libxl.h"
#include <tchar.h>
using namespace cv;
using namespace std;

using namespace libxl;



#include <bitset>


class ExportExcel {

	Book* ExcelBook;

public: ExportExcel(Book* book){
	ExcelBook = book;
}

private:string bitsToString(std::bitset<8> bits) {
	//string output = ""; 
	//for(int bit=0;bit<8;bit++)
	//{
//		output+=""+ std::to_string((int)bits[bit]);
//	}
	return bits.to_string();
}

public:TCHAR* convertStringToTCHAR(string input) {	
	size_t size = input.length();
	TCHAR* output= new TCHAR[size];
	for (size_t i = 0; i < size; ++i)
		output[i] = (input[i]);
	return output;
}


public:void exportPostPreEncryptionToExcel(int totalBlocks,int blockNumber,string imageName,
	cv::Mat originalImage, cv::Mat encryptedImage) {
	// export results to excel sheet
	//Book* book = xlCreateBook(); // xlCreateXMLBook() for xlsx
	 
	string blockName = imageName + "_block_number_"+ std::to_string(blockNumber);
	
	

	if (ExcelBook)
	{
		//Sheet* sheet = ExcelBook->addSheet(convertStringToTCHAR(blockName));
		Sheet* sheet = ExcelBook->addSheet(L"Block0");

		if (sheet)
		{			
			sheet->writeStr(1 , 1, L"R_Original_Byte");
			sheet->writeStr(1 , 2, L"G_Original_Byte");
			sheet->writeStr(1 , 3, L"B_Original_Byte");

			sheet->writeStr(1 , 4, L"R_Encrypted_Byte");
			sheet->writeStr(1 , 5, L"G_Encrypted_Byte");
			sheet->writeStr(1 , 6, L"B_Encrypted_Byte");
			int k = 0;

			for (int i = 0; i < originalImage.rows; i++)
			{
				Vec3b* originalPtr = originalImage.ptr<Vec3b>(i);
				Vec3b* encryptedPtr = encryptedImage.ptr<Vec3b>(i);
				for (int j = 0; j < originalImage.cols; j++)
				{
					int random_index = ((originalImage.rows) * i) + j;

					int originalPixel_r = originalPtr[j][0];
					int originalPixel_g = originalPtr[j][1];
					int originalPixel_b = originalPtr[j][2];
					int encryptedlPixel_r = encryptedPtr[j][0];
					int encryptedlPixel_g = encryptedPtr[j][1];
					int encryptedlPixel_b = encryptedPtr[j][2];

					std::bitset<8> originalPixel_r_in_bits = std::bitset<8>(originalPixel_r);
					std::bitset<8> originalPixel_g_in_bits = std::bitset<8>(originalPixel_g);
					std::bitset<8> originalPixel_b_in_bits = std::bitset<8>(originalPixel_b);

					std::bitset<8> encryptedPixel_r_in_bits = std::bitset<8>(encryptedlPixel_r);
					std::bitset<8> encryptedPixel_g_in_bits = std::bitset<8>(encryptedlPixel_g);
					std::bitset<8> encryptedPixel_b_in_bits = std::bitset<8>(encryptedlPixel_b);

					
					

					//sheet->writeNum(2 + k , 1, std::stoi(bitsToString(originalPixel_r_in_bits)));
					sheet->writeNum(2 + k , 1, originalPixel_r);
					//sheet->writeNum(2 + k , 2, std::stoi(bitsToString(originalPixel_g_in_bits)));
					sheet->writeNum(2 + k , 2, originalPixel_g);
					//sheet->writeNum(2 + k , 3, std::stoi(bitsToString(originalPixel_b_in_bits)));
					sheet->writeNum(2 + k , 3, originalPixel_b);
										
					//sheet->writeNum(2 + k, 4, std::stoi(bitsToString(encryptedPixel_r_in_bits)));
					sheet->writeNum(2 + k, 4, encryptedlPixel_r);
					//sheet->writeNum(2 + k, 5, std::stoi(bitsToString(encryptedPixel_g_in_bits)));
					sheet->writeNum(2 + k, 5, encryptedlPixel_g);
					//sheet->writeNum(2 + k, 6, std::stoi(bitsToString(encryptedPixel_b_in_bits)));
					sheet->writeNum(2 + k, 6, encryptedlPixel_b);

					k++;					
				}
			}
		}			
	}
}
};



