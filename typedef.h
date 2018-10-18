/******************************************************************************
 *  ファイル名
 *      typedef.h
 *  ブロック名
 *      -
 *  概要
 *      変数型定義
 ******************************************************************************/
#ifndef __CMD_IF_TYPEDEF_H__
#define __CMD_IF_TYPEDEF_H__

/*----------------------------------------------------------------------------*
 * 型定義
 *----------------------------------------------------------------------------*/
#define	TVOID void
typedef	char  TW8;
typedef	short TW16;
typedef long  TW32;
typedef	int   TINT;
typedef	unsigned int TUINT;
typedef	bool  TBOOL;

#ifndef TRUE
#define	TRUE	1
#endif
#ifndef FALSE
#define	FALSE	0
#endif

/* 8bit  */
typedef	unsigned char TW8U;
typedef	signed   char SINT08;
typedef signed   char TW8S;

/* 16bit */
typedef	unsigned short TW16U; 
typedef	signed   short SINT16;
typedef signed   short TW16S;

/* 32bit */
typedef	unsigned long TW32U;
typedef	signed   long SINT32;
typedef signed   long TW32S;

#endif  /* CMD_IF_TYPEDEF_H */
