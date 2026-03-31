#pragma once

#include "base_attr.hpp"


namespace ui{

class Grap{
protected:

public:
	class RGB{
	public:
		int r,g,b;

		gal_pixel toPixel(HDC hdc) const;
		static RGB fromPixel(HDC hdc, gal_pixel pixel);
		
		RGB() = default;
	    RGB(int _r, int _g, int _b);
		RGB(uint32_t rgb_hex);
	    virtual ~RGB() = default;
	};	

	class Rect{
	public:
		int x, y, w, h;
		RECT rc;

		const RECT& toRECT() const;
		
		Rect() = default;
	    Rect(int _x, int _y, int _w, int _h);
		Rect(const RECT& _rc);
	    virtual ~Rect() = default;
	};	

	// ²»Ìî³ä¾ØÐÎ¿ò
	static void rectangle(HDC hdc, const Rect& rc, const RGB& pen_color, int pen_width);


	// Ô²½Ç¾ØÐÎ
	static void roundRect(HDC hdc, const Rect& rc, int rw, int rh, const RGB& brush_color, const RGB& pen_color, int pen_width);

	// Ìî³ä¾ØÐÎ
	static void fillBox(HDC hdc, const Rect& rc, const RGB& brush_color);
	
    Grap() = default;
    virtual ~Grap() = default;
};
}