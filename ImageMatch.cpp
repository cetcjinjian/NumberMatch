#include "stdafx.h"
#include "ImageMatch.h"
#include <wingdi.h>
#include <assert.h>
#include <iostream>

CImageMatch::CImageMatch()
{
	m_NumImage = new st_NumberImage[kNum];

}


CImageMatch::~CImageMatch()
{

}


void CImageMatch::LoadImageFile(char* lpszFileName, int number)
{
	//读取目录下的图片文件
	FILE *fp = fopen(lpszFileName, "rb");
	if (fp == 0)
		return;

	//读文件头
	BITMAPFILEHEADER bm_filehead;
	fread(&bm_filehead, sizeof(BITMAPFILEHEADER), 1, fp);

	//读信息头
	BITMAPINFOHEADER bm_infohead;
	fread(&bm_infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//图像宽度，长度和位宽
	int bmpWidth = bm_infohead.biWidth;
	int bmpHeight = bm_infohead.biHeight;
	int biBitCount = bm_infohead.biBitCount;

	//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	m_NumImage[number].number = number;
	m_NumImage[number].height = bmpHeight;
	m_NumImage[number].width = lineByte;

	//读取一维数据
	m_NumImage[number].data = new BYTE[lineByte * bmpHeight];
	fread(m_NumImage[number].data, 1, lineByte * bmpHeight, fp);

	//将一维数据转换为二维矩阵形式
	m_NumImage[number].mat = new BYTE*[bmpHeight];
	for (int i = 0; i < bmpHeight; ++i) {
		m_NumImage[number].mat[i] = new BYTE[lineByte];
		for (int j = 0; j < lineByte; ++j) {
			m_NumImage[number].mat[i][j] = m_NumImage[number].data[i*lineByte + j];
		}
	}
	//关闭文件
	fclose(fp);
}

void CImageMatch::GetCapture(HWND hwnd, st_ImageCapture& imageCapture)
{
	//获取控件句柄，若为空则截取桌面
	HWND hDesktop = ::GetDesktopWindow();
	assert(hDesktop);
	if (NULL == hwnd) {
		hwnd = hDesktop;
	}
	//获取hwnd尺寸
	RECT rect;
	::GetWindowRect(hwnd, &rect);
	int nWidht = rect.right - rect.left;
	int nHeight = rect.bottom - rect.top;
	m_width = nWidht;
	m_height = nHeight;

	nWidht = nWidht / width_div;
	nHeight = nHeight / height_div;

	//获取系统dc
	HDC hSrcDC = ::GetWindowDC(hwnd);
	assert(hSrcDC);
	HDC hMemDC = ::CreateCompatibleDC(hSrcDC);
	assert(hMemDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hSrcDC, nWidht, nHeight);
	assert(hBitmap);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	::BitBlt(hMemDC, 0, 0, nWidht, nHeight, hSrcDC, (rect.right - rect.left)*width_off, 0, SRCCOPY);

	BITMAP bitmap = { 0 };
	::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	BITMAPINFOHEADER bi = { 0 };
	BITMAPFILEHEADER bf = { 0 };

	CONST int nBitCount = 24;
	bi.biSize = sizeof(BITMAPINFOHEADER);

	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = nBitCount;
	bi.biCompression = BI_RGB;
	DWORD dwSize = ((bitmap.bmWidth * nBitCount + 31) / 32) * 4 * bitmap.bmHeight;
	HANDLE hDib = GlobalAlloc(GHND, dwSize + sizeof(BITMAPINFOHEADER));
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//获取图像
	::GetDIBits(hMemDC, hBitmap, 0, bitmap.bmHeight, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);


	if (imageCapture.height != bitmap.bmHeight || imageCapture.width != (bitmap.bmWidth * nBitCount / 8 + 3) / 4 * 4)
	{
		delete[] imageCapture.mat;
		delete[] imageCapture.data;
		imageCapture.data = new BYTE[dwSize];
		memcpy(imageCapture.data, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), dwSize);
		imageCapture.height = bitmap.bmHeight;
		imageCapture.width = (bitmap.bmWidth * nBitCount / 8 + 3) / 4 * 4;
		imageCapture.mat = new BYTE*[bitmap.bmHeight];
		for (int i = 0; i < bitmap.bmHeight; ++i) {
			imageCapture.mat[i] = new BYTE[imageCapture.width];
			for (int j = 0; j < imageCapture.width; ++j) {
				imageCapture.mat[i][j] = imageCapture.data[i*imageCapture.width + j];
			}
		}
	}
	else
	{
		memcpy(imageCapture.data, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), dwSize);
		for (int i = 0; i < bitmap.bmHeight; ++i) {
			for (int j = 0; j < imageCapture.width; ++j) {
				imageCapture.mat[i][j] = imageCapture.data[i*imageCapture.width + j];
			}
		}
	}

	GlobalUnlock(hDib);
	GlobalFree(hDib);
	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hwnd, hSrcDC);
}


