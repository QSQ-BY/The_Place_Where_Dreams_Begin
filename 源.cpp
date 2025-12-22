#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "menu.h"
#include "readFile.h"
#include "draw.h"
#include "menu Button.h"
#include "check.h"
#include "Color Button.h"
// 主函数

int main(void) {
    // 初始化图形窗口，设置窗口大小为640x480像素
    initgraph(640, 480);

    // 设置绘图颜色和背景颜色
    setcolor(MAGENTA);      // 设置绘图颜色为品红色
    setfillcolor(YELLOW);   // 设置填充颜色为黄色
    setbkcolor(BLACK);       // 设置背景颜色为黑色
    setfontbkcolor(WHITE);  // 设置文字背景颜色为白色

    readFile();
    menu();
    return 0;
}
