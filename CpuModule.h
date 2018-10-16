#ifndef __C_MODULE_CpuModule_H__
#define __C_MODULE_CpuModule_H__

#include "typedef.h"
#include "Block.h"
#include <windows.h>
#include <process.h>

//CpuModule���W�X�^�J�n�A�h���X
#define CpuModule_BASE_ADDR		0x80
#define CpuModule_PC_ADDR			0x80
#define CpuModule_SP_ADDR			0x81
#define CpuModule_CCR_ADDR			0x82
#define CpuModule_INT_ADDR			0x83
#define CpuModule_USR_ADDR			0x84

//�P��bit�T�C�Y
#define CpuModule_STARTBIT			0
#define CpuModule_ENDBIT			15

//Bit����p
#define CpuModule_BIT0		0x0001
#define CpuModule_BIT1		0x0002
#define CpuModule_BIT2		0x0004
#define CpuModule_BIT3		0x0008
#define CpuModule_BIT4		0x0010
#define CpuModule_BIT5		0x0020
#define CpuModule_BIT6		0x0040
#define CpuModule_BIT7		0x0080
#define CpuModule_BIT8		0x0100
#define CpuModule_BIT9		0x0200
#define CpuModule_BIT10		0x0400
#define CpuModule_BIT11		0x0800
#define CpuModule_BIT12		0x1000
#define CpuModule_BIT13		0x2000
#define CpuModule_BIT14		0x4000
#define CpuModule_BIT15		0x8000

// �����ݗv��
#define Interrupt_Timer		CpuModule_BIT8
#define Interrupt_Software	CpuModule_BIT9
#define Interrupt_Dma		CpuModule_BIT10
#define Interrupt_Trigger0	CpuModule_BIT11
#define Interrupt_Trigger1	CpuModule_BIT12
#define Interrupt_Trigger2	CpuModule_BIT13
#define Interrupt_SCIRecv	CpuModule_BIT14
#define Interrupt_DETrap	CpuModule_BIT15

// �����݃x�N�^�e�[�u���J�n�A�h���X
#define Interrupt_Vector_Table	0x0010

//�I�y�R�[�h
#define OC_MIN			0
#define OC_MAX			61

//���̑�
#define NOP_OC			0
#define HOLT_OC			1
//���Z����
#define ADD_R_M_OC		2
#define ADD_R_R_OC		3
#define ADC_R_M_OC		4
#define ADC_R_R_OC		5
#define SUB_R_M_OC		6
#define SUB_R_R_OC		7
#define SUBC_R_M_OC		8
#define SUBC_R_R_OC		9
#define MUL_R_M_OC		10
#define MUL_R_R_OC		11
#define DIV_R_M_OC		12
#define DIV_R_R_OC		13
#define INC_R_OC		14
#define DEC_R_OC		15
#define CMP_R_M_OC		16
#define CMP_R_R_OC		17
#define CMP_R_OC		18
#define SLA_R_M_OC		19
#define SRA_R_M_OC		20
#define SL_R_M_OC		21
#define SR_R_M_OC		22
//�_�����Z
#define AND_R_M_OC		23
#define AND_R_R_OC		24
#define OR_R_M_OC		25
#define OR_R_R_OC		26
#define XOR_R_M_OC		27
#define XOR_R_R_OC		28
//Bit���Z
#define TST_R_B_OC		29
#define SET_R_B_OC		30
#define RST_R_B_OC		31
//����������
#define MOV_R_M_O_OC	32
#define MOV_R_R_O_OC	33
#define MOV_R_R_OC		34
#define MOV_R_M_OC		35
#define MOV_P_M_OC		36
#define MOV_S_M_OC		37
#define MOV_C_M_OC		38
//�X�^�b�N����
#define PUSH_R_OC		39
#define POP_R_OC		40
//����
#define BR_M_OC			41
#define BR_R_O_OC		42
#define BRC_M_OC		43
#define BRNC_M_OC		44
#define BRZ_M_OC		45
#define BRNZ_M_OC		46
#define BRP_M_OC		47
#define BRM_M_OC		48
#define BRPO_M_OC		49
#define BRPE_M_OC		50
#define BRGT_M_OC		51
#define BRLT_M_OC		52
#define BREQ_M_OC		53
#define BRNEQ_M_OC		54
//�T�u���[�`��
#define CALL_M_O_OC		55
#define CALL_R_OC		56
#define RET_OC			57
//���荞��
#define EI_OC			58
#define DI_OC			59
#define SWI_OC			60
#define RETI_OC			61

//�\���̒�`
//�I�y���[�V�����R�[�h�^
typedef struct _ST_ORDER_LIST{
	TINT opcode;							//�I�y�R�[�h
	TINT cycle;								//���s�T�C�N���l
	TINT operand;							//�I�y�����h��
} T_ORDER_LIST;

