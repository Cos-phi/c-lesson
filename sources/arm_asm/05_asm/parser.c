#include "clesson.h"

int substreq(char* s1, struct Substring s2){
    if( 0 == strncmp(s1, s2.str, s2.len) ){
        return 1;
    }else{
        return 0;
    }
}

int streq(char* s1, char* s2){
    if( 0 == strcmp(s1,s2) ){
        return 1;
    }else{
        return 0;
    }
}

int skip_whitespace(char* str){
/*
    文字列冒頭のスペースを読み飛ばして、読み飛ばした数をreturnします。
    e.g. "  abcd.." -> 2
*/
    int pos = 0;
    while( ' ' == str[pos] ){
        pos++;
    }
    return pos;
}

int skip_comma(char* str){
    int pos = skip_whitespace(str);
    if( ',' == str[pos] ){
        pos++;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int skip_sbracket(char* str){
    int pos = skip_whitespace(str);
    if( ('[' == str[pos])||(']' == str[pos]) ){
        pos++;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int is_register(char* str){
    int pos = skip_whitespace(str);
    if( 'r' == str[pos] ){
        return 1;
    }else{
        return 0;
    }
}
int is_sbracket(char* str){
    int pos = skip_whitespace(str);
    if( ('[' == str[pos])||(']' == str[pos]) ){
        return 1;
    }else{
        return 0;
    }
}

int parse_raw_value(char* str, int* out_value){ 
/*
    16進数を表す文字列をパースして、数値を返します。読んだ文字数をreturnします。
    e.g. "0x10" -> 16
*/
    *out_value = 0;
    int pos = skip_whitespace(str);
    if( ('0' == str[pos++]) && ('x' == str[pos++]) ){ 
        while(1){
            if( (str[pos] >= '0')&&(str[pos] <= '9') ){
                *out_value *= 16;
                *out_value += str[pos] - '0';
            }else if( (str[pos] >= 'A')&&(str[pos] <= 'F') ){
                *out_value *= 16;
                *out_value += str[pos] - 'A' + 0xA;
            }else if( (str[pos] >= 'a')&&(str[pos] <= 'f') ){
                *out_value *= 16;
                *out_value += str[pos] - 'a' + 0xA;
            }else{
                break;
            }
            pos++;
        };
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int parse_immediate_value(char* str, int* out_value){ 
/*
    即値を表す文字列をパースして、数値を返します。読んだ文字数をreturnします。
    e.g. " #0x1A" -> 26
*/
    *out_value = 0;
    int pos = skip_whitespace(str);
    if( '#' != str[pos++] ){ 
        return PARSE_FAIL;
    }
    int value_sign;
    if( '-' == str[pos] ){ //即値が負の時  e.g. #-0x123
        value_sign = -1;
        pos++;
    }else{ //即値が正の時
        value_sign = 1;
    }
    
    if( ('0' == str[pos++]) && ('x' == str[pos++]) ){ 
        while(1){
            if( (str[pos] >= '0')&&(str[pos] <= '9') ){
                *out_value *= 16;
                *out_value += str[pos] - '0';
            }else if( (str[pos] >= 'A')&&(str[pos] <= 'F') ){
                *out_value *= 16;
                *out_value += str[pos] - 'A' + 0xA;
            }else if( (str[pos] >= 'a')&&(str[pos] <= 'f') ){
                *out_value *= 16;
                *out_value += str[pos] - 'a' + 0xA;
            }else{
                break;
            }
            pos++;
        };
        *out_value *= value_sign;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}

int parse_register(char* str, int* out_register){ 
/*
    レジスタを表す文字列をパースして、レジスタ番号を返します。読んだ文字数をreturnします。
    e.g. "r4" -> 4
*/
    int pos = skip_whitespace(str);
    if( 'r' == str[pos] ){
        pos++;
        if( '0'==str[pos]||('2'<=str[pos] && '9'>=str[pos]) ){ //e.g. r0, r2 ~ r9
            *out_register = str[pos]-'0';
            pos++;
        }else if( '1' == str[pos] ){ //e.g. r10 ~ r15
            pos++;
            if( '0'<=str[pos] && '5'>=str[pos] ){
                *out_register = 10 + str[pos] - '0';
                pos++;
            }else{
                *out_register = 1;
            }
        }else{
            return PARSE_FAIL;
        }
    }else{
        return PARSE_FAIL;
    }
    return pos;
}

int parse_one(char *str, struct Substring* out_subs){
/*
    文字列をパースして、文字の単語・コロン・ドット・なにもなしのどれかをSubstringで返します。読んだ文字数をreturnします。
    e.g. "    "      -> "    ",   return 4
         " abc123: " -> "abc123", return 7
         ":"         -> ":",      return 1
*/
    int pos = skip_whitespace(str);
    if( '\0' == str[pos] ){ // 空白のまま終わった場合（なにもなし）
        out_subs->str = str;
        out_subs->len = pos;
        return pos;
    }

    if( ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){ //e.g. "_abc123"
        out_subs->str = &str[pos];
        int start_pos = pos++;
        while( ('0'<=str[pos] && '9'>=str[pos]) || ('A'<=str[pos] && 'z'>=str[pos]) || '_'==str[pos] ){
            pos++;
        } 
        if( (' ' == str[pos]) || (':' == str[pos]) || (',' == str[pos]) || ('\0' == str[pos]) ){
            out_subs->len = (pos - start_pos);
            return pos;
        }else{
            return PARSE_FAIL;
        }
    }else if( (':' == str[pos])||('.' == str[pos])){ //e.g. ":"
        out_subs->str = &str[pos];
        pos++;
        out_subs->len = pos;
        return pos;
    }else{
        return PARSE_FAIL;
    }
}