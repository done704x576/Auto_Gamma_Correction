// Auto_Gamma_Correction.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cmath>
#include <iostream>  

#include <opencv2\core\core.hpp>  
#include <opencv2\highgui\highgui.hpp>  
#include <opencv2\imgproc\imgproc.hpp>  

using namespace cv;
using namespace std;

float get_Gamma_Value(Mat& gray_img);

void create_Gamma_Table(unsigned char* gama_table, float gama_value);

void Gamma_Correction(Mat& gray_img, Mat& dst_img, unsigned char* gama_table);

int _tmain(int argc, _TCHAR* argv[])
{
	char input_image_name[100];
	char output_image_name[100];
	int image_num = 476;

	for (int i = 1; i <= image_num; i++)
	{
		sprintf(input_image_name, "../%s\\%d.jpg","temp", i);
		Mat input_image = imread(input_image_name);

		if (input_image.empty())
		{
			cout << "Failed to load image !" << endl;
			continue;;
		}

		Mat gray_image;
		cvtColor(input_image, gray_image, CV_BGR2GRAY);

		// Start a timer
		double duration;
		duration = static_cast<double>(cv::getTickCount());

		float gama_value = get_Gamma_Value(gray_image);

		unsigned char LUT[256];
		create_Gamma_Table(LUT, gama_value);

		Mat result_image(gray_image.rows, gray_image.cols, gray_image.type());
		Gamma_Correction(gray_image, result_image, LUT);

		// Calculate the time cost and print
		duration = static_cast<double>(cv::getTickCount()) - duration;
		duration /= cv::getTickFrequency();
		std::cout << duration * 1000 << " ms" << std::endl;

		imshow("Source_Image", input_image);
		imshow("Gamma_Correction", result_image);
		//imwrite("test6.bmp",result_image);

		waitKey(1);
	}

	return 0;
}

/****************************************************
①当Gamma值比1大时，在输入值相同的情况下，输出值减小；
②当Gamma值为1时，输出值不变；
③当Gamma值比1小时，在输入值相同的情况下，输出值增加。
****************************************************/

//先计算灰度图像的像素均值mean，将计算出来的均值带入 gammaVal = log(mean/255)/log(0.5) 这个公式中，就可以得到Gamma值了。
float get_Gamma_Value(Mat& gray_img)
{
	if (gray_img.empty())
	{
		return -1.0;
	}

	cv::Scalar meam_value = cv::mean(gray_img);

	float val = meam_value.val[0];
	//float gamma_val = (log10(val / 255.0)) / (log10(0.5));
	float gamma_val = (log10(0.5)) / (log10(val / 255.0));

	return gamma_val;
}

void create_Gamma_Table(unsigned char* gama_table, float gama_value)
{
	for (int i = 0; i < 256; i++)
	{
		float f = (i + 0.5f) / 255.0;
		f = (float)(pow(f, gama_value));
		gama_table[i] = saturate_cast<uchar>(f * 255.0f - 0.5f);
	}
}

void Gamma_Correction(Mat& gray_img, Mat& dst_img, unsigned char* gama_table)
{
	if(gray_img.channels() != dst_img.channels() || gray_img.cols != dst_img.cols || gray_img.rows != dst_img.rows)
	{
		return;
	}

	for (int i = 0; i < gray_img.rows; i++)
	{
		for (int j = 0; j < gray_img.cols; j++)
		{
			dst_img.at<uchar>(i, j) = gama_table[(int)(gray_img.at<uchar>(i, j))];
		}
	}
}