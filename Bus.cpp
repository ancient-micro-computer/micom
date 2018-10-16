#include "Bus.h"
#include "Block.h"
#include "macro.h"
#include <iostream>

/*============================================================================*
 * ���[�J���Q��
 *============================================================================*/
using namespace Simulator;

Bus::Bus()
{
	m_modules = 0;
	memset(m_moduleinfo, 0x0, sizeof(struct st_MODULE_INFO) * D_MAX_MODULES);

	m_address = 0x0;
	m_data    = 0x0;

	// �o�X���b�N�pMutex
	m_hBusMutex = CreateMutex(NULL, FALSE, NULL);
	m_hBusMutexSub = CreateMutex(NULL, FALSE, NULL);
	m_locking = false;
}

Bus::~Bus()
{
	CloseHandle(m_hBusMutex);
	CloseHandle(m_hBusMutexSub);
}

// �C���^�[�t�F�[�X�֐�
int Bus::set_address(unsigned long address)
{
	m_address = address;
	return D_OK;
}

int Bus::set_data(unsigned long data)
{
	m_data = data;
	return D_OK;
}

unsigned long Bus::get_address(void)
{
	return m_address;
}

unsigned long Bus::get_data(void)
{
	return m_data;
}

// �o�X���b�NMutex
int Bus::lock()			// �o�X�A�N�Z�X�O
{
	WaitForSingleObject( m_hBusMutex, INFINITE );
	return D_OK;
}

int Bus::unlock()		// �o�X�A�N�Z�X������
{
	ReleaseMutex(m_hBusMutex);
	return D_OK;
}


// ���W���[���ڑ�
int Bus::connect(int module_id, int addr_assign, unsigned long start_address, unsigned long end_address, Block *p_module)
{
	m_moduleinfo[m_modules].module_id = module_id;
	m_moduleinfo[m_modules].addr_assign = addr_assign;
	m_moduleinfo[m_modules].start_address = start_address;
	m_moduleinfo[m_modules].end_address = end_address;
	m_moduleinfo[m_modules].p_module = p_module;
	p_module->SetParentBus(this);

	m_modules++;

	return D_OK;
}

// �N���b�N�����i�e���W���[����Reset/Exec/Status�֐����Ăяo���j
int Bus::Reset()
{
	int i;

	for(i = 0; i < m_modules; i++)
	{
		m_moduleinfo[i].p_module->Reset();
	}
	return D_OK;
}

int Bus::Exec()
{
	int i;

	for(i = 0; i < m_modules; i++)
	{
		m_moduleinfo[i].p_module->Exec();
	}
	return D_OK;
}

int Bus::Status()
{
	int i;

	for(i = 0; i < m_modules; i++)
	{
		m_moduleinfo[i].p_module->Status();
	}
	return D_OK;
}

int Bus::Exec(int module_id) 
{
	int ret = -1;

	for(int i = 0; i < m_modules; i++) {
		if (m_moduleinfo[i].module_id == module_id) {
			ret = m_moduleinfo[i].p_module->Exec(module_id);
			break;
		}
	}
	return ret;
}

int Bus::GetStatus(int module_id)
{
	int ret = -1;

	for(int i = 0; i < m_modules; i++) {
		if (m_moduleinfo[i].module_id == module_id) {
			ret = m_moduleinfo[i].p_module->Status();
			break;
		}
	}
	return ret;
}

int Bus::set_reg(int module_id, TINT addr, TW32U &value)
{
	int ret = -1;

	for(int i = 0; i < m_modules; i++) {
		if (m_moduleinfo[i].module_id == module_id) {
			ret = m_moduleinfo[i].p_module->SetReg(addr, value);
			break;
		}
	}

	return ret;
}

int Bus::get_reg(int module_id, TINT addr, TW32U &value)
{
	int ret = -1;

	for(int i = 0; i < m_modules; i++) {
		if (m_moduleinfo[i].module_id == module_id) {
			ret = m_moduleinfo[i].p_module->GetReg(addr, value);
			break;
		}
	}
	return ret;
}

// Read�A�N�Z�X
int Bus::access_read(void)
{
	int i;
	int flg;
	unsigned long address;
	unsigned long data;
	unsigned long valid;

	address = get_address();
	data = 0;

	for(flg = 0, i = 0; i < m_modules; i++)
	{
		if (m_moduleinfo[i].addr_assign == 1 &&				// �A�h���X���蓖�Ă��Ȃ���Ă�
			address >= m_moduleinfo[i].start_address &&		// �J�n�A�h���X��
			address <= m_moduleinfo[i].end_address)			// �I���A�h���X�͈͓̔��ł����
		{
			m_moduleinfo[i].p_module->GetMem(address - m_moduleinfo[i].start_address, data, valid);
			set_data(data);
			flg = 1;
			break;
		}
	}

	if (flg == 1) {
		return D_OK;
	} else {
		return D_NG;
	}
}

// Write�A�N�Z�X
int Bus::access_write(void)
{
	int i;
	int flg;
	unsigned long address;

	address = get_address();

	for(flg = 0, i = 0; i < m_modules; i++)
	{
		if (m_moduleinfo[i].addr_assign == 1 &&
			address >= m_moduleinfo[i].start_address &&
			address <= m_moduleinfo[i].end_address)
		{
			m_moduleinfo[i].p_module->SetMem(address - m_moduleinfo[i].start_address, this->get_data());
			flg = 1;
			break;
		}
	}

	if (flg == 1) {
		return D_OK;
	} else {
		return D_NG;
	}
}

// �����ݒʒm
int Bus::InterruptRequest(int src_module_id, int dst_module_id, TW32U &param)
{
	int ret = D_NG;
	
	for(int i = 0; i < m_modules; i++) {
		if(m_moduleinfo[i].module_id == dst_module_id) {
			ret = m_moduleinfo[i].p_module->Interrupt(src_module_id, param);
			break;
		}
	}

	return ret;
}