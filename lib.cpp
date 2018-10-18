#include "lib.h"

char aryVars[512][1024];               // Split関数で分割された文字列の格納配列

// strで示される文字列を 10進/16進数とみなして数値に変換する
unsigned long htoi(char *str) {
	unsigned long val = 0;
	int idx;
	int n = 10;

	if(str == NULL) {
		return 0;
	}

	for(idx = 0; idx < (int)strlen(str); idx++) {
		if(str[idx] == '$' || str[idx] == 'x') {
			n = 16;	// $～、又は 0x～表記の場合は16進数とする
		}

		val *= n;
		if(str[idx] >= '0' && str[idx] <= '9') {
			val += (str[idx] - '0');
		} else if(str[idx] >= 'a' && str[idx] <= 'f') {
			val += (str[idx] - 'a' + 10);
		}
	}

	return val;
}

// p_Splitstrをp_Delimiter文字列で区切った文字列をp_ary[] に格納し、token数を返す
// p_ary[]は文字列の配列であり、十分な容量を確保すること
int Split(char *p_Splitstr, const char *p_Delimiter)
{
    char    *ptr, *prevptr;
    int     cnt = 0;
    char    buf[2048];

    strcpy_s(buf, 2048, p_Splitstr);
    ptr = buf; prevptr = buf;
    if (strstr(p_Splitstr, p_Delimiter) == NULL) {
        strcpy_s(aryVars[0], 2048, p_Splitstr);
        if(strlen(p_Splitstr) > 0) {cnt = 1;}
        return cnt;
    }

    while( 1 )
    {
        if (cnt == 1024) break;
        ptr = strstr(ptr, p_Delimiter);
        if (ptr == NULL) break;

		if((ptr - prevptr) != 0) {
			strcpy_s(aryVars[cnt], 1024, "");
			strncpy_s(aryVars[cnt], 1024, prevptr, (ptr - prevptr));
			aryVars[cnt][ptr - prevptr] = '\0';
			cnt++;
		}

        /* 次のトークンを取得します。 */
        ptr += strlen(p_Delimiter);
        prevptr = ptr;
    }

    strncpy_s(aryVars[cnt], 1024, prevptr, (buf + strlen(buf) - prevptr));
    aryVars[cnt][buf + strlen(buf) - prevptr] = '\0';
    cnt++;

    return cnt;
}

// p_Srcに含まれるp_aをp_bに置換して、io_Dstに格納
// io_Dstは十分な領域を確保のこと
int Replace(char *p_Src, char *p_a, char *p_b, char *io_Dst)
{
    int     aryCnt, i;

    if (strstr(p_Src, p_a) == NULL) {
        strcpy_s(io_Dst, 2048, p_Src);
    } else {
        aryCnt = Split(p_Src, p_a);
        io_Dst[0] = '\0';
        for(i=0; i<aryCnt; i++) {
            strncat_s(io_Dst, 1024, aryVars[i], strlen(aryVars[i]));
            if (i != (aryCnt-1)) {
                strncat_s(io_Dst, 1024, p_b, strlen(p_b));
            }
        }
    }

    return strlen(io_Dst);
}

char *Left(char *p_Src, int p_Len, char *io_Dst)
{
    strncpy_s(io_Dst, 1024, p_Src, p_Len);
    io_Dst[p_Len] = '\0';
    return io_Dst;
}

char *Right(char *p_Src, int p_Len, char *io_Dst)
{
    strncpy_s(io_Dst, 1024, p_Src + strlen(p_Src) - p_Len, p_Len);
    io_Dst[p_Len] = '\0';
    return io_Dst;
}

char *LCase(char *p_Src, char *io_Dst)
{
    int l;

    strcpy_s(io_Dst, 2048, p_Src);
    for(l=0; l<(int)strlen(p_Src); l++) {
        io_Dst[l] = (char)tolower((int)io_Dst[l]);
    }
    return io_Dst;
}
char *UCase(char *p_Src, char *io_Dst)
{
    int l;

    strcpy_s(io_Dst, 2048, p_Src);
    for(l=0; l<(int)strlen(p_Src); l++) {
        io_Dst[l] = (char)toupper((int)io_Dst[l]);
    }
    return io_Dst;
}

int Trim(char *p_Src, char *io_Dst)
{
    int st, ed, i, j;

    st = 0;
    while(p_Src[st] == ' ' || p_Src[st] == 0x09) {
        st++;
    }

    ed = strlen(p_Src);
    while(p_Src[ed] == ' ' || p_Src[ed] == 0x09 || p_Src[ed] == 0x00) {
        ed--;
    }

    for(i = st, j = 0; i <= ed; i++, j++) {
        io_Dst[j] = p_Src[i];
    }
    io_Dst[j] = '\0';

    return ed - st;
}


// VC++に存在してgccに存在しない関数の定義
void gstrrev(char *buf)
{
    int st, ed;
    char    tmp;

    st = 0;
    ed = strlen(buf)-1;

    while(st<ed) {
        tmp = buf[st];
        buf[st] = buf[ed];
        buf[ed] = tmp;
        st++;
        ed--;
    }
}

