#include <windows.h>
#include <bits/stdc++.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
int sy = 72, sx = 128,n=1000,rate=1;
string color_cache[1080][1920];
void gotoxy(int x,int y) 
{
	COORD c;
	c.X=x;
	c.Y=y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}
// 初始化控制台以支持 ANSI 转义序列
void ResizeConsoleWindow(int cols, int lines) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 1. 设置屏幕缓冲区大小（Buffer）
    COORD bufferSize = { (SHORT)cols, (SHORT)lines };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // 2. 设置窗口大小（Window）
    SMALL_RECT windowSize = { 0, 0, (SHORT)(cols - 1), (SHORT)(lines - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}
void SetConsoleFontSize(int size) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = { sizeof(fontInfo) };
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    fontInfo.dwFontSize.Y = size; // 设置字体高度（宽度会自动调整）
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
}
void enable_ansi() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING; // 关键：启用 ANSI 支持
    SetConsoleMode(hOut, dwMode);
} 
// 输出图像
void print() {
    stringstream buffer;
    for (int i = 0; i < sy; i++) {
        for (int j = 0; j < sx; j++) {
            buffer << color_cache[i][j];
        }
        buffer << "\033[0m\n"; // 每行结束后重置颜色
    }
    cout << buffer.str() << flush; // 强制刷新缓冲区
}
void setsize(int n)
{
	int fontSize = n; // 字体高度（像素）
    int fontWidth = fontSize / 2; // 字体宽度（估算）

    int cols = 800 / fontWidth;  // 计算列数
    int lines = 600 / fontSize;  // 计算行数
    SetConsoleFontSize(fontSize);
    ResizeConsoleWindow(cols, lines);
}
int main() {
	system("title 基于cmd的视频播放器");
	
	//system("c.py");
	HWND hwnd=GetConsoleWindow();
	//ShowWindow(hwnd,SW_MAXIMIZE);
    enable_ansi(); 
    string filename;
    
//	filename="level!.txt";
//  filename="xxrrwmdcs.txt";
	filename="output.txt";
//	filename="moonhalo.txt";
//	filename="worldexecuteme.txt";
//	filename="disillusion.txt";
		
	ifstream file(filename);
    double temp;
    temp=clock(); 
    //system("start 1.mp3");
	Sleep(1000); 
	SetForegroundWindow(hwnd);
	setsize(8);
	ShowWindow(hwnd,SW_MAXIMIZE);
	int fps;
    for(int i=1;;i++)
    {
    	if(i==n)
	    {
	    	file.close();
	    	ifstream file(filename);
	    	i=1;
		} 
    	gotoxy(0,0);
    	file>>n>>rate>>sx>>sy;
    	for (int i = 0; i < sy; i++) {
	        for (int j = 0; j < sx; j++) {
	            int r, g, b;
	            file >> r >> g >> b;
	            color_cache[i][j] = "\033[48;2;" +
	                to_string(r) + ";" +
	                to_string(g) + ";" +
	                to_string(b) + "m  ";
	        }
 		}
	    print();
	    puts("");
	    while(clock()-temp<=33.0*rate);
	    fps=temp;
	    temp=clock();
	    printf(" Frame:%d/%d  \n",i,n);
	    printf(" FPS:%.4lf  \n",1000.0/(clock()-fps));
	    printf(" STD:%.4lf",30.0/rate);
		//Sleep(1000);
	}
    return 0;
}

