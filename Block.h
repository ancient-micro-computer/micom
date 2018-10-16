/******************************************************************************
 *  ファイル名
 *      Block.h
 *  概要
 *      ブロックの基底クラス
 ******************************************************************************/

#ifndef __C_SIM_BLOCK_H__
#define __C_SIM_BLOCK_H__

#include "typedef.h"
#include "define.h"

namespace Simulator {

class Bus;

public class Block
{
public:
	Block();
    Block(TINT id);
	TVOID SetParentBus(Bus* pBus);
    virtual ~Block();

	// ブロック制御メソッド
	virtual	TINT	Reset(TVOID);									// リセット
    virtual TINT    Exec();											// 機能の実行関数
    virtual TINT    Exec(int module_id);							// 特定モジュールの実行関数
    virtual TINT    Status(TVOID);                                  // ステータス取得

	// リソースI/F定義(以下全て純粋仮想関数となっているので、派生クラスにおいて必ず実体の定義を行うこと)
    virtual TINT    GetReg(TINT addr, TW32U &value) = 0;							// レジスタget/set
    virtual TINT	SetReg(TINT addr, TW32U value) = 0;
    virtual TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid) = 0;	// メモリget/set
    virtual TINT	SetMem(TW32U addr, TW32U value) = 0;
	virtual	TW32U*	MemPtr(TW32U addr) = 0;								// メモリポインタ取得

	virtual TINT	Interrupt(int src_module_id, TW32U &param);			// 割込み受付け

	inline TINT GetID() { return m_ID; }

protected:
    TINT    m_Busy;                                                 // ビジーフラグ
	TINT	m_ID;
	Bus*	m_ParentBus;
};

};

#endif  //__C_SIM_BLOCK_H__
