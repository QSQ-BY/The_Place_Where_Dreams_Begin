#pragma once
#include <stdint.h>

void graphic(void);

typedef enum { SHAPE_POLYGON, SHAPE_CIRCLE } ShapeType;

/**
 * diagram - 存储单个图形的元数据
 *
 * 字段说明：
 *  - type: 图形类型，多边形 (SHAPE_POLYGON) 或 圆 (SHAPE_CIRCLE)
 *  - fill: 是否填充（true = 填充，false = 仅绘制轮廓）
 *  - side_num: 多边形的边数；对于圆，此字段可为 0
 *  - fix_point: 顶点/参数数组（固定长度 10）：

 *      - 多边形：按照 x0,y0,x1,y1,... 的顺序存储最多 5 个顶点（因此数组长度为 10）
 *      - 圆：使用前 3 个元素表示 {cx, cy, radius}
 *    注意：当前实现为简化起见使用定长数组，超出部分会被截断。
 *  - colorIndex: 颜色索引（0..6）对应工程中 colors 数组（WHITE, RED, ...）
 */
typedef struct diagram {
    ShapeType type;   // 图形类型：多边形或圆
    bool fill;        // 是否填充
    int side_num;     // 边数（多边形有效，圆为 0）
    int fix_point[10]; // 顶点坐标数组；多边形为 x0,y0,x1,y1..., 圆为 [0]=cx [1]=cy [2]=radius
    int colorIndex;   // 颜色索引（0..6 对应 colors）
} diagram;

// 图形数组的最大长度（程序中固定为 20），用于保存/加载与索引管理
#define MAX_GRAPH 20

extern diagram graph[MAX_GRAPH];
extern int i; // 下一个要写入的 graph 索引，写入后会取模 MAX_GRAPH 循环覆盖旧项
extern int graphCount; // 当前有效图形数量（<= MAX_GRAPH）