#include "CHBrowserManager.h"
#include <Ultralight/platform/Platform.h>
#include <AppCore/Platform.h>

#include <chrono>
#include <thread>
#include <format>

using namespace ultralight;
BrowserManager::BrowserManager()
{
	Config config;
	config.device_scale = 1.0;
	config.font_family_standard = "Arial";
	config.use_gpu_renderer = false;
	config.resource_path = "Resources"; // Required to make SSL Requests
	config.cache_path = "Resources";
	config.enable_images = true;
	config.enable_javascript = true;
	
	Platform::instance().set_config(config);
	Platform::instance().set_font_loader(GetPlatformFontLoader());

	GLRBrowserRenderer = Renderer::Create();
	GLRBrowserView = GLRBrowserRenderer->CreateView(800, 9000, false, GLRBrowserRenderer->default_session());
	GLRBrowserView->set_load_listener(this);
}


void BrowserManager::SearchSteamGames(const std::string& SearchWord)
{
	//We are making a new search or this is are first time, make sure our "Done" flag is set to false.
	IsDone = false;
	
	////Base URL
	std::string URL = "https://store.steampowered.com/search/results?term=" + SearchWord + "&count=50000&start=0&catagory1=998"; //According another program, this is how I would search the steam store itself? 

	////Append each "Word" for use on our search
	//std::istringstream iss(SearchWord);
	//for (std::string s; iss >> s;)
	//	URL += s + "+";
	//URL = URL.substr(0, URL.size() - 1);

	
	////Load Webpage based on our search keys
	GLRBrowserView->LoadURL(URL.c_str());
	
	
	//Have it run
	Run();
}


void BrowserManager::Run()
{
	//Wait until the browser says the page has finished loading, and while its doing that, update and render it.
	while(!IsDone)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
        GLRBrowserRenderer->Update();
	}
	
	//What does the browser see?
	GenerateDebugImage();
}


void BrowserManager::GenerateDebugImage()
{
	//Update the Browsers view
	GLRBrowserRenderer->Render();

	//Put that all into a PNG for us to check
    BitmapSurface* bitmap_surface = (BitmapSurface*)GLRBrowserView->surface();
    RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();
    bitmap->SwapRedBlueChannels();
    bitmap->WritePNG("result.png");
}

//std::string HeadlessBrowserManager::GetStringFromJSString(JSStringRef str)
//{
//	// Get UTF-8 C-String from JSString and copy it into our string value 
//	size_t len = JSStringGetMaximumUTF8CStringSize(str);	// First we determin how large the arreay will be
//	char* buffer = new char[len];							// Then we create a new char array with our found length
//	JSStringGetUTF8CString(str, buffer, len);				// Create a UTF8Ctring with all of our found data
//	std::string result = std::string(buffer, len);			// Copy it into ur STD::String
//	delete[] buffer;										// Delete Allocated Memory
//	JSStringRelease(str);
//
//	return result;
//}
//

std::vector<Game> BrowserManager::GetList()
{
	return GameList;
}


void BrowserManager::OnFinishLoading(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const ultralight::String& url)
{
	//Tell our loop we are done
	IsDone = true;
}


void BrowserManager::OnDOMReady(ultralight::View* caller, uint64_t frame_id, bool is_main_frame, const String& url)
{
	//Since we are parsing a page, we can assume we want to remove the previous list and start a new one
	GameList.clear();
	
	//Lock the Current Context
	Ref<JSContext> context = caller->LockJSContext();

	//Get the Current JSContextRef for use to evaluate scripts
	JSContextRef ctx = context.get();

	//---Now we need to find out how large the list is so we can loop through and populate a list with the information we need
	JSStringRef String = JSStringCreateWithUTF8CString("document.getElementsByClassName('search_result_row').length"); //This is a javascript command that will get the amount of games within the list
	JSValueRef result = JSEvaluateScript(ctx, String, nullptr, nullptr, 0, nullptr);
	int ListSize = static_cast<int>(JSValueToNumber(ctx, result, nullptr));
	JSStringRelease(String);
	
	//Loop through, we can be safe that if the ListSize is valid, then each entry will have an APPID, Type, and Name
	for (int i = 0; i < ListSize; i++)
	{
		//Create a new Game Variable
		Game TempGame;

		//Create the Script to get the Name
		std::string Script = "document.getElementsByClassName('search_result_row')[" + std::to_string(i) + "].getElementsByClassName('title')[0].innerText";
		JSStringRef JString = JSStringCreateWithUTF8CString(Script.c_str()); //This is a javascript command that will get the amount of games within the list
		JSValueRef JResult = JSEvaluateScript(ctx, JString, nullptr, nullptr, 0, nullptr);
		char buf[1024];
		JSStringGetUTF8CString(JSValueToStringCopy(ctx, JResult, nullptr), buf, 1024);
		TempGame.Name = buf;
		JSStringRelease(JString);


		//Create the Script to get the AppID
		Script = "document.getElementsByClassName('search_result_row')[" + std::to_string(i) + "].getAttribute('data-ds-appid')";
		JString = JSStringCreateWithUTF8CString(Script.c_str());
		JResult = JSEvaluateScript(ctx, JString, nullptr, nullptr, 0, nullptr);
		TempGame.AppID = static_cast<int>(JSValueToNumber(ctx, JResult, nullptr));
		JSStringRelease(JString);

		//Add it to our list
		GameList.emplace_back(TempGame);
	}
}