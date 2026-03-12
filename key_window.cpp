#include "key_window.hpp"

void KeyWindow::registerWidget(const std::string &name, KeyWindowWidget &widget){
	BaseWindow::registerWidget(name, widget);
	key_layout.emplace_back();          // 添加空行
    key_layout.back().push_back(&widget); // 该行只包含此控件
}

void KeyWindow::registerWidget(const std::string &name, size_t row, KeyWindowWidget &widget){
	BaseWindow::registerWidget(name, widget);
	// 策略：自动填充缺失的中间行（空行）
    if (row >= key_layout.size()) {
        key_layout.resize(row + 1);
    }
    key_layout[row].push_back(&widget);
}

void KeyWindow::registerWidget(const std::string &name, size_t row, size_t col, KeyWindowWidget &widget){
	BaseWindow::registerWidget(name, widget);
	if (row >= key_layout.size()) {
        LOG_ERROR("越界", "row超过key_layout.size()，无法创建");
		return;
    }

	if (col > key_layout[row].size()) {
        LOG_ERROR("越界", "col超过key_layout[row].size()，无法创建");
		return;
    }
	key_layout[row].insert(key_layout[row].begin() + col, &widget);
}

std::vector<KeyWindowWidget*>& KeyWindow::getRow(size_t row) {
    if (row >= key_layout.size()) {
        throw std::out_of_range("Row index out of range");
    }
    return key_layout[row];
}

