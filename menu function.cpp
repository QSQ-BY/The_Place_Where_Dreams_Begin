#include <graphics.h>
#include <stdlib.h>
#include "draw.h"
#include "menu Button.h"
#include "Color Button.h"
#include "graphic button.h"
#include "save.h"
#include "readFile.h"
#include "ui.h"
#include "query.h"

// 主菜单函数
void menu() {
    // 设置默认绘图与背景颜色
    setcolor(MAGENTA);
    setfillcolor(YELLOW);
    setbkcolor(BLACK);
    setfontbkcolor(BLACK);

    // 主循环：显示主菜单并响应用户输入
    char* strBuff = (char*)malloc(sizeof(char) * 50);
    do {
        // 弹出输入框获取用户选择
        inputbox_getline("欢迎来到我的画板", "根据菜单选择：\n1.作画 2.清除屏幕 3.退出\n4.保存画布 5.加载画布 6.查询图形\n7.保存图形数据 8.加载图形数据\n", strBuff, 50);
        drawMenuButton();
        drawColorMenu();
        drawGraphicButton();

        // 根据用户选择执行操作
        switch (strBuff[0]) {
        case '1':
            showTempMessage("开始作画", 10, 10, 500);
            delay_ms(500);
            draw();
            break;

        case '2':
            // 清空画布
            cleardevice();
            showTempMessage("屏幕已清除", 10, 10, 500);
            delay_ms(500);
            cleardevice();
            break;

        case '3': {
            // 退出前询问是否保存
            char* saveBuf = (char*)malloc(sizeof(char) * 50);
            inputbox_getline("退出", "是否保存画布？\n1.保存 2.不保存", saveBuf, 50);
            if (saveBuf[0] == '1') {
                saveCanvasAs();
            }
            free(saveBuf);
            free(strBuff);
            closegraph();
            return;
        }

        case '4':
            // 另存为
            saveCanvasAs();
            break;

        case '5':
            // 加载画布
            if (loadCanvasWithPrompt()) {
                showTempMessage("加载成功", 10, 10, 800);
            }
            break;

        case '6':
            // 查询图形
            queryShapes();
            break;

        case '7':
            // 保存图形数据
            saveShapesToFile();
            break;

        case '8':
            // 加载图形数据
            loadShapesFromFile();
            break;

        default:
            showTempMessage("输入错误", 10, 10, 800);
            break;
        }
    } while (1);
}