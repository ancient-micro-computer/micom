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

/* ���S�ȏ�Ԃł͂���܂���I */
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

	// �X���b�h������~
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
	// �p���t�H�[������ override ���āA�t�H�[�����������̃R�[�h���쐬����
	mHealthCounter = 0;

	m_bp = new BreakPoint;

	// �o�X�ڑ��R���t�B�M�����[�V���� -------------
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

	// �V�~�����[�^�X���b�h������-------------------
	m_Running = false;

	m_hBusMutex = CreateMutex (NULL, FALSE, NULL);
	m_hGTmrMutex = CreateMutex (NULL, FALSE, NULL);

	m_thBus = gcnew Thread(gcnew ThreadStart( this, &FormMain::Run1Cycle ));
	m_thBus->IsBackground = true;	// �o�b�N�O���E���h�����Ă���N��
	m_thBus->Start();

	m_thGTmr = gcnew Thread(gcnew ThreadStart( this, &FormMain::RunGTimer ));
	m_thGTmr->IsBackground = true;	// �o�b�N�O���E���h�����Ă���N��
	m_thGTmr->Start();

	// ���񃊃Z�b�g
	m_Bus->Reset();

	// ���j���[������
	�X�^�[�gToolStripMenuItem->Enabled = true;
	�X�g�b�vToolStripMenuItem->Enabled = false;

	// �T�u�t�H�[��������
	frmMemory = nullptr;
	frmPeripheral = nullptr;

	return D_FORMBASE_OK;
}

// ���Z�b�g����
int reset() {
	 m_Bus->lock();
	 m_Bus->Reset();
	 m_Bus->unlock();

	 mHealthCounter = 0;
	 return D_OK;
}

// ���s/��~�ݒ�
void SetRunning(bool running) {
	 WaitForSingleObject( m_hBusMutex, INFINITE );

	 m_Running = running;

	 // ���j���[ & �{�^��UI�̐���
	 cmdStop->Enabled = m_Running;
	 �X�g�b�vToolStripMenuItem->Enabled = m_Running;

	 cmdFastRun->Enabled = !m_Running;
	 �X�^�[�gToolStripMenuItem->Enabled = !m_Running;

	 cmdStep->Enabled = !m_Running;
	 �X�e�b�vToolStripMenuItem->Enabled = !m_Running;

	 ReleaseMutex(m_hBusMutex);
}

// CPU���̍X�V
System::Void RefreshCpuInfo(bool trace_current_pc) {
	 if(GetChild(D_FORMNAME_MEMORY) != nullptr) {
		 frmMemory->proc(0, (trace_current_pc ? D_FORMBASE_TRUE : D_FORMBASE_FALSE), 0, 0);
	 }
}

// ���zI/O�̍X�V
System::Void RefreshIO() {
	 if(GetChild(D_FORMNAME_PERIPHERAL) != nullptr) {
		 frmPeripheral->proc(2, 0, 0, 0);
	 }
}

//�o�X�쓮
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

		 // HALT or BreakPoint���o��Cycle���s��~
		 if(m_Bus->GetStatus(0) != 0 || m_bp->Check(pc)) {
			 BeginInvoke(gcnew SetRunningDelegate(this, &FormMain::SetRunning), false);
			 BeginInvoke(gcnew RefreshCpuInfoDelegate(this, &FormMain::RefreshCpuInfo), true);
		 }

		 // 1cycle ���s
		 if(m_Bus->GetStatus(0) == 0) {
			 m_Bus->Exec();
		 }
	 }
}

//�O���^�C�}�쓮
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
			 m_Bus->Exec(D_MODULEID_GTMR);			// �^�C�}�[�쓮
		 }
	 }
 }


// �f�[�^���[�h
int LoadFromFile(char* filename) {
	// char*��String^ �� System::Runtime::InteropServices::Marshal::PtrToStringAnsi((IntPtr)str)
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
