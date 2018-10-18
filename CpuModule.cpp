/*============================================================================*
　接続するブロック（機能エンジンモジュール）毎に、本ラッパー関数を個別に定義する
　ことで、モジュールの機能定義とUI接続定義を分離しています。

　機能モジュール側でもレジスタやメモリの入出力関数を定義をして下さい。
 
 *============================================================================*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mutex>
#include "typedef.h"
#include "simulator.h"
#include "macro.h"
#include "CpuModule.h"
#include "Bus.h"

using namespace Simulator;

//変数宣言
T_ORDER_LIST gCodeParam[] = {
//オぺレーションコード、実行サイクル、オペランド数(fetch回数)
		{0,  1, 0},
		{1,  1, 0},
		{2,  2, 2},
		{3,  2, 2},
		{4,  2, 2},
		{5,  2, 2},
		{6,  2, 2},
		{7,  2, 2},
		{8,  2, 2},
		{9,  2, 2},
		{10, 3, 2},
		{11, 3, 2},
		{12, 6, 2},
		{13, 6, 2},
		{14, 1, 1},
		{15, 1, 1},
		{16, 3, 2},
		{17, 3, 2},
		{18, 3, 1},
		{19, 1, 2},
		{20, 1, 2},
		{21, 1, 2},
		{22, 1, 2},
		{23, 2, 2},
		{24, 2, 2},
		{25, 2, 2},
		{26, 2, 2},
		{27, 2, 2},
		{28, 2, 2},
		{29, 2, 2},
		{30, 2, 2},
		{31, 2, 2},
		{32, 4, 3},
		{33, 4, 3},
		{34, 4, 2},
		{35, 2, 2},
		{36, 2, 2},
		{37, 2, 2},
		{38, 2, 2},
		{39, 2, 1},
		{40, 2, 1},
		{41, 2, 1},
		{42, 3, 2},
		{43, 2, 1},
		{44, 2, 1},
		{45, 2, 1},
		{46, 2, 1},
		{47, 2, 1},
		{48, 2, 1},
		{49, 2, 1},
		{50, 2, 1},
		{51, 2, 1},
		{52, 2, 1},
		{53, 2, 1},
		{54, 2, 1},
		{55, 2, 1},
		{56, 2, 1},
		{57, 2, 0},
		{58, 1, 0},
		{59, 1, 0},
		{60, 1, 0},
		{61, 2, 0}
};

// 割込み情報
T_INTERRUPT_INFO gInterruptList[] = {
	{Interrupt_Timer,		CpuModule_BIT0},
	{Interrupt_Software,	CpuModule_BIT1},
	{Interrupt_Dma,			CpuModule_BIT2},
	{Interrupt_Trigger0,	CpuModule_BIT3},
	{Interrupt_Trigger1,	CpuModule_BIT4},
	{Interrupt_Trigger2,	CpuModule_BIT5},
	{Interrupt_SCIRecv,		CpuModule_BIT6},
	{Interrupt_DETrap,		0},
	{0, 0}
};

// コンストラクタ：Block接続処理が実行されるタイミングでコールされます
CpuModule::CpuModule() : Block()
{
	//レジスタ初期化
	memset(m_cpu_regs, 0x00, sizeof(TW16U) * 32);

	//命令文格納領域初期化
	for(int i = 0; i < 4; i++) {
		m_value_oc[i] = 0;
	}
	//サイクル値の初期化
	m_cycleCount = 0;
	m_Busy = 0;

	// 割込み処理関連初期化
	m_InterruptTrap = 0x0;
	m_InterruptEnable = false;

}

CpuModule::~CpuModule()
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
TINT    CpuModule::Reset(TVOID)
{
	TINT counter;
	
	//必要に応じてエンジン固有の初期化処理関数をコールして下さい
	//レジスタ初期化
	for(counter = 0;counter <= 15;counter++){
		SetReg(CpuModule_BASE_ADDR+counter,0x0000);
	}
	//スタックポインタ初期化
	SetReg(CpuModule_SP_ADDR,0xf000);

	//命令文格納領域初期化
	int i;
	for(i = 0; i < 4; i++) {
		m_value_oc[i] = 0;
	}
	//サイクル値の初期化
	m_cycleCount = 0;

	m_Busy = 0;

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
TINT    CpuModule::Status(TVOID)                                  
{
    return m_Busy;							// ブロックステータス取得
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
TINT    CpuModule::Exec()
{
	/*  以下 cmd,addr,data の値に応じた処理を記述して下さい。
        戻り値に D_CMDIF_ACK を設定すると、スクリプトエンジンは次のコマンドを読み込みます
        戻り値に D_CMDIF_NACK を設定すると、スクリプトエンジンは同一コマンドを与え続けます(次のコマンドに進みません)
        戻り値に D_CMDIF_VL_QUIT を設定するとスクリプト処理を途中でも強制終了します

        戻り値に D_EXEC_OK   を設定すると、ブロックの実行を継続します
        戻り値に D_EXEC_HOLD を設定すると、実行を終了します

		なお、go コマンド実行時は、cmd,addr,data = -1, -1, -1 固定となっています
    */
	//TW32U theTmp = 0x0, valid = 0x01010101;
	TINT fetchCount = 0;			//フェッチ処理回数格納変数
	TINT status;					//Exec処理結果格納変数
	TW32U	reg_value = 0x0;
	TW32U	reg_pc = 0x0;
	TW32U	int_val;

	//変数初期化
	status = 0;

	// 割込みチェック
	if(m_cycleCount <= 0){
		// Critical Section Start
		WaitForSingleObject( m_Mutex, INFINITE );

		// 割込み有効＆現在割込み処理中でなければ
		if(m_InterruptEnable) {
			// 割込み設定を取得
			GetReg(CpuModule_INT_ADDR, reg_value);

			// 割込みチェック
			for(int intnum = 0; gInterruptList[intnum].reason != 0; intnum++) {
				if( (m_InterruptTrap & gInterruptList[intnum].reason) == gInterruptList[intnum].reason) {
					if( (gInterruptList[intnum].mask > 0 && (reg_value & gInterruptList[intnum].reason) == 0) || gInterruptList[intnum].mask == 0) {
						// 割込み発生
						reg_value |= gInterruptList[intnum].reason;
						SetReg(CpuModule_INT_ADDR, reg_value);				// 割込み要因設定
						// ※次に割込みを受け付けるためには、割込みルーチン内で割込み要因をクリアすること

						m_InterruptTrap &= (0xFFFFFFFF - gInterruptList[intnum].reason);	// 割込みトラップ解除

						//PC内の値をスタックに退避
						pushr(CpuModule_PC_ADDR);

						//割込みベクタへ分岐
						m_ParentBus->lock();
						m_ParentBus->set_address(Interrupt_Vector_Table + intnum);
						m_ParentBus->access_read();
						reg_pc = m_ParentBus->get_data();
						m_ParentBus->unlock();
						SetReg(CpuModule_PC_ADDR, reg_pc);
					}
				}
			}
		}

		// Critical Section End
		ReleaseMutex(m_Mutex);
	}

	//オペランドコードの読み込み処理
	if(m_cycleCount <= 0){
		for(int i = 0;i <= fetchCount && (fetchCount >= 0);i++){
			//フェッチ処理
			m_value_oc[i] = fetch();
			if(i == 0){
				//デコード処理（フェッチ回数設定）
				fetchCount = decode(m_value_oc[0],m_cycleCount);
				if(fetchCount == D_ERR_CMDERR) {
					int_val = Interrupt_DETrap;
					Interrupt(D_MODULEID_CPU, int_val);	// データ実行例外割込み
					m_cycleCount = 0;					// 命令実行中止
					break;	// デコードエラー検出
				}
			}
			
		}
	}
	//実行サイクル判定
	if(m_cycleCount == 1 /*&& fetchCount >= 0*/){
		//実行処理
		exe(m_value_oc[0],m_value_oc[1],m_value_oc[2],m_value_oc[3]);
	}

	//サイクルカウント更新
	m_cycleCount--;

	//結果判断
	switch(m_value_oc[0]){
		case HOLT_OC:
			status = D_EXEC_HOLD;	// エンジン実行処理終了の場合
			break;
		default:
			status = D_EXEC_OK;		// 継続実行中の場合
			break;
	}
	return status;
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
TINT   CpuModule::GetReg(TINT addr, TW32U &value)
{
    /* addrで示すレジスタアドレスのレジスタ値を value に設定する処理を記述して下さい */
	TINT	baddr;
	if(addr < CpuModule_BASE_ADDR){
		baddr = (addr+CpuModule_USR_ADDR) - CpuModule_BASE_ADDR;
	} else {
		baddr = addr - CpuModule_BASE_ADDR;
	}

	value = m_cpu_regs[baddr];
    return D_ERR_OK;
}

