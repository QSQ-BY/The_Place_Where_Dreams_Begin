#pragma once

// 快照与撤回/重做接口
void saveSnapshot();
void undoLast();
void clearSnapshots();
void redoLast();
