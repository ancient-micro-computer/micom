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

    TINT    Reset(TVOID);                                   // �u���b�N���Z�b�g
    TINT    Exec();											// �u���b�N�@�\�̎��s�֐�
    TINT    Status(TVOID);                                  // �u���b�N�X�e�[�^�X�擾

	TINT    GetReg(TINT addr, TW32U &value);						// ���W�X�^get/set
    TINT	SetReg(TINT addr, TW32U value);
    TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid);// ������get/set
    TINT    SetMem(TW32U addr, TW32U value);
	TW32U*	MemPtr(TW32U addr);

private:
	TW32U *m_cells;
};

};
