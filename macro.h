/******************************************************************************
 *  �t�@�C����
 *      cmdif_macro.h
 *  �u���b�N��
 *      -
 *  �T�v
 *      �}�N����`
 ******************************************************************************/
#ifndef __MACRO_H__
#define __MACRO_H__

/*============================================================================*
 * �O���[�o���}�N��
 *============================================================================*/
// ���W�X�^�Ǘ��o�b�t�@
#define MEM_IF_MAXLISTS         256
#define MEM_IF_MAXMEMORIESS     256

// ���[�h�E�n�[�t���[�h�E�o�C�g��`
#define D_CMDIF_CUI_TXT   0
#define D_CMDIF_CUI_BIN   1
#define D_CMDIF_VERILOG     2

#define CMDIF_ENDIAN_BIG    0
#define CMDIF_ENDIAN_LITTLE 1
#ifdef WIN32
#define CMDIF_DEFAULT_ENDIAN    CMDIF_ENDIAN_BIG
#else
#define CMDIF_DEFAULT_ENDIAN    CMDIF_ENDIAN_LITTLE
#endif

// �o�b�`
#define D_CMDIF_CMD_INVALID -1
#define D_CMDIF_CMD_CUI   0
#define D_CMDIF_CMD_VERILOG 1

// �R�}���h�n
// 1.CUI�`��
/*#define D_CMDIF_CMDNOTFOUND -1
#define D_CMDIF_LOAD        1
#define D_CMDIF_SAVE        2
#define D_CMDIF_SHOW        3
#define D_CMDIF_SET         4
#define D_CMDIF_GO          5
#define D_CMDIF_SYSCMD      6
#define D_CMDIF_LOADB       7
#define D_CMDIF_SAVEB       8
#define D_CMDIF_LOADV       9
#define D_CMDIF_SAVEV       10
#define D_CMDIF_BATCH       11
#define D_CMDIF_BREAK       12
#define D_CMDIF_CONNECT     13
#define D_CMDIF_NETLOAD     14
#define D_CMDIF_NETLOADB    15
#define D_CMDIF_NETLOADV    16
#define D_CMDIF_NETSAVE     17
#define D_CMDIF_NETSAVEB    18
#define D_CMDIF_NETSAVEV    19
#define D_CMDIF_NETRECV     20
#define D_CMDIF_NOP         97
#define D_CMDIF_HELP        98
#define D_CMDIF_QUIT        99
*/
// 2.verilog�`��
#define D_CMDIF_VL_WRITE    1
#define D_CMDIF_VL_READ     2
#define D_CMDIF_VL_NOP      3
#define D_CMDIF_ACK         30      // �֐�Ack
#define D_CMDIF_NACK        31      //     Nack
#define D_CMDIF_MOREDATA	32		// �����E�f�[�^�p��
#define D_CMDIF_TIMEOUT     10000   // 10000�T�C�N���҂����ꍇ�̓^�C���A�E�g
#define D_CMDIF_VL_QUIT     98
#define D_CMDIF_EXEC_BREAK  99

// �G���[�n
#define D_ERR_CMD_COMPLETE      99
#define D_ERR_OK                0
#define D_ERR_NAME_FOUND        1
#define D_ERR_NAME_NOTFOUND     -1
#define D_ERR_CMDERR            -2
#define D_ERR_CANTOPEN          -3
#define D_ERR_NAME_MISMATCH     -4
#define D_ERR_BLK_MISMATCH      -5
#define D_ERR_OVER_MAXLISTS     -6
#define D_ERR_MEM_BANKS         -7
#define D_ERR_TIMEOUT           -8
#define D_ERR_CANT_CALLBACK     -9
#define D_ERR_REGIDX_OVER       -10
#define D_ERR_MEMADR_OVER       -11
#define D_ERR_CANT_ERASE_BP     -12
#define D_ERR_CANT_ERASE_CN     -13
#define D_ERR_CANT_ACCESS	    -14
#define D_ERR_SYSTEM            -99

// �G���W������
#define D_EXEC_OK					30
#define D_EXEC_HOLD					96
#define D_EXEC_BREAK				97

/*----------------------------------------------------------------------------*
 * �T�[�r�X�}�N����`(�悩������g���Ă�������)
 *----------------------------------------------------------------------------*/
/* �p�����[�^�擾 : PARAM �� [MSB:LSB]�͈͂�؂�o�����l��Ԃ� */
#define D_GET_PARAM(PARAM, MSB, LSB) ( ((PARAM) >> (LSB)) & ((1 << ((MSB) - (LSB) + 1)) - 1) )

/* CM���W�X�^�p�����[�^unpack : CMVAL�� [MSB:LSB]�͈͂�؂�o�����l��Ԃ� */
#define D_UNPACK_REGPARAM(CMVAL, MSB, LSB) ( ((TW16U)(CMVAL) >> (LSB)) & ((1 << ((MSB) - (LSB) + 1)) - 1) )

/* CM���W�X�^�p�����[�^pack : CMVAL��[MSB:LSB]�͈͂�PARAM����(Packing)�����l�� CMVAL�ɑ������ */
#define D_PACK_REGPARAM(CMVAL, PARAM, MSB, LSB) CMVAL = ( ((TW16U)(CMVAL) & (~(((1 << ((MSB) - (LSB) + 1)) - 1) << (LSB)))) | (((PARAM) & ((1 << ((MSB) - (LSB) + 1)) - 1))<< (LSB)) )

/* ��Βl : CODE �̐�Βl��Ԃ� */
#define D_ABS( CODE ) ((CODE) < 0 ? -(CODE) : (CODE))

/* MSB/LSB���o�C�g�P�ʂłЂ�����Ԃ� */
#define D_REV_EDN( v )  (((v) & 0xFF) << 24 | (((v)>>8) & 0xFF) << 16 | (((v)>>16) & 0xFF) << 8 | (((v)>>24) & 0xFF))

#endif  /* __MACRO_H__ */
