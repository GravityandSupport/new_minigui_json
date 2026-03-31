#pragma once

#include "common.hpp"

namespace ui{
class KeyWindowWidget : public Widget{
private:
	bool m_isFocusGained = false; // 是否获取窗口的输入焦点
	bool m_isCanFocus = false;
public:

    KeyWindowWidget(const std::string& str):Widget(str){

    }
    virtual ~KeyWindowWidget() = default;

	virtual bool isFocusGained() { return m_isFocusGained; } // 是否获取窗口的输入焦点

	virtual void setCanFocus(bool _canFocus){ m_isCanFocus = _canFocus;} // 设置这个控件是否允许获得焦点
	
	virtual bool canFocus() const { // 这个控件是否允许获得焦点
        return m_isCanFocus;  // 默认允许，子类可重写
    }
	
    virtual void onFocusLost(){ // 丢失焦点
		LOG_DEBUG("焦点", name, "丢失焦点");

		m_isFocusGained = false;
		updateDirtyArea(); // 更新控件
	}
    virtual void onFocusGained() { // 获取焦点
		LOG_DEBUG("焦点", name, "获取焦点");

		m_isFocusGained = true;
		updateDirtyArea(); // 更新控件
	}
    
};    

class KeyWidget : public Widget {
public:
	std::pair<int, int> current_focus = {-1, -1};					   // {row, col}，-1 表示无焦点

	// 二维是横向，一维是纵向
	std::vector<std::vector<KeyWindowWidget*>> key_layout; // 按键布局


	virtual void registerWidget(KeyWindowWidget &widget) ;
	virtual void registerWidget(size_t row, KeyWindowWidget &widget);

	bool setFocus(size_t row, size_t col); // 设置当前焦点到指定位置
	bool setFocus(Widget* widget); // 设置当前焦点到指定控件（通过指针）
	bool trySetFocus(int row, int col); // 尝试设置焦点，如果目标 widget 不可焦点则返回 false
	std::pair<int, int> getFocus() const { return current_focus; } // 获取当前焦点位置

	bool hasFocus() const { return current_focus.first >= 0 && current_focus.second >= 0; } // 判断是否有焦点
	KeyWindowWidget* getCurrentWidget() const; // 获取当前焦点控件（返回 nullptr 表示无焦点）

	void clearFocus(); // 清除焦点
	bool focusFirst(); // 移动到第一行第一个控件
	bool focusLast();     // 移动到最后一个控件

	bool focusUp();// 向上移动焦点
    bool focusDown();// 向下移动焦点
    bool focusLeft();// 向左移动焦点
    bool focusRight();// 向右移动焦点
	// 此函数负责焦点的上下左右移动控制，子类可以重写此函数改变上下左右移动的按键消息
	// 如果该函数处理了消息会返回true，否则返回 false
    virtual bool focusMoveControl(WPARAM wParam, LPARAM lParam); 
	size_t getRowCount() const { return key_layout.size(); } // 获取行数
	size_t getColumnCount(size_t row) const; // 获取某行的列数

	std::pair<int, int> findWidgetPosition(Widget* widget) const; // 查找控件在布局中的位置
	
	// 版本1：传递给当前焦点控件（如果存在）
    bool sendMessageToCurrent(void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam);
	
	KeyWidget(const std::string& str):Widget(str){}
	virtual ~KeyWidget() = default;
};

template <typename T>
class __KeyWindow : public T {
public:	
	std::pair<int, int> current_focus = {-1, -1};					   // {row, col}，-1 表示无焦点
	
     __KeyWindow(const std::string& str):T(str){ }
    virtual ~__KeyWindow() = default;
	
	// 二维是横向，一维是纵向
	std::vector<std::vector<KeyWindowWidget*>> key_layout; // 按键布局
//	using BaseWindow::registerWidget;
	virtual void registerWidget(const std::string &name, KeyWindowWidget &widget) ; // 默认是添加一行
	virtual void registerWidget(const std::string &name, size_t row, KeyWindowWidget &widget); // 往一行后面追加控件，该行不存在会自动生成一行
//	virtual void registerWidget(const std::string &name, size_t row, size_t col, KeyWindowWidget &widget); // 往固定位置添加控件，如果该位置原本没有控件会抛异常，注意捕获

	// 获取指定行的引用（用于直接操作）
	std::vector<KeyWindowWidget*>& getRow(size_t row); // 不存在会抛错误，注意捕获
	KeyWindowWidget* getRow(size_t row, size_t col) const;// 获取指定控件指针，不存在返回空，要判空

	bool setFocus(size_t row, size_t col); // 设置当前焦点到指定位置
	bool setFocus(const std::string& name); // 设置当前焦点到指定控件（通过名称）
	bool setFocus(Widget* widget); // 设置当前焦点到指定控件（通过指针）
	bool trySetFocus(int row, int col); // 尝试设置焦点，如果目标 widget 不可焦点则返回 false
	std::pair<int, int> getFocus() const { return current_focus; } // 获取当前焦点位置

	bool hasFocus() const { return current_focus.first >= 0 && current_focus.second >= 0; } // 判断是否有焦点
	KeyWindowWidget* getCurrentWidget() const; // 获取当前焦点控件（返回 nullptr 表示无焦点）
	void clearFocus(); // 清除焦点
	bool focusFirst(); // 移动到第一行第一个控件
	bool focusLast();     // 移动到最后一个控件
//	bool focusLeftMost();// 移动到当前行最左边
//	bool focusRightMost();// 移动到当前行最右边
	
	bool focusUp();// 向上移动焦点
    bool focusDown();// 向下移动焦点
    bool focusLeft();// 向左移动焦点
    bool focusRight();// 向右移动焦点
	// 此函数负责焦点的上下左右移动控制，子类可以重写此函数改变上下左右移动的按键消息
	// 如果该函数处理了消息会返回true，否则返回 false
    virtual bool focusMoveControl(WPARAM wParam, LPARAM lParam); 
	size_t getRowCount() const { return key_layout.size(); } // 获取行数
	size_t getColumnCount(size_t row) const; // 获取某行的列数
	
	// 查找控件在布局中的位置
    std::pair<int, int> findWidgetPosition(Widget* widget) const;
    std::pair<int, int> findWidgetPosition(const std::string& name) const;

	// 父类窗口按键处理函数只负责上下左右移动，不负责焦点控件消息转发工作，这是为了提取窗口共性，减小各个窗口之间的耦合性
//	virtual void msg_keyup(WPARAM wParam, LPARAM lParam) override;
//    virtual void msg_keydown(WPARAM wParam, LPARAM lParam) override;
//	virtual void msg_key_long_press(WPARAM wParam, LPARAM lParam) override;
//	virtual void msg_keyup_long(WPARAM wParam, LPARAM lParam) override;

	// ===== 消息传递函数 =====（暂时不可用，勿用，有未知问题） ====== (修复可用，通过类成员函数指针来实现转发功能)
    
    // 版本1：传递给当前焦点控件（如果存在）
    bool sendMessageToCurrent(void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam);

	// 版本2：传递给指定位置的控件（如果存在）
    bool sendMessageToWidget(size_t row, size_t col, 
    									void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam);
    
    // 版本3：传递给指定名称的控件（如果存在）
    bool sendMessageByName(const std::string& name, 
    									void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam);
    
    // 版本4：传递给所有控件
    void sendMessageToAll(void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam);
};

using KeyWindow = __KeyWindow<BaseWindow>;
using KeyWindowDesktop = __KeyWindow<DesktopWindow>;

#include "key_window.inl"

}


