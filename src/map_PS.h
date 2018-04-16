#ifndef _MAP_PS_H
#define _MAP_PS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>


//-----------------------------readSrcMapImg()---------------------------
//                             ��ȡ��ͼ�ļ�
//filename: ��ͼ�ļ�����·�����ļ���
//srcMapImg:�����洢��ȡ�ĵ�ͼ�ļ�
bool readSrcMapImg(const std::string& filename, cv::Mat& srcMapImg)
{
	srcMapImg = cv::imread(filename,0);//����Ҷ�ͼ
	if (!srcMapImg.data)
	{
		std::cout << "loading map error" << std::endl;
		return false;
	}
	else
	{
		std::cout << "loading map success" << std::endl;
		return true;
	}
		
}


//--------------------------saveMapValue()-----------------------------------
//��������ֵ��TXT�ļ�
//MapImg������ͼ��
//filename������ļ���
//mode: ��ʽ
void saveMapValue(cv::Mat MapImg, const std::string& filename, int mode = 0)
{

	std::ofstream fout(filename);
	char buf[20];
	if (mode == 0)//��IplImage���Ͱ����б�������
	{
		IplImage tempImg;
		tempImg = IplImage(MapImg);//MatתIplImage
		uchar* ptr;
		for (int i = 0; i < tempImg.height; i++)
		{
			for (int j = 0; j < tempImg.width; j++)
			{
				ptr = cvPtr2D(&tempImg, i, j, NULL);//��ȡ��������������ֵ
				sprintf(buf, "%d\t", ptr[0]);//ͨ����ʽ���Ʒ�תΪʮ�����������buf��
				fout << buf;//д���ļ�
			}
		}
	}
	else
		if (mode == 1)//��Mat������ָ���������
		{
			cv::Mat outputImage;
			// ������ԭͼ��ȳߴ��ͼ��
			outputImage.create(MapImg.size(), MapImg.type());
			int nr = MapImg.rows;
			// ��3ͨ��ת��Ϊ1ͨ��
			int nl = MapImg.cols*MapImg.channels();
			for (int k = 0; k<nr; k++)
			{
				// ÿһ��ͼ���ָ��
				const uchar* inData = MapImg.ptr<uchar>(k);
				uchar* outData = outputImage.ptr<uchar>(k);
				for (int i = 0; i<nl; i++)
				{
					//if (inData[i] == 0)//����ɫ����ֵΪ205
					//{
					//	outData[i] = 205;
					//}
					sprintf(buf, "%x\t", inData[i]);//ͨ����ʽ���Ʒ�תΪʮ�����������buf��
					fout << buf;//д���ļ�
				}
			}
		 }
	std::cout << "save txt over" << std::endl;
}


//-------------------------------------fillMap()-------------------------------------------------
//����ͼ�����ص�
//inputImg�� ����ͼ��
//outputImg�����ͼ��
//contour��  ��������
//measureDist:�����㵽�����ľ���
//			  ��measureDist����Ϊtrue ʱ��������ֵΪ������ʾ���������ڲ�������ֵΪ������ʾ�������ⲿ������ֵΪ0����ʾ�������ϡ� 
//            ��measureDist����Ϊfalseʱ��������ֵΪ + 1����ʾ���������ڲ�������ֵΪ - 1����ʾ�������ⲿ������ֵΪ0����ʾ�������ϡ�
void fillMap(cv::Mat &inputImg, cv::Mat &outputImg, std::vector < cv::Point> contour, bool measureDist)
{
	if (inputImg.channels() == 1)//��ͨ��ͼ��
	{
		std::cout << "fill map ..." << std::endl;
		int nr = inputImg.rows;
		int nl = inputImg.cols;
		for (int k = 0; k<nr; k++)
		{
			// ÿһ��ͼ���ָ��
			const uchar* inData = inputImg.ptr<uchar>(k);
			uchar* outData = outputImg.ptr<uchar>(k);
			for (int i = 0; i<nl; i++)
			{
				if (cv::pointPolygonTest(contour, cv::Point(i, k), measureDist)>0 && inData[i] < 230)//����ɫ����ֵΪ255
				{
					outData[i] = 255;
				}
			}
		}
		
		cv::imshow("result",outputImg);//��ʾ�������
		cv::imwrite("../data/result.pgm", outputImg);//���洦�����
		std::cout << "fill map success" << std::endl;
	}
	else
		if (inputImg.channels() == 3)
		{
			std::cout << "please trans srcImage to 1 channal" << std::endl;
		}
}



