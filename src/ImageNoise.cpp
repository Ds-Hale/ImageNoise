// ImageNoise.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp> 
#include <opencv2/OpenCV.hpp>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <math.h> 
using namespace cv;
using namespace std;
/*
������˹�����ĺ���������ά���ٿ�
*/
#define TWO_PI 6.2831853071795864769252866

double generateGaussianNoise()
{
	static bool hasSpare = false;
	static double rand1, rand2;

	if (hasSpare)
	{
		hasSpare = false;
		return sqrt(rand1) * sin(rand2);
	}

	hasSpare = true;

	rand1 = rand() / ((double)RAND_MAX);
	if (rand1 < 1e-100) rand1 = 1e-100;
	rand1 = -2 * log(rand1);
	rand2 = (rand() / ((double)RAND_MAX)) * TWO_PI;

	return sqrt(rand1) * cos(rand2);
}

void addNoise(Mat& picture){
	int channels = picture.channels();
	int Rows = picture.rows;
	int Cols = picture.cols*channels;
	if (picture.isContinuous()){
		Cols *= Rows;
		Rows = 1;
	}
	int i, j;
	uchar *p;//���picture���е�ָ��
	for (i = 0; i < Rows; ++i)
	{
		p = picture.ptr<uchar>(i);//��ֵΪ��ǰ��ָ��
		for (j= 0; j < Cols; ++j)
		{
			if (j%5==0)
			{
				double val = p[j] + generateGaussianNoise() * 128;
				if (val < 0)
					val = 0;
				if (val > 255)
					val = 255;

				p[j] = (uchar)val;
			}
			
			
		}
	}

}
Mat  eliminateNoise(Mat *mypics,int size){
	//��ȡ��һ���ļ�
	Mat one = mypics[0];
	Mat out = one.clone();

	//����ļ���Ϣ
	int channels = one.channels();
	int Rows = one.rows;
	int Cols = one.cols*channels;
	if (one.isContinuous()){
		Cols *= Rows;
		Rows = 1;
	}

	int i, j;
	//uchar *p;//���picture���е�ָ��
	uchar **p = new uchar *[10];
	uchar *out_p;
	for (i = 0; i < Rows; ++i)
	{
		//����ļ�����ָ��
		out_p = out.ptr<uchar>(i);
		for (size_t z = 0; z < size; ++z)
			//��ֵΪ��ǰ��ָ��
			p[z] = mypics[z].ptr<uchar>(i);
		for (j = 0; j < Cols; ++j)
		{
			double val = 0;
			//�ۼ�ÿ��ͼƬÿ�����ص��ֵ
			for (size_t z = 0; z <size; z++)
				val += floor(p[z][j]);
			//cout << val/10<<endl;
			//���������õ�ƽ��ֵ
			out_p[j] = (uchar)(val / size);

		}
	}
	return out;
}
void ManyImages(vector<Mat> Images, Mat& dst, int imgRows)
{
	int Num = Images.size();//�õ�Vector������ͼƬ����
	//�趨������ЩͼƬ�Ĵ��ڴ�С
	Mat Window(300 * ((Num - 1) / imgRows + 1), 300 * imgRows, CV_8UC3, Scalar(0, 0, 0));
	Mat Std_Image;//��ű�׼��С��ͼƬ
	Mat imageROI;//ͼƬ��������
	Size Std_Size = Size(300, 300);//ÿ��ͼƬ��ʾ��С300*300
	int x_Begin = 0;
	int y_Begin = 0;
	for (int i = 0; i < Num; i++)
	{
		x_Begin = (i % imgRows)*Std_Size.width;//ÿ��ͼƬ��ʼ����
		y_Begin = (i / imgRows)*Std_Size.height;
		resize(Images[i], Std_Image, Std_Size, 0, 0, INTER_LINEAR);//��ͼ����Ϊ��׼��С
		//��������Window��
		imageROI = Window(Rect(x_Begin, y_Begin, Std_Size.width, Std_Size.height));
		Std_Image.copyTo(imageROI);
	}
	dst = Window;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//string name ="image.jpg";
	//int ImageSize =30;
	int ImageSize = atoi(argv[2]);
	string name = argv[1];

	Mat pic = imread(name);
	//imshow("ԭʼͼƬ", pic);
	//waitKey();
	//������������ͼƬ
	string newname;
	for (size_t i = 0; i < ImageSize; i++)
	{
		Mat newImage = pic.clone();
		addNoise(newImage);
		ostringstream ss;
		ss << "noise" << i << ".jpg";
		newname = ss.str();
		imwrite(newname, newImage);
	}
	//�������ɵ�����ͼƬ�����е���
	Mat *mypics=new Mat[ImageSize];
	//ͼƬ��ͨ��
	string file;
	for (size_t i = 0; i < ImageSize; i++)
	{
		ostringstream name;
		name << "noise" << i << ".jpg";
		 file = name.str();
		mypics[i] = imread(file, 1);
	}
	//����������������
	Mat && out = std::move(eliminateNoise(mypics, ImageSize));
	ostringstream ss;
	ss << "output" << ImageSize << ".jpg";
	 newname = ss.str();
	imwrite(newname, out);
	vector<Mat> manyimgV;
	manyimgV.push_back(std::move(pic));
	manyimgV.push_back(out);

	Mat dst;
	ManyImages(manyimgV,dst, 2);
	imshow("������,���ΪԭʼͼƬ���ұ�Ϊ���ͼƬ", dst);
	waitKey(0);
	delete mypics;
	
	return 0;
}

