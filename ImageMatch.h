#pragma once

#include <vector>
#include <memory>
#include <Windows.h>

typedef unsigned char BYTE ;

class CImageMatch
{
public:
	CImageMatch();
	~CImageMatch();




private:

	const int kNum = 10;

	//截图保存的文件数据
	typedef struct IMAGE_CAPTURE {
		int width;		//数据的宽度
		int height;		//数据的高度
		BYTE* data;		//保存图像数据的一维矩阵 
		BYTE** mat;     //保存图像数据的二维矩阵
	}st_ImageCapture;

	//存储数字的结构体
	typedef struct IMAGE_DATA {
		int number;//表示表示的数字
		int width;//数据的宽度
		int height;//数据的高度  
		BYTE* data;//保存图像数据的一维矩阵 
		BYTE** mat;//保存图像数据的二维矩阵
	}st_NumberImage;

	
	st_NumberImage* m_NumImage;
	st_NumberImage m_TargetImage;

	//保存找到的数字
	typedef struct NUM_FOUND {
		int x;
		int y;
		int number;
	}st_NumberFound;

	std::vector<st_NumberFound> m_Numfnd;


	//截图的宽度和高度
	int m_width;
	int m_height;

	//缩小截图尺寸,加速处理速度 
	//宽度变为原来的4分之1,高度变为原来的16分之1
	//x起点为宽度的0.75，y起点为0
	const int width_div = 4;
	const int height_div = 10;
	const float width_off = 0.75;
	const float height_off = 0;

	//获取交易软件的截图
	st_ImageCapture image_capture;


public:

	void LoadImageFile(char* lpszFileName, int number);
	void LoadImageTargetFile(char* lpszTargetFileName, int number);
	void GetCapture(HWND hwnd, st_ImageCapture& imageCapture);//对句hwnd截图
	void _2dRabinKarpMatcher(BYTE **T, int n1, int n2, BYTE **P, int m1, int m2, int d, int q, int num);//2维kmp变换
	void BeginImgMatch();
	void BeginImgMatch2();
	void PrintFndNum();
};

