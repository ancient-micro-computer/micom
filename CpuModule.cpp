/*============================================================================*
�@�ڑ�����u���b�N�i�@�\�G���W�����W���[���j���ɁA�{���b�p�[�֐����ʂɒ�`����
�@���ƂŁA���W���[���̋@�\��`��UI�ڑ���`�𕪗����Ă��܂��B

�@�@�\���W���[�����ł����W�X�^�⃁�����̓��o�͊֐����`�����ĉ������B
 
 *============================================================================*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "typedef.h"
#include "simulator.h"
#include "macro.h"
#include "CpuModule.h"
#include "Bus.h"

using namespace Simulator;

//�ϐ��錾
T_ORDER_LIST gCodeParam[] = {
//�I�؃��[�V�����R�[�h�A���s�T�C�N���A�I�y�����h
		{0,  1, 0},
		{1,  1, 0},
		{2,  2, 2},
		{3,  2, 2},
		{4,  2, 2},
		{5,  2, 2},
		{6,  2, 2},
		{7,  2, 2},
		{8,  2, 2},
		{9,  2, 2},
		{10, 3, 2},
		{11, 3, 2},
		{12, 6, 2},
		{13, 6, 2},
		{14, 1, 1},
		{15, 1, 1},
		{16, 3, 2},
		{17, 3, 2},
		{18, 3, 1},
		{19, 1, 2},
		{20, 1, 2},
		{21, 1, 2},
		{22, 1, 2},
		{23, 2, 2},
		{24, 2, 2},
		{25, 2, 2},
		{26, 2, 2},
		{27, 2, 2},
		{28, 2, 2},
		{29, 2, 2},
		{30, 2, 2},
		{31, 2, 2},
		{32, 4, 3},
		{33, 4, 3},
		{34, 4, 2},
		{35, 2, 2},
		{36, 2, 2},
		{37, 2, 2},
		{38, 2, 2},
		{39, 2, 1},
		{40, 2, 1},
		{41, 2, 1},
		{42, 3, 2},
		{43, 2, 1},
		{44, 2, 1},
		{45, 2, 1},
		{46, 2, 1},
		{47, 2, 1},
		{48, 2, 1},
		{49, 2, 1},
		{50, 2, 1},
		{51, 2, 1},
		{52, 2, 1},
		{53, 2, 1},
		{54, 2, 1},
		{55, 2, 1},
		{56, 2, 1},
		{57, 2, 0},
		{58, 1, 0},
		{59, 1, 0},
		{60, 1, 0},
		{61, 2, 0}
};

// �����ݏ��
T_INTERRUPT_INFO gInterruptList[] = {
	{Interrupt_Timer,		CpuModule_BIT0},
	{Interrupt_Software,	CpuModule_BIT1},
	{Interrupt_Dma,			CpuModule_BIT2},
	{Interrupt_Trigger0,	CpuModule_BIT3},
	{Interrupt_Trigger1,	CpuModule_BIT4},
	{Interrupt_Trigger2,	CpuModule_BIT5},
	{Interrupt_SCIRecv,		CpuModule_BIT6},
	{Interrupt_DETrap,		0},
	{0, 0}
};

// �R���X�g���N�^�FBlock�ڑ����������s�����^�C�~���O�ŃR�[������܂�
CpuModule::CpuModule() : Block()
{
	//���W�X�^������
	memset(m_cpu_regs, 0x00, sizeof(TW16U) * 32);

	//���ߕ��i�[�̈揉����
	for(int i = 0; i < 4; i++) {
		m_value_oc[i] = 0;
	}
	//�T�C�N���l�̏�����
	m_cycleCount = 0;
	m_Busy = 0;

	// �����ݏ����֘A������
	m_InterruptTrap = 0x0;
	m_InterruptEnable = false;
	m_Mutex = CreateMutex (NULL, FALSE, NULL);

}

CpuModule::~CpuModule()
{
	CloseHandle(m_Mutex);
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
TINT    CpuModule::Reset(TVOID)
{
	TINT counter;
	
	//�K�v�ɉ����ăG���W���ŗL�̏����������֐����R�[�����ĉ�����
	//���W�X�^������
	for(counter = 0;counter <= 15;counter++){
		SetReg(CpuModule_BASE_ADDR+counter,0x0000);
	}
	//�X�^�b�N�|�C���^������
	SetReg(CpuModule_SP_ADDR,0xf000);

	//���ߕ��i�[�̈揉����
	int i;
	for(i = 0; i < 4; i++) {
		m_value_oc[i] = 0;
	}
	//�T�C�N���l�̏�����
	m_cycleCount = 0;

	m_Busy = 0;

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
TINT    CpuModule::Status(TVOID)                                  
{
    return m_Busy;							// �u���b�N�X�e�[�^�X�擾
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
TINT    CpuModule::Exec()
{
	/*  �ȉ� cmd,addr,data �̒l�ɉ������������L�q���ĉ������B
        �߂�l�� D_CMDIF_ACK ��ݒ肷��ƁA�X�N���v�g�G���W���͎��̃R�}���h��ǂݍ��݂܂�
        �߂�l�� D_CMDIF_NACK ��ݒ肷��ƁA�X�N���v�g�G���W���͓���R�}���h��^�������܂�(���̃R�}���h�ɐi�݂܂���)
        �߂�l�� D_CMDIF_VL_QUIT ��ݒ肷��ƃX�N���v�g������r���ł������I�����܂�

        �߂�l�� D_EXEC_OK   ��ݒ肷��ƁA�u���b�N�̎��s���p�����܂�
        �߂�l�� D_EXEC_HOLD ��ݒ肷��ƁA���s���I�����܂�

		�Ȃ��Ago �R�}���h���s���́Acmd,addr,data = -1, -1, -1 �Œ�ƂȂ��Ă��܂�
    */
	//TW32U theTmp = 0x0, valid = 0x01010101;
	TINT fetchCount = 0;			//�t�F�b�`�����񐔊i�[�ϐ�
	TINT status;					//Exec�������ʊi�[�ϐ�
	TW32U	reg_value = 0x0;
	TW32U	reg_pc = 0x0;
	TW32U	int_val;

	//�ϐ�������
	status = 0;

	// �����݃`�F�b�N
	if(m_cycleCount <= 0){
		// Critical Section Start
		WaitForSingleObject( m_Mutex, INFINITE );

		// �����ݗL�������݊����ݏ������łȂ����
		if(m_InterruptEnable) {
			// �����ݐݒ���擾
			GetReg(CpuModule_INT_ADDR, reg_value);

			// �����݃`�F�b�N
			for(int intnum = 0; gInterruptList[intnum].reason != 0; intnum++) {
				if( (m_InterruptTrap & gInterruptList[intnum].reason) == gInterruptList[intnum].reason) {
					if( (gInterruptList[intnum].mask > 0 && (reg_value & gInterruptList[intnum].reason) == 0) || gInterruptList[intnum].mask == 0) {
						// �����ݔ���
						reg_value |= gInterruptList[intnum].reason;
						SetReg(CpuModule_INT_ADDR, reg_value);				// �����ݗv���ݒ�
						// �����Ɋ����݂��󂯕t���邽�߂ɂ́A�����݃��[�`�����Ŋ����ݗv�����N���A���邱��

						m_InterruptTrap &= (0xFFFFFFFF - gInterruptList[intnum].reason);	// �����݃g���b�v����

						//PC���̒l���X�^�b�N�ɑޔ�
						pushr(CpuModule_PC_ADDR);

						//�����݃x�N�^�֕���
						m_ParentBus->lock();
						m_ParentBus->set_address(Interrupt_Vector_Table + intnum);
						m_ParentBus->access_read();
						reg_pc = m_ParentBus->get_data();
						m_ParentBus->unlock();
						SetReg(CpuModule_PC_ADDR, reg_pc);
					}
				}
			}
		}

		// Critical Section End
		ReleaseMutex(m_Mutex);
	}

	//�I�y�����h�R�[�h�̓ǂݍ��ݏ���
	if(m_cycleCount <= 0){
		for(int i = 0;i <= fetchCount && (fetchCount >= 0);i++){
			//�t�F�b�`����
			m_value_oc[i] = fetch();
			if(i == 0){
				//�f�R�[�h�����i�t�F�b�`�񐔐ݒ�j
				fetchCount = decode(m_value_oc[0],m_cycleCount);
				if(fetchCount == D_ERR_CMDERR) {
					int_val = Interrupt_DETrap;
					Interrupt(D_MODULEID_CPU, int_val);	// �f�[�^���s��O������
					m_cycleCount = 0;					// ���ߎ��s���~
					break;	// �f�R�[�h�G���[���o
				}
			}
			
		}
	}
	//���s�T�C�N������
	if(m_cycleCount == 1 /*&& fetchCount >= 0*/){
		//���s����
		exe(m_value_oc[0],m_value_oc[1],m_value_oc[2],m_value_oc[3]);
	}

	//�T�C�N���J�E���g�X�V
	m_cycleCount--;

	//���ʔ��f
	switch(m_value_oc[0]){
		case HOLT_OC:
			status = D_EXEC_HOLD;	// �G���W�����s�����I���̏ꍇ
			break;
		default:
			status = D_EXEC_OK;		// �p�����s���̏ꍇ
			break;
	}
	return status;
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
TINT   CpuModule::GetReg(TINT addr, TW32U &value)
{
    /* addr�Ŏ������W�X�^�A�h���X�̃��W�X�^�l�� value �ɐݒ肷�鏈�����L�q���ĉ����� */
	TINT	baddr;
	if(addr < CpuModule_BASE_ADDR){
		baddr = (addr+CpuModule_USR_ADDR) - CpuModule_BASE_ADDR;
	} else {
		baddr = addr - CpuModule_BASE_ADDR;
	}

	value = m_cpu_regs[baddr];
    return D_ERR_OK;
}

