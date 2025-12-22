#include <stdio.h>
#include <graphics.h>
#include <stdbool.h>
#include <string.h>
#include "draw.h"
#include "undo.h"
#include "graphic function.h"

// 存储图形数组已在头文件声明

// 全局数据：
//  - graph: 存储绘制过的图形元数据，固定长度为 MAX_GRAPH。每个元素为一个 diagram 结构，
//           包含图形类型、是否填充、边数、坐标/参数和颜色索引。
//  - i: 下一个写入 graph 的索引位置。写入后 i 增 1 并对 MAX_GRAPH 取模以循环覆盖旧记录。
//  - graphCount: 当前有效图形数量（<= MAX_GRAPH），用于保存/加载时只写入实际项数。
// 注意：graph 为程序级缓存，用于查询与重绘功能；并非持久化存储，持久化由 saveShapesToFile/loadShapesFromFile 处理。

diagram graph[MAX_GRAPH];  // 存储最多 MAX_GRAPH 个图形，循环使用
int i = 0;           // 当前图形插入索引，插入后 i = (i+1) % MAX_GRAPH
int graphCount = 0;  // 当前有效图形数量，<= MAX_GRAPH

/**
 * graphic
 * ----
 * 交互式绘制图形的主流程函数。该函数通过一系列弹窗与用户交互，完成多边形或圆的
 * 输入、验证、绘制与记录。
 *
 * 行为与约定：
 *  - 使用 inputbox_getline 与用户交互，任何一次输入无效或用户取消将导致当前绘图
 *    操作取消并从函数返回（由调用者决定后续控制流）。
 *  - 在实际绘制到画布之前会调用 saveSnapshot() 保存画布快照，以便支持撤回（undo）操作。
 *  - 绘制完成后，会把该图形的元数据写入全局 graph 数组：圆使用 fix_point[0]=cx, fix_point[1]=cy, fix_point[2]=r；
 *    多边形按 x0,y0,x1,y1,... 的顺序存入 fix_point（最多存储 10 个值，即最多 5 个顶点），超出部分将被截断。
 *  - i 指向下一写入位置，写入后会 `(i + 1) % MAX_GRAPH` 循环覆盖旧记录，确保数组不会越界。
 *
 * 输入校验说明：
 *  - 坐标与半径以整数读取，半径必须为正整数。
 *  - 多边形边数限定为 3..9，顶点坐标逐个输入并做整数字符串校验；若任意一项无效，当前操作会取消。
 *  - 颜色通过索引选择，要求 1..7 范围（对应 colors 数组）；外部保证 colors 的顺序和索引一致。
 *
 * 返回值：
 *  - 无返回值。函数结束时会返回调用者（通常为 menu() 或 draw() 主循环），由调用者决定是否重新绘制或退出。
 */
