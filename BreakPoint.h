#pragma once

#include "windows.h"
#include "typedef.h"

#define D_MAX_BREAKPOINTS	256

class BreakPoint
{
public:
	struct st_bp {
		bool enable;
		TW32U addr;
	};

	BreakPoint(void);
	BreakPoint(int);
	~BreakPoint(void);

	void Set(TW32U addr);
	void Del(TW32U addr);
	void Clear();
	bool Check(TW32U addr);

private:
	int m_bp_cnt;
	struct st_bp m_addrs[D_MAX_BREAKPOINTS];
	HANDLE		m_hMutex;
};