typedef struct _ST_INTERRUPT_INFO{
	int reason;			// �v��Bit(bit8�`bit15���w��)
	int mask;			// 0:NMI(Non Maskable Interrupt)
} T_INTERRUPT_INFO;

namespace Simulator {
    public class CpuModule : public Block
    {
    public:
        CpuModule();
        ~CpuModule();

		TINT    GetReg(TINT addr, TW32U &value);                          // ���W�X�^get/set
        TINT    SetReg(TINT addr, TW32U value);
        TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid);// ������get/set
        TINT    SetMem(TW32U addr, TW32U value);
        TW32U*	MemPtr(TW32U addr);

        TINT    Reset(TVOID);                                   // �u���b�N���Z�b�g
        TINT    Exec();											// �u���b�N�@�\�̎��s�֐�
        TINT    Status(TVOID);                                  // �u���b�N�X�e�[�^�X�擾
		TINT	Interrupt(int src_module_id, TW32U &param);		// �����ݎ�t��

	private:
        /* �ȉ��K�v�ɉ����ă����o�ϐ��̒ǉ����s���ĉ����� */
        TVOID	dumpRegisters(TVOID);
        //FILE*   m_verilog;						/* �o��Verilog�t�@�C�� */
        //TW8		m_reg_filename[2][256];			/* ���W�X�^�l�o�̓t�@�C���� */

		//�����o�ϐ��̐錾
		TW16U	m_cpu_regs[32];						//CpuModule���W�X�^�l�i�[�p
		TW16U	m_value_oc[4];						//���ߌ�i�[�ϐ�
		TINT	m_cycleCount;						//�I�y���[�V�������̃T�C�N���l�i�[�ϐ�
		T_ORDER_LIST m_odlist[OC_MAX+1];

		//�����o�֐��̐錾
		TW16U	fetch(TVOID);								//�t�F�b�`���\�b�h
		TINT	decode(TW16U mem_value,TINT &exeycle);	//�f�R�[�h���\�b�h
		TINT	exe(TINT value,TW16U addr1,TW16U addr2,TW16U addr3);						//���s���\�b�h

		//�I�y�R�[�h���\�b�h
		TINT	oc_init(TINT code,TINT cycle,TINT operand);
		//���̑�
		TINT	nop(TVOID);
		TINT	halt(TVOID);
		//���Z����
		TINT	addrm(TW16U reg,TW16U value);
		TINT	addrr(TW16U reg1,TW16U reg2);
		TINT	subrm(TW16U reg,TW16U value);
		TINT	subrr(TW16U reg1,TW16U reg2);
		TINT	incr(TW16U reg1);
		TINT	decr(TW16U reg1);
		TINT	cmprm(TW16U reg,TW16U value);
		TINT	cmprr(TW16U reg1,TW16U reg2);
		TINT	cmpr(TW16U reg);
		TINT	slarm(TW16U reg,TW16U value);
		TINT	slarr(TW16U reg1,TW16U reg2);
		//�_�����Z
		TINT	andrm(TW16U reg,TW16U value);
		TINT	andrr(TW16U reg1,TW16U reg2);
		TINT	orrm(TW16U reg,TW16U value);
		TINT	orrr(TW16U reg1,TW16U reg2);
		TINT	xorrm(TW16U reg,TW16U value);
		TINT	xorrr(TW16U reg1,TW16U reg2);
		//Bit���Z
		TINT	tstrb(TW16U reg,TW16U value);
		TINT	setrb(TW16U reg,TW16U value);
		TINT	rstrb(TW16U reg,TW16U value);
		//����������
		TINT	movrmo(TW16U reg1,TW16U value,TW16U of);
		TINT	movrro(TW16U reg1,TW16U reg2,TW16U of);
		TINT	movrr(TW16U reg1,TW16U reg2);
		TINT	movrm(TW16U reg1,TW16U value);
		TINT	movpm(TW16U reg1,TW16U reg2);
		TINT	movsm(TW16U value);
		TINT	movcm(TW16U value);
		//�X�^�b�N����
		TINT	pushr(TW16U reg);
		TINT	popr(TW16U reg);
		//����
		TINT	brm(TW16U addr);
		TINT	brro(TW16U reg,TW16U of);
		TINT	brzm(TW16U addr);
		TINT	brnzm(TW16U addr);
		TINT	brgt(TW16U addr);
		TINT	brlt(TW16U addr);
		TINT	breq(TW16U addr);
		TINT	brneq(TW16U addr);

		//�T�u���[�`��
		TINT	callm(TW16U addr);
		TINT	callr(TW16U reg);
		TINT	ret(TVOID);

		// ������
		TINT	ei(TVOID);
		TINT	di(TVOID);
		TINT	swi(TVOID);
		TINT	reti(TVOID);

		//�����ݏ����pMUTEX
		bool	m_InterruptEnable;	// EI/DI����p
		TW32U	m_InterruptTrap;	// �����ݔ������m
		HANDLE    m_Mutex;
    };

};

#endif  //__C_MODULE_CpuModule_H__
