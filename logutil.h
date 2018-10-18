/************************************************************/
/****   logutil.h										*****/
/************************************************************/
#ifndef _LOGUTIL_H_
#define _LOGUTIL_H_

//Note:
// logutil.c で実装している関数機能を無効にするには、"#define _LOG_ 1" をコメント化してください。
#define _LOG_ 1

#define strcpy_s(a, b, c) strcpy(a, c)
#define vsprintf_s(a,b,c,d) vsprintf(a, c, d)

// ローカルログ関連
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
