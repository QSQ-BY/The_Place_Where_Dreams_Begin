#include <graphics.h>

//全局变量
int colorCount = 7;
int COLOR_MENU_X = 550;
int COLOR_MENU_Y = 20;

// 检查菜单按钮点击
bool checkMenuButtonClick(int x, int y) {
    int distance_sq = (x - 620) * (x - 620) + (y - 20) * (y - 20);
    return distance_sq <= 225;
}

// 检查颜色菜单点击
int checkColorMenuClick(int x, int y) {
    if (x < COLOR_MENU_X || x > COLOR_MENU_X + 20) return -1;

    int buttonSize = 20;
    int spacing = 5;
    int index = (y - COLOR_MENU_Y) / (buttonSize + spacing);

    if (index >= 0 && index < colorCount) {
        return index;
    }
    return -1;
}

//检查图形按钮点击
bool checkGraphicButtonClick(int x, int y) {
    int distance_sq = (x - 620) * (x - 620) + (y - 60) * (y - 60);
    return distance_sq <= 225;
}