// 16進数→10進数変換
unsigned long Hex(char *hexval)
{
    int idx = 0, a = 10;
    unsigned long val = 0;
	long	sign = 1;


    // 1文字目が数値か"$"でなければ普通の文字列なのでエラー
    if (!( (hexval[0] >= '0' && hexval[0] <= '9') || (hexval[0] == '$') || (hexval[0] == ' ') || (hexval[0] == 9) || (hexval[0] == '-'))) {
        return -1;
    }

    while (hexval[idx] != 0) {
        if (hexval[idx] == ' ' || hexval[idx] == 9 || hexval[idx] == 10 || hexval[idx] == '.' || hexval[idx] == '_') {idx++; continue;}
        if (hexval[idx] == '$' || hexval[idx] == 'x') {
            a = 16;
            idx++;
        }
		if (hexval[idx] == '-') {
			sign = -1;
		}

        val *= a;
        if (hexval[idx] >= '0' && hexval[idx] <= '9') {
            val += hexval[idx] - '0';
        }
        if (a == 16) {
            if (hexval[idx] >= 'a' && hexval[idx] <= 'f') {
                val += hexval[idx] - 'a' + 10;
            } else  if (hexval[idx] >= 'A' && hexval[idx] <= 'F') {
                val += hexval[idx] - 'A' + 10;
            }
        }
        idx++;
    }

    return val * sign;

}

// 数値判定
// 1=True(valは数値) 0=False(valは文字列)
int isNumeric(char *val)
{
    char buf[256];
	int	i;
	int ret;		//判定結果

	//初期化
	ret = 1;
	i = 0;

	//文字列整形
    Trim(val, buf);

	//1文字目が　'$' or '0-9' or '-' or '+'　で無ければ数値では無い
	//2文字目以降が　'0-9' or 'A-F' で無ければ数値では無い
	if(strlen(buf) >= 1){
		if(buf[0] == '$' && buf[1] != '\0'){
		//16進　チェック
			for(i = 1; buf[i] != '\0'; i++){
				if( !(buf[i] >= '0' && buf[i] <= '9') && !(buf[i] >= 'A' && buf[i] <= 'F') && !(buf[i] >= 'a' && buf[i] <= 'f') ){
					ret = 0;
					break;
				}
			}
		} else if((buf[0] >= '0' && buf[0] <= '9') || (buf[0] == '+') || (buf[0] == '-')){
		//10進　チェック
			if(((buf[0] == '+') || (buf[0] == '-')) && buf[1] == '\0'){
				ret = 0;
			}
			for(i = 1; buf[i] != '\0'; i++){
				if( !(buf[i] >= '0' && buf[i] <= '9') ){
					ret = 0;
					break;
				}
			}
		} else {
			//16進・10進でもない
			ret = 0;
		}
	}
	return ret;
}


// 文字列整形(コメント・不要スペース除去)
int StrScrape(char *inStr, char *outStr)
{
    char    *tmp;
    int     i;
	char	tmp2[1024];
	char	tab[2];
	tab[0] = 9;
	tab[1] = '\0';

    strcpy_s(outStr, 2048, inStr);

	//  2008 02 07 cyberbeing toh start
    if ((tmp = strstr(outStr,";")) != NULL) {                                 // コメント除去
        outStr[ (tmp - outStr) ] = '\0';
    }
    while(1) {
		if ((tmp = strstr(outStr,"\t")) != NULL) {                                // tab文字置き換え
			outStr[ (tmp - outStr) ] = ' ';
		}
		if(tmp == NULL) break;
	}
    if ((tmp = strstr(outStr,tab)) != NULL) {                                 // tab文字置き換え
        outStr[ (tmp - outStr) ] = ' ';
    }
    while(1) {
		if ((tmp = strstr(outStr,",")) != NULL) {                                 // カンマ文字置き換え
			outStr[ (tmp - outStr) ] = ' ';
		}
		if(tmp == NULL) break;
	}
    if ((tmp = strstr(outStr,"\r")) != NULL) {                                 // 改行コード変換
        outStr[ (tmp - outStr) ] = '\0';
    }
    if ((tmp = strstr(outStr,"\n")) != NULL) {                                 // 改行コード変換
        outStr[ (tmp - outStr) ] = '\0';
    }
	//  2008 02 07 cyberbeing toh end

	if ((tmp = strstr(outStr,"#")) != NULL) {                                 // コメント除去
        outStr[ (tmp - outStr) ] = '\0';
    }
    if ((tmp = strstr(outStr,"//")) != NULL) {                                // コメント除去
        outStr[ (tmp - outStr) ] = '\0';
    }
    for(i=0; i<(int)strlen(outStr); i++) {                                    // 不要な制御コードを除去
        if (outStr[i] < 0x1c && outStr[i] != 0x9) {
            outStr[i] = 0;
            break;
        }
    }
	strcpy_s(tmp2, 1024, outStr);
    Trim(tmp2, outStr);

    return 0;
}


#ifndef WIN32
char    *itoa(int val, char *ibuf, int size)
{
    int cnt=0;

    if (val == 0) {
        strcpy_s(ibuf, 2048, "0");
    } else {
        strcpy_s(ibuf, 2048, "");
        while(val != 0) {
            switch(val % 10) {
            case 0: strcat(ibuf, "0"); break;
            case 1: strcat(ibuf, "1"); break;
            case 2: strcat(ibuf, "2"); break;
            case 3: strcat(ibuf, "3"); break;
            case 4: strcat(ibuf, "4"); break;
            case 5: strcat(ibuf, "5"); break;
            case 6: strcat(ibuf, "6"); break;
            case 7: strcat(ibuf, "7"); break;
            case 8: strcat(ibuf, "8"); break;
            case 9: strcat(ibuf, "9"); break;
            }
            val /= 10;
            cnt++;
            if (cnt == size) {
                break;
            }
        }
        gstrrev(ibuf);
    }

    return ibuf;
}

#endif

#ifndef WIN32
char tolower(char c)
{
    char tmp;

    tmp = c;
    if (tmp>='A' && tmp<='Z') {
        tmp = tmp - 'A' + 'a';
    }

    return tmp;
}

char toupper(char c)
{
    char tmp;

    tmp = c;
    if (tmp>='a' && tmp<='z') {
        tmp = tmp - 'a' + 'A';
    }

    return tmp;
}
#endif
