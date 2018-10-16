#pragma once

#include "define.h"
#include "simulator.h"
#include "typedef.h"
#include "windows.h"
#define D_MAX_MODULES	32	// �ő�32���W���[���܂Őڑ��\�Ƃ���

namespace Simulator {
class Block;

public class Bus
{
	struct st_MODULE_INFO
	{
		int module_id;		// ���W���[��ID
		int addr_assign;	// 0:�A�h���X�����Ė��� 1:�A�h���X�����ėL��
		unsigned long start_address;
		unsigned long end_address;
		Block *p_module;
	};

public:
	Bus();
	~Bus();

	int connect(int module_id, int addr_assign, unsigned long start_address, unsigned long end_address, Block *p_module);

    int Reset(TVOID);                                   // �u���b�N���Z�b�g
    int Exec();											// �u���b�N�@�\�̎��s�֐�
	int Exec(int module_id);							// ���胂�W���[���̋N��
    int Status(TVOID);                                  // �u���b�N�X�e�[�^�X�擾

	int GetStatus(int);

	int set_reg(int id, TINT addr, TW32U &value);
	int get_reg(int id, TINT addr, TW32U &value);

	int set_address(unsigned long address);
	int set_data(unsigned long data);
	unsigned long get_address(void);
	unsigned long get_data(void);
	int access_read(void);
	int access_write(void);
	int lock();		// �o�X�A�N�Z�X�O
	int unlock();		// �o�X�A�N�Z�X������

	int InterruptRequest(int src_module_id, int dst_module_id, TW32U &value);	// �����ݒʒm

private:
	HANDLE   m_hBusMutex;
	HANDLE   m_hBusMutexSub;
	bool	 m_locking;	// true=�o�X���b�N��
	unsigned long m_address;
	unsigned long m_data;
	int  m_modules;
	struct st_MODULE_INFO m_moduleinfo[D_MAX_MODULES];
};

};