#ifndef __C_MODULE_TIMER_H__
#define __C_MODULE_TIMER_H__

#include "typedef.h"
#include "simulator.h"
#include "Block.h"
#include "CpuModule.h"

namespace Simulator {
    public class TimerModule : public Block
    {
    public:
        TimerModule();
        ~TimerModule();

		TINT    GetReg(TINT addr, TW32U &value);                          // レジスタget/set
        TINT    SetReg(TINT addr, TW32U value);
        TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid);// メモリget/set
        TINT    SetMem(TW32U addr, TW32U value);
        TW32U*	MemPtr(TW32U addr);

        TINT    Reset(TVOID);                                   // ブロックリセット
        TINT    Exec(int module_id);							// ブロック機能の実行関数
        TINT    Status(TVOID);                                  // ブロックステータス取得
	private:

	};
};


#endif	// __C_MODULE_TIMER_H__
