#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4244)
#pragma warning(disable:4305)
#include "DxLib.h"
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include "MMTimerV3.h"
#include "sharememory_dtcl.h"
#include <math.h>
#include <time.h>
#define PI (3.141592)

//グローバル変数
char szClassName[] = "PathFollower";                   //ｳｨﾝﾄﾞｳのｸﾗｽ名
HWND hWnd;                                       //ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ

struct VPOINT {
	double x;
	double y;
	double z;
	double l;
	double a;
	int i;
	unsigned long t;
};
double vang[32] = { -30.6667,	-9.3333,
				-29.3333,	-8.0,
				-28.0,		-6.6666,
				-26.6667,	-5.3333,
				-25.3333,	-4.0,
				-24.0,		-2.6667,
				-22.6667,	-1.3333,
				-21.3333,	0.0,
				-20.0,		1.3333,
				-18.6667,	2.6667,
				-17.3333,	4.0,
				-16.0,		5.3333,
				-14.6667,	6.6667,
				-13.3333,	8.0,
				-12.0,		9.3333,
				-10.6667,	10.6667 };
double vang_vlp16[32] = { -15,1,-13,3,-11,5,-9,7,-7,9,-5,11,-3,13,-1,15,-15,1,-13,3,-11,5,-9,7,-7,9,-5,11,-3,13,-1,15 };
double vang_vlp32c[32] = { -25,-1,-1.667,-15.639,-11.31,0,-0.667,-8.843,-7.254,0.333,-0.333,-6.148,-5.333,1.333,0.667,-4,-4.667,1.667,1,-3.667,-3.333,3.333,2.333,-2.667,-3,7,4.667,-2.333,-2,15,10.333,-1.333 };
double vlp32choffset[32] = { 1.4,-4.2,1.4,-1.4,1.4,-1.4,4.2,-1.4,1.4,-4.2,1.4,-1.4,4.2,-1.4,4.2,-1.4,1.4,-4.2,1.4,-4.2,4.2,-1.4,1.4,-1.4,1.4,-1.4,1.4,-4.2,4.2,-1.4,1.4,-1.4 };



double coscal[72001], sincal[72001], coscalv[32], sincalv[32];


HANDLE hFMVelo;
int VeloNG = 0;
static char* lpstr;
VPOINT p[40000];
VPOINT p_32B[40000];
VPOINT ptmp[40000];
VPOINT p_tmp[10000];
VPOINT p_L[10000];
VPOINT p_R[10000];
VPOINT p_F[10000];
VPOINT p_B[10000];


double ptmpX[40000];
double ptmpY[40000];
double ptmpZ[40000];

double ptmpX2[40000];
double ptmpY2[40000];
double ptmpZ2[40000];
unsigned char ptmpNG[40000];
unsigned char ptmpNG2[40000];

double ptmpX2_32B[40000];
double ptmpY2_32B[40000];
double ptmpZ2_32B[40000];
unsigned char ptmpNG_32B[40000];
unsigned char ptmpNG2_32B[40000];
unsigned char tmpdata[1206000];
int velovalidflg = 0;
int velopointnum = 0;
int velopointnum_L = 0;
int velopointnum_R = 0;
int velopointnum_F = 0;
int velopointnum_B = 0;
int velopointnum_32B = 0;
double coscalv_16[32], sincalv_16[32];
double coscalv_16hr[32], sincalv_16hr[32];
int hfree = 1;
double calrange = 200;
double xpos = 0, ypos = 0, yawangle = 0;
int now_month, now_day, now_hour, now_minute, now_sec, now_year;
//OS1関係

double vangOS1[64] = { 17.181, 16.567, 16.011, 15.444, 14.987, 14.397, 13.83, 13.292,
					12.828, 12.264, 11.705, 11.156, 10.71, 10.158, 9.595, 9.057,
					8.605, 8.055, 7.504, 6.96, 6.517, 5.951, 5.412, 4.863,
					4.416, 3.86, 3.323, 2.775, 2.339, 1.779, 1.234, 0.679,
					0.239, -0.318, -0.862, -1.412, -1.863, -2.412, -2.957, -3.519,
					-3.954, -4.498, -5.049, -5.606, -6.055, -6.602, -7.148, -7.718,
					-8.169, -8.698, -9.254, -9.829, -10.274, -10.815, -11.376, -11.951,
					-12.406, -12.959, -13.515, -14.091, -14.572, -15.12, -15.69, -16.279 };
double hangOS1[64] = { 3.044, 0.872, -1.274, -3.421, 3.034, 0.887, -1.255, -3.359,
					3.039, 0.886, -1.217, -3.304, 3.022, 0.909, -1.183, -3.278,
					3.034, 0.931, -1.148, -3.245, 3.04, 0.958, -1.131, -3.194,
					3.065, 0.983, -1.114, -3.174, 3.084, 0.999, -1.076, -3.147,
					3.091, 1.018, -1.055, -3.128, 3.136, 1.059, -1.015, -3.09,
					3.164, 1.081, -1, -3.074, 3.205, 1.111, -0.971, -3.062,
					3.243, 1.151, -0.947, -3.047, 3.286, 1.193, -0.913, -3.038,
					3.355, 1.228, -0.887, -3.022, 3.412, 1.271, -0.862, -3 };

double coscalvOS1[64], sincalvOS1[64];
int OS1NG = 0;
HANDLE hFMOS1;

unsigned char tmpdataOS1[4532 * 100];
VPOINT p_OS1[30000];
int velopointnum_OS1 = 0;

double POSOFFSET = 1.2;

void main2();
unsigned long GetTimer();
void HSBitmap(HDC, int, int, int, int, int, unsigned char*);
void velocheck();
void velosharememoryinit();
int velogethdl32();
int velogetvlp16();
int velogetvlp32c(int);
int velogetvlp16_multi_L();
int velogetvlp16_multi_R();
int velogetvlp16_multi_F();
int velogetvlp16_multi_B();
int velogetvlp32c_F();
int velogetvlp32c_B();
void saveBMP(HDC, int, int, LPCTSTR);
int velogetOS1();


unsigned long GetTimer()	//	高分解能タイマ取得
{
	LARGE_INTEGER timeFrequency;
	LARGE_INTEGER timeCounter;
	LONGLONG Counttime;
	BOOL ret;

	ret = QueryPerformanceFrequency(&timeFrequency);	//	タイマ周波数の取得
	if (ret == 0)	return 0;

	QueryPerformanceCounter(&timeCounter);		//	タイマカウンタ取得
	if (ret == 0)	return 0;

	Counttime = timeCounter.QuadPart * 1000 / timeFrequency.QuadPart;
	return (unsigned long)Counttime;

}

