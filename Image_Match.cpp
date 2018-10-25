// Image_Match.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ImageMatch.h"
#include <Windows.h>
#include <iostream>

int main()
{
	CImageMatch image;
	char filePath[100];
	char fileTargetPath[100];
	for (int i = 0; i < 10; ++i) {
		sprintf(filePath, "dzh365/img/%d.bmp", i);
		image.LoadImageFile(filePath, i);
	}

	sprintf(filePath, "dzh365/img/target2.bmp");
	image.LoadImageTargetFile(filePath, -1);


	double time = 0;
	double counts = 0;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);//��ʼ��ʱ  

	image.BeginImgMatch2();

	QueryPerformanceCounter(&nEndTime);//ֹͣ��ʱ  
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//�������ִ��ʱ�䵥λΪs  
	std::cout << "����ʱ�䣺" << time * 1000 << "ms" << std::endl;
	image.PrintFndNum();

    return 0;
}