//----------------------colorReduce()-------------------------------
//��ɫ��
//inputImage������ͼ��
//outputImage�����ͼ��
//div��  �ȷ�
void colorReduce(const cv::Mat& inputImage, cv::Mat& outputImage, int div)
 {
	// ������ԭͼ��ȳߴ��ͼ��
	 outputImage.create(inputImage.size(), inputImage.type());
	 int nr = inputImage.rows;
	// ��3ͨ��ת��Ϊ1ͨ��
	 int nl = inputImage.cols*inputImage.channels();
	for (int k = 0; k<nr; k++)
	{
		// ÿһ��ͼ���ָ��
		const uchar* inData = inputImage.ptr<uchar>(k);
		uchar* outData = outputImage.ptr<uchar>(k);
		for (int i = 0; i<nl; i++)
		{
			 outData[i] = inData[i] / div*div + div / 2;//��ɫ��
		}
	}
 }


//------------------------------mouseHandler()-------------------------------------
//���ص���������ȡ����ֵ
//
//
void mouseHandler(int event, int x, int y, int flags, void* param)
{
	IplImage *img0, *img1;
	img0 = (IplImage*)param;
	img1 = cvCreateImage(cvSize(img0->width, img0->height), img0->depth, img0->nChannels);
	cvCopy(img0, img1, NULL);

	CvFont font;
	uchar* ptr;
	char label[20];
	char label2[20];

	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, 1);    //��ʼ������

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		//��ȡ����
		ptr = cvPtr2D(img0, y, x, NULL);

		sprintf(label, "Grey Level:%d", ptr[0]);
		sprintf(label2, "Pixel: (%d, %d)", x, y);
		//������ʾλ��
		if (img0->width - x <= 180 || img0->height - y <= 20)
		{
			cvRectangle(img1, cvPoint(x - 180, y - 40), cvPoint(x - 10, y - 10), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
			cvPutText(img1, label, cvPoint(x - 180, y - 30), &font, CV_RGB(255, 255, 255));
			cvPutText(img1, label2, cvPoint(x - 180, y - 10), &font, CV_RGB(255, 255, 255));
		}
		else
		{
			cvRectangle(img1, cvPoint(x + 10, y - 12), cvPoint(x + 180, y + 20), CV_RGB(255, 0, 0), CV_FILLED, 8, 0);
			cvPutText(img1, label, cvPoint(x + 10, y), &font, CV_RGB(255, 255, 255));
			cvPutText(img1, label2, cvPoint(x + 10, y + 20), &font, CV_RGB(255, 255, 255));
		}
		//�����Ϊ���Ļ���
		CvPoint centerPoint;
		centerPoint.x = x;
		centerPoint.y = y;
		cvCircle(img1, centerPoint, 3, CV_RGB(0, 0, 0), 1, 8, 3);

		cvShowImage("img", img1);
	}
}

void getPixelValue(IplImage* img)
{
	int exit = 0;
	int c;
	assert(img);//�������img�ĺϷ��ԣ�imgΪ�գ�����ֹ���򲢷��ش���ֵ

	if (img->nChannels != 1) //�ж��Ƿ�Ϊ�Ҷ�ͼ�������ǣ���ת��Ϊ�Ҷ�ͼ��
	{
		//ת��Ϊ�Ҷ�ͼ��
		IplImage* gray = cvCreateImage(cvGetSize(img), img->depth, 1);
		cvCvtColor(img, gray, CV_BGR2GRAY);
	}
	cvNamedWindow("img", 1);
	cvSetMouseCallback("img", mouseHandler, (void*)img);
	cvShowImage("img", img);
	//���������룬��Q���˳�����
	while (!exit)
	{
		c = cvWaitKey(0);
		switch (c)
		{
		case 'q':
			exit = 1;
			break;
		default:
			continue;
		}
	}
	cvDestroyAllWindows();
	cvReleaseImage(&img);
}


