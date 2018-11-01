#include <iostream>
#include <vector>
#include <thread>

#include "Bus.h"
#include "CpuModule.h"
#include "MemModule.h"
#include "TimerModule.h"
#include "BreakPoint.h"
#include "fstream"
#include "lib.h"

/* 完全な状態ではありません！ */
using namespace std;
using namespace Simulator;

// global variables
int mHealthCounter;
Bus*		m_Bus;
CpuModule*	m_Cpu;
MemModule*	m_Mem;
TimerModule*	m_GTmr;
BreakPoint* m_bp;
bool		m_Running = false;
bool		m_Halt = false;
HANDLE		m_hBusMutex;
HANDLE		m_hGTmrMutex;

// prototypes
void init();
void deinit();
int reset();
void start();
void stop();
void SetRunning(bool running);
void stopThread(std::thread *thBus, std::thread *thGTmr);
void Run1Cycle();
void RunGTimer();
int LoadFromFile(char* filename);

/*============================================================================*
 *  関数名
 *      main関数
 *  概要
 *      とにかく実行させる
 *  パラメタ説明
 *      
 *  戻り値
 *      0：正常　負数：異常
 *============================================================================*/
int main(int argc, char** argv) {
	char	filename[1024];
	
	// 初期化
	init();

	if(argc < 2) {
		printf("実行するバイナリファイル名を入力してください。 \n");
		scanf("%s", filename);
		printf("ファイル名: %s\n", filename);
	} else {
		printf("ファイル名: %s\n", argv[1]);
		strcpy_s(filename, 1024, argv[1]);
	}

	//ファイルオープン
	if (LoadFromFile(filename) != D_OK) {
		printf("読めませんでした\n");
		return -1;
	}

	// スレッド開始
	std::thread m_thBus(Run1Cycle);
	std::thread m_thGTmr(RunGTimer);

/*	{
		// 3000msecだけCPUを駆動させるデモ的挙動
		std::chrono::milliseconds interval_wait( 3000 );
		std::this_thread::sleep_for( interval_wait );
	}
	{
		// displayの代わり
		TW32U r0, r1;
		m_Bus->get_reg(D_MODULEID_CPU, 0, r0);	// r0
		printf("r0:%lx\n", r0);
		m_Bus->get_reg(D_MODULEID_CPU, 1, r1);	// r1
		printf("r1:%lx\n", r1);

		m_Bus->set_address(r1);
		m_Bus->access_read();
		printf("$%lx:%lx\n", r1, m_Bus->get_data());	// $2000番地の値
	}
*/
    // CUI
    while(true) {
        char command[64];
        cout << "start/stop/reset/exit? > ";
        cin >> command;
        if (strcmp(command, "start") == 0) {
            start();
        } else if (strcmp(command, "stop") == 0) {
            stop();
        } else if (strcmp(command, "reset") == 0) {
            reset();
        } else if (strcmp(command, "exit") == 0) {
            stop();
            break;
        }
    }

    stop();
    // スレッド強制停止
    stopThread(&m_thBus, &m_thGTmr);
    deinit();

	return 0;
}


void init() {
	printf("init\n");
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

	// 初回リセット
	m_Bus->Reset();
}


void deinit() {
	printf("deinit\n");
	delete [] m_Mem;
	delete m_Cpu;
	delete m_Bus;
	delete m_GTmr;
}


// リセット処理
int reset() {
	printf("reset\n");
	SetRunning(false);
	 m_Bus->lock();
	 m_Bus->Reset();
	 m_Bus->unlock();

	 mHealthCounter = 0;
	 return D_OK;
}

// 実行開始
void start() {
	printf("start\n");
	SetRunning(true);
}

// 実行停止
void stop() {
	printf("stop\n");
	SetRunning(false);
}

// スレッド停止
void stopThread(std::thread *thBus, std::thread *thGTmr) {
	printf("stopThread\n");
	m_Halt = true;
	
	while(true) {
		thGTmr->join();
		break;
	}

	while(true) {
		thBus->join();
		break;
	}
}

// 実行/停止設定
void SetRunning(bool running) {
	 WaitForSingleObject( m_hBusMutex, INFINITE );

	 m_Running = running;

	 ReleaseMutex(m_hBusMutex);
}

//バス駆動
void Run1Cycle() {
	 bool running;
	 bool halt;
	 TW32U	pc;

	 while(true) {
		 WaitForSingleObject( m_hBusMutex, INFINITE );
		 running = m_Running;
		 halt = m_Halt;
		 ReleaseMutex(m_hBusMutex);
		 if (halt) {
			break;
		 }
		 
		 if(!running) {
			 std::chrono::milliseconds interval_wait( 20 );
			 std::this_thread::sleep_for( interval_wait );
			 continue;
		 }

		 m_Bus->get_reg(D_MODULEID_CPU, CpuModule_PC_ADDR, pc);

		 // HALT or BreakPoint検出→Cycle実行停止
		 if(m_Bus->GetStatus(0) != 0 || m_bp->Check(pc)) {
			 SetRunning(false);
			 continue;
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
	 bool	halt;
	 TW32U	timer_enable = 0;
	 TW32U	timer_interval = 0;

	 while(true) {
		 WaitForSingleObject( m_hBusMutex, INFINITE );
		 running = m_Running;
		 halt = m_Halt;
		 ReleaseMutex(m_hBusMutex);
		 if (halt) {
			break;
		 }

		 if(!running) {
			 std::chrono::milliseconds interval_wait( 20 );
			 std::this_thread::sleep_for( interval_wait );
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

			 std::chrono::milliseconds interval_wait( timer_interval );
			 std::this_thread::sleep_for( interval_wait ); // n msec wait
			 m_Bus->Exec(D_MODULEID_GTMR);			// タイマー駆動
		 }
	 }
 }


// データロード
int LoadFromFile(char* filename) {
	char buf[1024];
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
