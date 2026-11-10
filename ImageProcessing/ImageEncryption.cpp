#include <random>
#include "ImageManipulation.cpp"
#include <bitset>
#include "cryptopp/include/cryptopp/dll.h"    
#include "cryptopp/include/cryptopp/default.h"    
#include "cryptopp/include/cryptopp/osrng.h"    
#include "cryptopp/include/cryptopp/aes.h"    
#include "cryptopp/include/cryptopp/ccm.h"    
#include <iostream>    

#include <thread>
#include "StaticPRNG.cpp"
//using CryptoPP::AutoSeededRandomPool;
//using CryptoPP::AES;


using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;
//using namespace libxl;
//using namespace cv;
//using namespace std;



 class ImageEncryption {
 
 public: ImageManipulation Image;
 public: StaticPRNG PRNG;
 public: int** FisherRandomNumbers = new int* [512];


 public: ImageEncryption(ImageManipulation image, string prng)  {
	 Image = image;
	 PRNG = StaticPRNG(prng);

	 for (int i = 0; i < 512; ++i)
	 {
		 FisherRandomNumbers[i] = new int[512];
	 }
 }

 private: array<int, 64> fisherYates(cv::Mat &image, int row) {
	 //AutoSeededRandomPool prng;
	 //CryptoPP::byte key[AES::DEFAULT_KEYLENGTH];
	 //prng.GenerateBlock(key, sizeof(key));

	 //byte iv[AES::BLOCKSIZE];
	 //iv[AES::BLOCKSIZE] = 0;


	 std::srand(std::time(nullptr));	 
	 array<int, 64> randomNumbers = {};

	 for (int i = (image.cols - 1); i > 0; i--) {
		 randomNumbers[i] = i;
	 }

	 for (int i = (image.cols-1); i > 0; i--) {		 
		 std::srand(std::time(nullptr));
		 //int random_index = (int)std::rand() % i;
		 int random_index = PRNG.generatePRNG() % i;
		 int current_index = i;
		 int temp_index;
		 
		 Vec3b temp = image.ptr<Vec3b>(row)[current_index];

		 image.ptr<Vec3b>(row)[current_index] = image.ptr<Vec3b>(row)[random_index];
		 image.ptr<Vec3b>(row)[random_index] = temp;
		 temp_index = randomNumbers[current_index];
		 randomNumbers[current_index] = randomNumbers[random_index];
		 randomNumbers[random_index] = temp_index;
	 }

	 return randomNumbers;
	   }

 public:  cv::Mat encryptImage_alter_fisher(cv::Mat blockEncrypted) {

	 //int random_variables_r[262144] = {};
	 //int random_variables_g[262144] = {};
	 //int random_variables_b[262144] = {};
	 int random_variables_r = 0;
	 int random_variables_g = 0;
	 int random_variables_b = 0;
	 std::ofstream outfile;


	 //int counter = 0;
	 // for loop into each row of the block
	 //array<array<int, 512>, 512>  randomNumbers = {};
	 PRNG.zeroIndex();

	 for (int i = 0; i < blockEncrypted.rows; i++)
	 {

		 Vec3b* ptr = blockEncrypted.ptr<Vec3b>(i);
		 //int previous_encrypted_bit_r = NULL;
		 std::bitset<8> previous_encrypted_bit_r = NULL;
		 std::bitset<8> previous_encrypted_bit_g = NULL;
		 std::bitset<8> previous_encrypted_bit_b = NULL;

		 // for loop into each pixel of a row
		 for (int j = 0; j < blockEncrypted.cols; j++)
		 {
			 //int random_index = ((blockEncrypted.rows) * i) + j;
			 // generate random byte for each pixel
			 //if (random_variables_r == 0 || random_variables_g == 0 || random_variables_b == 0) {
				 //std::srand(std::time(nullptr)); // use current time as seed for random generator
				 //random_variables_r = (int)std::rand() % 255;
				 random_variables_r = PRNG.generatePRNG() % 255;
				 //std::srand(std::time(nullptr));
				 //random_variables_g = (int)std::rand() % 255;
				 random_variables_g = PRNG.generatePRNG() % 255;
				 //std::srand(std::time(nullptr));
				 //random_variables_b = (int)std::rand() % 255;
				 random_variables_b = PRNG.generatePRNG() % 255;


				 //outfile.open("keys.txt", std::ios_base::app);
				 //outfile << std::to_string(random_variables_r) + " ";
				 //outfile << std::to_string(random_variables_g) + " ";
				 //outfile << std::to_string(random_variables_b) + " ";
				 //outfile.close();



			 //}

			 cv::Mat resultLAB_r;
			 cv::Mat resultLAB_g;
			 cv::Mat resultLAB_b;

			 std::bitset<8> iv_r;
			 std::bitset<8> iv_g;
			 std::bitset<8> iv_b;

			 // get the previous encrypted MSB bit value from the previous byte for each RGB channel
			 if (j != 0) {
				 iv_r = previous_encrypted_bit_r;
				 iv_g = previous_encrypted_bit_g;
				 iv_b = previous_encrypted_bit_b;
			 }
			 else {
				 iv_r = PRNG.generatePRNG() % 255;
				 iv_g = PRNG.generatePRNG() % 255;
				 iv_b = PRNG.generatePRNG() % 255;
			 }
			 int pixel_r = (int)ptr[j][0];
			 int pixel_g = (int)ptr[j][1];
			 int pixel_b = (int)ptr[j][2];

			 // get the MSB bit of each byte of each RGB channel
			 std::bitset<8> msb_r = std::bitset<8>(pixel_r);
			 std::bitset<8> msb_g = std::bitset<8>(pixel_g);
			 std::bitset<8> msb_b = std::bitset<8>(pixel_b);
			 std::bitset<8> msb_random_variables_r = (std::bitset<8>(random_variables_r));
			 std::bitset<8> msb_random_variables_g = (std::bitset<8>(random_variables_g));
			 std::bitset<8> msb_random_variables_b = (std::bitset<8>(random_variables_b));

			 // XOR the MSB bit
			 std::bitset<8> encrypted_r = msb_r ^ iv_r ^ msb_random_variables_r;
			 std::bitset<8> encrypted_g = msb_g ^ iv_g ^ msb_random_variables_g;
			 std::bitset<8> encrypted_b = msb_b ^ iv_b ^ msb_random_variables_b;



			 // Save the result for the next pixel to be used in the eencryption
			 previous_encrypted_bit_r = encrypted_r;
			 previous_encrypted_bit_g = encrypted_g;
			 previous_encrypted_bit_b = encrypted_b;


			 // replace the encrypted pixel back into the block			 
			 blockEncrypted.ptr<Vec3b>(i)[j] = Vec3b((int)(encrypted_r.to_ulong()), (int)(encrypted_g.to_ulong()), (int)(encrypted_b.to_ulong()));
		 }

	 }
	 // return the block

	 return blockEncrypted;
 }
 public:  cv::Mat encryptImage(bool withFisher,bool withMSB,bool withChannelSwap,cv::Mat image, int(&random_variables_r)[4096], int(&random_variables_g)[4096], int(&random_variables_b)[4096],int test=0) {
	 
	 std::ofstream outfile;
	

	 int counter = 0;	 
	 // for loop into each row of the block
	 array<array<int, 64>, 64>  randomNumbers = {};
	 for (int i = 0; i < image.rows; i++)
	 {
		 Vec3b* ptr = image.ptr<Vec3b>(i);
		 //int previous_encrypted_bit_r = NULL;
		 std::bitset<8> previous_encrypted_bit_r = NULL;
		 std::bitset<8> previous_encrypted_bit_g = NULL;
		 std::bitset<8> previous_encrypted_bit_b = NULL;
		 
		 // for loop into each pixel of a row
		 for (int j = 0; j < image.cols; j++)
		 {
			 
			 int random_index = ((image.rows) * i) + j;			 
			 // generate random byte for each pixel
			 if (random_variables_r[random_index] == 0 || random_variables_g[random_index] == 0 || random_variables_b[random_index] == 0) {				 
				 std::srand(std::time(nullptr)); // use current time as seed for random generator
				 //random_variables_r[random_index] = (int)std::rand() % 255;
				 random_variables_r[random_index] = PRNG.generatePRNG() % 255;
				 std::srand(std::time(nullptr));
				 //random_variables_g[random_index] = (int)std::rand() % 255;
				 random_variables_g[random_index] = PRNG.generatePRNG() % 255;
				 std::srand(std::time(nullptr));
				 //random_variables_b[random_index] = (int)std::rand() % 255;
				 random_variables_b[random_index] = PRNG.generatePRNG() % 255;
				 

				outfile.open("keys.txt", std::ios_base::app);				 
				outfile << std::to_string(random_variables_r[random_index]) + " ";
				outfile << std::to_string(random_variables_g[random_index]) + " ";
				outfile << std::to_string(random_variables_b[random_index]) + " ";
				outfile.close();
				


			 }

			 cv::Mat resultLAB_r;
			 cv::Mat resultLAB_g;
			 cv::Mat resultLAB_b;
			 
			 std::bitset<8> iv_r;
			 iv_r.set(7, 0);
			 std::bitset<8> iv_g;
			 iv_g.set(7, 1);
			 std::bitset<8> iv_b;
			 iv_b.set(7, 1);

			 std::bitset<8> msb_extractor(128);

			 // get the previous encrypted MSB bit value from the previous byte for each RGB channel
			 if (j != 0) {				 
				 iv_r = previous_encrypted_bit_r ;
				 iv_g = previous_encrypted_bit_g ;
				 iv_b = previous_encrypted_bit_b ;
			 }
			 int pixel_r = (int) ptr[j][0];
			 int pixel_g = (int) ptr[j][1];
			 int pixel_b = (int) ptr[j][2];		

			 std::bitset<8> new_msb_r = std::bitset<8>(pixel_r);
			 std::bitset<8> new_msb_g = std::bitset<8>(pixel_g);
			 std::bitset<8> new_msb_b = std::bitset<8>(pixel_b);
			 
			 if(withMSB)	
			 {
				 // get the MSB bit of each byte of each RGB channel
				 std::bitset<8> msb_r = std::bitset<8>(pixel_r) & msb_extractor;
				 std::bitset<8> msb_g = std::bitset<8>(pixel_g) & msb_extractor;
				 std::bitset<8> msb_b = std::bitset<8>(pixel_b) & msb_extractor;
				 std::bitset<8> msb_random_variables_r = (std::bitset<8>(random_variables_r[random_index])) & msb_extractor;
				 std::bitset<8> msb_random_variables_g = (std::bitset<8>(random_variables_g[random_index])) & msb_extractor;
				 std::bitset<8> msb_random_variables_b = (std::bitset<8>(random_variables_b[random_index])) & msb_extractor;

				 // XOR the MSB bit
				 std::bitset<8> encrypted_r = msb_r ^ iv_r ^ msb_random_variables_r;
				 std::bitset<8> encrypted_g = msb_g ^ iv_g ^ msb_random_variables_g;
				 std::bitset<8> encrypted_b = msb_b ^ iv_b ^ msb_random_variables_b;



				 // Save the result for the next pixel to be used in the eencryption
				 previous_encrypted_bit_r = encrypted_r;
				 previous_encrypted_bit_g = encrypted_g;
				 previous_encrypted_bit_b = encrypted_b;		

				 // replace the encrypted MSB bit back into the RGB channels
				 new_msb_r.set(7, encrypted_r[7]);
				 new_msb_g.set(7, encrypted_g[7]);
				 new_msb_b.set(7, encrypted_b[7]);

			 }

			 if(withChannelSwap)
			 {
				 // map the other 3 MSB for channel R with channel G
				 new_msb_r.set(6, std::bitset<8>(pixel_g)[4]);
				 new_msb_r.set(5, std::bitset<8>(pixel_g)[6]);
				 new_msb_r.set(4, std::bitset<8>(pixel_g)[5]);

				 // map the other 3 MSB for channel G with channel B
				 new_msb_g.set(6, std::bitset<8>(pixel_b)[4]);
				 new_msb_g.set(5, std::bitset<8>(pixel_b)[6]);
				 new_msb_g.set(4, std::bitset<8>(pixel_b)[5]);

				 // map the other 3 MSB for channel B with channel R
				 new_msb_b.set(6, std::bitset<8>(pixel_r)[4]);
				 new_msb_b.set(5, std::bitset<8>(pixel_r)[6]);
				 new_msb_b.set(4, std::bitset<8>(pixel_r)[5]);
			 }
				
			 
			 // replace the encrypted pixel back into the block			 
			 image.ptr<Vec3b>(i)[j] = Vec3b((int)(new_msb_r.to_ulong()), (int)(new_msb_g.to_ulong()), (int)(new_msb_b.to_ulong()));			 			 			 
		 }

		 //shuffle the columns using fisher-yates method

		 if (withFisher) {
			 randomNumbers[i] = fisherYates(image, i);
		 }
		 

	 }
	 // return the block
	 
	 return image;
 }

 public:  cv::Mat encryptImage(bool withMSB, bool withChannelSwap, cv::Mat block) {
	 PRNG.zeroIndex();
	 int random_variables_r = 0;
	 int random_variables_g = 0;
	 int random_variables_b = 0;
	 std::ofstream outfile;


	 int counter = 0;
	 // for loop into each row of the block
	 //array<array<int, 512>, 512>  randomNumbers = {};

	 for (int i = 0; i < block.rows; i++)
	 {

		 Vec3b* ptr = block.ptr<Vec3b>(i);
		 //int previous_encrypted_bit_r = NULL;
		 std::bitset<8> previous_encrypted_bit_r = NULL;
		 std::bitset<8> previous_encrypted_bit_g = NULL;
		 std::bitset<8> previous_encrypted_bit_b = NULL;

		 // for loop into each pixel of a row
		 for (int j = 0; j < block.cols; j++)
		 {

			 int random_index = ((block.rows) * i) + j;
			 // generate random byte for each pixel
			 //if (random_variables_r == 0 || random_variables_g == 0 || random_variables_b == 0) {
			 std::srand(std::time(nullptr)); // use current time as seed for random generator
			 //random_variables_r = (int)std::rand() % 255;				
			 random_variables_r = PRNG.generatePRNG() % 255;
			 std::srand(std::time(nullptr));
			 //random_variables_g = (int)std::rand() % 255;
			 random_variables_g = PRNG.generatePRNG() % 255;
			 std::srand(std::time(nullptr));
			 //random_variables_b = (int)std::rand() % 255;
			 random_variables_b = PRNG.generatePRNG() % 255;


			 //outfile.open("keys.txt", std::ios_base::app);
			 //outfile << std::to_string(random_variables_r) + " ";
			 //outfile << std::to_string(random_variables_g) + " ";
			 //outfile << std::to_string(random_variables_b) + " ";
			 //outfile.close();



		 //}

			 std::bitset<8> iv_r;
			 std::bitset<8> iv_g;
			 std::bitset<8> iv_b;
			 //std::bitset<8> msb_extractor(128);

			 // get the previous encrypted MSB bit value from the previous byte for each RGB channel
			 if (j != 0) {
				 iv_r = previous_encrypted_bit_r;
				 iv_g = previous_encrypted_bit_g;
				 iv_b = previous_encrypted_bit_b;
			 }
			 else {
				 int iv_rand_r;
				 int iv_rand_g;
				 int iv_rand_b;
				 std::srand(std::time(nullptr));
				 //iv_rand_r = (int)std::rand() % 255;
				 iv_rand_r = PRNG.generatePRNG() % 255;
				 std::srand(std::time(nullptr));
				 //iv_rand_g = (int)std::rand() % 255;
				 iv_rand_g = PRNG.generatePRNG() % 255;
				 std::srand(std::time(nullptr));
				 //iv_rand_b = (int)std::rand() % 255;
				 iv_rand_b = PRNG.generatePRNG() % 255;



				 iv_r = iv_rand_r;
				 iv_g = iv_rand_g;
				 iv_b = iv_rand_b;

			 }
			 int pixel_r = (int)ptr[j].val[0];
			 int pixel_g = (int)ptr[j].val[1];
			 int pixel_b = (int)ptr[j].val[2];

			 // get the MSB bit of each byte of each RGB channel
			 std::bitset<8> msb_r = std::bitset<8>(pixel_r);// &msb_extractor;
			 std::bitset<8> msb_g = std::bitset<8>(pixel_g);//& msb_extractor;
			 std::bitset<8> msb_b = std::bitset<8>(pixel_b);//& msb_extractor;
			 std::bitset<8> msb_random_variables_r = (std::bitset<8>(random_variables_r));//& msb_extractor;
			 std::bitset<8> msb_random_variables_g = (std::bitset<8>(random_variables_g));//& msb_extractor;
			 std::bitset<8> msb_random_variables_b = (std::bitset<8>(random_variables_b));//& msb_extractor;



			 std::bitset<8> new_msb_r = std::bitset<8>(pixel_r);
			 std::bitset<8> new_msb_g = std::bitset<8>(pixel_g);
			 std::bitset<8> new_msb_b = std::bitset<8>(pixel_b);

			 // XOR the MSB bit
			 if (withMSB)
			 {

				 std::bitset<8> encrypted_msb_r = msb_r ^ iv_r ^ msb_random_variables_r;
				 std::bitset<8> encrypted_msb_g = msb_g ^ iv_g ^ msb_random_variables_g;
				 std::bitset<8> encrypted_msb_b = msb_b ^ iv_b ^ msb_random_variables_b;

				 // replace the encrypted MSB bit back into the RGB channels
				 new_msb_r.set(7, encrypted_msb_r[7]);
				 new_msb_g.set(7, encrypted_msb_g[7]);
				 new_msb_b.set(7, encrypted_msb_b[7]);

				 // Save the result for the next pixel to be used in the eencryption
				 previous_encrypted_bit_r = new_msb_r;
				 previous_encrypted_bit_g = new_msb_g;
				 previous_encrypted_bit_b = new_msb_b;

			 }

			 if (withChannelSwap)
			 {
				 // map the other 3 MSB for channel R with channel G
				 new_msb_r.set(6, std::bitset<8>(pixel_g)[4]);
				 new_msb_r.set(5, std::bitset<8>(pixel_g)[6]);
				 new_msb_r.set(4, std::bitset<8>(pixel_g)[5]);

				 // map the other 3 MSB for channel G with channel B
				 new_msb_g.set(6, std::bitset<8>(pixel_b)[4]);
				 new_msb_g.set(5, std::bitset<8>(pixel_b)[6]);
				 new_msb_g.set(4, std::bitset<8>(pixel_b)[5]);

				 // map the other 3 MSB for channel B with channel R
				 new_msb_b.set(6, std::bitset<8>(pixel_r)[4]);
				 new_msb_b.set(5, std::bitset<8>(pixel_r)[6]);
				 new_msb_b.set(4, std::bitset<8>(pixel_r)[5]);
			 }


			 // replace the encrypted pixel back into the block			 
			 block.ptr<Vec3b>(i)[j] = Vec3b((int)(new_msb_r.to_ulong()), (int)(new_msb_g.to_ulong()), (int)(new_msb_b.to_ulong()));
		 }

		 //shuffle the columns using fisher-yates method
		 //randomNumbers[i]=fisherYates(block,i);
		 //if (withFisher)
		 //{
			 //fisherYates(i);
		 //}

	 }
	 // return the block

	 return block;
 }
 public: cv::Mat encryptImageFisher(cv::Mat block ) {
	 PRNG.zeroIndex();
	 int random_variables_r = 0;
	 int random_variables_g = 0;
	 int random_variables_b = 0;
	 std::ofstream outfile;


	 int counter = 0;
	 // for loop into each row of the block
	 for (int row = 0; row < block.rows; row++)
	 {
		 PRNG.zeroIndex();
		 //for (int i = 0; i < (Image.Image.cols); i++) {
		 for (int i = (block.cols - 1); i >=0; i--) {
			 //std::srand(std::time(nullptr));
			 int random_index;
			 if (i != 0)
			 {
				 //random_index = (int)std::rand() % i;
				 random_index = PRNG.generatePRNG() % (block.cols - 1);
			 }
			 else {
				 random_index = 0;// PRNG.generatePRNG() % Image.Image.cols;
			 }

			 int current_index = i;

			 Vec3b temp = block.ptr<Vec3b>(row)[current_index];
			 block.ptr<Vec3b>(row)[current_index] = block.ptr<Vec3b>(row)[random_index];
			 
			 block.ptr<Vec3b>(row)[random_index] = temp;
			 
			 FisherRandomNumbers[row][current_index] = random_index;
			 FisherRandomNumbers[row][random_index] = current_index;

		 }


	 }
	 return block;
	 // return the block

	 //return Image.Image;
 }

 public:  cv::Mat encryptImage_Feb(cv::Mat image, int(&random_variables)[4096]) {
	 int counter = 0;
	 cv::Mat* blocksXORed = new cv::Mat[(image.rows * image.cols)];
	 for (int i = 0; i < image.rows; i++)
	 {
		 Vec3b* ptr = image.ptr<Vec3b>(i);
		 for (int j = 0; j < image.cols; j++)
		 {
			 int random_index = ((image.rows) * i) + j;

			 if (random_variables[random_index] == 0) {
				 std::srand(std::time(nullptr)); // use current time as seed for random generator
				 //random_variables[random_index] = (int)std::rand() % 255;
				 random_variables[random_index] = PRNG.generatePRNG() % 255;
			 }


			 cv::Mat resultLAB;
			 Vec3b iv = 234;
			 if (j != 0) {
				 iv = ptr[j - 1];
			 }
			 Vec3b pixel = ptr[j];
			 cv::bitwise_xor(pixel, iv, resultLAB);
			 cv::bitwise_xor(resultLAB, random_variables[random_index], resultLAB);
			 image.ptr<Vec3b>(i)[j] = resultLAB;
			 pixel = resultLAB;
		 }
	 }
	 return image;
 }

public: cv::Mat xorWithPreviousBlock(cv::Mat previousBlock, cv::Mat currentBlock) {
	cv::Mat newBlock = currentBlock.clone();

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

public: int** encryptBlockFisher(cv::Mat Image) {
		  PRNG.zeroIndex();
		  int random_variables_r = 0;
		  int random_variables_g = 0;
		  int random_variables_b = 0;
		  std::ofstream outfile;


		  int counter = 0;
		  // for loop into each row of the block
		  for (int row = 0; row < Image.rows; row++)
		  {
			  PRNG.zeroIndex();
			  int previous_random_index;
			  //for (int i = 0; i < (Image.Image.cols); i++) {
			  for (int i = (Image.cols - 1); i >= 0; i--) {
				  //std::srand(std::time(nullptr));
				  int random_index;

				  if (i != 0)
				  {
					  //random_index = (int)std::rand() % i;
					  //if (i == (Image.Image.cols - 1)) {
					  random_index = PRNG.generatePRNG() % (i + 1);
					  //}else
					  //{
						  //do
						  //{
							  //random_index = PRNG.generatePRNG() % i;
						  //} while (previous_random_index == random_index && i != 1);
					  //}
				  }
				  else {
					  random_index = 0;// PRNG.generatePRNG() % Image.Image.cols;				
				  }
				  previous_random_index = random_index;

				  int current_index = i;

				  Vec3b temp = Image.ptr<Vec3b>(row)[current_index];

				  Image.ptr<Vec3b>(row)[current_index] = Image.ptr<Vec3b>(row)[random_index];
				  Image.ptr<Vec3b>(row)[random_index] = temp;

				  FisherRandomNumbers[row][current_index] = random_index;

				  FisherRandomNumbers[row][random_index] = current_index;
			  }

			  
		  }
		  return FisherRandomNumbers;
		  // return the block

		  //return Image.Image;
	  }
};