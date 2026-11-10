#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

class QualityAssessment {

public: QualityAssessment() {}

public: double* getPSNRMSE(const Mat& I1, const Mat& I2)
{
	double* result = new double[2];
	result[0]=0.0;
	result[1]=0.0;
	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
	s1 = s1.mul(s1);           // |I1 - I2|^2
	Scalar s = sum(s1);         // sum elements per channel
	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels
	if (sse <= 1e-10) // for small values return zero
		return result ;
	else
	{
		double  mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255 * 255) / mse);
		result[1] = mse;
		result[0] = psnr;
		return result;
	}
}

public: Scalar getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);
	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2
	/*************************** END INITS **********************************/
	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}
	  
	  
float entropy(Mat seq, Size size, int index)
	  {
		  int cnt = 0;
		  float entr = 0;
		  float total_size = size.height * size.width; //total size of all symbols in an image

		  for (int i = 0; i < index; i++)
		  {
			  float sym_occur = seq.at<float>(0, i); //the number of times a sybmol has occured

			  if (sym_occur > 0) //log of zero goes to infinity
			  {
				  cnt++;
				  entr += (sym_occur / total_size) * (log2(total_size / sym_occur));
			  }
		  }
		  std::cout << "cnt: " << cnt << std::endl;
		  return entr;

	  }

	  // myEntropy calculates relative occurrence of different symbols within given input sequence using histogram
	  Mat myEntropy(Mat seq, int histSize)
	  {

		  float range[] = { 0, 256 };
		  const float* histRange = { range };

		  bool uniform = true;
		  bool accumulate = false;

		  Mat hist;

		  /// Compute the histograms:
		  calcHist(&seq, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

		  return hist;
	  }

};