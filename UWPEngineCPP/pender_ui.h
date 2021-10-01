#pragma once
#include <AsceneEngine.h>

class Pender {
public:
	struct Window {
		float x;
		float y;
		float width;
		float height;
	};

	struct ColorScheme {
		Color windowBgColor;
		Color textColor;
		Color titleColor;
	};

	struct UIConfig {
		float fontSize;
		float titleFontSize;
		float padding;
		float windowRadius;
		float closeIconPadding;
	};

	enum UIElementType {
		TEXT,
		RECT,
		TITLEBAR,
		TITLEBAR_TRIANGLE
	};

	struct UIElement {
		UIElementType type;

		float x;
		float y;
		float width;
		float height;
		float fontSize;
		Platform::String^ txt;
	};

	ColorScheme colorScheme;

	AsceneEngine e;

	UIConfig config;

	float x;
	float y;

	float mouse_x;
	float mouse_y;

	float locked_mouse_x;
	float locked_mouse_y;

	bool mouse_down = false;

	bool mouse_down_previous = false;

	bool dragging_window_previous = false;

	float window_x = 0;
	float window_y = 0;

	float titleBarHeight;
	float titleBarWidth;

	bool collapsed;

	AsceneEngine::EngineTexture triangleTexture;
	AsceneEngine::EngineTexture upsideDownTriangleTexture;

	void Init(AsceneEngine eng, UIConfig cfg, ColorScheme scheme) {
		config = cfg;
		colorScheme = scheme;
		e = eng;

		triangleTexture = e.GenerateTexture("Assets/triangle.png");
		upsideDownTriangleTexture = e.GenerateTexture("Assets/upside_down_triangle.png");
	}

	void Begin(Platform::String^ title) {
		queue.clear();
		currentWindow = Window();
		currentWindow.x = window_x;
		currentWindow.y = window_y;

		x = 0;
		y = 0;

		Windows::Foundation::Rect titleBarSize = e.GetStringSizePX(title, config.titleFontSize);
		titleBarWidth = titleBarSize.Width + config.padding * 2;
		titleBarHeight = titleBarSize.Height + config.padding;

		TitleBar(titleBarWidth, titleBarHeight);

		y -= config.padding / 2;

		x += 16;

		Text(title, config.titleFontSize);

		x = 0;

		TitleBarArrow();
	}

	void Text(Platform::String^ text, float fontSizeOverride = 0) {
		UIElement el;
		el.x = x + config.padding;
		el.y = y + config.padding;
		el.txt = text;

		if(fontSizeOverride > 0) {
			el.fontSize = fontSizeOverride;
		}
		else {
			el.fontSize = config.fontSize;
		}
		
		el.type = UIElementType::TEXT;

		Windows::Foundation::Rect stringSize = e.GetStringSizePX(el.txt, config.fontSize);

		el.width = stringSize.Width + (config.padding * 4);

		queue.push_back(el);

		y += stringSize.Height + config.padding;
	}

	void End() {
		y += (config.padding * 2);

		currentWindow.width = 0;
		currentWindow.height = y;

		for (UIElement el : queue) {
			if (el.width > currentWindow.width) {
				currentWindow.width = el.width;
			}
		}

		if(!collapsed) e.Rect(currentWindow.x, currentWindow.y, currentWindow.width, currentWindow.height, colorScheme.windowBgColor, config.windowRadius);

		for (UIElement el : queue) {
			switch (el.type) {
				case UIElementType::TEXT:
					if(!collapsed || el.y < titleBarHeight) e.Text(currentWindow.x + el.x, currentWindow.y + el.y, el.txt, el.fontSize, colorScheme.textColor);
					break;

				case UIElementType::TITLEBAR:
					e.Rect(currentWindow.x + el.x, currentWindow.y + el.y, currentWindow.width, el.height, colorScheme.titleColor, config.windowRadius);
					if(!collapsed) e.Rect(currentWindow.x + el.x, currentWindow.y + el.y + el.height / 2, currentWindow.width, el.height / 2, colorScheme.titleColor);

					titleBarBoundingBox = e.CreateBoundingBox(currentWindow.x + el.x, currentWindow.y + el.y, currentWindow.width, titleBarHeight);
					break;

				case UIElementType::TITLEBAR_TRIANGLE:
					if (collapsed) {
						e.TexturedRect(upsideDownTriangleTexture, currentWindow.x + 5, currentWindow.y + 5, titleBarHeight - 10, titleBarHeight - 10);
					}
					else {
						e.TexturedRect(triangleTexture, currentWindow.x + 5, currentWindow.y + 5, titleBarHeight - 10, titleBarHeight - 10);
					}
						
					collapseToggleBoundingBox = e.CreateBoundingBox(currentWindow.x + 5, currentWindow.y + 5, titleBarHeight - 10, titleBarHeight - 10);

					break;
			}
		}

		CheckMouse();
		CloseIcon();
	}

	void UpdateMouse(float x, float y, bool down) {
		mouse_x = x;
		mouse_y = y;
		mouse_down = down;

		mouseBoundingBox = e.CreateBoundingBox(mouse_x, mouse_y, 20, 20);
	};

	void CheckMouse() {
		if (e.IntersectAABB(mouseBoundingBox, collapseToggleBoundingBox) && mouse_down && !mouse_down_previous) {
			mouse_down_previous = true;
			collapsed = !collapsed;
		} else if (e.IntersectAABB(mouseBoundingBox, titleBarBoundingBox) && mouse_down) {
			if (!dragging_window_previous) {
				locked_mouse_x = currentWindow.x - mouse_x;
				locked_mouse_y = currentWindow.y - mouse_y;
			}

			dragging_window_previous = true;

			window_x = mouse_x + locked_mouse_x;
			window_y = mouse_y + locked_mouse_y;
		}
		else {
			dragging_window_previous = false;
			mouse_down_previous = false;
		}
	}

private:
	std::vector<UIElement> queue;
	Window currentWindow;

	AsceneEngine::BoundingBox titleBarBoundingBox;
	AsceneEngine::BoundingBox mouseBoundingBox;
	AsceneEngine::BoundingBox collapseToggleBoundingBox;

	void TitleBar(float width, float height) {
		UIElement el;
		el.x = x;
		el.y = y;
		el.width = width;
		el.height = height;
		el.type = UIElementType::TITLEBAR;

		queue.push_back(el);
	}

	void CloseIcon() {
		float size = titleBarHeight - config.closeIconPadding;

		float x = ((currentWindow.x + currentWindow.width) - size) - config.closeIconPadding / 2;
		float y = currentWindow.y + (config.closeIconPadding / 2);

		e.Line(x, y, x + size, y + size, Colors::White);
		e.Line(x + size, y, x, y + size, Colors::White);
	}

	void TitleBarArrow() {
		UIElement el;
		el.type = UIElementType::TITLEBAR_TRIANGLE;

		queue.push_back(el);
	}
};