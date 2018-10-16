/******************************************************************************
 *  �t�@�C����
 *      Block.h
 *  �T�v
 *      �u���b�N�̊��N���X
 ******************************************************************************/

#ifndef __C_SIM_BLOCK_H__
#define __C_SIM_BLOCK_H__

#include "typedef.h"
#include "define.h"

namespace Simulator {

class Bus;

public class Block
{
public:
	Block();
    Block(TINT id);
	TVOID SetParentBus(Bus* pBus);
    virtual ~Block();

	// �u���b�N���䃁�\�b�h
	virtual	TINT	Reset(TVOID);									// ���Z�b�g
    virtual TINT    Exec();											// �@�\�̎��s�֐�
    virtual TINT    Exec(int module_id);							// ���胂�W���[���̎��s�֐�
    virtual TINT    Status(TVOID);                                  // �X�e�[�^�X�擾

	// ���\�[�XI/F��`(�ȉ��S�ď������z�֐��ƂȂ��Ă���̂ŁA�h���N���X�ɂ����ĕK�����̂̒�`���s������)
    virtual TINT    GetReg(TINT addr, TW32U &value) = 0;							// ���W�X�^get/set
    virtual TINT	SetReg(TINT addr, TW32U value) = 0;
    virtual TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid) = 0;	// ������get/set
    virtual TINT	SetMem(TW32U addr, TW32U value) = 0;
	virtual	TW32U*	MemPtr(TW32U addr) = 0;								// �������|�C���^�擾

	virtual TINT	Interrupt(int src_module_id, TW32U &param);			// �����ݎ�t��

	inline TINT GetID() { return m_ID; }

protected:
    TINT    m_Busy;                                                 // �r�W�[�t���O
	TINT	m_ID;
	Bus*	m_ParentBus;
};

};

#endif  //__C_SIM_BLOCK_H__
