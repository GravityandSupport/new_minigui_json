#pragma once

#include "key_window.hpp"

/*
// 版本1：传递给当前焦点控件（如果存在）
template<typename Func, typename... Args>
bool KeyWindow::sendMessageToCurrent(Func&& func, Args&&... args) {
    Widget* current = getCurrentWidget();
    if (!current) {
        return false;  // 无焦点控件
    }
    
    // 完美转发参数并调用函数
    std::forward<Func>(func)(*current, std::forward<Args>(args)...);
    return true;
}


// 版本2：传递给指定位置的控件（如果存在）
template<typename Func, typename... Args>
bool KeyWindow::sendMessageToWidget(size_t row, size_t col, Func&& func, Args&&... args) {
    if (row >= key_layout.size() || col >= key_layout[row].size()) {
        return false;  // 位置越界
    }
    
    Widget* widget = key_layout[row][col];
    if (!widget) {
        return false;
    }
    
    std::forward<Func>(func)(*widget, std::forward<Args>(args)...);
    return true;
}

// 版本3：传递给指定名称的控件（如果存在）
template<typename Func, typename... Args>
bool KeyWindow::sendMessageByName(const std::string& name, Func&& func, Args&&... args) {
    Widget* widget = findWidget(name);
    if (!widget) {
        return false;  // 未找到控件
    }
    
    std::forward<Func>(func)(*widget, std::forward<Args>(args)...);
    return true;
}

// 版本4：传递给所有控件
template<typename Func, typename... Args>
void KeyWindow::sendMessageToAll(Func&& func, Args&&... args) {
    for (auto& row : key_layout) {
        for (Widget* widget : row) {
            if (widget) {
                // 每次调用都使用原始参数的副本
                std::forward<Func>(*widget, std::forward<Args>(args)...);
            }
        }
    }
}
*/