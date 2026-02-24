#include "key_window.hpp"

void KeyWindow::registerWidget(const std::string &name, Widget &widget){
	BaseWindow::registerWidget(name, widget);
	key_layout.emplace_back();          // 添加空行
    key_layout.back().push_back(&widget); // 该行只包含此控件
}

void KeyWindow::registerWidget(const std::string &name, size_t row, Widget &widget){
	BaseWindow::registerWidget(name, widget);
	// 策略：自动填充缺失的中间行（空行）
    if (row >= key_layout.size()) {
        key_layout.resize(row + 1);
    }
    key_layout[row].push_back(&widget);
}

void KeyWindow::registerWidget(const std::string &name, size_t row, size_t col, Widget &widget){
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

std::vector<Widget*>& KeyWindow::getRow(size_t row) {
    if (row >= key_layout.size()) {
        throw std::out_of_range("Row index out of range");
    }
    return key_layout[row];
}

Widget* KeyWindow::getRow(size_t row, size_t col) const {
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
//    if (hasFocus()) {
//        Widget* old = getCurrentWidget();
//        if (old) old->onFocusLost();
//    }
    
    current_focus = {static_cast<int>(row), static_cast<int>(col)};
    
    // 获得焦点回调
//    Widget* current = getCurrentWidget();
//    if (current) current->onFocusGained();
    
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


bool KeyWindow::focusUp() {
    if (!hasFocus()) return focusFirst();  // 无焦点时跳到第一个
    
    int row = current_focus.first;
    int col = current_focus.second;
    
    // 向上找同一列的控件
    for (int r = row - 1; r >= 0; --r) {
        if (col < static_cast<int>(key_layout[r].size())) {
            return setFocus(r, col);
        }
    }
    
    // 如果下面没有，跳到最后一个
    return focusLast();
}

bool KeyWindow::focusDown() {
    if (!hasFocus()) return focusFirst();  // 无焦点时跳到第一个
    
    int row = current_focus.first;
    int col = current_focus.second;
    
    // 向下找同一列的控件
    for (size_t r = row + 1; r < key_layout.size(); ++r) {
        if (col < static_cast<int>(key_layout[r].size())) {
            return setFocus(r, col);
        }
    }
    
    // 如果下面没有，跳到第一个
    return focusFirst();
}

bool KeyWindow::focusLeft() {
    if (!hasFocus()) return false;
    
    int row = current_focus.first;
    int col = current_focus.second;
    
    // 向左移动
    if (col > 0) {
        return setFocus(row, col - 1);
    }
    
    // 已经是最左边，跳到最右
    return focusRightMost();
}

bool KeyWindow::focusRight() {
    if (!hasFocus()) return false;
    
    int row = current_focus.first;
    int col = current_focus.second;
    
    // 向右移动
    if (col + 1 < static_cast<int>(key_layout[row].size())) {
        return setFocus(row, col + 1);
    }
    
    // 已经是最右边，跳到最左
    return focusLeftMost();
}


bool KeyWindow::focusFirst() {
    if (key_layout.empty()) return false;
    
    // 找到第一个非空行
    for (size_t r = 0; r < key_layout.size(); ++r) {
        if (!key_layout[r].empty()) {
            return setFocus(r, 0);
        }
    }
    return false;
}

bool KeyWindow::focusLast() {
    if (key_layout.empty()) return false;
    
    // 从后往前找第一个非空行
    for (int r = static_cast<int>(key_layout.size()) - 1; r >= 0; --r) {
        if (!key_layout[r].empty()) {
            return setFocus(r, key_layout[r].size() - 1);
        }
    }
    return false;
}

// 移动到当前行最左边
bool KeyWindow::focusLeftMost() {
    int row = current_focus.first;
    return setFocus(row, 0);
}

// 移动到当前行最右边
bool KeyWindow::focusRightMost() {
    int row = current_focus.first;
    if (row >= 0 && row < static_cast<int>(key_layout.size())) {
        return setFocus(row, key_layout[row].size() - 1);
    }
    return false;
}

Widget* KeyWindow::getCurrentWidget() const {
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
			return focusUp();
			break;
		case SCANCODE_CURSORBLOCKDOWN:
			return focusDown();
			break;
		case SCANCODE_CURSORBLOCKLEFT:
			return focusLeft();
			break;
		case SCANCODE_CURSORBLOCKRIGHT:
			return focusRight();
			break;
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