void HSBitmap(HDC memhs, int WIDTH, int HEIGHT, int rg, int gg, int bg, unsigned char* t) {

	static LPDWORD lpPixel;
	static BITMAPINFO bmpInfo;
	int r1, g1, b1;
	unsigned char r, g, b;

	lpPixel = (LPDWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, WIDTH * HEIGHT * 4);

	//DIBの情報を設定する
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = WIDTH;
	bmpInfo.bmiHeader.biHeight = HEIGHT;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	int x, y;
	//描画
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			r1 = *(t + x * HEIGHT + y) * rg; if (r1 > 255)r1 = 255; if (r1 < 0)r1 = 0; r = r1;
			g1 = *(t + x * HEIGHT + y) * gg; if (g1 > 255)g1 = 255; if (g1 < 0)g1 = 0; g = g1;
			b1 = *(t + x * HEIGHT + y) * bg; if (b1 > 255)b1 = 255; if (b1 < 0)b1 = 0; b = b1;

			if (*(t + x * HEIGHT + y) == 255)r = g = b = 0;



			lpPixel[x + y * WIDTH] = r * 256 * 256 + g * 256 + b;    //赤
		}
	}
	//表画面へ転送
	SetDIBitsToDevice(memhs, 0, 0, WIDTH, HEIGHT,
		0, 0, 0, HEIGHT, lpPixel, &bmpInfo, DIB_RGB_COLORS);
	//StretchDIBits(hdc,250,0,WIDTH,HEIGHT,
	  //  0,0,WIDTH,HEIGHT,lpPixel,&bmpInfo,DIB_RGB_COLORS,SRCCOPY);
	HeapFree(GetProcessHeap(), 0, lpPixel);
}
void velocheck() {

	static unsigned int velooldtime = 0;
	unsigned char tmpdatachk[1206];
	static double oldtimestamp = 0;
	unsigned char tmpdatachk_OS1[4352];
	static double oldtimestamp_OS1 = 0;



	velovalidflg = 0;
	memcpy(tmpdatachk, lpstr, 1206);
	if (oldtimestamp != (tmpdatachk[1203] * 256.0 * 256.0 * 256.0 + tmpdatachk[1202] * 256 * 256 + tmpdatachk[1201] * 256 + tmpdatachk[1200]) / 1000) {
		oldtimestamp = (tmpdatachk[1203] * 256.0 * 256.0 * 256.0 + tmpdatachk[1202] * 256 * 256 + tmpdatachk[1201] * 256 + tmpdatachk[1200]) / 1000;
		velovalidflg = 1;

		velopointnum = velogetvlp32c_F();
		velopointnum_32B = velogetvlp32c_B();
		velopointnum_L = velogetvlp16_multi_L();
		velopointnum_R = velogetvlp16_multi_R();
		velopointnum_F = velogetvlp16_multi_F();
		velopointnum_B = velogetvlp16_multi_B();
	}

	memcpy(tmpdatachk_OS1, lpstr + 1206 * 300, 4352);
	if (oldtimestamp_OS1 != (tmpdatachk_OS1[32] + tmpdatachk_OS1[33] * 256 + tmpdatachk_OS1[34] * 256 * 256) / 1000) {
		oldtimestamp_OS1 = (tmpdatachk_OS1[32] + tmpdatachk_OS1[33] * 256 + tmpdatachk_OS1[34] * 256 * 256) / 1000;
		velopointnum_OS1 = velogetOS1();
	}
}
void velosharememoryinit() {
	int i;

	if (!hFMVelo)CloseHandle(hFMVelo);
	hFMVelo = CreateFileMapping(
		(HANDLE)-1,
		NULL,
		PAGE_READWRITE,
		0,
		1206 * 1000,
		"NEKODEMOWAKARU");

	hFMVelo = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "NEKODEMOWAKARU");
	lpstr = (char*)malloc(1206 * 1000);
	if (hFMVelo != NULL)lpstr = (LPSTR)MapViewOfFile(hFMVelo, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (hFMVelo == NULL) { MessageBox(hWnd, "LIDARの共有メモリが存在しません", "エラー", MB_OK); VeloNG = 1; }



	for (i = 0; i < 32; i++)vang[i] = vang_vlp32c[i];
	for (i = 0; i < 72001; i++) { coscal[i] = cos(-(double)i / 100.0 / 180.0 * PI); sincal[i] = sin(-(double)i / 100.0 / 180.0 * PI); }
	for (i = 0; i < 32; i++) { coscalv[i] = cos(vang[i] / 180.0 * PI); sincalv[i] = sin(vang[i] / 180.0 * PI); }

	for (i = 0; i < 32; i++) { coscalv_16[i] = cos(vang_vlp16[i] / 180.0 * PI); sincalv_16[i] = sin(vang_vlp16[i] / 180.0 * PI); }
	for (i = 0; i < 32; i++) { coscalv_16hr[i] = cos(vang_vlp16[i] / 180.0 * PI * 2.0 / 3.0); sincalv_16hr[i] = sin(vang_vlp16[i] / 180.0 * PI * 2.0 / 3.0); }
	for (i = 0; i < 64; i++) { coscalvOS1[i] = cos(vangOS1[i] / 180.0 * PI); sincalvOS1[i] = sin(vangOS1[i] / 180.0 * PI); }


}

