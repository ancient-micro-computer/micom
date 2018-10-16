#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "logutil.h"

//Note:
// logutil.c �Ŏ������Ă���֐��@�\�𖳌��ɂ���ɂ́Alogutil.h�� "#define _LOG_ 1" ���R�����g�����Ă��������B

// ���[�J�����O�֘A
#define         LOGS        512
static  FILE    *fp[LOGS];      /* �ő�512���̃��O�t�@�C���|�C���^�Ǘ��z�� */
static  char    logname[LOGS][256];
static  int     fpidx;

//--------------------------------------------------------------------------------
//  [brief]         Output log string to file.
//  [function]  ���O�o�͊֐�
//  [argument]  log_file    ���O�t�@�C����
//              format      ���O����
//  [return]    1=����A0=�ُ�I��
//  [note]      �������A���s��\n�͕s�v
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

void log(char *log_file, const char *format, ... )
{
#if _LOG_
    int         i;
    FILE        *f;
    char        buf[1024];
    va_list     vl;                         /* �ϗv�f�������X�g */

    /* ����A�N�Z�X���̓t�@�C���|�C���^��ݒ� */
    for(f = NULL, i = 0; i < fpidx; i++) {
        if (strcmp(logname[i], log_file)==0) {
            f = fp[i];
            break;
        }
    }
    if (f == NULL) {
        if (fp[fpidx] == NULL && fpidx < LOGS) {
            if ( (fopen_s(&fp[fpidx], log_file, "w")) != 0) {
                return;
            }
            strcpy_s(logname[fpidx], 256, log_file);
            f = fp[i];
            fpidx++;
        } else {
            return;     /* �Ǘ����O�t�@�C�����I�[�o�[ */
        }
    }

    /* buf �Ƀ��O��������i�[ */
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

