#include <string.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include "save.h"
#include "undo.h"
#include "ui.h"

// 启动时询问是否读取画布，并在需要时提示输入文件名进行加载
void readFile() {
    char choiceBuf[50] = {0};
    inputbox_getline("欢迎来到我的画板", "根据菜单选择：\n1.读取文件\n2.不读取文件\n请输入你的选择:", choiceBuf, 50);

    bool loadedAny = false;

    if (strcmp(choiceBuf, "1") == 0) {
        showTempMessage("您选择了：读取文件", 10, 20, 800);
        // 询问文件名，最多三次机会
        char filename[260] = {0};
        int attempts = 0;
        bool loaded = false;
        while (attempts < 3 && !loaded) {
            inputbox_getline("读取画布文件", "请输入要加载的文件名（不含路径，默认为 .png）：", filename, 260);
            if (filename[0] == '\0') {
                // 用户取消或输入为空
                showTempMessage("未输入文件名，取消加载", 10, 40, 800);
                break;
            }
            if (loadCanvasFromFile(filename)) {
                loaded = true;
                loadedAny = true;
                // 保存快照以便之后能撤回到加载状态
                saveSnapshot();
                char msg[300];
                sprintf_s(msg, sizeof(msg), "已成功加载：%s", filename);
                showTempMessage(msg, 10, 40, 1000);
                break;
            } else {
                char msg[300];
                sprintf_s(msg, sizeof(msg), "加载失败：%s", filename);
                showTempMessage(msg, 10, 40, 800);
                attempts++;
                if (attempts < 3) {
                    char retry[100];
                    sprintf_s(retry, sizeof(retry), "请重试（%d 次剩余）", 3 - attempts);
                    showTempMessage(retry, 10, 60, 800);
                } else {
                    showTempMessage("多次尝试失败，已放弃加载", 10, 60, 1000);
                }
            }
        }
    }
    else if (strcmp(choiceBuf, "2") == 0) {
        showTempMessage("您选择了：不读取文件", 10, 20, 800);
    }
    else {
        showTempMessage("输入错误，程序将继续启动但不读取文件", 10, 20, 1000);
    }

    // 仅当没有加载任何画布时清空屏幕
    if (!loadedAny) {
        cleardevice();
    }
}