#pragma once

// 保存当前画布到当前文件或执行另存为
void saveCanvas();
void saveCanvasAs();

// 交互式加载画布（提示输入文件名），成功返回 true
bool loadCanvasWithPrompt();

// 通过指定文件名加载画布，成功返回 true
bool loadCanvasFromFile(const char* filename);

// 获取当前画布文件名（若未设置返回 NULL）
const char* getCurrentCanvasFilename();

// 保存/加载图形结构体数组（binary 格式）
void saveShapesToFile();
void loadShapesFromFile();
