#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")


static UINT	TimerPeriod = 0;
static HWND		hWndPeriod;
BOOL	f_MMTimerPost = TRUE;
static MMRESULT	mID;
static TIMECAPS TimerCaps = { 0, 0 };
//static void CALLBACK TimerPeriodProc(UINT wID, UINT wUser, DWORD dwUser, DWORD dw1, DWORD dw2);
static void CALLBACK TimerPeriodProc(UINT wID, UINT wUser, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

UINT		MMTimerInit();
MMRESULT	MMTimerOnce(HWND, UINT);
MMRESULT	MMTimerPeriod(HWND, UINT);
BOOL		MMTimerStop();
extern void main2();



MMRESULT MMTimerPeriod(HWND hwnd, UINT uDelay)
{
	hWndPeriod = hwnd;
	TimerPeriod = 0;
	f_MMTimerPost = TRUE;
	mID = timeSetEvent(uDelay, TimerCaps.wPeriodMin, TimerPeriodProc,
		0, TIME_PERIODIC);
	return mID;
}


UINT MMTimerInit()
{
	timeGetDevCaps(&TimerCaps, sizeof(TimerCaps));
	timeBeginPeriod(TimerCaps.wPeriodMin);
	return TimerCaps.wPeriodMin;
}

BOOL MMTimerStop()
{

	if (mID){
	timeKillEvent(mID);
	mID = 0;
	}

	if (TimerCaps.wPeriodMin){
	timeEndPeriod(TimerCaps.wPeriodMin);
	TimerCaps.wPeriodMin = 0;
	}
	return TRUE;
}

static void CALLBACK TimerPeriodProc(UINT wID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    main2();
}