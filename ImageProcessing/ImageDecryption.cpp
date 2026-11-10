
#include <random>
#include <iostream>    
#include <thread>
//#include "ImageManipulation.cpp"
#include <opencv2/opencv.hpp>
#include <bitset>
using namespace std;
#include "StaticPRNG.cpp"
using namespace cv;





class ImageDecryption
{
public: cv::Mat Image;
public: StaticPRNG DEC_PRNG;

public: ImageDecryption(cv::Mat image, string prng) {
	DEC_PRNG = StaticPRNG(prng);
	//DEC_PRNG = StaticPRNG(prng);
	Image = image;
};

private: int**  getFisherRandomNumbers() {
	int** fisherRandomNumbers = new int* [512];
	for (int i = 0; i < 512; ++i)
	{
		fisherRandomNumbers[i] = new int[512];

		for (int j = 0; j < 512; j++) {
			fisherRandomNumbers[i][j] = j;
		}
	}

	int counter = 0;
	for (int row = 0; row < Image.rows; row++)
	{
		DEC_PRNG.zeroIndex();
		for (int i = (Image.cols - 1); i >= 0; i--) {
			int random_index;
			if (i != 0)
			{
				random_index = DEC_PRNG.generatePRNG() % (i + 1);
			}
			else {
				random_index = 0;
			}
			int current_index = i;
			fisherRandomNumbers[row][current_index] = random_index;
			fisherRandomNumbers[row][random_index] = current_index;
		}
	}
	return fisherRandomNumbers;
}

public: void decryptFisherYates(
	int** fisherRandomNumbers
) {
	for (int row = 0; row < Image.rows; row++)
	{
		DEC_PRNG.zeroIndex();
		for (int i = 0; i < (Image.cols); i++) {
			//for (int i = (Image.cols-1); i >=0 ; i--) {


			int random_index = fisherRandomNumbers[row][i];// 0,0  => 100
			//int random_index;// = DEC_PRNG.generatePRNG() % i;// 0,0  => 100
			//if (i != 0) {
				//random_index = DEC_PRNG.generatePRNG() % i;
			//}
			//else {
				//random_index = 0;
			//}

			int current_index = i;							//0,100 => temp


			Vec3b temp = Image.ptr<Vec3b>(row)[current_index];
			Image.ptr<Vec3b>(row)[current_index] = Image.ptr<Vec3b>(row)[random_index];
			Image.ptr<Vec3b>(row)[random_index] = temp;

		}
	}
	
	//int** fisherRandomNumbers = getFisherRandomNumbers();
	/*
	for (int row = 0; row < Image.rows; row++)
	{
		DEC_PRNG.zeroIndex();
		//for (int i = 0; i < (Image.cols); i++) {
		for (int i = (Image.cols-1); i >=0 ; i--) {

			//int random_index = fisherRandomNumbers[row][i];// 0,0  => 100
			int random_index;// = DEC_PRNG.generatePRNG() % i;// 0,0  => 100
			if (i != 0) {
				random_index = DEC_PRNG.generatePRNG() % (i+1);
			}
			else {
				random_index = 0;
			}

			int current_index = i;							//0,100 => temp


			Vec3b temp = Image.ptr<Vec3b>(row)[current_index];
			Image.ptr<Vec3b>(row)[current_index] = Image.ptr<Vec3b>(row)[random_index];
			Image.ptr<Vec3b>(row)[random_index] = temp;

		}
	}*/
}

public:  cv::Mat decryptImageMSB(
	//int** iv, int*** randomNumbers,
	bool withChannelSwap, bool withMSB
) {
	DEC_PRNG.zeroIndex();
	int random_variables_r = 0;
	int random_variables_g = 0;
	int random_variables_b = 0;

	int counter = 0;
	cv::Mat encrypted_image = Image.clone();// .clone();

	for (int i = 0; i < Image.rows; i++)
	{

		Vec3b* ptr = Image.ptr<Vec3b>(i);
		Vec3b* ptr_encrypted = encrypted_image.ptr<Vec3b>(i);
		//int previous_encrypted_bit_r = NULL;
		std::bitset<8> previous_encrypted_bit_r = NULL;
		std::bitset<8> previous_encrypted_bit_g = NULL;
		std::bitset<8> previous_encrypted_bit_b = NULL;

		// for loop into each pixel of a row
		//for (int j = Image.cols-1 ; j >=0 ; j--)
		for (int j = 0; j < Image.cols; j++)
		{


			// get random byte for each pixel

			//random_variables_r = randomNumbers[i][j][0];
			//random_variables_g = randomNumbers[i][j][1];
			//random_variables_b = randomNumbers[i][j][2];

			random_variables_r = DEC_PRNG.generatePRNG() % 255;
			random_variables_g = DEC_PRNG.generatePRNG() % 255;
			random_variables_b = DEC_PRNG.generatePRNG() % 255;

			std::bitset<8> iv_r = 0;
			std::bitset<8> iv_g = 0;
			std::bitset<8> iv_b = 0;
			//std::bitset<8> msb_extractor(128);

			// Save the result for the next pixel to be used in the eencryption
			std::bitset<8> previous_pixel_r;
			std::bitset<8> previous_pixel_b;
			std::bitset<8> previous_pixel_g;


			// get the previous encrypted MSB bit value from the previous byte for each RGB channel
			if (j != 0) {

				previous_pixel_r = (int)ptr_encrypted[j - 1].val[0];
				previous_pixel_g = (int)ptr_encrypted[j - 1].val[1];
				previous_pixel_b = (int)ptr_encrypted[j - 1].val[2];


				iv_r = previous_pixel_r;// &msb_extractor;
				iv_g = previous_pixel_g;// & msb_extractor;
				iv_b = previous_pixel_b;// & msb_extractor;
			}
			else {
				//iv_r= (std::bitset<8>) iv[i][0];
				//iv_g= (std::bitset<8>) iv[i][1];
				//iv_b= (std::bitset<8>) iv[i][2];

				iv_r = (std::bitset<8>) (DEC_PRNG.generatePRNG() % 255);
				iv_g = (std::bitset<8>) (DEC_PRNG.generatePRNG() % 255);
				iv_b = (std::bitset<8>) (DEC_PRNG.generatePRNG() % 255);
			}
			//int encrypted_pixel_r = (int)ptr[j][0];
			int encrypted_pixel_r = (int)ptr[j].val[0];
			int encrypted_pixel_g = (int)ptr[j].val[1];
			int encrypted_pixel_b = (int)ptr[j].val[2];


			// get the MSB bit of each byte of each RGB channel
			std::bitset<8> encrypted_msb_r = std::bitset<8>(encrypted_pixel_r);// & msb_extractor;
			std::bitset<8> encrypted_msb_g = std::bitset<8>(encrypted_pixel_g);// & msb_extractor;
			std::bitset<8> encrypted_msb_b = std::bitset<8>(encrypted_pixel_b);//& msb_extractor;
			std::bitset<8> msb_random_variables_r = (std::bitset<8>(random_variables_r));//& msb_extractor;
			std::bitset<8> msb_random_variables_g = (std::bitset<8>(random_variables_g));//& msb_extractor;
			std::bitset<8> msb_random_variables_b = (std::bitset<8>(random_variables_b));//& msb_extractor;



			std::bitset<8> new_decrypted_r = std::bitset<8>(encrypted_pixel_r);
			std::bitset<8> new_decrypted_g = std::bitset<8>(encrypted_pixel_g);
			std::bitset<8> new_decrypted_b = std::bitset<8>(encrypted_pixel_b);


			std::bitset<8> decrypted_msb_r = encrypted_msb_r;
			std::bitset<8> decrypted_msb_g = encrypted_msb_g;
			std::bitset<8> decrypted_msb_b = encrypted_msb_b;


			if (withChannelSwap)
			{
				// map the other 3 MSB for channel R with channel G
				new_decrypted_r.set(6, std::bitset<8>(encrypted_msb_b)[5]);
				new_decrypted_r.set(5, std::bitset<8>(encrypted_msb_b)[4]);
				new_decrypted_r.set(4, std::bitset<8>(encrypted_msb_b)[6]);

				// map the other 3 MSB for channel G with channel B
				new_decrypted_g.set(6, std::bitset<8>(encrypted_msb_r)[5]);
				new_decrypted_g.set(5, std::bitset<8>(encrypted_msb_r)[4]);
				new_decrypted_g.set(4, std::bitset<8>(encrypted_msb_r)[6]);

				// map the other 3 MSB for channel B with channel R
				new_decrypted_b.set(6, std::bitset<8>(encrypted_msb_g)[5]);
				new_decrypted_b.set(5, std::bitset<8>(encrypted_msb_g)[4]);
				new_decrypted_b.set(4, std::bitset<8>(encrypted_msb_g)[6]);
			}





			// XOR the MSB bit
			if (withMSB)
			{
				decrypted_msb_r = encrypted_msb_r ^ iv_r ^ msb_random_variables_r;
				decrypted_msb_g = encrypted_msb_g ^ iv_g ^ msb_random_variables_g;
				decrypted_msb_b = encrypted_msb_b ^ iv_b ^ msb_random_variables_b;

				// replace the decrypted MSB bit back into the RGB channels
				new_decrypted_r.set(7, decrypted_msb_r[7]);
				new_decrypted_g.set(7, decrypted_msb_g[7]);
				new_decrypted_b.set(7, decrypted_msb_b[7]);
			}


			Image.ptr<Vec3b>(i)[j] = Vec3b((new_decrypted_r.to_ulong()), (new_decrypted_g.to_ulong()),
				(new_decrypted_b.to_ulong()));
		}

	}
	return Image;
}

public:  cv::Mat decryptImage_alter_fisher(
	//int*** alterRandomNumbers, int** iv_alter
) {
	DEC_PRNG.zeroIndex();
	int random_variables_r = 0;
	int random_variables_g = 0;
	int random_variables_b = 0;


	int counter = 0;
	cv::Mat encrypted_image = Image.clone();
	// for loop into each row of the block
	//array<array<int, 512>, 512>  randomNumbers = {};

	for (int i = 0; i < Image.rows; i++)
	{

		Vec3b* ptr = Image.ptr<Vec3b>(i);
		Vec3b* ptr_encrypted = encrypted_image.ptr<Vec3b>(i);
		//int previous_encrypted_bit_r = NULL;

		// for loop into each pixel of a row
		for (int j = 0; j < Image.cols; j++)
		{

			// get random byte for each pixel
			//random_variables_r = alterRandomNumbers[i][j][0] ;
			//random_variables_g = alterRandomNumbers[i][j][1] ;
			//random_variables_b = alterRandomNumbers[i][j][2] ;

			random_variables_r = DEC_PRNG.generatePRNG() % 255;
			random_variables_g = DEC_PRNG.generatePRNG() % 255;
			random_variables_b = DEC_PRNG.generatePRNG() % 255;

		

			std::bitset<8> iv_r;
			std::bitset<8> iv_g;
			std::bitset<8> iv_b;

			// Save the result for the next pixel to be used in the decryption
			std::bitset<8> previous_pixel_r;
			std::bitset<8> previous_pixel_b;
			std::bitset<8> previous_pixel_g;

			// get the previous encrypted MSB bit value from the previous byte for each RGB channel
			if (j != 0) {

				previous_pixel_r = (int)ptr_encrypted[j - 1].val[0];
				previous_pixel_g = (int)ptr_encrypted[j - 1].val[1];
				previous_pixel_b = (int)ptr_encrypted[j - 1].val[2];

				iv_r = previous_pixel_r;
				iv_g = previous_pixel_g;
				iv_b = previous_pixel_b;
			}
			else {
				//IV;
				//iv_r = (std::bitset<8>) iv_alter[i][0];
				//iv_g = (std::bitset<8>) iv_alter[i][1];
				//iv_b = (std::bitset<8>) iv_alter[i][2];

				iv_r = DEC_PRNG.generatePRNG() % 255;
				iv_g = DEC_PRNG.generatePRNG() % 255;
				iv_b = DEC_PRNG.generatePRNG() % 255;
			}
			int encrypted_pixel_r = (int)ptr[j].val[0];
			int encrypted_pixel_g = (int)ptr[j].val[1];
			int encrypted_pixel_b = (int)ptr[j].val[2];

			// get the MSB bit of each byte of each RGB channel
			std::bitset<8> msb_r = std::bitset<8>(encrypted_pixel_r);
			std::bitset<8> msb_g = std::bitset<8>(encrypted_pixel_g);
			std::bitset<8> msb_b = std::bitset<8>(encrypted_pixel_b);
			std::bitset<8> msb_random_variables_r = (std::bitset<8>(random_variables_r));
			std::bitset<8> msb_random_variables_g = (std::bitset<8>(random_variables_g));
			std::bitset<8> msb_random_variables_b = (std::bitset<8>(random_variables_b));

			// XOR the MSB bit
			std::bitset<8> decrypted_r = msb_r ^ iv_r ^ msb_random_variables_r;
			std::bitset<8> decrypted_g = msb_g ^ iv_g ^ msb_random_variables_g;
			std::bitset<8> decrypted_b = msb_b ^ iv_b ^ msb_random_variables_b;


			// replace the encrypted pixel back into the block			 
			Image.ptr<Vec3b>(i)[j] = Vec3b((int)(decrypted_r.to_ulong()), (int)(decrypted_g.to_ulong()), (int)(decrypted_b.to_ulong()));
		}

	}
	// return the block

	return Image;
}

public: cv::Mat xorWithPreviousBlock(cv::Mat previousBlock, cv::Mat currentBlock) {
		  cv::Mat newBlock = currentBlock;

		  for (int i = 0; i < previousBlock.rows; i++)
		  {
			  Vec3b* previousBlockPtr = previousBlock.ptr<Vec3b>(i);
			  Vec3b* currentBlockPtr = currentBlock.ptr<Vec3b>(i);
			  Vec3b* newBlockPtr = newBlock.ptr<Vec3b>(i);

			  for (int j = 0; j < previousBlock.cols; j++)
			  {
				  newBlockPtr[j][0] = previousBlockPtr[j][0] ^ currentBlockPtr[j][0];
				  newBlockPtr[j][1] = previousBlockPtr[j][1] ^ currentBlockPtr[j][1];
				  newBlockPtr[j][2] = previousBlockPtr[j][2] ^ currentBlockPtr[j][2];
			  }
		  }

		  return newBlock;

	  }

};

