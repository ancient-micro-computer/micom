#ifndef __C_MODULE_TIMER_H__
#define __C_MODULE_TIMER_H__

#include "typedef.h"
#include "simulator.h"
#include "Block.h"
#include "CpuModule.h"

namespace Simulator {
    public class TimerModule : public Block
    {
    public:
        TimerModule();
        ~TimerModule();

		TINT    GetReg(TINT addr, TW32U &value);                          // ���W�X�^get/set
        TINT    SetReg(TINT addr, TW32U value);
        TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid);// ������get/set
        TINT    SetMem(TW32U addr, TW32U value);
        TW32U*	MemPtr(TW32U addr);

        TINT    Reset(TVOID);                                   // �u���b�N���Z�b�g
        TINT    Exec(int module_id);							// �u���b�N�@�\�̎��s�֐�
        TINT    Status(TVOID);                                  // �u���b�N�X�e�[�^�X�擾
	private:

	};
};


#endif	// __C_MODULE_TIMER_H__
