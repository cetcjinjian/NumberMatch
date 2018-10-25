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

	//��ͼ������ļ�����
	typedef struct IMAGE_CAPTURE {
		int width;		//���ݵĿ��
		int height;		//���ݵĸ߶�
		BYTE* data;		//����ͼ�����ݵ�һά���� 
		BYTE** mat;     //����ͼ�����ݵĶ�ά����
	}st_ImageCapture;

	//�洢���ֵĽṹ��
	typedef struct IMAGE_DATA {
		int number;//��ʾ��ʾ������
		int width;//���ݵĿ��
		int height;//���ݵĸ߶�  
		BYTE* data;//����ͼ�����ݵ�һά���� 
		BYTE** mat;//����ͼ�����ݵĶ�ά����
	}st_NumberImage;

	
	st_NumberImage* m_NumImage;
	st_NumberImage m_TargetImage;

	//�����ҵ�������
	typedef struct NUM_FOUND {
		int x;
		int y;
		int number;
	}st_NumberFound;

	std::vector<st_NumberFound> m_Numfnd;


	//��ͼ�Ŀ�Ⱥ͸߶�
	int m_width;
	int m_height;

	//��С��ͼ�ߴ�,���ٴ����ٶ� 
	//��ȱ�Ϊԭ����4��֮1,�߶ȱ�Ϊԭ����16��֮1
	//x���Ϊ��ȵ�0.75��y���Ϊ0
	const int width_div = 4;
	const int height_div = 10;
	const float width_off = 0.75;
	const float height_off = 0;

	//��ȡ��������Ľ�ͼ
	st_ImageCapture image_capture;


public:

	void LoadImageFile(char* lpszFileName, int number);
	void LoadImageTargetFile(char* lpszTargetFileName, int number);
	void GetCapture(HWND hwnd, st_ImageCapture& imageCapture);//�Ծ�hwnd��ͼ
	void _2dRabinKarpMatcher(BYTE **T, int n1, int n2, BYTE **P, int m1, int m2, int d, int q, int num);//2άkmp�任
	void BeginImgMatch();
	void BeginImgMatch2();
	void PrintFndNum();
};