TINT    CpuModule::SetReg(TINT addr, TW32U value)
{
    /* addrで示すレジスタアドレスにレジスタ値 value を設定する処理を記述して下さい */
	TINT	baddr;
	if(addr < CpuModule_BASE_ADDR){
		baddr = (addr+CpuModule_USR_ADDR) - CpuModule_BASE_ADDR;
	} else {
		baddr = addr - CpuModule_BASE_ADDR;
	}

	m_cpu_regs[baddr] = (TW16U)value;
    return D_ERR_OK;
}

TINT   CpuModule::GetMem(TW32U addr, TW32U &value, TW32U &valid)
{
    /* MemIDで示される対象メモリ(バンク番号=MemBank)の、アドレスaddrから値をReadしてvalueに設定します */
    //上位bitを消す
	addr = addr & 0x0000ffff;

	m_ParentBus->lock();
	m_ParentBus->set_address(addr);
	m_ParentBus->access_read();
	value = m_ParentBus->get_data();
	m_ParentBus->unlock();

    return D_ERR_OK;
}

TINT    CpuModule::SetMem(TW32U addr, TW32U value)
{
    /* MemIDで示される対象メモリ(バンク番号=MemBank)の、アドレスaddrへ値valueをWriteします */
	m_ParentBus->lock();
	m_ParentBus->set_address(addr);
	m_ParentBus->set_data(value);
	m_ParentBus->access_write();
	m_ParentBus->unlock();

	return D_ERR_OK;
}