void CImageMatch::_2dRabinKarpMatcher(BYTE **T, int n1, int n2, BYTE **P, int m1, int m2, int d, int q, int num)
{
	int *pattern = new int[m2];
	int *text = new int[n2];
	for (int i = 0; i<m2; ++i) {//将2维模式转换为1维
		pattern[i] = 0;
		for (int j = 0; j<m1; ++j) {
			pattern[i] = (d*pattern[i] + P[j][i]) % q;
		}
	}
	for (int i = 0; i<n2; ++i) {//列数
		text[i] = 0;
		for (int j = 0; j<m1; ++j) {//和模式行数一样
			text[i] = (d*text[i] + T[j][i]) % q;
		}
	}
	int h = static_cast<int>(pow(d*1.0, m2 - 1)) % q;
	int p = 0;
	int t = 0;
	for (int i = 0; i<m2; ++i) {//将一维数组转换成单元素值
		p = (d*p + pattern[i]) % q;
		t = (d*t + text[i]) % q;
	}
	delete[] pattern;
	for (int he = 0; he <= n1 - m1; ++he) {//T数组的行
		if (he <= n1 - m1 && he>0) {
			for (int c = 0; c<n2; ++c) {
				int th = (T[he - 1][c] * h) % q;
				int t_th = text[c] - th;
				while (t_th<0) {
					t_th += q;
				}
				t_th = (t_th*d) % q;
				t_th = t_th + T[he + m1 - 1][c];
				text[c] = t_th%q;
			}
			t = 0;//初始值为0
			for (int i = 0; i<m2; ++i) {//将一维数组转换成单元素值
				t = (d*t + text[i]) % q;
			}
		}
		for (int w = 0; w <= n2 - m2; ++w) {
			if (p == t) {
				int i = 0, j = 0;
				bool flag = false;//判断是否存在不相等的情况
				for (i = 0; i<m1; ++i) {
					if (flag) {
						break;
					}
					for (j = 0; j<m2; ++j) {
						if (P[i][j] != T[he + i][w + j]) {
							flag = true;
							break;
						}
					}
				}
				if (!flag) {
					//cout << "pattern occurs with shift (" << he << " , " << w << ") " << endl;
					int x = he;
					int y = w;
					//得到x，y之后，因为bmp图像的翻转排列问题，将相对坐标值变换回去
					x = n1 - 1 - x;
					y = y;
					//再坐标变换到完整截图的位置（当前为局部图像）
					y = y / 3 + m_width - m_width / width_div;
					st_NumberFound temp;
					temp.y = x;
					temp.x = y;
					temp.number = num;
					m_Numfnd.push_back(temp);
				}
			}
			if (w<n2 - m2) {
				//利用霍纳法则计算下一个值
				//Ts+1=(d(Ts-T[s+1]h)+T[s+m+1])mod q
				int ts = (text[w] * h) % q;
				int t_ts = t - ts;
				while (t_ts<0) {
					t_ts += q;
				}
				t_ts = (t_ts*d) % q;
				t = t_ts + text[w + m2];
				t = t%q;
			}
		}
	}
	delete[] text;
}

void CImageMatch::BeginImgMatch()
{
	//如果截图大小<数字图片大小，退出
	for (int i = 0; i < 10; ++i) {
		if (image_capture.height < m_NumImage[i].height ||
			image_capture.width < m_NumImage[i].width)
			return;
		_2dRabinKarpMatcher(image_capture.mat, image_capture.height, image_capture.width,
			m_NumImage[i].mat, m_NumImage[i].height, m_NumImage[i].width - 3, 1, 100, i);
	}

}


void CImageMatch::BeginImgMatch2()
{
	//如果截图大小<数字图片大小，退出
	for (int i = 0; i < 10; ++i) {
		if (m_TargetImage.height < m_NumImage[i].height ||
			m_TargetImage.width < m_NumImage[i].width)
			return;
		_2dRabinKarpMatcher(m_TargetImage.mat, m_TargetImage.height, m_TargetImage.width,
			m_NumImage[i].mat, m_NumImage[i].height, m_NumImage[i].width - 3, 1, 100, i);
	}
	int a = 10;
}


void CImageMatch::LoadImageTargetFile(char* lpszTargetFileName, int number)
{
	//读取目录下的图片文件
	FILE *fp = fopen(lpszTargetFileName, "rb");
	if (fp == 0)
		return;

	//读文件头
	BITMAPFILEHEADER bm_filehead;
	fread(&bm_filehead, sizeof(BITMAPFILEHEADER), 1, fp);

	//读信息头
	BITMAPINFOHEADER bm_infohead;
	fread(&bm_infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//图像宽度，长度和位宽
	int bmpWidth = bm_infohead.biWidth;
	int bmpHeight = bm_infohead.biHeight;
	int biBitCount = bm_infohead.biBitCount;

	//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;

	m_TargetImage.number = number;
	m_TargetImage.height = bmpHeight;
	m_TargetImage.width = lineByte;

	//读取一维数据
	m_TargetImage.data = new BYTE[lineByte * bmpHeight];
	fread(m_TargetImage.data, 1, lineByte * bmpHeight, fp);

	//将一维数据转换为二维矩阵形式
	m_TargetImage.mat = new BYTE*[bmpHeight];
	for (int i = 0; i < bmpHeight; ++i) {
		m_TargetImage.mat[i] = new BYTE[lineByte];
		for (int j = 0; j < lineByte; ++j) {
			m_TargetImage.mat[i][j] = m_TargetImage.data[i*lineByte + j];
		}
	}
	//关闭文件
	fclose(fp);
}

void CImageMatch::PrintFndNum()
{
	std::cout << "匹配出来的数字为" << std::endl;
	for each (auto it in m_Numfnd)	
		std::cout << it.number << " ";
	std::cout << "  " << std::endl;
}