#include <graphics.h>
#include "draw.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "graphic function.h"
#include "ui.h"

static char currentFilename[260] = {0};
static char currentShapesFilename[260] = {0}; // 用于保存结构体数组的文件名

// 返回当前画布文件名（若未设置返回 NULL）
const char* getCurrentCanvasFilename() {
    return (currentFilename[0] ? currentFilename : NULL);
}

// 辅助：若文件名没有扩展名，追加 .png
static void ensure_png_ext(char* name) {
    if (strchr(name, '.') == NULL) {
        strcat(name, ".png");
    }
}

// 保存当前画布到当前文件（若未设置则调用另存为）
void saveCanvas() {
    if (currentFilename[0] == '\0') {
        saveCanvasAs();
        return;
    }
    PIMAGE img = newimage(getwidth(), getheight());
    getimage(img, 0, 0, getwidth(), getheight());
    int r = savepng(img, currentFilename, 0);
    char msg[300];
    if (r == 0) {
        sprintf(msg, "保存成功：%s", currentFilename);
        showTempMessage(msg, 10, 10, 1000);
    } else {
        sprintf(msg, "保存失败：%s", currentFilename);
        showTempMessage(msg, 10, 10, 1000);
    }
    delimage(img);
    delay_ms(1000);
}

// 另存为：提示输入文件名并保存
void saveCanvasAs() {
    char* nameBuf = (char*)malloc(260);
    nameBuf[0] = '\0';
    inputbox_getline("保存画布", "请输入文件名（不含路径，默认为 .png）:", nameBuf, 260);
    if (nameBuf[0] == '\0') {
        free(nameBuf);
        return; // 取消或为空
    }
    ensure_png_ext(nameBuf);
    // 复制到 currentFilename
    strncpy(currentFilename, nameBuf, sizeof(currentFilename)-1);
    currentFilename[sizeof(currentFilename)-1] = '\0';

    PIMAGE img = newimage(getwidth(), getheight());
    getimage(img, 0, 0, getwidth(), getheight());
    int r = savepng(img, currentFilename, 0);
    char msg[300];
    if (r == 0) {
        sprintf(msg, "已保存为：%s", currentFilename);
        showTempMessage(msg, 10, 10, 1000);
    } else {
        sprintf(msg, "保存失败：%s", currentFilename);
        showTempMessage(msg, 10, 10, 1000);
    }
    delimage(img);
    free(nameBuf);
    delay_ms(1000);
}

// 通过提示加载画布；成功返回 true
bool loadCanvasWithPrompt() {
    char* nameBuf = (char*)malloc(260);
    nameBuf[0] = '\0';
    inputbox_getline("加载画布", "请输入文件名（不含路径，默认为 .png）:", nameBuf, 260);
    if (nameBuf[0] == '\0') {
        free(nameBuf);
        return false; // 取消
    }
    ensure_png_ext(nameBuf);

    PIMAGE img = newimage(getwidth(), getheight());
    int ok = getimage_pngfile(img, nameBuf);
    if (ok != 0) {
        // 尝试 BMP
        ok = getimage(img, nameBuf);
    }
    if (ok == 0) {
        putimage(0, 0, img);
        // 设置当前文件名，后续保存会覆盖
        strncpy(currentFilename, nameBuf, sizeof(currentFilename)-1);
        currentFilename[sizeof(currentFilename)-1] = '\0';
        delimage(img);
        free(nameBuf);
        showTempMessage("加载成功", 10, 10, 800);
        return true;
    }
    delimage(img);
    free(nameBuf);
    showTempMessage("未找到文件或加载失败", 10, 10, 1000);
    return false;
}

// 通过指定文件名加载；成功返回 true
bool loadCanvasFromFile(const char* filename) {
    if (filename == NULL || filename[0] == '\0') return false;
    char nameBuf[260];
    strncpy(nameBuf, filename, sizeof(nameBuf)-1);
    nameBuf[sizeof(nameBuf)-1] = '\0';
    // 若未提供扩展名，先尝试 .png 再尝试 .bmp
    bool hasExt = (strchr(nameBuf, '.') != NULL);

    PIMAGE img = newimage(getwidth(), getheight());
    int ok = -1;
    if (!hasExt) {
        char tryname[260];
        strncpy(tryname, nameBuf, sizeof(tryname)-1);
        tryname[sizeof(tryname)-1] = '\0';
        strcat(tryname, ".png");
        ok = getimage_pngfile(img, tryname);
        if (ok == 0) strncpy(nameBuf, tryname, sizeof(nameBuf)-1);
        else {
            strncpy(tryname, nameBuf, sizeof(tryname)-1);
            strcat(tryname, ".bmp");
            ok = getimage(img, tryname);
            if (ok == 0) strncpy(nameBuf, tryname, sizeof(nameBuf)-1);
        }
    } else {
        // 用户提供了扩展名
        ok = getimage_pngfile(img, nameBuf);
        if (ok != 0) ok = getimage(img, nameBuf);
    }

    if (ok == 0) {
        putimage(0,0,img);
        // 设置当前文件名，后续保存会覆盖
        strncpy(currentFilename, nameBuf, sizeof(currentFilename)-1);
        currentFilename[sizeof(currentFilename)-1] = '\0';
        delimage(img);
        showTempMessage("加载成功", 10, 10, 800);
        return true;
    }
    delimage(img);
    showTempMessage("加载失败", 10, 10, 1000);
    return false;
}

