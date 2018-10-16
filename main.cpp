#include <iostream>
#include <vcclr.h>
#include <vector>

#include "Bus.h"
#include "CpuModule.h"
#include "MemModule.h"
#include "TimerModule.h"
#include "BreakPoint.h"
#include "fstream"
#include "lib.h"

/* 完全な状態ではありません！ */
int mHealthCounter;
Bus*		m_Bus;
CpuModule*	m_Cpu;
MemModule*	m_Mem;
TimerModule*	m_GTmr;
BreakPoint* m_bp;
bool		m_Running;
Thread^		m_thBus;
Thread^		m_thGTmr;
HANDLE		m_hBusMutex;
HANDLE		m_hGTmrMutex;

void main(void)
{
	init();
}

void deinit() {
	if (components)
	{
		delete components;
	}
	delete [] m_Mem;
	delete m_Cpu;
	delete m_Bus;
	delete m_GTmr;

	// スレッド強制停止
	m_thGTmr->Abort();
	while(true) {
		m_thGTmr->Join();
		break;
	}

	m_thBus->Abort();
	while(true) {
		m_thBus->Join();
		break;
	}
}


void init() {
	// 継承フォーム側で override して、フォーム初期化時のコードを作成する
	mHealthCounter = 0;

	m_bp = new BreakPoint;

	// バス接続コンフィギュレーション -------------
	m_Bus = new Bus;
	m_Cpu = new CpuModule;
	m_GTmr = new TimerModule;
	m_Mem = new MemModule [8];

	m_Bus->connect(D_MODULEID_CPU,  0,    0,    0, m_Cpu);
	m_Bus->connect(D_MODULEID_GTMR, 0,    0,    0, m_GTmr);

	unsigned long addr = 0;
	for(int i = 0; i < 8; i++) {
		m_Bus->connect(D_MODULEID_MEM + i, 1, addr, addr + 8192 - 1, &m_Mem[i]);
		addr += 8192;
	}

	// シミュレータスレッド初期化-------------------
	m_Running = false;

	m_hBusMutex = CreateMutex (NULL, FALSE, NULL);
	m_hGTmrMutex = CreateMutex (NULL, FALSE, NULL);

	m_thBus = gcnew Thread(gcnew ThreadStart( this, &FormMain::Run1Cycle ));
	m_thBus->IsBackground = true;	// バックグラウンド化してから起動
	m_thBus->Start();

	m_thGTmr = gcnew Thread(gcnew ThreadStart( this, &FormMain::RunGTimer ));
	m_thGTmr->IsBackground = true;	// バックグラウンド化してから起動
	m_thGTmr->Start();

	// 初回リセット
	m_Bus->Reset();

	// メニュー初期化
	スタートToolStripMenuItem->Enabled = true;
	ストップToolStripMenuItem->Enabled = false;

	// サブフォーム初期化
	frmMemory = nullptr;
	frmPeripheral = nullptr;

	return D_FORMBASE_OK;
}

// リセット処理
int reset() {
	 m_Bus->lock();
	 m_Bus->Reset();
	 m_Bus->unlock();

	 mHealthCounter = 0;
	 return D_OK;
}

// 実行/停止設定
void SetRunning(bool running) {
	 WaitForSingleObject( m_hBusMutex, INFINITE );

	 m_Running = running;

	 // メニュー & ボタンUIの制御
	 cmdStop->Enabled = m_Running;
	 ストップToolStripMenuItem->Enabled = m_Running;

	 cmdFastRun->Enabled = !m_Running;
	 スタートToolStripMenuItem->Enabled = !m_Running;

	 cmdStep->Enabled = !m_Running;
	 ステップToolStripMenuItem->Enabled = !m_Running;

	 ReleaseMutex(m_hBusMutex);
}

// CPU情報の更新
System::Void RefreshCpuInfo(bool trace_current_pc) {
	 if(GetChild(D_FORMNAME_MEMORY) != nullptr) {
		 frmMemory->proc(0, (trace_current_pc ? D_FORMBASE_TRUE : D_FORMBASE_FALSE), 0, 0);
	 }
}

// 仮想I/Oの更新
System::Void RefreshIO() {
	 if(GetChild(D_FORMNAME_PERIPHERAL) != nullptr) {
		 frmPeripheral->proc(2, 0, 0, 0);
	 }
}

//バス駆動
void Run1Cycle() {
	 bool running;
	 TW32U	pc;

	 while(true) {
		 WaitForSingleObject( m_hBusMutex, INFINITE );
		 running = m_Running;
		 ReleaseMutex(m_hBusMutex);
		 if(!running) {
			 m_thBus->Sleep(20);
			 continue;
		 }

		 m_Bus->get_reg(D_MODULEID_CPU, CpuModule_PC_ADDR, pc);

		 // HALT or BreakPoint検出→Cycle実行停止
		 if(m_Bus->GetStatus(0) != 0 || m_bp->Check(pc)) {
			 BeginInvoke(gcnew SetRunningDelegate(this, &FormMain::SetRunning), false);
			 BeginInvoke(gcnew RefreshCpuInfoDelegate(this, &FormMain::RefreshCpuInfo), true);
		 }

		 // 1cycle 実行
		 if(m_Bus->GetStatus(0) == 0) {
			 m_Bus->Exec();
		 }
	 }
}

//外部タイマ駆動
void RunGTimer() {
	 bool	running;
	 TW32U	timer_enable = 0;
	 TW32U	timer_interval = 0;

	 while(true) {
		 WaitForSingleObject( m_hBusMutex, INFINITE );
		 running = m_Running;
		 ReleaseMutex(m_hBusMutex);
		 if(!running) {
			 m_thGTmr->Sleep(20);
			 continue;
		 }
		 
		 m_Bus->lock();
		 m_Bus->set_address(D_SYSTEM_GTMR_ENABLE);
		 m_Bus->access_read();
		 timer_enable = m_Bus->get_data();
		 m_Bus->unlock();

		 if(timer_enable == 0x0001) {
			 m_Bus->lock();
			 m_Bus->set_address(D_SYSTEM_GTMR_INTERVAL);
			 m_Bus->access_read();
			 timer_interval = m_Bus->get_data();
			 m_Bus->unlock();

			 m_thGTmr->Sleep(timer_interval);		// n msec wait
			 m_Bus->Exec(D_MODULEID_GTMR);			// タイマー駆動
		 }
	 }
 }


// データロード
int LoadFromFile(char* filename) {
	// char*→String^ ＝ System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)str)
	// 
	char buf[1024];
	char *nexttok;
	char *p;
	TW32U baseaddr = 0, value = 0;
	int offset = 0;

	ifstream fin(filename);

	if (!fin) {
		return D_NG;
	}

	while(!fin.eof()) {
		memset(buf, 0x0, 1024);
		fin.getline(buf, 1024);

		offset = 0;
		p = strtok_s(buf, " ", &nexttok);
		while(p != NULL) {
			if(offset == 0) {
				baseaddr = htoi(p);
				p = strtok_s(NULL, " ", &nexttok);
			}

			if(p == NULL) break;

			value = htoi(p);

			m_Bus->set_address(baseaddr + offset);
			m_Bus->set_data(value);
			m_Bus->access_write();

			offset++;
			p = strtok_s(NULL, " ", &nexttok);
		}
	}

	fin.close();
	return D_OK;
}
