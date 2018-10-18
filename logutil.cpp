#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "logutil.h"

//Note:
// logutil.c で実装している関数機能を無効にするには、logutil.hの "#define _LOG_ 1" をコメント化してください。

// ローカルログ関連
#define         LOGS        512
static  FILE    *fp[LOGS];      /* 最大512個分のログファイルポインタ管理配列 */
static  char    logname[LOGS][256];
static  int     fpidx;

//--------------------------------------------------------------------------------
//  [brief]         Output log string to file.
//  [function]  ログ出力関数
//  [argument]  log_file    ログファイル名
//              format      ログ文言
//  [return]    1=正常、0=異常終了
//  [note]      文言中、改行の\nは不要
//--------------------------------------------------------------------------------
void log_init(void) {
#if _LOG_
    int i;

    for(i = 0; i < LOGS; i++) {
        fp[i] = NULL;
        logname[i][0] = 0x0;
    }
    fpidx = 0;

#endif
}

void log_close(void) {
#if _LOG_
    int i;

    for(i = 0; i < LOGS; i++) {
        if (fp[i] != NULL) {
            fclose(fp[i]);
        } else {
            break;
        }
    }
#endif
}

void log(const char *log_file, const char *format, ... )
{
#if _LOG_
    int         i;
    FILE        *f;
    char        buf[1024];
    va_list     vl;                         /* 可変要素引数リスト */

    /* 初回アクセス時はファイルポインタを設定 */
    for(f = NULL, i = 0; i < fpidx; i++) {
        if (strcmp(logname[i], log_file)==0) {
            f = fp[i];
            break;
        }
    }
    if (f == NULL) {
        if (fp[fpidx] == NULL && fpidx < LOGS) {
            if ( (fp[fpidx] = fopen(log_file, "w")) != 0) {
                return;
            }
            strcpy_s(logname[fpidx], 256, log_file);
            f = fp[i];
            fpidx++;
        } else {
            return;     /* 管理ログファイル数オーバー */
        }
    }

    /* buf にログ文字列を格納 */
    va_start( vl, format );
    vsprintf_s( buf, 1024, format, vl );
    va_end( vl );

    fprintf(f, "%s", buf);
//	if(buf[strlen(buf)-1] != '\n') {
//		fprintf(f, "\n", buf);
//	}
	fflush(f);
#endif
}