void graphic(void) {
    while (1) {
        // 选择绘制类型：多边形或圆
        char typeBuf[50] = {0};
        inputbox_getline("图形绘制", "选择图形类型：\n1.多边形\n2.圆\n无效输入将退出", typeBuf, 50);
        if (typeBuf[0] != '1' && typeBuf[0] != '2') {
            // 用户输入无效或取消，返回到调用者，不在此处直接跳转到 draw()
            return;
        }

        if (typeBuf[0] == '2') {
            // ===========================
            // 绘制圆流程
            // ===========================
            char centerBuf[50] = {0};
            inputbox_getline("绘制圆", "请输入圆心坐标 x y（以空格分隔，例如: 100 120）:\n无效输入将退出", centerBuf, 50);
            int cx, cy;
            if (sscanf(centerBuf, "%d %d", &cx, &cy) != 2) { return; }

            char radiusBuf[50] = {0};
            inputbox_getline("绘制圆", "请输入半径（整数）:\n无效输入将退出", radiusBuf, 50);
            int r = atoi(radiusBuf);
            if (r <= 0) { return; }

            char fillBuf[50] = {0};
            inputbox_getline("绘制圆", "是否填充：1.填充 2.不填充\n无效输入将退出", fillBuf, 50);
            if (fillBuf[0] != '1' && fillBuf[0] != '2') { return; }
            bool fill = (fillBuf[0] == '1');

            char colorBuf[50] = {0};
            inputbox_getline("绘制圆", "请选择颜色：\n1.白色 2.红色 3.绿色 4.蓝色\n5.黄色 6.青色 7.紫色\n无效输入将退出", colorBuf, 50);
            if (colorBuf[0] <= '0' || colorBuf[0] > '7') { return; }
            int colorIndex = atoi(colorBuf) - 1;

            // 保存快照并绘制（保证可撤回）
            saveSnapshot();
            if (fill) {
                setfillcolor(colors[colorIndex]);
                fillellipse(cx, cy, r, r);
            } else {
                setcolor(colors[colorIndex]);
                circle(cx, cy, r);
            }

            // 存储到 graph
            // 说明：圆使用 fix_point[0]=cx, fix_point[1]=cy, fix_point[2]=radius
            graph[i].type = SHAPE_CIRCLE;
            graph[i].fill = fill;
            graph[i].side_num = 0; // 圆的 side_num 设置为 0
            graph[i].fix_point[0] = cx;
            graph[i].fix_point[1] = cy;
            graph[i].fix_point[2] = r;
            graph[i].colorIndex = colorIndex;

            // 更新索引（循环覆盖）
            i = (i + 1) % MAX_GRAPH;
            if (graphCount < MAX_GRAPH) graphCount++;

            // 询问下一步：返回画板或继续绘图
            char nextBuf[50] = {0};
            inputbox_getline("绘制圆", "请输入选择：\n1.重返画板\n2.继续绘图\n无效输入将退出", nextBuf, 50);
            if (nextBuf[0] == '1') { return; }
            else if (nextBuf[0] == '2') continue;
            else { return; }
        }
        else {
            // ===========================
            // 绘制多边形流程
            // ===========================
            char sideBuf[50] = {0};
            inputbox_getline("图形绘制", "请输入图形的边数(最少为3最多为9)\n无效输入将退出", sideBuf, 50);
            if (*sideBuf <= '2' || *sideBuf > '9') { return; }
            int sides = atoi(sideBuf);

            // 输入是否填充
            char* strBuff2 = (char*)malloc(sizeof(char) * 50);
            inputbox_getline("图形绘制", "请输入是否填充（1填充2不填充）\n无效输入将退出", strBuff2, 50);
            if (*strBuff2 <= '0' || *strBuff2 > '2') { free(strBuff2); return; }
            bool fill = (atoi(strBuff2) == 1);
            free(strBuff2);

            // 输入顶点坐标（按 x y 顺序逐个输入）
            int coords[18]; // 最多9边 -> 18个坐标
            memset(coords, 0, sizeof(coords));
            bool inputError = false;
            for (int j = 0; j < 2 * sides; j++) {
                char* strBuff3 = (char*)malloc(sizeof(char) * 50);
                inputbox_getline("图形绘制", "请依次输入顶点的xy坐标，输入一个整数按一次回车\n无效输入将退出", strBuff3, 50);

                // 验证输入是否为有效整数（允许正负号）
                bool isValidInteger = true;
                int k = 0;
                if (strBuff3[k] == '+' || strBuff3[k] == '-') k++;

                for (; strBuff3[k] != '\0' && strBuff3[k] != '\n'; k++) {
                    if (strBuff3[k] < '0' || strBuff3[k] > '9') { isValidInteger = false; break; }
                }
                if (k == 0 || (k == 1 && (strBuff3[0] == '+' || strBuff3[0] == '-'))) isValidInteger = false;

                if (isValidInteger) {
                    coords[j] = atoi(strBuff3);
                } else { free(strBuff3); inputError = true; break; }
                free(strBuff3);
            }
            if (inputError) { return; }

            // 选择颜色
            char* colorBuf = (char*)malloc(sizeof(char) * 50);
            inputbox_getline("图形绘制", "请输入颜色:\n1.白色 2.红色 3.绿色 4.蓝色\n5.黄色 6.青色 7.紫色\n无效输入将退出", colorBuf, 50);
            if (*colorBuf <= '0' || *colorBuf > '7') { free(colorBuf); return; }
            int colorIndex = atoi(colorBuf) - 1;
            free(colorBuf);

            // 在绘制之前保存快照以支持撤回
            saveSnapshot();

            // 绘制多边形
            if (fill) {
                setfillcolor(colors[colorIndex]);
                fillpoly(sides, coords);
            } else {
                setcolor(colors[colorIndex]);
                // drawpoly 有时需要最后一点等于第一点以闭合，如果缺失会出现一条边缺失
                // 因此我们手动绘制线段并确保闭合
                for (int p = 0; p < sides; ++p) {
                    int x1 = coords[2*p];
                    int y1 = coords[2*p+1];
                    int nx = coords[2*((p+1)%sides)];
                    int ny = coords[2*((p+1)%sides)+1];
                    moveto(x1, y1);
                    lineto(nx, ny);
                }
            }

            // 存储到 graph
            graph[i].type = SHAPE_POLYGON;
            graph[i].fill = fill;
            graph[i].side_num = sides;
            graph[i].colorIndex = colorIndex;
            // 只存储最多 10 个坐标（即最多 5 个顶点），超出会被截断
            for (int t = 0; t < 2*sides && t < 10; ++t) graph[i].fix_point[t] = coords[t];

            // 更新索引（循环覆盖）
            i = (i + 1) % MAX_GRAPH;
            if (graphCount < MAX_GRAPH) graphCount++;

            // 选择下一步操作
            char* strBuff = (char*)malloc(sizeof(char) * 50);
            inputbox_getline("图形绘制", "请输入选择:\n1.重返画板\n2.继续绘图\n无效输入将退出", strBuff, 50);
            if (*strBuff <= '0' || *strBuff > '2') { free(strBuff); return; }
            if (atoi(strBuff) == 1) { free(strBuff); return; }
            free(strBuff);
        }
    }
}