//-----------------------------------��onMouse( )������--------------------------------------    
//���onMouse�ص���������ˮ���
//---------------------------------------------------------------------------------------------  
cv::Mat dst;//��ˮ������ͼ��
int g_nFillMode = 1;//��ˮ����ģʽ  
int g_nLowDifference = 10, g_nUpDifference = 10;//�������ֵ���������ֵ  
int g_nConnectivity = 8;//��ʾfloodFill������ʶ���Ͱ�λ����ֵͨ  
int g_bIsColor  = false;//�Ƿ�Ϊ��ɫͼ�ı�ʶ������ֵ  
bool g_bUseMask = false;//�Ƿ���ʾ��Ĥ���ڵĲ���ֵ  
int g_nNewMaskVal = 255;//�µ����»��Ƶ�����ֵ  
std::vector < cv::Point> contour;
bool measureDist;

static void onMouse(int event, int x, int y, int, void*)
{
	// ��������û�а��£��㷵��  
	if (event != CV_EVENT_LBUTTONDOWN)
		return;

	//-------------------��<1>����floodFill����֮ǰ�Ĳ���׼�����֡�---------------  
	cv::Point seed = cv::Point(x, y);
	if(cv::pointPolygonTest(contour, seed, measureDist)>0)//���ӵ���������
	{
	uchar pixelValue=dst.at<uchar>(y,x);
	printf("Point(%d, %d):%d\n",x,y,pixelValue);
	if(pixelValue==205)//ֻ������ɫ����(205)
	{
	int LowDifference = g_nFillMode == 0 ? 0 : g_nLowDifference;//�շ�Χ����ˮ��䣬��ֵ��Ϊ0��������Ϊȫ�ֵ�g_nLowDifference  
	int UpDifference = g_nFillMode == 0 ? 0 : g_nUpDifference;//�շ�Χ����ˮ��䣬��ֵ��Ϊ0��������Ϊȫ�ֵ�g_nUpDifference  
	int flags = g_nConnectivity + (g_nNewMaskVal << 8) +
		(g_nFillMode == 1 ? CV_FLOODFILL_FIXED_RANGE : 0);//��ʶ����0~7λΪg_nConnectivity��8~15λΪg_nNewMaskVal����8λ��ֵ��16~23λΪCV_FLOODFILL_FIXED_RANGE����0��  

	cv::Rect ccomp;//�����ػ��������С�߽��������  

	cv::Scalar newVal = 254;//���ػ��������ص���ֵ(254)
	 
	int area;

	//--------------------��<2>��ʽ����floodFill������-----------------------------  

		area = floodFill(dst, seed, newVal, &ccomp, cv::Scalar(LowDifference, LowDifference, LowDifference),
			cv::Scalar(UpDifference, UpDifference, UpDifference), flags);

	//cv::imshow("Ч��ͼ", dst);
	cv::imwrite("../data/result.pgm", dst);//����Ч��ͼ
	//std::cout << area << " �����ر��ػ�\n";
	}
	else
	{
		;
	}
	}
	else
	{
		std::cout <<"Please select the points in the coutour!"<<std::endl;
	}
}


//-----------------------------------��floodFillonMouse( )������--------------------------------------    
//��ˮ���,�������ز���
//---------------------------------------------------------------------------------------------  
int floodFillonMouse(cv::Mat inputImg,std::vector < cv::Point> _contour, bool _measureDist)
{
	inputImg.copyTo(dst);//����ͼ�񿽱�
	contour=_contour;
	measureDist=_measureDist;
	cv::namedWindow("resultImg", CV_WINDOW_NORMAL);
	//���ص�����  
	cv::setMouseCallback("resultImg", onMouse, 0);
	
	while (1)
	{
		cv::imshow("resultImg", dst);
		if (cv::waitKey(10) == 27)
		{
			return 0;
		}
	}
}

#endif