KeyWindowWidget* KeyWindow::getRow(size_t row, size_t col) const {
    if (row >= key_layout.size() || col >= key_layout[row].size()) {
		LOG_ERROR("越界", row, key_layout.size(), col);
        return nullptr; // 安全返回，避免异常
    }
    return key_layout[row][col];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool KeyWindow::setFocus(size_t row, size_t col) {
    if (row >= key_layout.size() || col >= key_layout[row].size()) {
        return false;
    }
    
    // 失去焦点回调（如果有）
    if (hasFocus()) {
        KeyWindowWidget* old = getCurrentWidget();
        if (old) old->onFocusLost();
    }
    
    current_focus = {static_cast<int>(row), static_cast<int>(col)};
    
    // 获得焦点回调
    KeyWindowWidget* current = getCurrentWidget();
    if (current) current->onFocusGained();
    
    return true;
}

bool KeyWindow::setFocus(const std::string& name) {
    auto it = registry_widget.find(name);
    if (it == registry_widget.end()) return false;
    return setFocus(it->second);
}

bool KeyWindow::setFocus(Widget* widget) {
    if (!widget) return false;
    auto pos = findWidgetPosition(widget);
    if (pos.first == -1) return false;
    return setFocus(pos.first, pos.second);
}

// ────────────────────────────────────────────────

// 辅助函数：尝试设置焦点，如果目标 widget 不可焦点则返回 false
bool KeyWindow::trySetFocus(int row, int col) {
    if (row < 0 || row >= static_cast<int>(key_layout.size())) {
        return false;
    }
    if (col < 0 || col >= static_cast<int>(key_layout[row].size())) {
        return false;
    }
    
    KeyWindowWidget* widget = key_layout[row][col];
    if (widget && widget->canFocus()) {
        return setFocus(row, col);
    }
    return false;
}

// ────────────────────────────────────────────────

////////////////////////////////////////////////////////////
// 移动相关开始


bool KeyWindow::focusUp() {
    if (!hasFocus()) return focusFirst();

    int row = current_focus.first;
    int col = current_focus.second;

    // 优先：向上找同一列 第一个可焦点的
    for (int r = row - 1; r >= 0; --r) {
        if (trySetFocus(r, col)) {
            return true;
        }
    }

    // 没找到同列 → 尝试上一行的最右边
    if (row > 0) {
        int prev_row = row - 1;
        int last_col = static_cast<int>(key_layout[prev_row].size()) - 1;
        if (last_col >= 0) {
            if (trySetFocus(prev_row, last_col)) {
                return true;
            }
        }
    }

    // 实在找不到 → 跳到最后一个可焦点的
    return focusLast();
}


bool KeyWindow::focusDown() {
    if (!hasFocus()) return focusFirst();

    int row = current_focus.first;
    int col = current_focus.second;

    // 优先：向下找同一列 第一个可焦点的
    for (size_t r = row + 1; r < key_layout.size(); ++r) {
        if (trySetFocus(static_cast<int>(r), col)) {
            return true;
        }
    }

    // 没找到同列 → 尝试下一行的最右边（你当前代码风格）
    if (row + 1 < static_cast<int>(key_layout.size())) {
        int next_row = row + 1;
        size_t last = key_layout[next_row].size();
        if (last > 0) {
            if (trySetFocus(next_row, static_cast<int>(last) - 1)) {
                return true;
            }
        }
    }

    return focusFirst();
}

bool KeyWindow::focusLeft() {
    if (!hasFocus()) return false;

    int row = current_focus.first;
    int col = current_focus.second;

    // 同一行向左找第一个可焦点的
    for (int c = col - 1; c >= 0; --c) {
        if (trySetFocus(row, c)) {
            return true;
        }
    }

    // 到达行首 → 上一行最右（找可焦点的）
    if (row > 0) {
        int prev_row = row - 1;
        for (int c = static_cast<int>(key_layout[prev_row].size()) - 1; c >= 0; --c) {
            if (trySetFocus(prev_row, c)) {
                return true;
            }
        }
    }

    // 找不到 → 跳全局最后一个可焦点
    return focusLast();
}


bool KeyWindow::focusRight() {
    if (!hasFocus()) return false;

    int row = current_focus.first;
    int col = current_focus.second;
    size_t cols = key_layout[row].size();

    // 同一行向右找第一个可焦点的
    for (size_t c = col + 1; c < cols; ++c) {
        if (trySetFocus(row, static_cast<int>(c))) {
            return true;
        }
    }

    // 到达行尾 → 下一行最左（找可焦点的）
    if (static_cast<size_t>(row) + 1 < key_layout.size()) {
        int next_row = row + 1;
        for (size_t c = 0; c < key_layout[next_row].size(); ++c) {
            if (trySetFocus(next_row, static_cast<int>(c))) {
                return true;
            }
        }
    }

    // 找不到 → 跳全局第一个可焦点
    return focusFirst();
}


bool KeyWindow::focusFirst() {
    if (key_layout.empty()) return false;

    for (size_t r = 0; r < key_layout.size(); ++r) {
        for (size_t c = 0; c < key_layout[r].size(); ++c) {
            if (trySetFocus(static_cast<int>(r), static_cast<int>(c))) {
                return true;
            }
        }
    }
    return false;
}

bool KeyWindow::focusLast() {
    if (key_layout.empty()) return false;

    for (int r = static_cast<int>(key_layout.size()) - 1; r >= 0; --r) {
        for (int c = static_cast<int>(key_layout[r].size()) - 1; c >= 0; --c) {
            if (trySetFocus(r, c)) {
                return true;
            }
        }
    }
    return false;
}

// 暂时不使用以下两个函数
//
//// 移动到当前行最左边
//bool KeyWindow::focusLeftMost() {
//    int row = current_focus.first;
//    return setFocus(row, 0);
//}
//
//// 移动到当前行最右边
//bool KeyWindow::focusRightMost() {
//    int row = current_focus.first;
//    if (row >= 0 && row < static_cast<int>(key_layout.size())) {
//        return setFocus(row, key_layout[row].size() - 1);
//    }
//    return false;
//}

// 移动相关结束
////////////////////////////////////////////////////////////

KeyWindowWidget* KeyWindow::getCurrentWidget() const {
    if (!hasFocus()) return nullptr;
    return key_layout[current_focus.first][current_focus.second];
}

void KeyWindow::clearFocus() {
//    if (hasFocus()) {
//        Widget* current = getCurrentWidget();
//        if (current) current->onFocusLost();
//    }
    current_focus = {-1, -1};
}

std::pair<int, int> KeyWindow::findWidgetPosition(Widget* widget) const {
    if (!widget) return {-1, -1};
    
    for (size_t r = 0; r < key_layout.size(); ++r) {
        const auto& row = key_layout[r];
        for (size_t c = 0; c < row.size(); ++c) {
            if (row[c] == widget) {
                return {static_cast<int>(r), static_cast<int>(c)};
            }
        }
    }
    return {-1, -1};
}

std::pair<int, int> KeyWindow::findWidgetPosition(const std::string& name) const {
    auto it = registry_widget.find(name);
    if (it == registry_widget.end()) return {-1, -1};
    return findWidgetPosition(it->second);
}

size_t KeyWindow::getColumnCount(size_t row) const {
    if (row >= key_layout.size()) return 0;
    return key_layout[row].size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool KeyWindow::focusMoveControl(WPARAM wParam, LPARAM lParam)  {
    switch(wParam){
		case SCANCODE_CURSORBLOCKUP:
			focusUp();
			return true;
		case SCANCODE_CURSORBLOCKDOWN:
			return focusDown();
			return true;
		case SCANCODE_CURSORBLOCKLEFT:
			return focusLeft();
			return true;
		case SCANCODE_CURSORBLOCKRIGHT:
			return focusRight();
			return true;
		case SCANCODE_ESCAPE: // 默认关闭窗口消息
			Close();
			return true;
		default:break;
    }
	
	return false;
}

/*

===== 消息传递函数使用示例 =====

1. 传递给当前焦点控件
sendMessageToCurrent(&Widget::msg_keyup, wParam, lParam);
sendMessageToCurrent(&Widget::msg_keydown, wParam, lParam);
sendMessageToCurrent(&Widget::msg_key_long_press, wParam, lParam);
sendMessageToCurrent(&Widget::msg_keyup_long, wParam, lParam);

2. 传递给指定位置的控件
sendMessageToWidget(0, 0, &Widget::msg_keyup, wParam, lParam);
sendMessageToWidget(0, 0, &Widget::msg_keydown, wParam, lParam);
sendMessageToWidget(0, 0, &Widget::msg_key_long_press, wParam, lParam);
sendMessageToWidget(0, 0, &Widget::msg_keyup_long, wParam, lParam);

3. 传递给指定名称的控件
sendMessageByName("xxx", &Widget::msg_keyup, wParam, lParam);
sendMessageByName("xxx", &Widget::msg_keydown, wParam, lParam);
sendMessageByName("xxx", &Widget::msg_key_long_press, wParam, lParam);
sendMessageByName("xxx", &Widget::msg_keyup_long, wParam, lParam);

4. 传递给所有控件
sendMessageToAll(&Widget::msg_keyup, wParam, lParam);
sendMessageToAll(&Widget::msg_keydown, wParam, lParam);
sendMessageToAll(&Widget::msg_key_long_press, wParam, lParam);
sendMessageToAll(&Widget::msg_keyup_long, wParam, lParam);

*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool KeyWindow::sendMessageToCurrent(void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam){
	Widget* current = getCurrentWidget();
    if (!current) {
		LOG_INFO("无效", "当前无有效焦点");
        return false;  // 无焦点控件
    }

	(current->*func)(wParam, lParam);
	return true;
}

bool KeyWindow::sendMessageToWidget(size_t row, size_t col, 
    									void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam) {
    Widget* current = getRow(row, col);
    if (!current) {
		LOG_INFO("无效", "当前无有效焦点");
        return false;  // 无焦点控件
    }

	(current->*func)(wParam, lParam);
	return true;
}

bool KeyWindow::sendMessageByName(const std::string& name, 
    									void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam) {
	Widget* widget = findWidget(name);
	if (!widget) {
		return false;  // 未找到控件
	}
	
	(widget->*func)(wParam, lParam);
	return true;
}

void KeyWindow::sendMessageToAll(void (Widget::*func)(WPARAM, LPARAM), WPARAM wParam, LPARAM lParam) {
	for (auto& row : key_layout) {
		for (Widget* widget : row) {
			if (widget) {
				(widget->*func)(wParam, lParam);
			}
		}
	}
}

