#pragma once 

#include <iostream>
#include <string>
#include <unistd.h>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <fcntl.h>
#include <vector>
#include <functional>
#include <tuple>
#include <array>
#include <algorithm>
#include <termios.h> //串口配置需要的头文件
#include <stdexcept> //错误头文文件
#include <thread>
#include <array>
#include <atomic>
#include <utility> // std::pair
#include <numeric>
#include <cmath>
#include <iomanip>
#include <queue>
#include <map>
#include <semaphore.h>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <pthread.h>
#include <sys/prctl.h>

#include "json.hpp"

#include "outLog.hpp"

#ifdef __cplusplus
extern "C" { 
#endif

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/rkfb.h>

#ifdef __cplusplus
}
#endif

class BaseAttr {
protected:
public:
	bool is_can_update = true; // 是否可以更新脏区域，仅用于 updateDirtyArea() 函数
	
    // 窗口/控件 名字
    std::string name;

    RECT rc; // 控件大小

    bool status; // 控件状态，是否显示

    // 窗口句柄
    HWND hWnd;

    nlohmann::json json;

    void parseJson(){
        name = json.value("name", std::string("xxxx"));
        status = json.value("status", true);
        rc.left = json.value("x", 0);  // 如果不存在就默认 0
        rc.top = json.value("y", 0);
        rc.right = json.value("w", 0) + rc.left;
        rc.bottom = json.value("h", 0) + rc.top;
    }

	// 计算包围盒
	static RECT computBoundBox(RECT &rc1, RECT &rc2) {
		RECT ret_rc = {
			std::min(rc1.left, rc2.left), std::min(rc1.top, rc2.top),
			std::max(rc1.right, rc2.right), std::max(rc1.bottom, rc2.bottom)
		};
		return ret_rc;
	}
	
    /// @brief 更新脏区域
    virtual void updateDirtyArea(){
    	if(is_can_update==false) return;
        InvalidateRect(hWnd, &rc, true);
    }

	/// @brief 更新脏区域
	// 在 unifiedUpdate 函数调用期间， widgets 列表内的部件会被禁止刷新，待函数调用结束时再统一刷新
	static void unifiedUpdate(HWND hWnd, const std::initializer_list<BaseAttr*> &widgets, const std::function<void(void)> &call){
		bool first = true;
		RECT prev_rc;
		if (widgets.size() == 0) {LOG_WARN("空列表", "widgets 是空列表"); return;}
		for (BaseAttr *w : widgets){
			if(hWnd != w->hWnd) {LOG_WARNC("控件父窗口不符", "%s 不是此窗口的子部件", w->name.c_str()); continue;}
			w->is_can_update = false; // 禁止刷新
			if(first==true){
				first = false;
				prev_rc = w->rc;
			}else{
				prev_rc = computBoundBox(prev_rc, w->rc);
			}
		}
		call();
		InvalidateRect(hWnd, &prev_rc, true);
		for (BaseAttr *w : widgets){
			w->is_can_update = true; // 允许刷新
		}
	}

	static void unifiedUpdate(BaseAttr* base_attr, const std::initializer_list<BaseAttr*> &widgets, const std::function<void(void)> &call){
		if(!base_attr) {LOG_ERROR("空指针", "base_attr 是空指针"); return;}
		unifiedUpdate(base_attr->hWnd, widgets, call);
	}

	static void unifiedUpdate(BaseAttr& base_attr, const std::initializer_list<BaseAttr*> &widgets, const std::function<void(void)> &call){
		unifiedUpdate(base_attr.hWnd, widgets, call);
	}

    virtual void msg_init(WPARAM wParam, LPARAM lParam){};
    virtual void msg_command(WPARAM wParam, LPARAM lParam){};
    virtual void msg_lbutton_down(int x, int y){};
    virtual void msg_lbutton_up(int x, int y){};
    virtual void msg_mousemove(int x, int y){};
    virtual void msg_keyup(WPARAM wParam, LPARAM lParam){};
    virtual void msg_keydown(WPARAM wParam, LPARAM lParam){};
	virtual void msg_key_long_press(WPARAM wParam, LPARAM lParam) {};
	virtual void msg_keyup_long(WPARAM wParam, LPARAM lParam) {};
    virtual void msg_timer(WPARAM wParam, LPARAM lParam){};
    virtual void msg_paint(HDC hdc){};
    virtual void msg_close(WPARAM wParam, LPARAM lParam){};
    virtual void msg_nc_lbutton_up(WPARAM wParam, LPARAM lParam){};
    virtual void msg_destroy(WPARAM wParam, LPARAM lParam){};


    BaseAttr(const std::string& str){
        json = nlohmann::json::parse(str);
        parseJson();
    }
    virtual ~BaseAttr() = default;
};




