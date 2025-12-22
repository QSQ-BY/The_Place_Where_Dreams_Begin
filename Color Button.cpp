
// 绘制颜色选择菜单
#include <graphics.h>
#include "draw.h"
#include "check.h"

void drawColorMenu() {
    int x = COLOR_MENU_X;
    int y = COLOR_MENU_Y;
    int buttonSize = COLOR_MENU_Y;
    int spacing = 5;

    for (int i = 0; i < colorCount;i++) {
        setfillcolor(colors[i]);
        rectangle(x, y + i * (buttonSize + spacing),
            x + buttonSize, y + i * (buttonSize + spacing) + buttonSize);
        fillellipse(x + buttonSize / 4, y + i * (buttonSize + spacing), buttonSize / 4, buttonSize / 4);
    }
}