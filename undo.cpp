#include <graphics.h>
#include "draw.h"

// 撤回（Undo）快照栈
#define MAX_SNAPSHOTS 10
static PIMAGE snapshotStack[MAX_SNAPSHOTS] = { 0 };
static int snapshotTop = 0; // 当前快照数

// 重做（Redo）栈
static PIMAGE redoStack[MAX_SNAPSHOTS] = { 0 };
static int redoTop = 0;

// 将当前画面保存到快照栈
void saveSnapshot() {
    // 新动作时清空 redo 栈
    for (int r = 0; r < redoTop; ++r) {
        if (redoStack[r]) delimage(redoStack[r]);
        redoStack[r] = NULL;
    }
    redoTop = 0;

    // 如果栈已满，删除最旧的快照
    if (snapshotTop == MAX_SNAPSHOTS) {
        delimage(snapshotStack[0]);
        for (int i = 1; i < MAX_SNAPSHOTS; ++i) snapshotStack[i - 1] = snapshotStack[i];
        snapshotTop = MAX_SNAPSHOTS - 1;
    }
    // 创建新图像并保存当前画面
    PIMAGE img = newimage(getwidth(), getheight());
    getimage(img, 0, 0, getwidth(), getheight());
    snapshotStack[snapshotTop++] = img;
}

// 撤回到上一个快照并显示（支持重做）
void undoLast() {
    if (snapshotTop == 0) return; // 没有可撤回的快照
    // 将当前屏幕保存到 redo 栈
    PIMAGE cur = newimage(getwidth(), getheight());
    getimage(cur, 0, 0, getwidth(), getheight());
    if (redoTop == MAX_SNAPSHOTS) {
        delimage(redoStack[0]);
        for (int i = 1; i < MAX_SNAPSHOTS; ++i) redoStack[i - 1] = redoStack[i];
        redoTop = MAX_SNAPSHOTS - 1;
    }
    redoStack[redoTop++] = cur;

    // 取出最后一个快照并显示，但不立即 delimage，而是保留在 redo 中
    PIMAGE img = snapshotStack[--snapshotTop];
    putimage(0, 0, img);
    // 将该快照移动到 redo 栈前面（作为可重做对象）
    if (redoTop == MAX_SNAPSHOTS) {
        delimage(redoStack[0]);
        for (int i = 1; i < MAX_SNAPSHOTS; ++i) redoStack[i - 1] = redoStack[i];
        redoTop = MAX_SNAPSHOTS - 1;
    }
    redoStack[redoTop++] = img;
    snapshotStack[snapshotTop] = NULL;
}

// 重做最近一次被撤回的画面
void redoLast() {
    if (redoTop == 0) return; // 无可重做项
    // 取出 redo 最后一项并显示
    PIMAGE img = redoStack[--redoTop];
    putimage(0,0,img);
    // 将其保存回 snapshot 栈作为新的状态
    if (snapshotTop == MAX_SNAPSHOTS) {
        delimage(snapshotStack[0]);
        for (int i = 1; i < MAX_SNAPSHOTS; ++i) snapshotStack[i - 1] = snapshotStack[i];
        snapshotTop = MAX_SNAPSHOTS - 1;
    }
    snapshotStack[snapshotTop++] = img;
}

// 清空所有快照并释放资源
void clearSnapshots() {
    for (int i = 0; i < snapshotTop; ++i) {
        if (snapshotStack[i]) delimage(snapshotStack[i]);
        snapshotStack[i] = NULL;
    }
    snapshotTop = 0;
    for (int i = 0; i < redoTop; ++i) {
        if (redoStack[i]) delimage(redoStack[i]);
        redoStack[i] = NULL;
    }
    redoTop = 0;
}

// 导出函数声明（供其他文件调用）
void saveSnapshot();
void undoLast();
void clearSnapshots();
void redoLast();