TINT    CpuModule::SetReg(TINT addr, TW32U value)
{
    /* addr�Ŏ������W�X�^�A�h���X�Ƀ��W�X�^�l value ��ݒ肷�鏈�����L�q���ĉ����� */
	TINT	baddr;
	if(addr < CpuModule_BASE_ADDR){
		baddr = (addr+CpuModule_USR_ADDR) - CpuModule_BASE_ADDR;
	} else {
		baddr = addr - CpuModule_BASE_ADDR;
	}

	m_cpu_regs[baddr] = (TW16U)value;
    return D_ERR_OK;
}

TINT   CpuModule::GetMem(TW32U addr, TW32U &value, TW32U &valid)
{
    /* MemID�Ŏ������Ώۃ�����(�o���N�ԍ�=MemBank)�́A�A�h���Xaddr����l��Read����value�ɐݒ肵�܂� */
    //���bit������
	addr = addr & 0x0000ffff;

	m_ParentBus->lock();
	m_ParentBus->set_address(addr);
	m_ParentBus->access_read();
	value = m_ParentBus->get_data();
	m_ParentBus->unlock();

    return D_ERR_OK;
}

TINT    CpuModule::SetMem(TW32U addr, TW32U value)
{
    /* MemID�Ŏ������Ώۃ�����(�o���N�ԍ�=MemBank)�́A�A�h���Xaddr�֒lvalue��Write���܂� */
	m_ParentBus->lock();
	m_ParentBus->set_address(addr);
	m_ParentBus->set_data(value);
	m_ParentBus->access_write();
	m_ParentBus->unlock();

	return D_ERR_OK;
}

