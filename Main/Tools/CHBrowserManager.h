#pragma once
#include <Ultralight/Ultralight.h>
#include <AppCore/JSHelpers.h>
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

	void SearchSteamDB(const std::string& SearchWord);
	void Run();
	//void CreateList();

	void OnFinishLoading(ultralight::View* caller) override;
};