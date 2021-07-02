#pragma once
#include "AppCore/AppCore.h"

#include "../DataType/GameStruc.h"
#include <string>
#include <vector>


class BrowserManager : public ultralight::LoadListener
{
protected:
	ultralight::RefPtr<ultralight::Renderer> GLRBrowserRenderer;
	ultralight::RefPtr<ultralight::View> GLRBrowserView;
	ultralight::RefPtr<ultralight::Session> GLRSession;
	
	bool IsDone = false;

	std::vector<Game> GameList;

public:
	BrowserManager();

	void SearchSteamGames(const std::string& SearchWord);
	void Run();
	
	std::vector<Game> GetList();

	void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const ultralight::String& url) override;
	void OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const ultralight::String& url) override;
	
	void GenerateDebugImage();
	



	//std::string GetStringFromJSString(JSStringRef str);
	//void OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const ultralight::String& url) override;

};