// 使用二进制文件保存 graph 数组，文件名与图像文件分开，默认扩展 .shp
//用户可以在日常绘图中直接加载调用已保存的图形数据，无需重新绘制，增加绘图效率

static void ensure_shp_ext(char* name) {
    if (strchr(name, '.') == NULL) {
        strcat(name, ".shp");
    }
}

// 将 graph 数组绘制到当前画布（不清除画布），只绘制前 count 项
static void drawGraphToCanvas(size_t count) {
    if (count == 0) return;
    if (count > MAX_GRAPH) count = MAX_GRAPH;

    // 保持并切换到屏幕目标，确保可见
    PIMAGE prevTarget = gettarget();
    settarget(NULL);

    for (size_t idx = 0; idx < count; ++idx) {
        
        int colorIndex = graph[idx].colorIndex;
        if (colorIndex < 0 || colorIndex >= 7) colorIndex = 0;
        if (graph[idx].type == SHAPE_CIRCLE) {
            int cx = graph[idx].fix_point[0];
            int cy = graph[idx].fix_point[1];
            int r = graph[idx].fix_point[2];
            if (r <= 0) continue; 
            if (graph[idx].fill) {
                setfillcolor(colors[colorIndex]);
                fillellipse(cx, cy, r, r);
            } else {
                setcolor(colors[colorIndex]);
                circle(cx, cy, r);
            }
        } else if (graph[idx].type == SHAPE_POLYGON) {
            int npoints = graph[idx].side_num;
            if (npoints < 3) continue;
            if (npoints > 5) npoints = 5; // fix_point 存储最多 5 个点（10 个坐标）
            int pts[10] = {0};
            for (int t = 0; t < npoints*2; ++t) pts[t] = graph[idx].fix_point[t];
            if (graph[idx].fill) {
                setfillcolor(colors[colorIndex]);
                if (npoints >= 3) fillpoly(npoints, pts);
            } else {
                setcolor(colors[colorIndex]);
                for (int p = 0; p < npoints; ++p) {
                    int x1 = pts[2*p];
                    int y1 = pts[2*p+1];
                    int nx = pts[2*((p+1)%npoints)];
                    int ny = pts[2*((p+1)%npoints)+1];
                    moveto(x1, y1);
                    lineto(nx, ny);
                }
            }
        }
    }

    // 恢复原目标
    settarget(prevTarget);
}

void saveShapesToFile() {
    char* nameBuf = (char*)malloc(260);
    nameBuf[0] = '\0';
    inputbox_getline("保存图形数据", "请输入用于保存图形数据的文件名（不含路径，默认 .shp）:", nameBuf, 260);
    if (nameBuf[0] == '\0') { free(nameBuf); return; }
    ensure_shp_ext(nameBuf);

    FILE* f = NULL;
    f = fopen(nameBuf, "wb");
    if (!f) {
        showTempMessage("打开文件失败",10,20,1000); free(nameBuf); return;
    }
    // 写入固定大小的结构体数组
    // 使用指针对文件进行写入：按项写入 graph
    size_t written = 0;
    for (int idx = 0; idx < graphCount; ++idx) {
        diagram* p = &graph[idx];
        if (fwrite(p, sizeof(diagram), 1, f) != 1) {
            showTempMessage("写入图形数据失败",10,20,1000);
            fclose(f);
            free(nameBuf);
            return;
        }
        written++;
    }
    if (written == 0) {
        showTempMessage("没有图形可保存",10,20,1000);
    } else {
        strncpy(currentShapesFilename, nameBuf, sizeof(currentShapesFilename)-1);
        currentShapesFilename[sizeof(currentShapesFilename)-1] = '\0';
        char msg[300];
        sprintf(msg, "图形数据已保存: %s (共 %zu 项)", nameBuf, written);
        showTempMessage(msg,10,20,1000);
    }
    fclose(f);
    free(nameBuf);
    delay_ms(1000);
}

void loadShapesFromFile() {
    char* nameBuf = (char*)malloc(260);
    nameBuf[0] = '\0';
    inputbox_getline("加载图形数据", "请输入要加载的图形数据文件名（不含路径，默认 .shp）:", nameBuf, 260);
    if (nameBuf[0] == '\0') { free(nameBuf); return; }
    ensure_shp_ext(nameBuf);

    FILE* f = NULL;
    f = fopen(nameBuf, "rb");
    if (!f) {
        showTempMessage("打开图形数据文件失败",10,20,1000); free(nameBuf); return;
    }
    // 使用指针对文件进行读取：按项读取到 graph 中，最多 MAX_GRAPH
    size_t readCount = 0;
    for (int idx = 0; idx < MAX_GRAPH; ++idx) {
        diagram* p = &graph[idx];
        size_t r = fread(p, sizeof(diagram), 1, f);
        if (r != 1) break;
        readCount++;
    }
    if (readCount == 0) {
        showTempMessage("读取图形数据失败或文件格式不正确",10,20,1000);
        fclose(f);
        free(nameBuf);
        return;
    }
    // 更新计数并设置当前文件名
    graphCount = (int)readCount;
    i = graphCount % MAX_GRAPH;
    strncpy(currentShapesFilename, nameBuf, sizeof(currentShapesFilename)-1);
    currentShapesFilename[sizeof(currentShapesFilename)-1] = '\0';

    char msg[300];
    sprintf(msg, "图形数据已加载: %s (共 %zu 项)", nameBuf, readCount);
    showTempMessage(msg,10,20,1000);

    // 绘制已加载的图形
    drawGraphToCanvas(readCount);

    fclose(f);
    free(nameBuf);
    delay_ms(1000);
}