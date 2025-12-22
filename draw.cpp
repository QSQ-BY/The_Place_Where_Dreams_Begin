#include <stdio.h>
#include <graphics.h>
#include "check.h"
#include "menu Button.h"
#include "Color Button.h"
#include "menu.h"
#include "graphic button.h"
#include "graphic function.h"
#include "draw.h"
#include "undo.h"
#include "ui.h"

// 全局变量
int colors[] = { WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA };
int currentColor = WHITE;

// 绘图函数（主循环）
void draw() {
    MOUSEMSG m;
    bool isDrawing = false;

    // 在界面上短暂提示撤回操作说明
    showTempMessage("提示: 在画布上点击右键可撤回上一步，鼠标中键重做\n提示：点击画布右上角红圈进入图形绘制\n白圈重回主菜单", 0, 0, 1500);
    drawMenuButton();
    drawGraphicButton();
    drawColorMenu();

    while (1) {
        m = GetMouseMsg();

        // 处理颜色选择
        if (m.uMsg == WM_LBUTTONDOWN) {
            int colorIndex = checkColorMenuClick(m.x, m.y);
            if (colorIndex != -1) {
                currentColor = colors[colorIndex];
                continue;
            }
        }

        // 处理菜单按钮点击
        if (m.uMsg == WM_LBUTTONDOWN && checkMenuButtonClick(m.x, m.y)) {
            menu();
        }

        // 处理图形按钮点击
        if (m.uMsg == WM_LBUTTONDOWN && checkGraphicButtonClick(m.x,m.y)) {
            // 进入图形绘制前保存快照，以便支持撤回
            saveSnapshot();
            graphic();
        }

        switch (m.uMsg) {
        case WM_LBUTTONDOWN:
            if (!checkMenuButtonClick(m.x, m.y) && checkColorMenuClick(m.x, m.y) == -1) {
                // 鼠标按下时保存快照，以便撤回最近一笔
                saveSnapshot();
                isDrawing = true;
                moveto(m.x, m.y);
            }
            break;
        case WM_MOUSEMOVE:
            if (isDrawing) {
                setcolor(currentColor);
                setfillcolor(currentColor);
                lineto(m.x, m.y);
            }
            break;
        case WM_LBUTTONUP:
            isDrawing = false;
            break;
        }

        // 右键撤回：在画布上右键单击时撤回上一步
        if (m.uMsg == WM_RBUTTONDOWN) {
            undoLast();
            continue;
        }

        // 中键重做（重画）
        if (m.uMsg == WM_MBUTTONDOWN) {
            redoLast();
            continue;
        }

        // 绘制 UI 元素
        drawMenuButton();
        drawGraphicButton();
        drawColorMenu();
        
    }
}