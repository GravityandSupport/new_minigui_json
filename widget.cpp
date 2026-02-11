#include "widget.hpp"

Widget::Widget(const std::string& str)
    : BaseAttr(str){

}

void Widget::registerWidget(Widget &widget){
	widget.parentWidget = this;
    registry_widget.emplace_back(&widget);
}

void Widget::getParentOffset(){
    // RECT sub_rc;
    // sub_rc.left = x + rc.left;
    // sub_rc.top = y + rc.top;
    // return sub_rc;
    if(parentWidget==nullptr) {return ;}
    int w = RECTW(parentWidget->rc);
    int h = RECTH(parentWidget->rc);
    rc.left += parentWidget->rc.left;
    rc.top += parentWidget->rc.top;
    rc.right += rc.left + w;
    rc.bottom += rc.top + h;
}


void Widget::msg_init(WPARAM wParam, LPARAM lParam)  {
    // getParentOffset(); // 计算对于父部件的坐标偏移
    for(auto& pair : registry_widget){
        pair->hWnd = hWnd;
        pair->msg_init(wParam, lParam);
    }
}
void Widget::msg_command(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_command(wParam, lParam);
    }
}
void Widget::msg_lbutton_down(int x, int y)  {
    for(auto& pair : registry_widget){
        pair->msg_lbutton_down(x, y);
    }
}
void Widget::msg_lbutton_up(int x, int y)  {
    for(auto& pair : registry_widget){
        pair->msg_lbutton_up(x, y);
    }
}
void Widget::msg_mousemove(int x, int y)  {
    for(auto& pair : registry_widget){
        pair->msg_mousemove(x, y);
    }
}
void Widget::msg_keyup(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_keyup(wParam, lParam);
    }
}
void Widget::msg_keydown(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_keydown(wParam, lParam);
    }
}
void Widget::msg_timer(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_timer(wParam, lParam);
    }
}
void Widget::msg_paint(HDC hdc)  {
    for(auto& pair : registry_widget){
        pair->msg_paint(hdc);
    }
}
void Widget::msg_close(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_close(wParam, lParam);
    }
}
void Widget::msg_nc_lbutton_up(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_nc_lbutton_up(wParam, lParam);
    }
}
void Widget::msg_destroy(WPARAM wParam, LPARAM lParam)  {
    for(auto& pair : registry_widget){
        pair->msg_destroy(wParam, lParam);
    }
}