int velogetvlp16_multi_R() {

	int i, j, k;
	int datanum = 0;
	long pnum;
	int angle, angleraw;
	unsigned long timestp;
	static int dualmode = 0;
	double c1, s1, tx, tz, ty, c2, s2;
	double vlpvangle;
	double vlphangle;
	double h = 10;
	int jogai = 0;

	double VLP16VANGLE_R = 5500;
	double VLP16HANGLE_R = 200;
	double VLPX_R = 2.25;
	double VLPY_R = -0.85;
	double VLPZ_R = 2.33;



	if (hfree == 1)h = 1000;
	if (hfree == 0)h = 3;

	vlpvangle = VLP16VANGLE_R;
	vlphangle = VLP16HANGLE_R;
	c1 = cos(vlpvangle / 100.0 / 180.0 * PI);
	s1 = sin(vlpvangle / 100.0 / 180.0 * PI);

	for (i = 150; i < 175; i++) {
		if (lpstr[0 + 1206 * i] == 0 && lpstr[1 + 1206 * i] == 0 && lpstr[2 + 1206 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdata, lpstr, 1206 * datanum);
	pnum = 0;


	for (i = 150; i < datanum; i++) {
		timestp = (tmpdata[1203 + 1206 * i] * 256.0 * 256.0 * 256.0 + tmpdata[1202 + 1206 * i] * 256 * 256 + tmpdata[1201 + 1206 * i] * 256 + tmpdata[1200 + 1206 * i]) / 1000;
		for (j = 0; j < 12; j += 1) {
			angleraw = tmpdata[3 + j * 100 + 1206 * i] * 256 + tmpdata[2 + j * 100 + 1206 * i];
			if (tmpdata[1206 * i + j * 100] == 0xff && tmpdata[1206 * i + j * 100 + 1] == 0xee)for (k = 0; k < 32; k++) {
				if (k < 16)angle = angleraw + vlphangle;
				if (k >= 16)angle = angleraw + 40 + vlphangle;
				p_tmp[pnum].a = angle / 100.0 / 180.0 * PI;

				p_tmp[pnum].l = (tmpdata[5 + j * 100 + 1206 * i + k * 3] * 256 + tmpdata[4 + j * 100 + 1206 * i + k * 3]) * 0.002;
				p_tmp[pnum].x = p_tmp[pnum].l * coscal[angle] * coscalv_16hr[k];
				p_tmp[pnum].y = p_tmp[pnum].l * sincal[angle] * coscalv_16hr[k];
				p_tmp[pnum].z = p_tmp[pnum].l * sincalv_16hr[k];
				ty = p_tmp[pnum].y;
				tz = p_tmp[pnum].z;
				p_tmp[pnum].y = ty * c1 - tz * s1;
				p_tmp[pnum].z = ty * s1 + tz * c1;

				/*
				tx=p_tmp[pnum].x;
				ty=p_tmp[pnum].y;
				p_tmp[pnum].x=tx*c2-ty*s2;
				p_tmp[pnum].y=tx*s2+ty*c2;
				*/

				p_tmp[pnum].i = tmpdata[6 + j * 100 + 1206 * i + k * 3];
				p_tmp[pnum].t = timestp;
				pnum++;
				if (pnum >= 10000)pnum = 9999;

			}
		}
	}


	int tpnum = 0;
	int NGflg = 0;

	for (i = 16; i < pnum - 16; i++) {
		NGflg = 0;

		if (fabs(p_tmp[i - 16].z - p_tmp[i + 16].z) < 0.05 && fabs(p_tmp[i - 16].z - p_tmp[i].z) > 0.2) NGflg = 1;
		if (p_tmp[i].l<0.5 || p_tmp[i].z + VLPZ_R>h) NGflg = 1;
		if (p_tmp[i].y + VLPY_R > -1.2)NGflg = 1;

		if (NGflg == 0) {
			p_R[tpnum].x = p_tmp[i].x + VLPX_R - POSOFFSET;
			p_R[tpnum].y = p_tmp[i].y + VLPY_R;
			p_R[tpnum].z = p_tmp[i].z + VLPZ_R;
			p_R[tpnum].l = p_tmp[i].l;
			tpnum++;
			if (tpnum >= 10000)tpnum = 9999;
		}
	}

	/*

		for(i=0;i<pnum;i++){
			p_tmp[i].x=p_tmp[i].x+VLPX_R-POSOFFSET;
			p_tmp[i].y=p_tmp[i].y+VLPY_R;
			p_tmp[i].z=p_tmp[i].z+VLPZ_R;
		}
		memcpy(p_R,p_tmp,sizeof(VPOINT)*pnum);
		return pnum;
	*/
	return tpnum;
}
int velogetvlp16_multi_L() {

	int i, j, k;
	int datanum = 0;
	long pnum;
	int angle, angleraw;
	unsigned long timestp;
	static int dualmode = 0;
	double c1, s1, tx, tz, ty, c2, s2;
	double vlpvangle;
	double vlphangle;
	double h = 10;
	int jogai = 0;

	double VLP16VANGLE_L = -5500;
	double VLP16HANGLE_L = 280;
	double VLPX_L = 2.25;
	double VLPY_L = 0.85;
	double VLPZ_L = 2.33;

	if (hfree == 1)h = 1000;
	if (hfree == 0)h = 3.0;
	vlpvangle = VLP16VANGLE_L;
	vlphangle = VLP16HANGLE_L;
	c1 = cos(vlpvangle / 100.0 / 180.0 * PI);
	s1 = sin(vlpvangle / 100.0 / 180.0 * PI);

	for (i = 100; i < 125; i++) {
		if (lpstr[0 + 1206 * i] == 0 && lpstr[1 + 1206 * i] == 0 && lpstr[2 + 1206 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdata, lpstr, 1206 * datanum);
	pnum = 0;


	for (i = 100; i < datanum; i++) {
		timestp = (tmpdata[1203 + 1206 * i] * 256.0 * 256.0 * 256.0 + tmpdata[1202 + 1206 * i] * 256 * 256 + tmpdata[1201 + 1206 * i] * 256 + tmpdata[1200 + 1206 * i]) / 1000;
		for (j = 0; j < 12; j += 1) {
			angleraw = tmpdata[3 + j * 100 + 1206 * i] * 256 + tmpdata[2 + j * 100 + 1206 * i];
			if (tmpdata[1206 * i + j * 100] == 0xff && tmpdata[1206 * i + j * 100 + 1] == 0xee)for (k = 0; k < 32; k++) {
				if (k < 16)angle = angleraw + vlphangle;
				if (k >= 16)angle = angleraw + 40 + vlphangle;
				p_tmp[pnum].a = angle / 100.0 / 180.0 * PI;

				p_tmp[pnum].l = (tmpdata[5 + j * 100 + 1206 * i + k * 3] * 256 + tmpdata[4 + j * 100 + 1206 * i + k * 3]) * 0.002;
				p_tmp[pnum].x = p_tmp[pnum].l * coscal[angle] * coscalv_16hr[k];
				p_tmp[pnum].y = p_tmp[pnum].l * sincal[angle] * coscalv_16hr[k];
				p_tmp[pnum].z = p_tmp[pnum].l * sincalv_16hr[k];
				ty = p_tmp[pnum].y;
				tz = p_tmp[pnum].z;
				p_tmp[pnum].y = ty * c1 - tz * s1;
				p_tmp[pnum].z = ty * s1 + tz * c1;

				/*
				tx=p_tmp[pnum].x;
				ty=p_tmp[pnum].y;
				p_tmp[pnum].x=tx*c2-ty*s2;
				p_tmp[pnum].y=tx*s2+ty*c2;
				*/

				p_tmp[pnum].i = tmpdata[6 + j * 100 + 1206 * i + k * 3];
				p_tmp[pnum].t = timestp;
				pnum++;
				if (pnum >= 10000)pnum = 9999;

			}
		}
	}


	int tpnum = 0;
	int NGflg = 0;

	for (i = 16; i < pnum - 16; i++) {
		NGflg = 0;

		if (fabs(p_tmp[i - 16].z - p_tmp[i + 16].z) < 0.05 && fabs(p_tmp[i - 16].z - p_tmp[i].z) > 0.2) NGflg = 1;
		if (p_tmp[i].l<0.5 || p_tmp[i].z + VLPZ_L>h) NGflg = 1;
		if (p_tmp[i].y + VLPY_L < 1.2)NGflg = 1;

		if (NGflg == 0) {
			p_L[tpnum].x = p_tmp[i].x + VLPX_L - POSOFFSET;
			p_L[tpnum].y = p_tmp[i].y + VLPY_L;
			p_L[tpnum].z = p_tmp[i].z + VLPZ_L;
			p_L[tpnum].l = p_tmp[i].l;
			tpnum++;
			if (tpnum >= 10000)tpnum = 9999;
		}
	}


	/*

		for(i=0;i<pnum;i++){
			p_tmp[i].x=p_tmp[i].x+VLPX_L-POSOFFSET;
			p_tmp[i].y=p_tmp[i].y+VLPY_L;
			p_tmp[i].z=p_tmp[i].z+VLPZ_L;
		}
		memcpy(p_L,p_tmp,sizeof(VPOINT)*pnum);
		return pnum;*/
	return tpnum;
}
int velogetvlp16_multi_F() {

	int i, j, k;
	int datanum = 0;
	long pnum;
	int angle, angleraw;
	unsigned long timestp;
	static int dualmode = 0;
	double c1, s1, tx, tz, ty, c2, s2;
	double vlpvangle;
	double vlphangle;
	double h = 10;
	int jogai = 0;

	double VLP16VANGLE_F = -2500;
	double VLP16HANGLE_F = 27000;
	double VLPX_F = 2.63;
	double VLPY_F = 0;
	double VLPZ_F = 2.41;

	if (hfree == 1)h = 1000;
	if (hfree == 0)h = 3.0;
	vlpvangle = VLP16VANGLE_F;
	vlphangle = VLP16HANGLE_F;
	c1 = cos(vlpvangle / 100.0 / 180.0 * PI);
	s1 = sin(vlpvangle / 100.0 / 180.0 * PI);

	for (i = 125; i < 150; i++) {
		if (lpstr[0 + 1206 * i] == 0 && lpstr[1 + 1206 * i] == 0 && lpstr[2 + 1206 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdata, lpstr, 1206 * datanum);
	pnum = 0;


	for (i = 125; i < datanum; i++) {
		timestp = (tmpdata[1203 + 1206 * i] * 256.0 * 256.0 * 256.0 + tmpdata[1202 + 1206 * i] * 256 * 256 + tmpdata[1201 + 1206 * i] * 256 + tmpdata[1200 + 1206 * i]) / 1000;
		for (j = 0; j < 12; j += 1) {
			angleraw = tmpdata[3 + j * 100 + 1206 * i] * 256 + tmpdata[2 + j * 100 + 1206 * i];
			if (tmpdata[1206 * i + j * 100] == 0xff && tmpdata[1206 * i + j * 100 + 1] == 0xee)for (k = 0; k < 32; k++) {
				if (k < 16)angle = angleraw + vlphangle;
				if (k >= 16)angle = angleraw + 40 + vlphangle;
				p_tmp[pnum].a = angle / 100.0 / 180.0 * PI;

				p_tmp[pnum].l = (tmpdata[5 + j * 100 + 1206 * i + k * 3] * 256 + tmpdata[4 + j * 100 + 1206 * i + k * 3]) * 0.002;
				p_tmp[pnum].x = p_tmp[pnum].l * coscal[angle] * coscalv_16hr[k];
				p_tmp[pnum].y = p_tmp[pnum].l * sincal[angle] * coscalv_16hr[k];
				p_tmp[pnum].z = p_tmp[pnum].l * sincalv_16hr[k];
				tx = p_tmp[pnum].x;
				tz = p_tmp[pnum].z;
				p_tmp[pnum].x = tx * c1 - tz * s1;
				p_tmp[pnum].z = tx * s1 + tz * c1;

				/*
				tx=p_tmp[pnum].x;
				ty=p_tmp[pnum].y;
				p_tmp[pnum].x=tx*c2-ty*s2;
				p_tmp[pnum].y=tx*s2+ty*c2;
				*/

				p_tmp[pnum].i = tmpdata[6 + j * 100 + 1206 * i + k * 3];
				p_tmp[pnum].t = timestp;
				pnum++;
				if (pnum >= 10000)pnum = 9999;
			}
		}
	}




	int tpnum = 0;
	int NGflg = 0;

	for (i = 16; i < pnum - 16; i++) {
		NGflg = 0;

		if (fabs(p_tmp[i - 16].z - p_tmp[i + 16].z) < 0.05 && fabs(p_tmp[i - 16].z - p_tmp[i].z) > 0.2) NGflg = 1;
		if (p_tmp[i].l<0.5 || p_tmp[i].z + VLPZ_F>h) NGflg = 1;
		if (NGflg == 0) {
			p_F[tpnum].x = p_tmp[i].x + VLPX_F - POSOFFSET;
			p_F[tpnum].y = p_tmp[i].y + VLPY_F;
			p_F[tpnum].z = p_tmp[i].z + VLPZ_F;
			p_F[tpnum].l = p_tmp[i].l;
			tpnum++;
			if (tpnum >= 10000)tpnum = 9999;
		}
	}




	/*
		for(i=0;i<pnum;i++){
			p_tmp[i].x=p_tmp[i].x+VLPX_F-POSOFFSET;
			p_tmp[i].y=p_tmp[i].y+VLPY_F;
			p_tmp[i].z=p_tmp[i].z+VLPZ_F;
		}
		memcpy(p_F,p_tmp,sizeof(VPOINT)*pnum);
	*/
	return tpnum;
}
int velogetvlp16_multi_B() {

	int i, j, k;
	int datanum = 0;
	long pnum;
	int angle, angleraw;
	unsigned long timestp;
	static int dualmode = 0;
	double c1, s1, tx, tz, ty, c2, s2;
	double vlpvangle;
	double vlphangle;
	double h = 10;
	int jogai = 0;

	double VLP16VANGLE_B = 5000;
	double VLP16HANGLE_B = 27200;
	double VLPX_B = -0.80;
	double VLPY_B = 0;
	double VLPZ_B = 2.33;

	if (hfree == 1)h = 1000;
	if (hfree == 0)h = 3.0;
	vlpvangle = VLP16VANGLE_B;
	vlphangle = VLP16HANGLE_B;
	c1 = cos(vlpvangle / 100.0 / 180.0 * PI);
	s1 = sin(vlpvangle / 100.0 / 180.0 * PI);

	for (i = 175; i < 200; i++) {
		if (lpstr[0 + 1206 * i] == 0 && lpstr[1 + 1206 * i] == 0 && lpstr[2 + 1206 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdata, lpstr, 1206 * datanum);
	pnum = 0;


	for (i = 175; i < datanum; i++) {
		timestp = (tmpdata[1203 + 1206 * i] * 256.0 * 256.0 * 256.0 + tmpdata[1202 + 1206 * i] * 256 * 256 + tmpdata[1201 + 1206 * i] * 256 + tmpdata[1200 + 1206 * i]) / 1000;
		for (j = 0; j < 12; j += 1) {
			angleraw = tmpdata[3 + j * 100 + 1206 * i] * 256 + tmpdata[2 + j * 100 + 1206 * i];
			if (tmpdata[1206 * i + j * 100] == 0xff && tmpdata[1206 * i + j * 100 + 1] == 0xee)for (k = 0; k < 32; k++) {
				if (k < 16)angle = angleraw + vlphangle;
				if (k >= 16)angle = angleraw + 40 + vlphangle;
				p_tmp[pnum].a = angle / 100.0 / 180.0 * PI;

				p_tmp[pnum].l = (tmpdata[5 + j * 100 + 1206 * i + k * 3] * 256 + tmpdata[4 + j * 100 + 1206 * i + k * 3]) * 0.002;
				p_tmp[pnum].x = p_tmp[pnum].l * coscal[angle] * coscalv_16hr[k];
				p_tmp[pnum].y = p_tmp[pnum].l * sincal[angle] * coscalv_16hr[k];
				p_tmp[pnum].z = p_tmp[pnum].l * sincalv_16hr[k];
				tx = p_tmp[pnum].x;
				tz = p_tmp[pnum].z;
				p_tmp[pnum].x = tx * c1 - tz * s1;
				p_tmp[pnum].z = tx * s1 + tz * c1;

				/*
				tx=p_tmp[pnum].x;
				ty=p_tmp[pnum].y;
				p_tmp[pnum].x=tx*c2-ty*s2;
				p_tmp[pnum].y=tx*s2+ty*c2;
				*/

				p_tmp[pnum].i = tmpdata[6 + j * 100 + 1206 * i + k * 3];
				p_tmp[pnum].t = timestp;
				pnum++;
				if (pnum >= 10000)pnum = 9999;
			}
		}
	}




	int tpnum = 0;
	int NGflg = 0;

	for (i = 16; i < pnum - 16; i++) {
		NGflg = 0;

		if (fabs(p_tmp[i - 16].z - p_tmp[i + 16].z) < 0.05 && fabs(p_tmp[i - 16].z - p_tmp[i].z) > 0.2) NGflg = 1;
		if (p_tmp[i].l<0.5 || p_tmp[i].z + VLPZ_B>h) NGflg = 1;
		if (NGflg == 0) {
			p_B[tpnum].x = p_tmp[i].x + VLPX_B - POSOFFSET;
			p_B[tpnum].y = p_tmp[i].y + VLPY_B;
			p_B[tpnum].z = p_tmp[i].z + VLPZ_B;
			p_B[tpnum].l = p_tmp[i].l;
			tpnum++;
			if (tpnum >= 10000)tpnum = 9999;
		}
	}




	/*
		for(i=0;i<pnum;i++){
			p_tmp[i].x=p_tmp[i].x+VLPX_F-POSOFFSET;
			p_tmp[i].y=p_tmp[i].y+VLPY_F;
			p_tmp[i].z=p_tmp[i].z+VLPZ_F;
		}
		memcpy(p_F,p_tmp,sizeof(VPOINT)*pnum);
	*/
	return tpnum;
}
int velogetvlp32c_F() {
	int i, j, k;
	int datanum = 0;
	static long pnum = 0;
	int angle;
	int angle2;
	unsigned long timestp;
	int cnt100 = 0;
	double c1, s1, tx, ty, tz;;
	double vlpvangle;
	double h;
	double Lposz = 0, Lposx = 0, Lposy = 0;
	double vang_koubai = 0;
	double c2, s2;
	double ph = 0;
	double vhmin1[32], vhmin2[32];
	double chkang = 0;

	double VLP32VANGLE_F = -450;
	double VLP32HANGLE_F = 27000;
	double VLP32X_F = 2.55;
	double VLP32Y_F = 0;
	double VLP32Z_F = 2.53;



	for (i = 0; i < 32; i++) { vhmin1[i] = 1000; vhmin2[i] = 1000; }

	if (hfree == 1)h = 1000;
	if (hfree == 0)h = 3.0;
	pnum = 0;
	double hang = VLP32HANGLE_F;
	vlpvangle = VLP32VANGLE_F;
	vang_koubai = 0;


	c1 = cos(vlpvangle / 100.0 / 180.0 * PI + vang_koubai);
	s1 = sin(vlpvangle / 100.0 / 180.0 * PI + vang_koubai);

	Lposx = xpos + (VLP32X_F - POSOFFSET) * cos(yawangle);
	Lposy = ypos + (VLP32X_F - POSOFFSET) * sin(yawangle);
	Lposz = 0;

	//有意なデータ量の確認とコピー

	for (i = 0; i < 100; i++) {
		if (lpstr[0 + 1206 * i] == 0 && lpstr[1 + 1206 * i] == 0 && lpstr[2 + 1206 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdata, lpstr, 1206 * datanum);
	//点群への変換
	//pnum=0;

	c2 = cos(sm[507]);
	s2 = sin(sm[507]);


	for (i = 0; i < datanum; i++) {
		timestp = (tmpdata[1203 + 1206 * i] * 256.0 * 256.0 * 256.0 + tmpdata[1202 + 1206 * i] * 256 * 256 + tmpdata[1201 + 1206 * i] * 256 + tmpdata[1200 + 1206 * i]) / 1000;
		for (j = 0; j < 12; j++) {
			angle = tmpdata[3 + j * 100 + 1206 * i] * 256 + tmpdata[2 + j * 100 + 1206 * i];
			angle = angle + hang;
			//干渉部除去
			chkang = angle / 100.0;
			if (chkang > 180)chkang = chkang - 360;
			if (chkang < -180)chkang = chkang + 360;
			if (chkang > 180)chkang = chkang - 360;
			if (chkang < -180)chkang = chkang + 360;
			//chkang < 175 && chkang > -175
			if (chkang < 175 && chkang > -175 && tmpdata[1206 * i + j * 100] == 0xff && tmpdata[1206 * i + j * 100 + 1] == 0xee)for (k = 0; k < 32; k++) {
				angle2 = angle + vlp32choffset[k] * 100; if (angle2 < 0)angle2 = 36000 + angle2;

				ptmp[pnum].a = angle2 / 100.0 / 180.0 * PI;
				ptmp[pnum].l = (tmpdata[5 + j * 100 + 1206 * i + k * 3] * 256 + tmpdata[4 + j * 100 + 1206 * i + k * 3]) * 0.004;
				ptmp[pnum].x = ptmp[pnum].l * coscal[angle2] * coscalv[k];
				ptmp[pnum].y = ptmp[pnum].l * sincal[angle2] * coscalv[k];
				ptmp[pnum].z = ptmp[pnum].l * sincalv[k];

				if (true) {
					tx = ptmp[pnum].x;
					tz = ptmp[pnum].z;
					ptmp[pnum].x = tx * c1 - tz * s1;
					ptmp[pnum].z = tx * s1 + tz * c1;
				}
				ptmp[pnum].x = ptmp[pnum].x + VLP32X_F - POSOFFSET;
				ptmp[pnum].z = ptmp[pnum].z + VLP32Z_F;
				ptmp[pnum].i = tmpdata[6 + j * 100 + 1206 * i + k * 3];
				ptmp[pnum].t = timestp;

				tx = ptmp[pnum].x;
				ty = ptmp[pnum].y;
				ptmpX[pnum] = tx * c2 - ty * s2 + xpos;
				ptmpY[pnum] = tx * s2 + ty * c2 + ypos;
				ptmpZ[pnum] = ptmp[pnum].z + Lposz;
				ph = 0;
				ptmpZ[pnum] = ptmpZ[pnum] - ph;
				ptmpNG[pnum] = 0;
				if (ph == 0) { ptmpZ[pnum] = ptmp[pnum].z; ptmpNG[pnum] = 1; }
				if (ph != 0) {
					if (vhmin1[pnum % 32] > ptmpZ[pnum] && ptmp[pnum].x > 5)vhmin1[pnum % 32] = ptmpZ[pnum];
				}
				pnum++;
				if (pnum >= 40000)pnum = 39999;
			}
		}
	}

	int tpnum = 0;
	int NGflg = 0;

	for (i = 32; i < pnum - 32; i++) {
		NGflg = 0;

		if (fabs(ptmp[i - 32].z - ptmp[i + 32].z) < 0.05 && fabs(ptmp[i - 32].z - ptmp[i].z) > 0.2 && ptmp[i].l < 25) NGflg = 1;
		else if (ptmp[i].l > calrange || ptmp[i].l<2 || ptmpZ[i]>h || fabs(ptmp[i].y) > 25) NGflg = 1;
		else if (ptmp[i].x<4.0 && ptmp[i].x>-4.0 && ptmp[i].y<1.2 && ptmp[i].y>-1.2)NGflg = 1;

		if (NGflg == 0) {
			p[tpnum] = ptmp[i];
			ptmpX2[tpnum] = ptmpX[i];
			ptmpY2[tpnum] = ptmpY[i];
			ptmpZ2[tpnum] = ptmpZ[i];
			if (ptmpNG[i] == 0 && vhmin1[i % 32] > 0 && ptmp[i].l < 80 && ptmp[i].x>5)ptmpZ2[tpnum] = ptmpZ2[tpnum] - vhmin1[i % 32];
			ptmpNG2[tpnum] = ptmpNG[i];
			tpnum++;
			if (tpnum >= 40000)tpnum = 39999;
		}
	}

	return(tpnum);
}
int velogetvlp32c_B() {
	int i, j, k;
	int datanum = 0;
	static long pnum = 0;
	int angle;
	int angle2;
	unsigned long timestp;
	int cnt100 = 0;
	double c1, s1, tx, ty, tz;;
	double vlpvangle;
	double h;
	double Lposz = 0, Lposx = 0, Lposy = 0;
	double vang_koubai = 0;
	double c2, s2;
	double ph = 0;
	double vhmin1[32], vhmin2[32];
	double chkang = 0;

	double VLP32VANGLE_B = 0;
	double VLP32HANGLE_B = 27200;
	double VLP32X_B = -0.75;
	double VLP32Y_B = 0;
	double VLP32Z_B = 2.68;


	for (i = 0; i < 32; i++) { vhmin1[i] = 1000; vhmin2[i] = 1000; }

	if (hfree == 1)h = 1000;
	if (hfree == 0)h = 3.0;
	pnum = 0;
	double hang = VLP32HANGLE_B;
	vlpvangle = VLP32VANGLE_B;
	vang_koubai = 0;


	c1 = cos(vlpvangle / 100.0 / 180.0 * PI + vang_koubai);
	s1 = sin(vlpvangle / 100.0 / 180.0 * PI + vang_koubai);

	Lposx = xpos + (VLP32X_B - POSOFFSET) * cos(yawangle);
	Lposy = ypos + (VLP32X_B - POSOFFSET) * sin(yawangle);
	Lposz = 0;

	//有意なデータ量の確認とコピー

	for (i = 200; i < 300; i++) {
		if (lpstr[0 + 1206 * i] == 0 && lpstr[1 + 1206 * i] == 0 && lpstr[2 + 1206 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdata, lpstr, 1206 * datanum);
	//点群への変換
	//pnum=0;

	c2 = cos(sm[507]);
	s2 = sin(sm[507]);


	for (i = 200; i < datanum; i++) {
		timestp = (tmpdata[1203 + 1206 * i] * 256.0 * 256.0 * 256.0 + tmpdata[1202 + 1206 * i] * 256 * 256 + tmpdata[1201 + 1206 * i] * 256 + tmpdata[1200 + 1206 * i]) / 1000;
		for (j = 0; j < 12; j++) {
			angle = tmpdata[3 + j * 100 + 1206 * i] * 256 + tmpdata[2 + j * 100 + 1206 * i];
			angle = angle + hang;
			//干渉部除去
			chkang = angle / 100.0;
			if (chkang > 180)chkang = chkang - 360;
			if (chkang < -180)chkang = chkang + 360;
			if (chkang > 180)chkang = chkang - 360;
			if (chkang < -180)chkang = chkang + 360;

			if ((chkang > 10 || chkang < -10) && tmpdata[1206 * i + j * 100] == 0xff && tmpdata[1206 * i + j * 100 + 1] == 0xee)for (k = 0; k < 32; k++) {
				angle2 = angle + vlp32choffset[k] * 100; if (angle2 < 0)angle2 = 36000 + angle2;

				ptmp[pnum].a = angle2 / 100.0 / 180.0 * PI;
				ptmp[pnum].l = (tmpdata[5 + j * 100 + 1206 * i + k * 3] * 256 + tmpdata[4 + j * 100 + 1206 * i + k * 3]) * 0.004;
				ptmp[pnum].x = ptmp[pnum].l * coscal[angle2] * coscalv[k];
				ptmp[pnum].y = ptmp[pnum].l * sincal[angle2] * coscalv[k];
				ptmp[pnum].z = ptmp[pnum].l * sincalv[k];

				if (true) {
					tx = ptmp[pnum].x;
					tz = ptmp[pnum].z;
					ptmp[pnum].x = tx * c1 - tz * s1;
					ptmp[pnum].z = tx * s1 + tz * c1;
				}
				ptmp[pnum].x = ptmp[pnum].x + VLP32X_B - POSOFFSET;
				ptmp[pnum].z = ptmp[pnum].z + VLP32Z_B;
				ptmp[pnum].i = tmpdata[6 + j * 100 + 1206 * i + k * 3];
				ptmp[pnum].t = timestp;

				tx = ptmp[pnum].x;
				ty = ptmp[pnum].y;
				ptmpX[pnum] = tx * c2 - ty * s2 + xpos;
				ptmpY[pnum] = tx * s2 + ty * c2 + ypos;
				ptmpZ[pnum] = ptmp[pnum].z + Lposz;
				ph = 0;
				ptmpZ[pnum] = ptmpZ[pnum] - ph;
				ptmpNG[pnum] = 0;
				if (ph == 0) { ptmpZ[pnum] = ptmp[pnum].z; ptmpNG[pnum] = 1; }
				if (ph != 0) {
					if (vhmin1[pnum % 32] > ptmpZ[pnum] && ptmp[pnum].x > 5)vhmin1[pnum % 32] = ptmpZ[pnum];
				}
				pnum++;
				if (pnum >= 40000)pnum = 39999;
			}
		}
	}

	int tpnum = 0;
	int NGflg = 0;

	for (i = 32; i < pnum - 32; i++) {
		NGflg = 0;

		if (fabs(ptmp[i - 32].z - ptmp[i + 32].z) < 0.05 && fabs(ptmp[i - 32].z - ptmp[i].z) > 0.2 && ptmp[i].l < 25) NGflg = 1;
		else if (ptmp[i].l > calrange || ptmp[i].l<2 || ptmpZ[i]>h || fabs(ptmp[i].y) > 25) NGflg = 1;
		else if (ptmp[i].x<4.0 && ptmp[i].x>-4.0 && ptmp[i].y<1.2 && ptmp[i].y>-1.2)NGflg = 1;

		if (NGflg == 0) {
			p_32B[tpnum] = ptmp[i];
			ptmpX2_32B[tpnum] = ptmpX[i];
			ptmpY2_32B[tpnum] = ptmpY[i];
			ptmpZ2_32B[tpnum] = ptmpZ[i];
			if (ptmpNG_32B[i] == 0 && vhmin1[i % 32] > 0 && ptmp[i].l < 80 && ptmp[i].x>5)ptmpZ2_32B[tpnum] = ptmpZ2_32B[tpnum] - vhmin1[i % 32];
			ptmpNG2_32B[tpnum] = ptmpNG_32B[i];
			tpnum++;
			if (tpnum >= 40000)tpnum = 39999;
		}
	}


	return(tpnum);
}

int velogetOS1() {
	int i, j, k;
	int datanum = 0;
	static long pnum = 0;
	double angle;
	int anglecal = 0;
	unsigned long timestp;
	double h;
	double VLP16VANGLE_OS1 = -100;
	double VLP16HANGLE_OS1 = 27000;
	double VLPX_OS1 = 2.60;
	double VLPY_OS1 = 0;
	double VLPZ_OS1 = 2.25;
	double c1, s1, tx, tz;

	c1 = cos(VLP16VANGLE_OS1 / 100.0 / 180.0 * PI);
	s1 = sin(VLP16VANGLE_OS1 / 100.0 / 180.0 * PI);

	pnum = 0;
	//	hang=0;

		//有意なデータ量の確認とコピー

	for (i = 0; i < 100; i++) {
		if (lpstr[1206 * 300 + 4352 * i] == 0 && lpstr[1206 * 300 + 1 + 4352 * i] == 0 && lpstr[1206 * 300 + 2 + 4352 * i] == 0)break;
	}
	datanum = i;
	memcpy(tmpdataOS1, lpstr + 1206 * 300, 4352 * datanum);
	//点群への変換
	//pnum=0;
	unsigned int tmpval;
	unsigned long tmpval2;




	for (i = 0; i < datanum; i++) {
		for (j = 0; j < 16; j++) {


			angle = (unsigned char)tmpdataOS1[32 + 8 + 268 * j + 4352 * i] + (unsigned char)tmpdataOS1[32 + 9 + 268 * j + 4352 * i] * 256;
			angle = angle / 1024.0 * 360.0;
			memcpy(&tmpval2, tmpdataOS1 + 32 + 268 * j + 4352 * i, 8);
			timestp = tmpval2;
			for (k = 0; k < 64; k++) {
				anglecal = (angle + hangOS1[k]) * 100 + VLP16HANGLE_OS1;
				if (anglecal < 0)anglecal = anglecal + 36000;
				ptmp[pnum].a = anglecal / 100.0 / 180.0 * PI;
				ptmp[pnum].l = (unsigned char)tmpdataOS1[44 + 4 * k + 268 * j + 4352 * i] + ((unsigned char)tmpdataOS1[45 + 4 * k + 268 * j + 4352 * i] & 0x7f) * 256;
				ptmp[pnum].l = ptmp[pnum].l * 0.008;
				//	ptmp[pnum].l=10;

				ptmp[pnum].i = (unsigned char)tmpdataOS1[46 + 4 * k + 268 * j + 4352 * i];
				ptmp[pnum].x = ptmp[pnum].l * coscal[anglecal] * coscalvOS1[k];
				ptmp[pnum].y = ptmp[pnum].l * sincal[anglecal] * (-1) * coscalvOS1[k];
				ptmp[pnum].z = ptmp[pnum].l * sincalvOS1[k] * (-1);

				tx = ptmp[pnum].x;
				tz = ptmp[pnum].z;
				ptmp[pnum].x = tx * c1 - tz * s1;
				ptmp[pnum].z = tx * s1 + tz * c1;
				ptmp[pnum].t = timestp;
				pnum++;

			}
		}
	}

	int tpnum = 0;
	int NGflg = 0;

	for (i = 64; i < pnum - 64; i++) {
		NGflg = 0;

		if (fabs(ptmp[i - 64].z - ptmp[i + 64].z) < 0.05 && fabs(ptmp[i - 64].z - ptmp[i].z) > 0.2) NGflg = 1;
		if (ptmp[i].l < 2.0) NGflg = 1;
		if (ptmp[i].z > 0) NGflg = 1;

		if (NGflg == 0) {

			p_OS1[tpnum].x = ptmp[i].x + VLPX_OS1 - POSOFFSET;
			p_OS1[tpnum].y = ptmp[i].y;
			p_OS1[tpnum].z = ptmp[i].z + VLPZ_OS1;
			p_OS1[tpnum].i = ptmp[i].i;
			p_OS1[tpnum].l = ptmp[i].l;
			p_OS1[tpnum].t = ptmp[i].t;
			tpnum++;
		}
	}

	return(tpnum);
}

void getlocaltime() {
	time_t now = time(NULL);
	struct tm* pnow = localtime(&now);

	now_year = pnow->tm_year + 1900;
	now_month = pnow->tm_mon + 1;
	now_day = pnow->tm_mday;
	now_hour = pnow->tm_hour;
	now_minute = pnow->tm_min;
	now_sec = pnow->tm_sec;
}
void saveBMP(HDC indc, int width, int height, LPCTSTR lpszFn) {

	DWORD dwSize, dwFSize, dwWidth, dwHeight, dwLength;
	HANDLE fh;
	LPBITMAPFILEHEADER lpHead;
	LPBITMAPINFOHEADER lpInfo;
	LPBYTE lpBuf, lpPixel;
	RECT rec;
	HDC hdc, hdcMem;
	HBITMAP hBMP, hOld;

	//	GetClientRect(hwnd,&rec); /* クライアント領域取得 */

	dwWidth = width;
	dwHeight = height;

	if ((dwWidth * 3) % 4 == 0) /* バッファの１ラインの長さを計算 */
		dwLength = dwWidth * 3;
	else
		dwLength = dwWidth * 3 + (4 - (dwWidth * 3) % 4);

	/* 書き込み用バッファのサイズ計算 */
	dwFSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwLength * dwHeight;

	/* バッファ確保とポインタ設定 */
	lpBuf = (LPBYTE)GlobalAlloc(GPTR, dwFSize);
	lpHead = (LPBITMAPFILEHEADER)lpBuf;
	lpInfo = (LPBITMAPINFOHEADER)(lpBuf + sizeof(BITMAPFILEHEADER));
	lpPixel = lpBuf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	/* 24ビットBMPファイルのヘッダ作成 */
	lpHead->bfType = 'M' * 256 + 'B';
	lpHead->bfSize = dwFSize;
	lpHead->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	lpInfo->biSize = sizeof(BITMAPINFOHEADER);
	lpInfo->biWidth = dwWidth;
	lpInfo->biHeight = dwHeight;
	lpInfo->biPlanes = 1;
	lpInfo->biBitCount = 24;

	/* ウインドウのデバイスコンテキスト取得 */
//	hdc=GetDC(hwnd);
	/* ウインドウのデバイスコンテキスト互換のBITMAP作成 */
	hBMP = CreateCompatibleBitmap(indc, dwWidth, dwHeight);
	hOld = CreateCompatibleBitmap(indc, dwWidth, dwHeight);
	/* BITMAPにウインドウのクライアント領域をコピー */
	hdcMem = CreateCompatibleDC(indc);
	SelectObject(hdcMem, hBMP);
	BitBlt(hdcMem, 0, 0, dwWidth, dwHeight, indc, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);
	GetDIBits(indc, hBMP, 0, dwHeight, lpPixel, (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);

	//	ReleaseDC(hwnd,hdc);
	DeleteObject(hBMP);
	DeleteObject(hdcMem);

	/* バッファをファイルに書き出す */
	fh = CreateFile(lpszFn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(fh, lpBuf, dwFSize, &dwSize, NULL);
	CloseHandle(fh);

	GlobalFree(lpBuf);

}


/*
		double cosang = cos(ang);
		double sinang = sin(ang);
		showpnt = 1;
		if (showpnt == 1 || showpnt == 8) {
			double tmpx, tmpy;

			for (int i = 0; i < velopointnum; i++) {
				tmpx = x + p[i].x * cosang - p[i].y * sinang;
				tmpy = y + p[i].x * sinang + p[i].y * cosang;
				DrawSphere3D(VGet(tmpx, p[i].z + 0.1, tmpy), 0.05, 4, GetColor(200, 0, 0), GetColor(255, 255, 255), TRUE);
			}
		}

		if (showpnt == 2 || showpnt == 8) {
			double tmpx, tmpy;
		for (int i = 0; i < velopointnum_32B; i++) {
		 tmpx = x + p_32B[i].x * cosang - p_32B[i].y * sinang;
		 tmpy = y + p_32B[i].x * sinang + p_32B[i].y * cosang;
		 DrawSphere3D(VGet(tmpx, p_32B[i].z + 0.1, tmpy), 0.05, 4, GetColor(0, 200, 200), GetColor(255, 255, 255), TRUE);
	 }
 }



if(showpnt==3 ||showpnt==8){
 for(int i=0;i<velopointnum_L;i++){
 double tmpx,tmpy;
 tmpx=x+p_L[i].x*cosang-p_L[i].y*sinang;
 tmpy=y+p_L[i].x*sinang+p_L[i].y*cosang;
 DrawSphere3D( VGet( tmpx, p_L[i].z + 0.1, tmpy ), 0.05, 0, GetColor( 0,200,0 ), GetColor( 255, 255, 255 ), TRUE ) ;
 }
 }

if(showpnt==4 ||showpnt==8){
 for(int i=0;i<velopointnum_F;i++){
 double tmpx,tmpy;
 tmpx=x+p_F[i].x*cosang-p_F[i].y*sinang;
 tmpy=y+p_F[i].x*sinang+p_F[i].y*cosang;
 DrawSphere3D( VGet( tmpx, p_F[i].z + 0.1, tmpy ), 0.05, 0, GetColor( 0,0,200 ), GetColor( 255, 255, 255 ), TRUE ) ;
 }
 }

if(showpnt==5 ||showpnt==8){
 for(int i=0;i<velopointnum_R;i++){
 double tmpx,tmpy;
 tmpx=x+p_R[i].x*cosang-p_R[i].y*sinang;
 tmpy=y+p_R[i].x*sinang+p_R[i].y*cosang;
 DrawSphere3D( VGet( tmpx, p_R[i].z + 0.1, tmpy ), 0.05, 0, GetColor( 200,200,0 ), GetColor( 255, 255, 255 ), TRUE ) ;
 }
 }

if (showpnt == 6 || showpnt == 8) {
	for (int i = 0; i < velopointnum_B; i++) {
		double tmpx, tmpy;
		tmpx = x + p_B[i].x * cosang - p_B[i].y * sinang;
		tmpy = y + p_B[i].x * sinang + p_B[i].y * cosang;
		DrawSphere3D(VGet(tmpx, p_B[i].z + 0.1, tmpy), 0.05, 0, GetColor(200, 0, 200), GetColor(255, 255, 255), TRUE);
	}
}

if (showpnt == 7 || showpnt == 8) {
	for (int i = 0; i < velopointnum_OS1; i++) {
		double tmpx, tmpy;
		tmpx = x + p_OS1[i].x * cosang - p_OS1[i].y * sinang;
		tmpy = y + p_OS1[i].x * sinang + p_OS1[i].y * cosang;
		DrawSphere3D(VGet(tmpx, p_OS1[i].z + 0.1, tmpy), 0.05, 0, GetColor(200, 200, 200), GetColor(255, 255, 255), TRUE);

	}
}


*/