TW32U*	CpuModule::MemPtr(TW32U addr)
{
	return NULL;	// Null→ポインタ変数名に変更して下さい
}

/*============================================================================*
 *  関数名
 *      割込み受付け
 *  概要
 *      外部タイマ等からの割込み受付け
 *  パラメタ説明
 *      src_module_id : 割込み元モジュールID(バスから渡される)
 *      param         : パラメータ
 *  戻り値
 *      0           : 正常終了、other : 異常終了
 *============================================================================*/
TINT CpuModule::Interrupt(int src_module_id, TW32U &param)
{
	TW32U	reg_value;

	// Critical Section Start
	WaitForSingleObject( m_Mutex, INFINITE );

	if(m_InterruptEnable) {
		// 割込み設定を取得
		GetReg(CpuModule_INT_ADDR, reg_value);

		// 割込み要因チェック
		switch(param) {
			case Interrupt_Timer:
				if((reg_value & CpuModule_BIT0) == CpuModule_BIT0) {
					m_InterruptTrap |= Interrupt_Timer;		// タイマー割込み
				}
				break;

			case Interrupt_Software:
				if((reg_value & CpuModule_BIT1) == CpuModule_BIT1) {
					m_InterruptTrap |= param;				// ソフトウェア割込み
				}
				break;

			case Interrupt_Dma:
				if((reg_value & CpuModule_BIT2) == CpuModule_BIT2) {
					m_InterruptTrap |= Interrupt_Dma;		// DMA転送完了割込み
				}
				break;

			case Interrupt_Trigger0:
				if((reg_value & CpuModule_BIT3) == CpuModule_BIT3) {
					m_InterruptTrap |= Interrupt_Trigger0;	// 外部トリガー0割込み
				}
				break;

			case Interrupt_Trigger1:
				if((reg_value & CpuModule_BIT4) == CpuModule_BIT4) {
					m_InterruptTrap |= Interrupt_Trigger1;	// 外部トリガー1割込み
				}
				break;

			case Interrupt_Trigger2:
				if((reg_value & CpuModule_BIT5) == CpuModule_BIT5) {
					m_InterruptTrap |= Interrupt_Trigger2;	// 外部トリガー2割込み
				}
				break;

			case Interrupt_SCIRecv:
				if((reg_value & CpuModule_BIT6) == CpuModule_BIT6) {
					m_InterruptTrap |= Interrupt_SCIRecv;	// シリアル受信割込み
				}
				break;

			case Interrupt_DETrap:
				m_InterruptTrap |= param;					// データ実行例外
				break;

			default:
				break;
		}
	}

	// Critical Section End
	ReleaseMutex(m_Mutex);
	return D_EXEC_OK;		// 継続実行中の場合
}

