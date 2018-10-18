#include "BreakPoint.h"

BreakPoint::BreakPoint(void)
{
	Clear();
}

BreakPoint::~BreakPoint(void)
{
}

void BreakPoint::Set(TW32U addr)
{
	int i;

	WaitForSingleObject( m_hMutex, INFINITE );

	if(m_bp_cnt >= D_MAX_BREAKPOINTS) {
		return;
	}

	for(i = 0; i < D_MAX_BREAKPOINTS; i++)
	{
		if(!m_addrs[i].enable) {
			m_addrs[i].addr = addr;
			m_addrs[i].enable = true;
			break;
		}
	}
	if(i >= m_bp_cnt) {
		m_bp_cnt++;
	}
	
	ReleaseMutex(m_hMutex);
}

void BreakPoint::Del(TW32U addr)
{
	int i;

	WaitForSingleObject( m_hMutex, INFINITE );

	if(m_bp_cnt <= 0) {
		return;
	}

	for(i = 0; i < D_MAX_BREAKPOINTS; i++)
	{
		if(m_addrs[i].enable && m_addrs[i].addr == addr) {
			m_addrs[i].addr = 0x0000;
			m_addrs[i].enable = false;
			break;
		}
	}
	
	// m_bp_cntの更新
	i = m_bp_cnt - 1;
	m_bp_cnt = 0;
	for(; i >= 0; i--) {
		if(m_addrs[i].enable) {
			m_bp_cnt = i + 1;
			break;
		}
	}

	ReleaseMutex(m_hMutex);
}

void BreakPoint::Clear()
{
	int i;

	WaitForSingleObject( m_hMutex, INFINITE );

	for(i = 0; i < D_MAX_BREAKPOINTS; i++)
	{
		m_addrs[i].enable = false;
		m_addrs[i].addr = 0x0000;
	}
	m_bp_cnt = 0;

	ReleaseMutex(m_hMutex);
}

bool BreakPoint::Check(TW32U addr)
{
	int i;
	bool ret = false;

	WaitForSingleObject( m_hMutex, INFINITE );

	for(i = 0; i < m_bp_cnt; i++)
	{
		if(m_addrs[i].enable && m_addrs[i].addr == addr)
		{
			ret = true;
			break;
		}
	}

	ReleaseMutex(m_hMutex);

	return ret;
}

