#pragma once

#include "typedef.h"
#include "Block.h"

#define D_CELL_SIZE 8192

namespace Simulator {

class MemModule : public Block
{
public:
	MemModule();
	~MemModule();

    TINT    Reset(TVOID);                                   // ブロックリセット
    TINT    Exec();											// ブロック機能の実行関数
    TINT    Status(TVOID);                                  // ブロックステータス取得

	TINT    GetReg(TINT addr, TW32U &value);						// レジスタget/set
    TINT	SetReg(TINT addr, TW32U value);
    TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid);// メモリget/set
    TINT    SetMem(TW32U addr, TW32U value);
	TW32U*	MemPtr(TW32U addr);

private:
	TW32U *m_cells;
};

};
