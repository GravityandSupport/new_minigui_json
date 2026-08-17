#pragma once

#include "../minigui_common.h"

#ifndef __cplusplus

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/rkfb.h>

#endif

#ifdef __cplusplus

#include "base_window.hpp"
#include "desktop_window.hpp"
#include "api_common.hpp"

/// @brief 暂时改变字体大小
/// @param hdc 
/// @param call 
void notChangeFontSize(HDC hdc, std::function<void(void)> call);

/// @brief 暂时改变字体颜色
/// @param hdc 
/// @param call 
void notChangeFontColor(HDC hdc, std::function<void(void)> call);

/// @brief  不改变原来的文件大小和颜色
/// @param hdc 
/// @param call 
void notChangeFontSizeColor(HDC hdc, std::function<void(void)> call);


// 序列化初始化控件
template<typename T>
void initWidget(const std::vector<T*> &widgets, const std::function<void(T*, int)> &call){
	for(size_t i=0; i<widgets.size(); i++){
		call(widgets[i], i);
	}
}


#endif

#ifdef __cplusplus
extern "C" { 
#endif

// 100ms 定时�?
#define __1000ms_Timer__ 10

// 1s 定时�?
#define __1s_Timer__ 100

// 双击检测定时器
#define __double_click_timer__ 0xae

#define __command_update__	0xfeaa // 可能会冲突

// 创建窗口
void windowCreate(const char* window_name, HWND hParent);
// 关闭窗口
void windowClose(const char* window_name);
// 判断窗口是否打开
bool windowIsOpen(const char* window_name);

void desktopWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam); // 执行所有桌面窗口的处理函数
void desktopWinProcPaint(HWND hWnd, HDC hdc); // 执行所有桌面窗口的处理函数的 MSG_PAINT

void windowStatus(const char* window_name, bool statuss);


#ifdef __cplusplus
}  
#endif

