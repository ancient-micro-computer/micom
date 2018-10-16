/************************************************************/
/****   logutil.h										*****/
/************************************************************/
#ifndef _LOGUTIL_H_
#define _LOGUTIL_H_

//Note:
// logutil.c �Ŏ������Ă���֐��@�\�𖳌��ɂ���ɂ́A"#define _LOG_ 1" ���R�����g�����Ă��������B
#define _LOG_ 1

// ���[�J�����O�֘A
void log_init(void);
void log_close(void);
void log(char *log_file, const char *format, ... );

/***********************************************************/
/****   defines										 	****/
/***********************************************************/
#define		LOG_ERROR				"error.log"
#define		LOG_INFO				"info.log"
#define		LOG_OBJ					"obj.log"


#endif	/* _LOGUTIL_H_ */