/*============================================================================*
 *  関数名
 *      フェッチ
 *  概要
 *      PCレジスタが指すメモリアドレスのオペコードを取得する
 *  パラメタ説明
 *
 *      
 *  戻り値
 *      	メモリ番地
 *============================================================================*/
TW16U	CpuModule::fetch(TVOID)
{
	TW32U value,valid,mem_value = 0;

	//PCの値取得
	GetReg(CpuModule_PC_ADDR,value);

	//PCが指し示すメモリアドレス値を取得
	GetMem(value, mem_value, valid);

	//PCの値更新
	SetReg(CpuModule_PC_ADDR,++value);

	return (TW16U) mem_value;
}
/*============================================================================*
 *  関数名
 *      デコード
 *  概要
 *      オペ値から実行するメソッドの値を返す
 *  パラメタ説明
 *			mem_value　　:　オペレーションコード値
 *			exeycle　　　:　オペレーションサイクル格納変数
 *      
 *  戻り値
 *      int		フェッチ処理回数値
 *============================================================================*/
TINT	CpuModule::decode(TW16U mem_value,TINT &exeycle)
{
	//オペレーション値確認
	if(mem_value < OC_MIN || mem_value > OC_MAX)
	{
		return D_ERR_CMDERR;
	}

	//実行サイクル値
	exeycle = gCodeParam[mem_value].cycle;

	return gCodeParam[mem_value].operand;
}
/*============================================================================*
 *  関数名
 *      実行
 *  概要
 *     値から実行するメソッドを選択し実行メソッドを呼び出す
 *  パラメタ説明
 *			value　　:	オペレーションコード　
 *          addr1    :　オペランド１
 *          addr2    :　オペランド２
 *          addr3    :　オペランド３
 *      
 *  戻り値
 *      int		実行メソッドインデックス
 *============================================================================*/