TW32U*	CpuModule::MemPtr(TW32U addr)
{
	return NULL;	// Null���|�C���^�ϐ����ɕύX���ĉ�����
}

/*============================================================================*
 *  �֐���
 *      �����ݎ�t��
 *  �T�v
 *      �O���^�C�}������̊����ݎ�t��
 *  �p�����^����
 *      src_module_id : �����݌����W���[��ID(�o�X����n�����)
 *      param         : �p�����[�^
 *  �߂�l
 *      0           : ����I���Aother : �ُ�I��
 *============================================================================*/
TINT CpuModule::Interrupt(int src_module_id, TW32U &param)
{
	TW32U	reg_value;

	// Critical Section Start
	WaitForSingleObject( m_Mutex, INFINITE );

	if(m_InterruptEnable) {
		// �����ݐݒ���擾
		GetReg(CpuModule_INT_ADDR, reg_value);

		// �����ݗv���`�F�b�N
		switch(param) {
			case Interrupt_Timer:
				if((reg_value & CpuModule_BIT0) == CpuModule_BIT0) {
					m_InterruptTrap |= Interrupt_Timer;		// �^�C�}�[������
				}
				break;

			case Interrupt_Software:
				if((reg_value & CpuModule_BIT1) == CpuModule_BIT1) {
					m_InterruptTrap |= param;				// �\�t�g�E�F�A������
				}
				break;

			case Interrupt_Dma:
				if((reg_value & CpuModule_BIT2) == CpuModule_BIT2) {
					m_InterruptTrap |= Interrupt_Dma;		// DMA�]������������
				}
				break;

			case Interrupt_Trigger0:
				if((reg_value & CpuModule_BIT3) == CpuModule_BIT3) {
					m_InterruptTrap |= Interrupt_Trigger0;	// �O���g���K�[0������
				}
				break;

			case Interrupt_Trigger1:
				if((reg_value & CpuModule_BIT4) == CpuModule_BIT4) {
					m_InterruptTrap |= Interrupt_Trigger1;	// �O���g���K�[1������
				}
				break;

			case Interrupt_Trigger2:
				if((reg_value & CpuModule_BIT5) == CpuModule_BIT5) {
					m_InterruptTrap |= Interrupt_Trigger2;	// �O���g���K�[2������
				}
				break;

			case Interrupt_SCIRecv:
				if((reg_value & CpuModule_BIT6) == CpuModule_BIT6) {
					m_InterruptTrap |= Interrupt_SCIRecv;	// �V���A����M������
				}
				break;

			case Interrupt_DETrap:
				m_InterruptTrap |= param;					// �f�[�^���s��O
				break;

			default:
				break;
		}
	}

	// Critical Section End
	ReleaseMutex(m_Mutex);
	return D_EXEC_OK;		// �p�����s���̏ꍇ
}

