/******************************************************************************
 *  ファイル名
 *      cmdif_macro.h
 *  ブロック名
 *      -
 *  概要
 *      マクロ定義
 ******************************************************************************/
#ifndef __MACRO_H__
#define __MACRO_H__

/*============================================================================*
 * グローバルマクロ
 *============================================================================*/
// レジスタ管理バッファ
#define MEM_IF_MAXLISTS         256
#define MEM_IF_MAXMEMORIESS     256

// ワード・ハーフワード・バイト定義
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

// バッチ
#define D_CMDIF_CMD_INVALID -1
#define D_CMDIF_CMD_CUI   0
#define D_CMDIF_CMD_VERILOG 1

// コマンド系
// 1.CUI形式
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
// 2.verilog形式
#define D_CMDIF_VL_WRITE    1
#define D_CMDIF_VL_READ     2
#define D_CMDIF_VL_NOP      3
#define D_CMDIF_ACK         30      // 関数Ack
#define D_CMDIF_NACK        31      //     Nack
#define D_CMDIF_MOREDATA	32		// 処理・データ継続
#define D_CMDIF_TIMEOUT     10000   // 10000サイクル待った場合はタイムアウト
#define D_CMDIF_VL_QUIT     98
#define D_CMDIF_EXEC_BREAK  99

// エラー系
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

// エンジン制御
#define D_EXEC_OK					30
#define D_EXEC_HOLD					96
#define D_EXEC_BREAK				97

/*----------------------------------------------------------------------------*
 * サービスマクロ定義(よかったら使ってください)
 *----------------------------------------------------------------------------*/
/* パラメータ取得 : PARAM の [MSB:LSB]範囲を切り出した値を返す */
#define D_GET_PARAM(PARAM, MSB, LSB) ( ((PARAM) >> (LSB)) & ((1 << ((MSB) - (LSB) + 1)) - 1) )

/* CMレジスタパラメータunpack : CMVALの [MSB:LSB]範囲を切り出した値を返す */
#define D_UNPACK_REGPARAM(CMVAL, MSB, LSB) ( ((TW16U)(CMVAL) >> (LSB)) & ((1 << ((MSB) - (LSB) + 1)) - 1) )

/* CMレジスタパラメータpack : CMVALの[MSB:LSB]範囲にPARAMを代入(Packing)した値を CMVALに代入する */
#define D_PACK_REGPARAM(CMVAL, PARAM, MSB, LSB) CMVAL = ( ((TW16U)(CMVAL) & (~(((1 << ((MSB) - (LSB) + 1)) - 1) << (LSB)))) | (((PARAM) & ((1 << ((MSB) - (LSB) + 1)) - 1))<< (LSB)) )

/* 絶対値 : CODE の絶対値を返す */
#define D_ABS( CODE ) ((CODE) < 0 ? -(CODE) : (CODE))

/* MSB/LSBをバイト単位でひっくり返す */
#define D_REV_EDN( v )  (((v) & 0xFF) << 24 | (((v)>>8) & 0xFF) << 16 | (((v)>>16) & 0xFF) << 8 | (((v)>>24) & 0xFF))

#endif  /* __MACRO_H__ */
