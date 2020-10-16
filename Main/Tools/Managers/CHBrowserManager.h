#pragma once
#include <Ultralight/Ultralight.h>
#include <AppCore/Platform.h>

#include "../DataType/GameStruc.h"
#include <string>
#include <vector>

class HeadlessBrowserManager : public ultralight::LoadListener
{
	ultralight::RefPtr<ultralight::Renderer> GLRBrowserRenderer;
	ultralight::RefPtr<ultralight::View> GLRBrowserView;
	bool IsDone = false;

public:
	HeadlessBrowserManager();
	virtual ~HeadlessBrowserManager();

	void Run();
	void SearchSteamDB(const std::string& SearchWord);
	std::vector<Game> GetList();

	std::string GetStringFromJSString(JSStringRef str);
	
	void OnFinishLoading(ultralight::View* caller) override;
	void OnDOMReady(ultralight::View* caller) override;
};