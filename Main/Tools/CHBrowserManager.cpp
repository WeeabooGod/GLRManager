#include "CHBrowserManager.h"

#include <sstream>
#include <thread>
#include <chrono>


using namespace ultralight;
HeadlessBrowserManager::HeadlessBrowserManager()
{
	Config config;
    config.device_scale = 1.0;
    config.font_family_standard = "Arial";
	config.use_gpu_renderer = false;
	config.resource_path = "./Resources/"; // Required to make SSL Requests
	Platform::instance().set_config(config);
	Platform::instance().set_font_loader(GetPlatformFontLoader());
	
	GLRBrowserRenderer = Renderer::Create();
	GLRBrowserView = GLRBrowserRenderer->CreateView(1800, 9000, false, nullptr);

	GLRBrowserView->set_load_listener(this);
}

HeadlessBrowserManager::~HeadlessBrowserManager()
{
	GLRBrowserRenderer = nullptr;
	GLRBrowserView = nullptr;
}

void HeadlessBrowserManager::SearchSteamDB(const std::string& SearchWord)
{
	//We are making a new search or this is are first time, make sure our "Done" flag is set to false.
	IsDone = false;
	
	//Base URL
	std::string URL = "https://steamdb.info/search/?a=app&q=";

	//Append each "Word" for use on our search
	std::istringstream iss(SearchWord);
	for (std::string s; iss >> s;)
		URL += s + "+";

	URL = URL.substr(0, URL.size() - 1);
	
	//Load Webpage based on our search keys
	GLRBrowserView->LoadURL(URL.c_str());
	
	//Have it run
	Run();
}

void HeadlessBrowserManager::Run()
{
	while(!IsDone)
	{
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
		GLRBrowserRenderer->Update();
		GLRBrowserRenderer->Render();
	}

	////Generate bitmap for testing purposes
	//BitmapSurface* bitmap_surface = (BitmapSurface*)GLRBrowserView->surface();
	//RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();
	//bitmap->SwapRedBlueChannels();
	//bitmap->WritePNG("result.png");
}

std::string HeadlessBrowserManager::GetStringFromJSString(JSStringRef str)
{
	// Get UTF-8 C-String from JSString and copy it into our string value 
	size_t len = JSStringGetMaximumUTF8CStringSize(str);	// First we determin how large the arreay will be
	char* buffer = new char[len];							// Then we create a new char array with our found length
	JSStringGetUTF8CString(str, buffer, len);				// Create a UTF8Ctring with all of our found data
	std::string result = std::string(buffer, len);			// Copy it into ur STD::String
	delete[] buffer;										// Delete Allocated Memory
	JSStringRelease(str);

	return result;
}

std::vector<Game> HeadlessBrowserManager::GetList()
{
	std::vector<Game> TempList;
	
	//How large is our list?
	JSValueRef result = GLRBrowserView->EvaluateScript("document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').length");
	int ListSize = static_cast<int>(JSValueToNumber(GLRBrowserView->js_context(), result, nullptr));

	//Loop through, we can be safe that if the ListSize is valid, then each entry will have an APPID, Type, and Name
	for (int i = 0; i < ListSize; i++)
	{
		Game Temp;

		//Create the Script for which we will Evaluate.
		std::string Script = "document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').item(" + std::to_string(i) + ").cells[1].textContent";
		JSValueRef AppType = GLRBrowserView->EvaluateScript(Script.c_str());

		//Set the Type
		Temp.Type = GetStringFromJSString(JSValueToStringCopy(GLRBrowserView->js_context(), AppType, nullptr));
		Temp.Type = Temp.Type.substr(0, Temp.Type.find('\0'));

		// Unknown apps are most likely always withdrawn, lets just not bother with them as I don't particularly see a point in including them
		if (Temp.Type == "Unknown")
			continue;

		//Get AppID
		Script = "document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').item(" + std::to_string(i) + ").cells[0].getElementsByTagName('a').item(0).textContent";
		JSValueRef AppID = GLRBrowserView->EvaluateScript(Script.c_str());
		
		//Set our AppID
		Temp.AppID = static_cast<int>(JSValueToNumber(GLRBrowserView->js_context(), AppID, nullptr));

		//Get AppName
		Script = "document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').item(" + std::to_string(i) + ").cells[2].textContent";
		JSValueRef AppName = GLRBrowserView->EvaluateScript(Script.c_str());

		//Set the Name
		Temp.Name = GetStringFromJSString(JSValueToStringCopy(GLRBrowserView->js_context(), AppName, nullptr));
		Temp.Name = Temp.Name.substr(0, Temp.Name.find('\0'));

		//Special Character Replacement
		//size_t place = Temp.Name.find(std::string("–"));
		//if (place != std::string::npos)
		//{
		//	std::string newname = Temp.Name.substr(0, place);
		//	newname += "-";
		//	Temp.Name = newname + Temp.Name.substr(place + 3, Temp.Name.size() - 1);
		//	
		//}
		
		//Add it to our list
		TempList.emplace_back(Temp);
	}

	return TempList;
}

void HeadlessBrowserManager::OnFinishLoading(View* caller)
{
	//Tell our loop we are done
	IsDone = true;
}


void HeadlessBrowserManager::OnDOMReady(ultralight::View* caller)
{
	//Make Steam.DB Show value 1k plus. We can safetly assume there will not be any more than 1000 values, else why are we even searching?
	caller->EvaluateScript("$('select[name=table-sortable_length]').val('1000').change()");
}
