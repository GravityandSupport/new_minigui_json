#pragma once

#include "base_attr.hpp"
#include "LanguageManager.hpp"
#include "common.hpp"

namespace ui{
namespace extend_widget{


template<typename T>
class IconWidget : public T {
public:
	int bitmap=-1;

	virtual void msg_paint(HDC hdc) override{
		T::bvm_PaintBitmap(hdc, bitmap, T::rc);
		T::msg_paint(hdc);
	}
	
	IconWidget(const std::string& str): T(str){}
    virtual ~IconWidget() = default;
};

}
}