TINT	CpuModule::exe(TINT value,TW16U addr1,TW16U addr2,TW16U addr3)
{
	switch(value){
		case NOP_OC:
			nop();
			break;
		case HOLT_OC:
			halt();
			break;
		case ADD_R_M_OC:
			addrm(addr1,addr2);
			break;
		case ADD_R_R_OC:
			addrr(addr1,addr2);
			break;
		case SUB_R_M_OC:
			subrm(addr1,addr2);
			break;
		case SUB_R_R_OC:
			subrr(addr1,addr2);
			break;			
		case INC_R_OC:
			incr(addr1);
			break;
		case DEC_R_OC:
			decr(addr1);
			break;
		case CMP_R_M_OC:
			cmprm(addr1,addr2);
			break;
		case CMP_R_R_OC:
			cmprr(addr1,addr2);
			break;
		case CMP_R_OC:
			cmpr(addr1);
			break;
		case SLA_R_M_OC:
			slarm(addr1,addr2);
			break;
		case SRA_R_M_OC:
			slarr(addr1,addr2);
			break;
		case AND_R_M_OC:
			andrm(addr1,addr2);
			break;
		case AND_R_R_OC:
			andrr(addr1,addr2);
			break;
		case OR_R_M_OC:
			orrm(addr1,addr2);
			break;
		case OR_R_R_OC:
			orrr(addr1,addr2);
			break;
		case XOR_R_M_OC:
			xorrm(addr1,addr2);
			break;
		case XOR_R_R_OC:
			xorrr(addr1,addr2);
			break;

		//Bit演算
		case TST_R_B_OC:
			tstrb(addr1, addr2);
			break;

		case SET_R_B_OC:
			setrb(addr1, addr2);
			break;

		case RST_R_B_OC:
			rstrb(addr1, addr2);
			break;

		case MOV_R_M_O_OC:
			movrmo(addr1,addr2,addr3);
			break;
		case MOV_R_R_O_OC:
			movrro(addr1,addr2,addr3);
			break;
		case MOV_R_R_OC:
			movrr(addr1,addr2);
			break;
		case MOV_R_M_OC:
			movrm(addr1,addr2);
			break;
		case MOV_P_M_OC:
			movpm(addr1,addr2);
			break;
		case MOV_S_M_OC:
			movsm(addr2);
			break;
		case MOV_C_M_OC:
			movcm(addr2);
			break;
		case PUSH_R_OC:
			pushr(addr1);
			break;
		case POP_R_OC:
			popr(addr1);
			break;
		case BR_M_OC:
			brm(addr1);
			break;
		case BR_R_O_OC:
			brro(addr1,addr2);
			break;
		case BRZ_M_OC:
			brzm(addr1);
			break;
		case BRNZ_M_OC:
			brnzm(addr1);
			break;
		case CALL_M_O_OC:
			callm(addr1);
			break;
		case CALL_R_OC:
			callr(addr1);
			break;
		case RET_OC:
			ret();
			break;
		case BRGT_M_OC:
			brgt(addr1);
			break;
		case BRLT_M_OC:
			brlt(addr1);
			break;
		case BREQ_M_OC:
			breq(addr1);
			break;
		case BRNEQ_M_OC:
			brneq(addr1);
			break;

		//割り込み
		case EI_OC:
			ei();
			break;

		case DI_OC:
			di();
			break;

		case SWI_OC:
			swi();
			break;

		case RETI_OC:
			reti();
			break;

		/*実行メソッドを随時追加していく*/
		default :
			halt();		// Unknown mnemonic. Force halt cpu.
			break;
	}
	return D_ERR_OK;
}

/*============================================================================*
 *  関数名
 *      　　実行メソッド群
 *  概要
 *     オペコードを実際に実現する際の処理メソッド群
 *  パラメタ説明
 *      
 *  戻り値
 *      
 *============================================================================*/
