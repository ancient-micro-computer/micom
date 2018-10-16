#pragma once

#include "define.h"
#include "simulator.h"
#include "typedef.h"
#include "windows.h"
#define D_MAX_MODULES	32	// 最大32モジュールまで接続可能とする

namespace Simulator {
class Block;

public class Bus
{
	struct st_MODULE_INFO
	{
		int module_id;		// モジュールID
		int addr_assign;	// 0:アドレス割当て無し 1:アドレス割当て有り
		unsigned long start_address;
		unsigned long end_address;
		Block *p_module;
	};

public:
	Bus();
	~Bus();

	int connect(int module_id, int addr_assign, unsigned long start_address, unsigned long end_address, Block *p_module);

    int Reset(TVOID);                                   // ブロックリセット
    int Exec();											// ブロック機能の実行関数
	int Exec(int module_id);							// 特定モジュールの起動
    int Status(TVOID);                                  // ブロックステータス取得

	int GetStatus(int);

	int set_reg(int id, TINT addr, TW32U &value);
	int get_reg(int id, TINT addr, TW32U &value);

	int set_address(unsigned long address);
	int set_data(unsigned long data);
	unsigned long get_address(void);
	unsigned long get_data(void);
	int access_read(void);
	int access_write(void);
	int lock();		// バスアクセス前
	int unlock();		// バスアクセス完了後

	int InterruptRequest(int src_module_id, int dst_module_id, TW32U &value);	// 割込み通知

private:
	HANDLE   m_hBusMutex;
	HANDLE   m_hBusMutexSub;
	bool	 m_locking;	// true=バスロック中
	unsigned long m_address;
	unsigned long m_data;
	int  m_modules;
	struct st_MODULE_INFO m_moduleinfo[D_MAX_MODULES];
};

};