/*============================================================================*
 *  �֐���
 *      �t�F�b�`
 *  �T�v
 *      PC���W�X�^���w���������A�h���X�̃I�y�R�[�h���擾����
 *  �p�����^����
 *
 *      
 *  �߂�l
 *      	�������Ԓn
 *============================================================================*/
TW16U	CpuModule::fetch(TVOID)
{
	TW32U value,valid,mem_value = 0;

	//PC�̒l�擾
	GetReg(CpuModule_PC_ADDR,value);

	//PC���w�������������A�h���X�l���擾
	GetMem(value, mem_value, valid);

	//PC�̒l�X�V
	SetReg(CpuModule_PC_ADDR,++value);

	return (TW16U) mem_value;
}
/*============================================================================*
 *  �֐���
 *      �f�R�[�h
 *  �T�v
 *      �I�y�l������s���郁�\�b�h�̒l��Ԃ�
 *  �p�����^����
 *			mem_value�@�@:�@�I�y���[�V�����R�[�h�l
 *			exeycle�@�@�@:�@�I�y���[�V�����T�C�N���i�[�ϐ�
 *      
 *  �߂�l
 *      int		�t�F�b�`�����񐔒l
 *============================================================================*/
TINT	CpuModule::decode(TW16U mem_value,TINT &exeycle)
{
	//�I�y���[�V�����l�m�F
	if(mem_value < OC_MIN || mem_value > OC_MAX)
	{
		return D_ERR_CMDERR;
	}

	//���s�T�C�N���l
	exeycle = gCodeParam[mem_value].cycle;

	return gCodeParam[mem_value].operand;
}
/*============================================================================*
 *  �֐���
 *      ���s
 *  �T�v
 *     �l������s���郁�\�b�h��I�������s���\�b�h���Ăяo��
 *  �p�����^����
 *			value�@�@:	�I�y���[�V�����R�[�h�@
 *          addr1    :�@�I�y�����h�P
 *          addr2    :�@�I�y�����h�Q
 *          addr3    :�@�I�y�����h�R
 *      
 *  �߂�l
 *      int		���s���\�b�h�C���f�b�N�X
 *============================================================================*/
