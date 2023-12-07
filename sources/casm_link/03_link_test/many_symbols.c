
extern int g_in_main; //グローバル変数の宣言
int g_in_hello_uninit; //未初期化のグローバル変数定義
int g_in_hello = 1; //初期化のあるグローバル変数定義
static int g_static_in_hello=1; //staticなグローバル変数定義
static int g_static_uninit; //staticな未初期化のグローバル変数
char *g_text = "abc"; //初期化のあるグローバル変数定義
char *g_text_uninit; //未初期化のグローバル変数定義
char g_text_arr[] = "def"; //初期化のあるグローバル変数定義
int g_large_buf[1024*1024]; //未初期化のグローバル変数定義


extern void func_in_main(char *str); //関数の宣言


int print_something(char *str) { //関数の定義
    func_in_main(str);
    func_in_main(g_text_uninit);
    return g_in_main;
}

