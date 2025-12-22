#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include "query.h"
#include "graphic function.h"
#include "ui.h"
#include "draw.h"

// 在临时画布上显示查询结果，然后在 15 秒后恢复原画布
void queryShapes() {
    // 询问用户查询条件（简单实现：按类型或边数查询）
    char cond[128] = {0};
    inputbox_getline("查询图形", "输入查询条件：\n1: 查询所有多边形\n2: 查询所有圆\n3: 查询特定边数的多边形（例如 5）", cond, 128);

    // 颜色名称映射（与 colors[] 顺序对应）
    const char* colorNames[] = { "白色", "红色", "绿色", "蓝色", "黄色", "青色", "紫色" };

    // 如果用户选择了选项 3，则再询问具体边数
    int querySides = -1;
    if (strcmp(cond, "3") == 0) {
        char sidesBuf[32] = {0};
        inputbox_getline("查询多边形边数", "请输入要查询的多边形边数（例如 3,4,5），输入 0 取消：", sidesBuf, 32);
        querySides = atoi(sidesBuf);
        if (querySides <= 0) {
            // 取消或无效，返回
            showTempMessage("已取消查询", 0, 0, 800);
            draw();
        }
    }

    // 复制当前画面到临时图像以便恢复
    PIMAGE backup = newimage(getwidth(), getheight());
    getimage(backup, 0, 0, getwidth(), getheight());

    // 创建一个空白画布显示查询结果（不要破坏用户原画布）
    PIMAGE temp = newimage(getwidth(), getheight());
    // 将绘图目标设置为 temp 并清空为白色背景
    settarget(temp);
    setbkcolor(BLACK);
    setfillcolor(WHITE);
    cleardevice();

    // 基本布局参数
    int marginX = 10;
    int marginY = 10;
    int colX = marginX;
    int y = marginY;
    int lineHeight = 18;
    int colWidth = getwidth() / 2 - marginX * 2;

    // 标题
    char title[128];
    if (strcmp(cond, "1") == 0) sprintf_s(title, sizeof(title), "查询结果：所有多边形");
    else if (strcmp(cond, "2") == 0) sprintf_s(title, sizeof(title), "查询结果：所有圆");
    else if (strcmp(cond, "3") == 0) sprintf_s(title, sizeof(title), "查询结果：%d 边多边形", querySides);
    else sprintf_s(title, sizeof(title), "查询结果：边数查询 %s", cond);

    outtextxy(colX, y, title);
    y += lineHeight + 4;
    outtextxy(colX, y, "--------------------------------------------------");
    y += lineHeight;

    // 输出匹配项的函数：当列用完时换列
    auto printLine = [&](const char* text) {
        // 如果超出当前列高度，移动到下一列或停止
        if (y > getheight() - 40) {
            // 移到右列或回到左并停止
            if (colX == marginX) {
                colX = getwidth() / 2 + marginX;
                y = marginY;
            } else {
                // 两列都满，提示更多并停止输出
                outtextxy(colX, y, "(结果过多，省略)");
                y += lineHeight;
                return false;
            }
        }
        outtextxy(colX, y, text);
        y += lineHeight;
        return true;
    };

    // 处理不同查询类型
    if (strcmp(cond, "1") == 0) {
        int count = 0;
        for (int idx = 0; idx < 20; ++idx) {
            if (graph[idx].type == SHAPE_POLYGON && graph[idx].side_num >= 3) {
                count++;
                char buf[512];
                const char* cname = "未知";
                if (graph[idx].colorIndex >= 0 && graph[idx].colorIndex < 7) cname = colorNames[graph[idx].colorIndex];
                sprintf_s(buf, sizeof(buf), "[%d]号 多边形 - 边数: %d  填充: %s  颜色: %s", idx+1, graph[idx].side_num, graph[idx].fill?"是":"否", cname);
                if (!printLine(buf)) break;

                // 顶点一行显示
                char vbuf[512]; vbuf[0] = '\0';
                int pos = 0;
                for (int t = 0; t < graph[idx].side_num*2 && t < 10; t+=2) {
                    pos += snprintf(vbuf+pos, sizeof(vbuf)-pos, "(%d,%d) ", graph[idx].fix_point[t], graph[idx].fix_point[t+1]);
                    if (pos > (int)sizeof(vbuf)-50) break;
                }
                if (!printLine(vbuf)) break;
                printLine(" ");
            }
        }
        char summary[80]; sprintf_s(summary, sizeof(summary), "共找到 %d 个多边形", count); printLine(summary);
    } else if (strcmp(cond, "2") == 0) {
        int count = 0;
        for (int idx = 0; idx < 20; ++idx) {
            if (graph[idx].type == SHAPE_CIRCLE) {
                count++;
                const char* cname = "未知";
                if (graph[idx].colorIndex >= 0 && graph[idx].colorIndex < 7) cname = colorNames[graph[idx].colorIndex];
                char buf[256];
                sprintf_s(buf, sizeof(buf), "[%d]号 圆 - 填充: %s  颜色: %s  中心: (%d,%d)  半径: %d", idx+1, graph[idx].fill?"是":"否", cname, graph[idx].fix_point[0], graph[idx].fix_point[1], graph[idx].fix_point[2]);
                if (!printLine(buf)) break;
            }
        }
        char summary[80]; sprintf_s(summary, sizeof(summary), "共找到 %d 个圆", count); printLine(summary);
    } else if (strcmp(cond, "3") == 0) {
        int count = 0;
        for (int idx = 0; idx < 20; ++idx) {
            if (graph[idx].type == SHAPE_POLYGON && graph[idx].side_num == querySides) {
                count++;
                const char* cname = "未知";
                if (graph[idx].colorIndex >= 0 && graph[idx].colorIndex < 7) cname = colorNames[graph[idx].colorIndex];
                char buf[256];
                sprintf_s(buf, sizeof(buf), "[%d]号 多边形 - 填充: %s  颜色: %s", idx+1, graph[idx].fill?"是":"否", cname);
                if (!printLine(buf)) break;
                            
                // 顶点
                char vbuf[512]; vbuf[0]='\0'; int pos=0;
                for (int t=0; t<graph[idx].side_num*2 && t<10; t+=2) {
                    pos += snprintf(vbuf+pos, sizeof(vbuf)-pos, "(%d,%d) ", graph[idx].fix_point[t], graph[idx].fix_point[t+1]);
                    if (pos > (int)sizeof(vbuf)-50) break;
                }
                if (!printLine(vbuf)) break;
                printLine(" ");
            }
        }
        char summary[80]; sprintf_s(summary, sizeof(summary), "共找到 %d 个 %d 边的多边形", count, querySides); printLine(summary);
    } else {
        printLine("无效查询条件");
    }

    // 恢复目标并把 temp 显示到屏幕
    settarget(NULL);
    putimage(0,0,temp);

    // 显示 15 秒钟，然后恢复原画布（不覆盖用户数据）
    delay_ms(15000);
    putimage(0,0,backup);

    // 清理
    delimage(temp);
    delimage(backup);
}
