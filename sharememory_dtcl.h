#include <windows.h>

HANDLE hFMWrite,hFMWrite2,hFMWrite3;

struct CAN {
	char time1;
	char time2;
	char time3;
	char length;
	char d0;
	char d1;
	char d2;
	char d3;
	char d4;
	char d5;
	char d6;
	char d7;
};

CAN *candat;
double *sm;
double* sm_dtcl;


void sharememoryinit(){
int i;
if (!hFMWrite)CloseHandle(hFMWrite);
hFMWrite = CreateFileMapping(
       (HANDLE)-1, 
       NULL,
       PAGE_READWRITE,
       0, 
       sizeof(CAN)*2048,
       "CANDATA");


if (!hFMWrite2)CloseHandle(hFMWrite2);
hFMWrite2 = CreateFileMapping(
       (HANDLE)-1, 
       NULL,
       PAGE_READWRITE,
       0, 
       sizeof(double)*2000,
       "SHAREDATA");
if (!hFMWrite3)CloseHandle(hFMWrite3);
hFMWrite3 = CreateFileMapping(
       (HANDLE)-1, 
       NULL,
       PAGE_READWRITE,
       0, 
       sizeof(double)*2000,
       "SHAREDATA_DTCL");


sm=(double *)malloc(sizeof(double)*2000);
sm= (double *)MapViewOfFile(hFMWrite2,FILE_MAP_ALL_ACCESS, 0, 0, 0);

sm_dtcl=(double *)malloc(sizeof(double)*2000);
sm_dtcl= (double *)MapViewOfFile(hFMWrite3,FILE_MAP_ALL_ACCESS, 0, 0, 0);

candat=(CAN *)malloc(sizeof(CAN)*2048);
candat =(CAN *)MapViewOfFile(hFMWrite,FILE_MAP_ALL_ACCESS, 0, 0, 0);

}