TINT	CpuModule::exe(TINT value,TW16U addr1,TW16U addr2,TW16U addr3)
{
	switch(value){
		case NOP_OC:
			nop();
			break;
		case HOLT_OC:
			halt();
			break;
		case ADD_R_M_OC:
			addrm(addr1,addr2);
			break;
		case ADD_R_R_OC:
			addrr(addr1,addr2);
			break;
		case SUB_R_M_OC:
			subrm(addr1,addr2);
			break;
		case SUB_R_R_OC:
			subrr(addr1,addr2);
			break;			
		case INC_R_OC:
			incr(addr1);
			break;
		case DEC_R_OC:
			decr(addr1);
			break;
		case CMP_R_M_OC:
			cmprm(addr1,addr2);
			break;
		case CMP_R_R_OC:
			cmprr(addr1,addr2);
			break;
		case CMP_R_OC:
			cmpr(addr1);
			break;
		case SLA_R_M_OC:
			slarm(addr1,addr2);
			break;
		case SRA_R_M_OC:
			slarr(addr1,addr2);
			break;
		case AND_R_M_OC:
			andrm(addr1,addr2);
			break;
		case AND_R_R_OC:
			andrr(addr1,addr2);
			break;
		case OR_R_M_OC:
			orrm(addr1,addr2);
			break;
		case OR_R_R_OC:
			orrr(addr1,addr2);
			break;
		case XOR_R_M_OC:
			xorrm(addr1,addr2);
			break;
		case XOR_R_R_OC:
			xorrr(addr1,addr2);
			break;

		//Bit���Z
		case TST_R_B_OC:
			tstrb(addr1, addr2);
			break;

		case SET_R_B_OC:
			setrb(addr1, addr2);
			break;

		case RST_R_B_OC:
			rstrb(addr1, addr2);
			break;

		case MOV_R_M_O_OC:
			movrmo(addr1,addr2,addr3);
			break;
		case MOV_R_R_O_OC:
			movrro(addr1,addr2,addr3);
			break;
		case MOV_R_R_OC:
			movrr(addr1,addr2);
			break;
		case MOV_R_M_OC:
			movrm(addr1,addr2);
			break;
		case MOV_P_M_OC:
			movpm(addr1,addr2);
			break;
		case MOV_S_M_OC:
			movsm(addr2);
			break;
		case MOV_C_M_OC:
			movcm(addr2);
			break;
		case PUSH_R_OC:
			pushr(addr1);
			break;
		case POP_R_OC:
			popr(addr1);
			break;
		case BR_M_OC:
			brm(addr1);
			break;
		case BR_R_O_OC:
			brro(addr1,addr2);
			break;
		case BRZ_M_OC:
			brzm(addr1);
			break;
		case BRNZ_M_OC:
			brnzm(addr1);
			break;
		case CALL_M_O_OC:
			callm(addr1);
			break;
		case CALL_R_OC:
			callr(addr1);
			break;
		case RET_OC:
			ret();
			break;
		case BRGT_M_OC:
			brgt(addr1);
			break;
		case BRLT_M_OC:
			brlt(addr1);
			break;
		case BREQ_M_OC:
			breq(addr1);
			break;
		case BRNEQ_M_OC:
			brneq(addr1);
			break;

		//���荞��
		case EI_OC:
			ei();
			break;

		case DI_OC:
			di();
			break;

		case SWI_OC:
			swi();
			break;

		case RETI_OC:
			reti();
			break;

		/*���s���\�b�h�𐏎��ǉ����Ă���*/
		default :
			halt();		// Unknown mnemonic. Force halt cpu.
			break;
	}
	return D_ERR_OK;
}

/*============================================================================*
 *  �֐���
 *      �@�@���s���\�b�h�Q
 *  �T�v
 *     �I�y�R�[�h�����ۂɎ�������ۂ̏������\�b�h�Q
 *  �p�����^����
 *      
 *  �߂�l
 *      
 *============================================================================*/