//nop
TINT	CpuModule::nop(TVOID){
	return D_ERR_OK;
}
//halt
TINT	CpuModule::halt(TVOID){
	m_Busy = D_EXEC_HOLD;
	return D_ERR_OK;
}
//add reg mem
TINT	CpuModule::addrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);
	
	//reg+valueをレジスタに格納する
	SetReg(reg,reg_value+value);

	return D_ERR_OK;
}
//add reg reg
TINT	CpuModule::addrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	//レジスタAから値Aを取り出す
	GetReg(reg1,value1);
	
	//レジスタAから値Bを取り出す
	GetReg(reg2,value2);
	
	//A+Bをレジスタに格納する
	SetReg(reg1,value1+value2);

	return D_ERR_OK;
}
//sub reg mem
TINT	CpuModule::subrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//reg-valueをレジスタに格納する
	SetReg(reg,reg_value-value);

	return D_ERR_OK;
}
//sub reg reg
TINT	CpuModule::subrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	//値Aを取り出す
	GetReg(reg1,value1);

	//値Bを取り出す
	GetReg(reg2,value2);

	//A-Bをレジスタに格納する
	SetReg(reg1,value1-value2);
	return D_ERR_OK;
}
//inc reg
TINT	CpuModule::incr(TW16U reg1){
	TW32U value1;

	GetReg(reg1,value1);

	//++reg_value をレジスタに格納する
	SetReg(reg1,++value1);
	return D_ERR_OK;
}
//dec reg
TINT	CpuModule::decr(TW16U reg1){
	TW32U value1;

	GetReg(reg1,value1);

	//--reg_value をレジスタに格納する
	SetReg(reg1,--value1);
	return D_ERR_OK;
}
//cmp reg mem
TINT	CpuModule::cmprm(TW16U reg,TW16U value){
	TW32U reg_value,ccr_value;

	//レジスタの値を取得する
	GetReg(reg,reg_value);

	//ccrレジスタの値を取得する
	GetReg(CpuModule_CCR_ADDR,ccr_value);


	if(reg_value > value){				//左が大きい
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT5;
	}	
	if(reg_value == value){				//等しい
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT6;
	}
	if(reg_value < value){				//左が小さい
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
	}

	SetReg(CpuModule_CCR_ADDR,ccr_value);
	return D_ERR_OK;
}
//cmp reg reg
TINT	CpuModule::cmprr(TW16U reg1,TW16U reg2){
	TW32U value1,value2,ccr_value;

	//レジスタの値Aを取得する
	GetReg(reg1,value1);

	//レジスタの値Bを取得する
	GetReg(reg2,value2);

	//ccrレジスタの値を取得する
	GetReg(CpuModule_CCR_ADDR,ccr_value);

	if(value1 > value2){				//左が大きい
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT5;
	} else if(value1 == value2){		//等しい
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT6;
	} else {							//左が小さい
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
	}

	SetReg(CpuModule_CCR_ADDR,ccr_value);
	return D_ERR_OK;
}
//cmp reg
TINT	CpuModule::cmpr(TW16U reg){
	TW32U value1,ccr_value;

	//レジスタの値Aを取得する
	GetReg(reg,value1);

	//ccrレジスタの値を取得する
	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//値は０か　y = 1	n = 0
	if(value1 == 0){
		ccr_value = ccr_value | CpuModule_BIT1;
	} else{
		ccr_value = ccr_value & (CpuModule_BIT1 ^ 0xffff);
	}
	//値はマイナスか　y = 1		n = 0
	if(value1 < 0){
		ccr_value = ccr_value | CpuModule_BIT2;
	} else {
		ccr_value = ccr_value & (CpuModule_BIT2 ^ 0xffff);
	}
	//値は奇数　or 偶数	  奇 = 1　偶 = 0
	if(value1%2){
		ccr_value = ccr_value | CpuModule_BIT3;
	} else {
		ccr_value = ccr_value & (CpuModule_BIT3 ^ 0xffff);
	}
	SetReg(CpuModule_CCR_ADDR,ccr_value);


	return D_ERR_OK;
}
//sla reg mem
TINT	CpuModule::slarm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//値を引数分左シフトする
	reg_value <<= value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//sla reg reg
