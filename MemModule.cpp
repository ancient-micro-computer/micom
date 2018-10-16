#include "MemModule.h"
#include <iostream>

using namespace Simulator;
using namespace std;

MemModule::MemModule(void)
{
	m_cells = new unsigned long[D_CELL_SIZE];
	memset(m_cells, 0x00, sizeof(unsigned long) * D_CELL_SIZE);
}

MemModule::~MemModule(void)
{
	delete [] m_cells;
}

TINT    MemModule::Reset(TVOID)                                   // �u���b�N���Z�b�g
{
	return 0;
}

TINT    MemModule::Exec()											// �u���b�N�@�\�̎��s�֐�
{
	return 0;
}

TINT    MemModule::Status(TVOID)                                  // �u���b�N�X�e�[�^�X�擾
{
	return 0;
}

TINT MemModule::GetReg(TINT addr, TW32U &value)							// ���W�X�^get/set
{
	return 0;
}

TINT MemModule::SetReg(TINT addr, TW32U value)
{
	return 0;
}

int MemModule::SetMem(TW32U addr, TW32U value)
{
	if (addr >= 0 && addr < D_CELL_SIZE)
	{
		m_cells[addr] = value;
	}
	return D_OK;
}

int MemModule::GetMem(TW32U addr, TW32U &value, TW32U &valid)
{
	if (addr >= 0 && addr < D_CELL_SIZE)
	{
		value = m_cells[addr];
	}
	return D_OK;
}

TW32U*	MemModule::MemPtr(TW32U addr)
{
	if (addr >= 0 && addr < D_CELL_SIZE)
	{
		return m_cells + addr;	// Null���|�C���^�ϐ����ɕύX���ĉ�����
	} else {
		return NULL;
	}
}

