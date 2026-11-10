#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;
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
private: float entropy(Mat seq, Size size, int index)
{
	int cnt = 0;
	float entr = 0;
	float total_size = size.height * size.width; //total size of all symbols in an image

	for (int i = 0; i < index; i++)
	{
		//float sym_occur = seq.at<float>(0, i); //the number of times a sybmol has occured
		float sym_occur = seq.at<float>(i, 0); //the number of times a sybmol has occured

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
private: Mat myEntropy(Mat seq, int histSize)
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

private: string generateImageName(string folderName, string criteria, string hist_type,float hist_value) {
	return folderName + "/" + criteria + "_"+ hist_type+"_" + std::to_string(hist_value) + ".bmp";
}

public: float* calculateEntropyHistogram(Mat image,string folderName, std::string image_criteria) {
	Mat src, converted_image, hist;

	/// Load image
	src = image;
	//src = originalImage.Image;


	/// Establish the number of bins
	int histSize = 256;


	Mat ycrcb[3], hist_b, hist_g, hist_r;
	cvtColor(src, converted_image, COLOR_BGR2YCrCb);
	split(converted_image, ycrcb);

	                          // Show our image inside it.


	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	hist_b = myEntropy(ycrcb[0], histSize);	// b	Y
	hist_g = myEntropy(ycrcb[1], histSize);	// g	Cr
	hist_r = myEntropy(ycrcb[2], histSize);	// r	Cb

	float Y = entropy(hist_b, src.size(), histSize);
	float Cr = entropy(hist_g, src.size(), histSize);
	float Cb = entropy(hist_r, src.size(), histSize);

	std::cout << "entropy Y: " << Y << std::endl;
	std::cout << "entropy Cr: " << Cr << std::endl;
	std::cout << "entropy Cb: " << Cb << std::endl;

	namedWindow("Original Image", WINDOW_AUTOSIZE);     // Create a window for display.
	imshow("Original Image", src);                    // Show our image inside it.

	namedWindow("Y", WINDOW_AUTOSIZE);                  // Create a window for display.
	imshow("Y", ycrcb[0]);                              // Show our image inside it.

	namedWindow("Cb", WINDOW_AUTOSIZE);                 // Create a window for display.
	imshow("Cb", ycrcb[1]);                             // Show our image inside it.

	namedWindow("Cr", WINDOW_AUTOSIZE);                 // Create a window for display.
	imshow("Cr", ycrcb[2]);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(hist_b, hist_b, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(hist_g, hist_g, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(hist_r, hist_r, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw histogram
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist_b.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(hist_b.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist_g.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(hist_g.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist_r.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(hist_r.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

	/// Display
	namedWindow("calcHist Demo", WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);

	imwrite(generateImageName(folderName,image_criteria,"Y",Y), ycrcb[0]);
	imwrite(generateImageName(folderName,image_criteria,"Cb", Cb), ycrcb[1]);
	imwrite(generateImageName(folderName,image_criteria,"Cr",Cr), ycrcb[2]);
	imwrite(generateImageName(folderName,image_criteria,"Histogram",0), histImage);

	//waitKey(0);

	src.release();
	hist.release();
	histImage.release();
	float* result= new float[3]();
	result[0] = Y;
	result[1] = Cr;
	result[2] = Cb;
	return result;

}
public: double correlation(cv::Mat& image_1, cv::Mat& image_2) {

	// convert data-type to "float"
	cv::Mat im_float_1;
	image_1.convertTo(im_float_1, CV_32F);
	cv::Mat im_float_2;
	image_2.convertTo(im_float_2, CV_32F);

	int n_pixels = im_float_1.rows * im_float_1.cols;

	// Compute mean and standard deviation of both images
	cv::Scalar im1_Mean, im1_Std, im2_Mean, im2_Std;
	meanStdDev(im_float_1, im1_Mean, im1_Std);
	meanStdDev(im_float_2, im2_Mean, im2_Std);

	// Compute covariance and correlation coefficient
	double covar = (im_float_1 - im1_Mean).dot(im_float_2 - im2_Mean) / n_pixels;
	double correl = covar / (im1_Std[0] * im2_Std[0]);

	return correl;
}
public: float correlationFloat(cv::Mat& image1, cv::Mat& image2)
{
	cv::Mat corr;
	//cv::matchTemplate(image1, image2, corr, cv::TM_CCOEFF_NORMED);
	cv::matchTemplate(image1, image2, corr, cv::TM_CCOEFF_NORMED);	
	return corr.at<float>(0, 0);  // corr only has one pixel
}

public: Mat histogramRGB(cv::Mat& image1) {
	Mat src, dst;

	/// Load image
	src = image1.clone();


	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 512;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

	/// Display
	namedWindow("calcHist Demo", WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);

	return histImage;

	
}

public: void informationEntropyRGB() {

}

};