TINT	CpuModule::slarr(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//値を引数値右シフトする
	reg_value >>= value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//and reg mem
TINT	CpuModule::andrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//値と引数値の積をとる
	reg_value = reg_value & value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//and reg reg
TINT	CpuModule::andrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	GetReg(reg1,value1);

	GetReg(reg2,value2);

	//reg1とreg2の積をとる
	value1 = value1 & value2;

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//or reg mem
TINT	CpuModule::orrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//regと引数値の和をとる
	reg_value = reg_value | value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//or reg reg
TINT	CpuModule::orrr(TW16U reg1,TW16U reg2)	{
	TW32U value1,value2;

	GetReg(reg1,value1);

	GetReg(reg2,value2);

	//reg1とreg2の和をとる
	value1 = value1 | value2;

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//xor reg mem
TINT	CpuModule::xorrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//regと引数値の排他をとる
	reg_value = reg_value ^ value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//xor reg reg
TINT	CpuModule::xorrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	GetReg(reg1,value1);

	GetReg(reg2,value2);

	//reg1とreg2の排他をとる
	value1 = value1 ^ value2;

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//mov reg mem offset
TINT	CpuModule::movrmo(TW16U reg1,TW16U value,TW16U of){
	TW32U value1,valid;

	//引数値のメモリ値を取得する
	GetMem((value+of),value1,valid);

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//mov reg reg offset
TINT	CpuModule::movrro(TW16U reg1,TW16U reg2,TW16U of){
	TW32U reg2_value,value1,valid;

	GetReg(reg2,reg2_value);

	//レジスタ値と引数値を足したメモリ値を取得する
	GetMem((reg2_value+of),value1,valid);

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//mov mem[reg] reg
TINT	CpuModule::movrr(TW16U reg1,TW16U reg2){
	TW32U reg1_value,reg2_value;

	//メモリアドレス値の取得
	GetReg(reg1,reg1_value);

	//格納値の取得
	GetReg(reg2,reg2_value);

	SetMem(reg1_value,reg2_value);

	return D_ERR_OK;
}
//mov reg mem
TINT	CpuModule::movrm(TW16U reg1,TW16U value){

	//レジスタに引数値を格納する
	SetReg(reg1,value);

	return D_ERR_OK;
}
//mov pc mem
TINT	CpuModule::movpm(TW16U reg1,TW16U reg2){
	TW32U reg2_value;

	//格納値の取得
	GetReg(reg2,reg2_value);

	//PCに引数値を格納する
	SetReg(reg1,reg2_value);
	
	return D_ERR_OK;
}
//mev sp mem
TINT	CpuModule::movsm(TW16U value){

	//SPに引数値を格納する
	SetReg(CpuModule_SP_ADDR,value);

	return D_ERR_OK;
}
//mov ccr mem
TINT	CpuModule::movcm(TW16U value){

	//CCRに引数値を格納する
	SetReg(CpuModule_CCR_ADDR,value);

	return D_ERR_OK;
}
//push reg
TINT	CpuModule::pushr(TW16U reg){
	TW32U sp_value,value1;

	//SPからアドレスを取得する
	GetReg(CpuModule_SP_ADDR,sp_value);

	//レジスタから値を取得する
	GetReg(reg,value1);

	//アドレス先に値を格納する
	SetMem(sp_value,value1);

	//SP値を更新する
	SetReg(CpuModule_SP_ADDR,++sp_value);
	return D_ERR_OK;
}
//pop reg
TINT	CpuModule::popr(TW16U reg){
	TW32U sp_value,value1,valid;

	//SPからアドレスを取得する
	GetReg(CpuModule_SP_ADDR,sp_value);

	//一つ前のアドレスにする
	sp_value--;

	//アドレス先の値を取得する
	GetMem(sp_value,value1,valid);

	//レジスタに値を格納する
	SetReg(reg,value1);

	//SPの値を更新する
	SetReg(CpuModule_SP_ADDR,sp_value);
	return D_ERR_OK;
}
//tst reg bit
TINT	CpuModule::tstrb(TW16U reg,TW16U value){
	TW32U reg_value,ccr_value,bit_value = 0x01;

	if(value >= CpuModule_STARTBIT && value <= CpuModule_ENDBIT){

		GetReg(reg,reg_value);

		GetReg(CpuModule_CCR_ADDR,ccr_value);

		bit_value <<= value;

		if(reg_value & bit_value){
			ccr_value = ccr_value | CpuModule_BIT1;
			SetReg(CpuModule_CCR_ADDR,ccr_value);
		} else {
			ccr_value = ccr_value & 0xfd;
			SetReg(CpuModule_CCR_ADDR,ccr_value);
		}
	}
	return D_ERR_OK;
}
//set reg bit
TINT	CpuModule::setrb(TW16U reg,TW16U value){
	TW32U reg_value,bit_value = 0x01;

	if(value >= CpuModule_STARTBIT && value <= CpuModule_ENDBIT){

		GetReg(reg,reg_value);

		bit_value <<= value;

		reg_value = reg_value | bit_value;

		SetReg(reg,reg_value);

	}
	return D_ERR_OK;
}
//rst reg bit
TINT	CpuModule::rstrb(TW16U reg,TW16U value){
	TW32U reg_value,bit_value = 0x01;

	if(value >= CpuModule_STARTBIT && value <= CpuModule_ENDBIT){

		GetReg(reg,reg_value);

		bit_value <<= value;

		reg_value = reg_value & (bit_value ^ 0xffff);

		SetReg(reg,reg_value);

	}
	return D_ERR_OK;
}
//br mem
TINT	CpuModule::brm(TW16U addr){
	//引数値をPCへ格納する
	SetReg(CpuModule_PC_ADDR,addr);
	return D_ERR_OK;
}
//br reg of
TINT	CpuModule::brro(TW16U reg,TW16U of){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//レジスタ値と値を足してPCへ格納する
	SetReg(CpuModule_PC_ADDR,reg_value+of);
	return D_ERR_OK;
}
//brz mem
TINT	CpuModule::brzm(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//CCRの1bit目が1だったら、値をPCへ格納する
	if((ccr_value & CpuModule_BIT1) == CpuModule_BIT1){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brnz mem
TINT	CpuModule::brnzm(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);
	
	//CCRの1bit目が0だったら、値をPCへ格納する
	if((ccr_value & CpuModule_BIT1) == 0){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brgt mem
TINT	CpuModule::brgt(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//CCRの5bit目が1だったら、値をPCへ格納する
	if((ccr_value & CpuModule_BIT5) == CpuModule_BIT5){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brlt mem
TINT	CpuModule::brlt(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);
	
	//CCRの5bit目が0だったら、値をPCへ格納する
	if((ccr_value & CpuModule_BIT5) == 0){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//breq mem
TINT	CpuModule::breq(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//CCRの6bit目が1だったら、値をPCへ格納する
	if((ccr_value & CpuModule_BIT6) == CpuModule_BIT6){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brneq mem
TINT	CpuModule::brneq(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);
	
	//CCRの6bit目が0だったら、値をPCへ格納する
	if((ccr_value & CpuModule_BIT6) == 0){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//call mem
TINT	CpuModule::callm(TW16U addr){

	//現在のPC内の値をスタックに退避
	pushr(CpuModule_PC_ADDR);

	//アドレスをPCに格納する
	SetReg(CpuModule_PC_ADDR,addr);

	return D_ERR_OK;
}
//call reg
TINT	CpuModule::callr(TW16U reg){
	TW32U reg_value;

	//PC内の値をスタックに退避
	pushr(CpuModule_PC_ADDR);

	//レジスタ値を取得
	GetReg(reg,reg_value);

	//アドレスをPCに格納する
	SetReg(CpuModule_PC_ADDR,reg_value);

	return D_ERR_OK;
}
//ret
TINT	CpuModule::ret(TVOID){
	popr(CpuModule_PC_ADDR);
	return D_ERR_OK;
}

// 割込み(EI/DI)
TINT	CpuModule::ei(TVOID)
{
	// Critical Section Start
	WaitForSingleObject( m_Mutex, INFINITE );

	m_InterruptEnable = true;

	// Critical Section End
	ReleaseMutex(m_Mutex);
	return D_ERR_OK;
}

TINT	CpuModule::di(TVOID)
{
	// Critical Section Start
	WaitForSingleObject( m_Mutex, INFINITE );

	m_InterruptEnable = false;

	// Critical Section End
	ReleaseMutex(m_Mutex);
	return D_ERR_OK;
}

// ソフトウェア割込み
TINT	CpuModule::swi(TVOID)
{
	TW32U	val;
	
	val = Interrupt_Software;
	Interrupt(D_MODULEID_CPU, val);
	return D_ERR_OK;
}

//reti(割込み復帰)
TINT	CpuModule::reti(TVOID){
	popr(CpuModule_PC_ADDR);
	return D_ERR_OK;
}
