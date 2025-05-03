#include <windows.h>
#include <bits/stdc++.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace std::chrono;

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

const int MAX_HEIGHT = 1080;
const int MAX_WIDTH = 1920;
int sy = 72, sx = 128, n = 1000, rate = 1;
string color_cache[MAX_HEIGHT][MAX_WIDTH];
char frame_buffer[MAX_HEIGHT * (MAX_WIDTH * 20 + 2)]; // 预分配帧缓冲区

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void ResizeConsoleWindow(int cols, int lines) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { (SHORT)cols, (SHORT)lines };
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SMALL_RECT windowSize = { 0, 0, (SHORT)(cols - 1), (SHORT)(lines - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void SetConsoleFontSize(int size) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = { sizeof(fontInfo) };
    GetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
    fontInfo.dwFontSize.Y = size;
    SetCurrentConsoleFontEx(hConsole, FALSE, &fontInfo);
}

void enable_ansi() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void print() {
    char* ptr = frame_buffer;
    for (int i = 0; i < sy; i++) {
        for (int j = 0; j < sx; j++) {
            memcpy(ptr, color_cache[i][j].data(), color_cache[i][j].size());
            ptr += color_cache[i][j].size();
        }
        memcpy(ptr, "\033[0m\n", 5);
        ptr += 5;
    }
    fwrite(frame_buffer, 1, ptr - frame_buffer, stdout);
    fflush(stdout);
}

void setsize(int n) {
    int fontSize = n;
    int fontWidth = fontSize / 2;
    int cols = 800 / fontWidth;
    int lines = 600 / fontSize;
    SetConsoleFontSize(fontSize);
    ResizeConsoleWindow(cols, lines);
}

int main() {
    system("title 基于cmd的视频播放器");
    HWND hwnd = GetConsoleWindow();
    enable_ansi();
    
    // 禁用同步和绑定，提高cout性能
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    
    string filename = "output.txt";
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << endl;
        return 1;
    }

    // 预分配字符串内存
    for (int i = 0; i < MAX_HEIGHT; i++) {
        for (int j = 0; j < MAX_WIDTH; j++) {
            color_cache[i][j].reserve(20);
        }
    }

    SetForegroundWindow(hwnd);
    setsize(8);
    ShowWindow(hwnd, SW_MAXIMIZE);

    auto frame_start = high_resolution_clock::now();
    auto last_fps_update = frame_start;
    int frame_count = 0;
    double fps = 0;

    for (int i = 1;; i++) {
        if (i == n) {
            file.close();
            file.open(filename);
            i = 1;
        }

        gotoxy(0, 0);
        file >> n >> rate >> sx >> sy;
        
        // 读取帧数据
        for (int y = 0; y < sy; y++) {
            for (int x = 0; x < sx; x++) {
                int r, g, b;
                file >> r >> g >> b;
                color_cache[y][x] = "\033[48;2;" + 
                    to_string(r) + ";" + 
                    to_string(g) + ";" + 
                    to_string(b) + "m  ";
            }
        }

        print();
        
        // 精确帧率控制
        frame_start += microseconds(static_cast<int>(1000000.0 / (30.0 / rate)));
        this_thread::sleep_until(frame_start);
        
        // 计算FPS
        frame_count++;
        auto now = high_resolution_clock::now();
        if (duration_cast<milliseconds>(now - last_fps_update).count() >= 500) {
            fps = frame_count * 1000.0 / duration_cast<milliseconds>(now - last_fps_update).count();
            frame_count = 0;
            last_fps_update = now;
        }
        
        printf(" Frame:%d/%d  \n", i, n);
        printf(" FPS:%.1lf  \n", fps);
        printf(" STD:%.4lf", 30.0 / rate);
    }

    return 0;
}
