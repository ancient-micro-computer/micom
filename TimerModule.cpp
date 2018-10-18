/******************************************************************************
 *  ファイル名
 *      Timer.cpp
 *  ブロック名
 *      -
 *  概要
 *      汎用タイマークラス定義
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "macro.h"
#include "Block.h"
#include "Bus.h"
#include "TimerModule.h"

using namespace Simulator;

// コンストラクタ：Block接続処理が実行されるタイミングでコールされます
TimerModule::TimerModule() : Block()
{
}

TimerModule::~TimerModule()
{
}

/*============================================================================*
 *  関数名
 *      リセット
 *  概要
 *      ブロックリセット
 *  パラメタ説明
 *      なし
 *  戻り値
 *      0           : 正常終了、other : 異常終了
 *============================================================================*/
TINT    TimerModule::Reset(TVOID)
{
	return D_ERR_OK;
}

/*============================================================================*
 *  関数名
 *      ブロックプロパティ
 *  概要
 *      ブロックに関する情報を返す
 *  パラメタ説明
 *      なし
 *  戻り値
 *      0           : 正常終了、other : 異常終了
 *============================================================================*/
TINT    TimerModule::Status(TVOID)                                  
{
	return D_ERR_OK;
}

/*============================================================================*
 *  関数名
 *      ブロック機能実行
 *  概要
 *      ブロック機能に関する実装
 *  パラメタ説明
 *      cmd         : コマンドID
 *      addr        : アドレス
 *      data        : データ（Verilog形式のコマンド仕様に依存します）
 *  戻り値
 *      0           : 正常終了、other : 異常終了
 *============================================================================*/
TINT    TimerModule::Exec(int module_id)
{
	TW32U param;

	// CPUモジュールに割込み通知
	param = Interrupt_Timer;

	m_ParentBus->lock();
	m_ParentBus->InterruptRequest(D_MODULEID_GTMR, D_MODULEID_CPU, param);
	m_ParentBus->unlock();
	return D_ERR_OK;
}

/*============================================================================*
 *  関数名
 *      アクセスI/F
 *  概要
 *      ブロック内リソースとのGet&Set-I/F定義
 *  パラメタ説明
 *      MemID       : メモリID
 *      addr        : アクセスアドレス
 *      value       : 書き込み値(Set時)
 *  戻り値
 *      0           : 正常終了、other : 異常終了
 *============================================================================*/
TINT   TimerModule::GetReg(TINT addr, TW32U &value)
{
	return D_ERR_OK;
}

TINT    TimerModule::SetReg(TINT addr, TW32U value)
{
	return D_ERR_OK;
}

TINT   TimerModule::GetMem(TW32U addr, TW32U &value, TW32U &valid)
{
	return D_ERR_OK;
}

TINT    TimerModule::SetMem(TW32U addr, TW32U value)
{
	return D_ERR_OK;
}

TW32U*	TimerModule::MemPtr(TW32U addr)
{
	return NULL;	// Null→ポインタ変数名に変更して下さい
}