//nop
TINT	CpuModule::nop(TVOID){
	return D_ERR_OK;
}
//halt
TINT	CpuModule::halt(TVOID){
	m_Busy = D_EXEC_HOLD;
	return D_ERR_OK;
}
//add reg mem
TINT	CpuModule::addrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);
	
	//reg+value�����W�X�^�Ɋi�[����
	SetReg(reg,reg_value+value);

	return D_ERR_OK;
}
//add reg reg
TINT	CpuModule::addrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	//���W�X�^A����lA�����o��
	GetReg(reg1,value1);
	
	//���W�X�^A����lB�����o��
	GetReg(reg2,value2);
	
	//A+B�����W�X�^�Ɋi�[����
	SetReg(reg1,value1+value2);

	return D_ERR_OK;
}
//sub reg mem
TINT	CpuModule::subrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//reg-value�����W�X�^�Ɋi�[����
	SetReg(reg,reg_value-value);

	return D_ERR_OK;
}
//sub reg reg
TINT	CpuModule::subrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	//�lA�����o��
	GetReg(reg1,value1);

	//�lB�����o��
	GetReg(reg2,value2);

	//A-B�����W�X�^�Ɋi�[����
	SetReg(reg1,value1-value2);
	return D_ERR_OK;
}
//inc reg
TINT	CpuModule::incr(TW16U reg1){
	TW32U value1;

	GetReg(reg1,value1);

	//++reg_value �����W�X�^�Ɋi�[����
	SetReg(reg1,++value1);
	return D_ERR_OK;
}
//dec reg
TINT	CpuModule::decr(TW16U reg1){
	TW32U value1;

	GetReg(reg1,value1);

	//--reg_value �����W�X�^�Ɋi�[����
	SetReg(reg1,--value1);
	return D_ERR_OK;
}
//cmp reg mem
TINT	CpuModule::cmprm(TW16U reg,TW16U value){
	TW32U reg_value,ccr_value;

	//���W�X�^�̒l���擾����
	GetReg(reg,reg_value);

	//ccr���W�X�^�̒l���擾����
	GetReg(CpuModule_CCR_ADDR,ccr_value);


	if(reg_value > value){				//�����傫��
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT5;
	}	
	if(reg_value == value){				//������
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT6;
	}
	if(reg_value < value){				//����������
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
	}

	SetReg(CpuModule_CCR_ADDR,ccr_value);
	return D_ERR_OK;
}
//cmp reg reg
TINT	CpuModule::cmprr(TW16U reg1,TW16U reg2){
	TW32U value1,value2,ccr_value;

	//���W�X�^�̒lA���擾����
	GetReg(reg1,value1);

	//���W�X�^�̒lB���擾����
	GetReg(reg2,value2);

	//ccr���W�X�^�̒l���擾����
	GetReg(CpuModule_CCR_ADDR,ccr_value);

	if(value1 > value2){				//�����傫��
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT5;
	} else if(value1 == value2){		//������
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
		ccr_value = ccr_value | CpuModule_BIT6;
	} else {							//����������
		ccr_value = ccr_value & (CpuModule_BIT6 ^ 0xffff);
		ccr_value = ccr_value & (CpuModule_BIT5 ^ 0xffff);
	}

	SetReg(CpuModule_CCR_ADDR,ccr_value);
	return D_ERR_OK;
}
//cmp reg
TINT	CpuModule::cmpr(TW16U reg){
	TW32U value1,ccr_value;

	//���W�X�^�̒lA���擾����
	GetReg(reg,value1);

	//ccr���W�X�^�̒l���擾����
	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//�l�͂O���@y = 1	n = 0
	if(value1 == 0){
		ccr_value = ccr_value | CpuModule_BIT1;
	} else{
		ccr_value = ccr_value & (CpuModule_BIT1 ^ 0xffff);
	}
	//�l�̓}�C�i�X���@y = 1		n = 0
	if(value1 < 0){
		ccr_value = ccr_value | CpuModule_BIT2;
	} else {
		ccr_value = ccr_value & (CpuModule_BIT2 ^ 0xffff);
	}
	//�l�͊�@or ����	  �� = 1�@�� = 0
	if(value1%2){
		ccr_value = ccr_value | CpuModule_BIT3;
	} else {
		ccr_value = ccr_value & (CpuModule_BIT3 ^ 0xffff);
	}
	SetReg(CpuModule_CCR_ADDR,ccr_value);


	return D_ERR_OK;
}
//sla reg mem
TINT	CpuModule::slarm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//�l�����������V�t�g����
	reg_value <<= value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//sla reg reg
