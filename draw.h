// draw.h
#pragma once

// 进入自由绘图主循环
void draw();

// 全局颜色数组和当前颜色
extern int colors[];
extern int currentColor;

// 保存/加载画布接口（在 save.cpp 中实现）
void saveCanvas();
void saveCanvasAs();
bool loadCanvasWithPrompt();
const char* getCurrentCanvasFilename();