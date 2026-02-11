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

#endif

#ifdef __cplusplus
extern "C" { 
#endif

// 100ms 定时�?
#define __1000ms_Timer__ 10

// 1s 定时�?
#define __1s_Timer__ 100


// 创建窗口
void windowCreate(const char* window_name, HWND hParent);
// 关闭窗口
void windowClose(const char* window_name);

#ifdef __cplusplus
}  
#endif

