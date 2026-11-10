/*
#include <string>
#include <bitset>
using namespace std;
//do psedo andom generator
// do CBC encryption

static int IndexCounter = 0;

class StaticPRNG
{
string PRNG = "0001100101000111011110001010111110111011011001100010010010101000001011100001011000000000111100101000000010000010011011111110";
string ModifiedPRNG = "10010010001001001010000111110111000111100000110000110110000011010111011111010110101100100110100011001001110011001110010111101000";


public: StaticPRNG()
{

};

public:static int generatePRNG() {
	extern int IndexCounter;
	int startIndex = IndexCounter;
	int endIndex = startIndex + 7;

	std::bitset<8> bits;
	//bits = (std::bitset<8>) PRNG.substr(startIndex,8);


	//IndexCounter = ((IndexCounter + 8) % PRNG.length());

	return (int) bits.to_ullong();
};

};






*/

#ifndef StaticPRNG_HEADER
#define StaticPRNG_HEADER

#include <string>
#include <bitset>
using namespace std;



class StaticPRNG {

private: string PRNG;// = "00011001010001110111100010101111101110110110011000100100101010000010111000010110000000001111001010000000100000100110111111100000";
	   //string ModifiedPRNG = "10010010001001001010000111110111000111100000110000110110000011010111011111010110101100100110100011001001110011001110010111101000";
private: int IndexCounter;
public: StaticPRNG(string secretKey) {
	PRNG = secretKey;
	IndexCounter = 0;
}
public: StaticPRNG() {
	PRNG = "00011001010001110111100010101111101110110110011000100100101010000010111000010110000000001111001010000000100000100110111111100000";
	IndexCounter = 0;
}
public:int generatePRNG() {
	int startIndex = IndexCounter;
	int endIndex = startIndex + 7;
	std::bitset<8> bits;
	bits = (std::bitset<8>) PRNG.substr(startIndex, 8);
	IndexCounter = ((IndexCounter + 8) % PRNG.length());
	return (int)bits.to_ullong();
};

public: void zeroIndex() {
	IndexCounter = 0;

}

};

#endif
