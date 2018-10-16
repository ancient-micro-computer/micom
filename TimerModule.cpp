/******************************************************************************
 *  �t�@�C����
 *      Timer.cpp
 *  �u���b�N��
 *      -
 *  �T�v
 *      �ėp�^�C�}�[�N���X��`
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "macro.h"
#include "Block.h"
#include "Bus.h"
#include "TimerModule.h"

using namespace Simulator;

// �R���X�g���N�^�FBlock�ڑ����������s�����^�C�~���O�ŃR�[������܂�
TimerModule::TimerModule() : Block()
{
}

TimerModule::~TimerModule()
{
}

/*============================================================================*
 *  �֐���
 *      ���Z�b�g
 *  �T�v
 *      �u���b�N���Z�b�g
 *  �p�����^����
 *      �Ȃ�
 *  �߂�l
 *      0           : ����I���Aother : �ُ�I��
 *============================================================================*/
TINT    TimerModule::Reset(TVOID)
{
	return D_ERR_OK;
}

/*============================================================================*
 *  �֐���
 *      �u���b�N�v���p�e�B
 *  �T�v
 *      �u���b�N�Ɋւ������Ԃ�
 *  �p�����^����
 *      �Ȃ�
 *  �߂�l
 *      0           : ����I���Aother : �ُ�I��
 *============================================================================*/
TINT    TimerModule::Status(TVOID)                                  
{
	return D_ERR_OK;
}

/*============================================================================*
 *  �֐���
 *      �u���b�N�@�\���s
 *  �T�v
 *      �u���b�N�@�\�Ɋւ������
 *  �p�����^����
 *      cmd         : �R�}���hID
 *      addr        : �A�h���X
 *      data        : �f�[�^�iVerilog�`���̃R�}���h�d�l�Ɉˑ����܂��j
 *  �߂�l
 *      0           : ����I���Aother : �ُ�I��
 *============================================================================*/
TINT    TimerModule::Exec(int module_id)
{
	TW32U param;

	// CPU���W���[���Ɋ����ݒʒm
	param = Interrupt_Timer;

	m_ParentBus->lock();
	m_ParentBus->InterruptRequest(D_MODULEID_GTMR, D_MODULEID_CPU, param);
	m_ParentBus->unlock();
	return D_ERR_OK;
}

/*============================================================================*
 *  �֐���
 *      �A�N�Z�XI/F
 *  �T�v
 *      �u���b�N�����\�[�X�Ƃ�Get&Set-I/F��`
 *  �p�����^����
 *      MemID       : ������ID
 *      addr        : �A�N�Z�X�A�h���X
 *      value       : �������ݒl(Set��)
 *  �߂�l
 *      0           : ����I���Aother : �ُ�I��
 *============================================================================*/
TINT   TimerModule::GetReg(TINT addr, TW32U &value)
{
	return D_ERR_OK;
}

TINT    TimerModule::SetReg(TINT addr, TW32U value)
{
	return D_ERR_OK;
}

TINT   TimerModule::GetMem(TW32U addr, TW32U &value, TW32U &valid)
{
	return D_ERR_OK;
}

TINT    TimerModule::SetMem(TW32U addr, TW32U value)
{
	return D_ERR_OK;
}

TW32U*	TimerModule::MemPtr(TW32U addr)
{
	return NULL;	// Null���|�C���^�ϐ����ɕύX���ĉ�����
}