TINT	CpuModule::slarr(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//�l�������l�E�V�t�g����
	reg_value >>= value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//and reg mem
TINT	CpuModule::andrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//�l�ƈ����l�̐ς��Ƃ�
	reg_value = reg_value & value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//and reg reg
TINT	CpuModule::andrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	GetReg(reg1,value1);

	GetReg(reg2,value2);

	//reg1��reg2�̐ς��Ƃ�
	value1 = value1 & value2;

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//or reg mem
TINT	CpuModule::orrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//reg�ƈ����l�̘a���Ƃ�
	reg_value = reg_value | value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//or reg reg
TINT	CpuModule::orrr(TW16U reg1,TW16U reg2)	{
	TW32U value1,value2;

	GetReg(reg1,value1);

	GetReg(reg2,value2);

	//reg1��reg2�̘a���Ƃ�
	value1 = value1 | value2;

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//xor reg mem
TINT	CpuModule::xorrm(TW16U reg,TW16U value){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//reg�ƈ����l�̔r�����Ƃ�
	reg_value = reg_value ^ value;

	SetReg(reg,reg_value);

	return D_ERR_OK;
}
//xor reg reg
TINT	CpuModule::xorrr(TW16U reg1,TW16U reg2){
	TW32U value1,value2;

	GetReg(reg1,value1);

	GetReg(reg2,value2);

	//reg1��reg2�̔r�����Ƃ�
	value1 = value1 ^ value2;

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//mov reg mem offset
TINT	CpuModule::movrmo(TW16U reg1,TW16U value,TW16U of){
	TW32U value1,valid;

	//�����l�̃������l���擾����
	GetMem((value+of),value1,valid);

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//mov reg reg offset
TINT	CpuModule::movrro(TW16U reg1,TW16U reg2,TW16U of){
	TW32U reg2_value,value1,valid;

	GetReg(reg2,reg2_value);

	//���W�X�^�l�ƈ����l�𑫂����������l���擾����
	GetMem((reg2_value+of),value1,valid);

	SetReg(reg1,value1);

	return D_ERR_OK;
}
//mov mem[reg] reg
TINT	CpuModule::movrr(TW16U reg1,TW16U reg2){
	TW32U reg1_value,reg2_value;

	//�������A�h���X�l�̎擾
	GetReg(reg1,reg1_value);

	//�i�[�l�̎擾
	GetReg(reg2,reg2_value);

	SetMem(reg1_value,reg2_value);

	return D_ERR_OK;
}
//mov reg mem
TINT	CpuModule::movrm(TW16U reg1,TW16U value){

	//���W�X�^�Ɉ����l���i�[����
	SetReg(reg1,value);

	return D_ERR_OK;
}
//mov pc mem
TINT	CpuModule::movpm(TW16U reg1,TW16U reg2){
	TW32U reg2_value;

	//�i�[�l�̎擾
	GetReg(reg2,reg2_value);

	//PC�Ɉ����l���i�[����
	SetReg(reg1,reg2_value);
	
	return D_ERR_OK;
}
//mev sp mem
TINT	CpuModule::movsm(TW16U value){

	//SP�Ɉ����l���i�[����
	SetReg(CpuModule_SP_ADDR,value);

	return D_ERR_OK;
}
//mov ccr mem
TINT	CpuModule::movcm(TW16U value){

	//CCR�Ɉ����l���i�[����
	SetReg(CpuModule_CCR_ADDR,value);

	return D_ERR_OK;
}
//push reg
TINT	CpuModule::pushr(TW16U reg){
	TW32U sp_value,value1;

	//SP����A�h���X���擾����
	GetReg(CpuModule_SP_ADDR,sp_value);

	//���W�X�^����l���擾����
	GetReg(reg,value1);

	//�A�h���X��ɒl���i�[����
	SetMem(sp_value,value1);

	//SP�l���X�V����
	SetReg(CpuModule_SP_ADDR,++sp_value);
	return D_ERR_OK;
}
//pop reg
TINT	CpuModule::popr(TW16U reg){
	TW32U sp_value,value1,valid;

	//SP����A�h���X���擾����
	GetReg(CpuModule_SP_ADDR,sp_value);

	//��O�̃A�h���X�ɂ���
	sp_value--;

	//�A�h���X��̒l���擾����
	GetMem(sp_value,value1,valid);

	//���W�X�^�ɒl���i�[����
	SetReg(reg,value1);

	//SP�̒l���X�V����
	SetReg(CpuModule_SP_ADDR,sp_value);
	return D_ERR_OK;
}
//tst reg bit
TINT	CpuModule::tstrb(TW16U reg,TW16U value){
	TW32U reg_value,ccr_value,bit_value = 0x01;

	if(value >= CpuModule_STARTBIT && value <= CpuModule_ENDBIT){

		GetReg(reg,reg_value);

		GetReg(CpuModule_CCR_ADDR,ccr_value);

		bit_value <<= value;

		if(reg_value & bit_value){
			ccr_value = ccr_value | CpuModule_BIT1;
			SetReg(CpuModule_CCR_ADDR,ccr_value);
		} else {
			ccr_value = ccr_value & 0xfd;
			SetReg(CpuModule_CCR_ADDR,ccr_value);
		}
	}
	return D_ERR_OK;
}
//set reg bit
TINT	CpuModule::setrb(TW16U reg,TW16U value){
	TW32U reg_value,bit_value = 0x01;

	if(value >= CpuModule_STARTBIT && value <= CpuModule_ENDBIT){

		GetReg(reg,reg_value);

		bit_value <<= value;

		reg_value = reg_value | bit_value;

		SetReg(reg,reg_value);

	}
	return D_ERR_OK;
}
//rst reg bit
TINT	CpuModule::rstrb(TW16U reg,TW16U value){
	TW32U reg_value,bit_value = 0x01;

	if(value >= CpuModule_STARTBIT && value <= CpuModule_ENDBIT){

		GetReg(reg,reg_value);

		bit_value <<= value;

		reg_value = reg_value & (bit_value ^ 0xffff);

		SetReg(reg,reg_value);

	}
	return D_ERR_OK;
}
//br mem
TINT	CpuModule::brm(TW16U addr){
	//�����l��PC�֊i�[����
	SetReg(CpuModule_PC_ADDR,addr);
	return D_ERR_OK;
}
//br reg of
TINT	CpuModule::brro(TW16U reg,TW16U of){
	TW32U reg_value;

	GetReg(reg,reg_value);

	//���W�X�^�l�ƒl�𑫂���PC�֊i�[����
	SetReg(CpuModule_PC_ADDR,reg_value+of);
	return D_ERR_OK;
}
//brz mem
TINT	CpuModule::brzm(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//CCR��1bit�ڂ�1��������A�l��PC�֊i�[����
	if((ccr_value & CpuModule_BIT1) == CpuModule_BIT1){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brnz mem
TINT	CpuModule::brnzm(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);
	
	//CCR��1bit�ڂ�0��������A�l��PC�֊i�[����
	if((ccr_value & CpuModule_BIT1) == 0){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brgt mem
TINT	CpuModule::brgt(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//CCR��5bit�ڂ�1��������A�l��PC�֊i�[����
	if((ccr_value & CpuModule_BIT5) == CpuModule_BIT5){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brlt mem
TINT	CpuModule::brlt(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);
	
	//CCR��5bit�ڂ�0��������A�l��PC�֊i�[����
	if((ccr_value & CpuModule_BIT5) == 0){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//breq mem
TINT	CpuModule::breq(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);

	//CCR��6bit�ڂ�1��������A�l��PC�֊i�[����
	if((ccr_value & CpuModule_BIT6) == CpuModule_BIT6){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//brneq mem
TINT	CpuModule::brneq(TW16U addr){
	TW32U ccr_value;

	GetReg(CpuModule_CCR_ADDR,ccr_value);
	
	//CCR��6bit�ڂ�0��������A�l��PC�֊i�[����
	if((ccr_value & CpuModule_BIT6) == 0){
		SetReg(CpuModule_PC_ADDR,addr);
	}

	return D_ERR_OK;
}
//call mem
TINT	CpuModule::callm(TW16U addr){

	//���݂�PC���̒l���X�^�b�N�ɑޔ�
	pushr(CpuModule_PC_ADDR);

	//�A�h���X��PC�Ɋi�[����
	SetReg(CpuModule_PC_ADDR,addr);

	return D_ERR_OK;
}
//call reg
TINT	CpuModule::callr(TW16U reg){
	TW32U reg_value;

	//PC���̒l���X�^�b�N�ɑޔ�
	pushr(CpuModule_PC_ADDR);

	//���W�X�^�l���擾
	GetReg(reg,reg_value);

	//�A�h���X��PC�Ɋi�[����
	SetReg(CpuModule_PC_ADDR,reg_value);

	return D_ERR_OK;
}
//ret
TINT	CpuModule::ret(TVOID){
	popr(CpuModule_PC_ADDR);
	return D_ERR_OK;
}

// ������(EI/DI)
TINT	CpuModule::ei(TVOID)
{
	// Critical Section Start
	WaitForSingleObject( m_Mutex, INFINITE );

	m_InterruptEnable = true;

	// Critical Section End
	ReleaseMutex(m_Mutex);
	return D_ERR_OK;
}

TINT	CpuModule::di(TVOID)
{
	// Critical Section Start
	WaitForSingleObject( m_Mutex, INFINITE );

	m_InterruptEnable = false;

	// Critical Section End
	ReleaseMutex(m_Mutex);
	return D_ERR_OK;
}

// �\�t�g�E�F�A������
TINT	CpuModule::swi(TVOID)
{
	TW32U	val;
	
	val = Interrupt_Software;
	Interrupt(D_MODULEID_CPU, val);
	return D_ERR_OK;
}

//reti(�����ݕ��A)
TINT	CpuModule::reti(TVOID){
	popr(CpuModule_PC_ADDR);
	return D_ERR_OK;
}
