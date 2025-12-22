#include <graphics.h>
#include <string.h>
#include "ui.h"

// 在画布指定位置显示临时消息，ms 毫秒后清除该区域（非破坏性，支持多行）
void showTempMessage(const char* msg, int x, int y, int ms) {
    if (!msg) return;
    // 拆分为多行
    const char* p = msg;
    int lineCount = 0;
    const char* lines[32]; // 支持最多 32 行
    char buf[1024];
    strncpy(buf, msg, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    char* token = strtok(buf, "\n");
    while (token && lineCount < 32) {
        lines[lineCount++] = token;
        token = strtok(NULL, "\n");
    }
    if (lineCount == 0) return;

    int padding = 4;
    int maxW = 0;
    int lh = textheight("A");
    for (int i = 0; i < lineCount; ++i) {
        int w = textwidth(lines[i]);
        if (w > maxW) maxW = w;
    }
    int totalW = maxW + padding * 2;
    int totalH = lh * lineCount + padding * 2;

    // 防止越界
    if (x + totalW > getwidth()) totalW = getwidth() - x;
    if (y + totalH > getheight()) totalH = getheight() - y;
    if (totalW <= 0 || totalH <= 0) return;

    // 保存被覆盖区域
    PIMAGE backup = newimage(totalW, totalH);
    if (backup) {
        getimage(backup, x, y, totalW, totalH);
    }

    // 绘制背景和文字
    color_t bk = getbkcolor();
    setfillcolor(bk);
    bar(x, y, x + totalW, y + totalH);
    for (int i = 0; i < lineCount; ++i) {
        outtextxy(x + padding, y + padding + i * lh, lines[i]);
    }

    // 等待
    delay_ms(ms);

    // 恢复原始内容
    if (backup) {
        putimage(x, y, backup);
        delimage(backup);
    } else {
        setfillcolor(bk);
        bar(x, y, x + totalW, y + totalH);
    }
}
