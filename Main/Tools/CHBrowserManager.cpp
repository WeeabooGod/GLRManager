#include "CHBrowserManager.h"

#include <sstream>
#include <thread>
#include <chrono>

const char* htmlString();

using namespace ultralight;
HeadlessBrowserManager::HeadlessBrowserManager()
{
	Config config;
    config.device_scale = 1.0;
    config.font_family_standard = "Arial";
	config.use_gpu_renderer = false;
	config.resource_path = "./resources/"; // Required to make SSL Requests
	Platform::instance().set_config(config);
	Platform::instance().set_font_loader(GetPlatformFontLoader());
	
	GLRBrowserRenderer = Renderer::Create();
	GLRBrowserView = GLRBrowserRenderer->CreateView(900, 900, false, nullptr);

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
		GLRBrowserRenderer->Update();
		GLRBrowserRenderer->Render();
	}

	//Generate bitmap for testing purposes
	BitmapSurface* bitmap_surface = (BitmapSurface*)GLRBrowserView->surface();
	RefPtr<Bitmap> bitmap = bitmap_surface->bitmap();
	bitmap->SwapRedBlueChannels();
	bitmap->WritePNG("result.png");
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
		std::string Script = "document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').item(" + std::to_string(i) + ").cells[0].getElementsByTagName('a').item(0).innerHTML";
		JSValueRef AppID = GLRBrowserView->EvaluateScript(Script.c_str());

		Script = "document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').item(" + std::to_string(i) + ").cells[1].innerHTML";
		JSValueRef AppType = GLRBrowserView->EvaluateScript(Script.c_str());

		Script = "document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').item(" + std::to_string(i) + ").cells[2].innerHTML";
		JSValueRef AppName = GLRBrowserView->EvaluateScript(Script.c_str());

		//Set our AppID
		Temp.AppID = static_cast<int>(JSValueToNumber(GLRBrowserView->js_context(), AppID, nullptr));

		//Set the Type
		Temp.Type = GetStringFromJSString(JSValueToStringCopy(GLRBrowserView->js_context(), AppType, nullptr));

		//Set the Name
		Temp.Name = GetStringFromJSString(JSValueToStringCopy(GLRBrowserView->js_context(), AppName, nullptr));

		//Add it to our list
		TempList.emplace_back(Temp);
	}

	return TempList;
}

void HeadlessBrowserManager::OnFinishLoading(View* caller)
{
	//Force the page to load all elements up to 1000
	//GLRBrowserView->EvaluateScript("$('select[name=table-sortable_length]').val('1000').change()");

	//Tell our loop we are done
	IsDone = true;
}

void HeadlessBrowserManager::OnUpdateHistory(ultralight::View* caller)
{
}

void HeadlessBrowserManager::OnDOMReady(ultralight::View* caller)
{
	//Make Steam.DB Show value 1k plus. We can safetly assume there will not be any more than 1000 values, else why are we even searching?
	caller->EvaluateScript("$('select[name=table-sortable_length]').val('1000').change()");
}

//void HeadlessBrowser::OnDOMReady(ultralight::View* caller)
//{
//	//How large is our list?
//	JSValueRef result = caller->EvaluateScript("document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').length");
//	int ListSize = static_cast<int>(JSValueToNumber(caller->js_context(), result, nullptr));
//
//	
//	
//	//Make Steam.DB Show value 1k plus. We can safetly assume there will not be any more than 1000 values, else why are we even searching?
//	//caller->EvaluateScript("$('select[name=table-sortable_length]').val('1000').change()");
//
//	////Get the table that all the apps are in, which is between tbody and tr values
//	//JSValueRef val = caller->EvaluateScript("document.getElementById('table-sortable').getElementsByTagName('tbody')[0].outerHTML");
//
//	//if (!JSValueIsString(caller->js_context(), val))
//	//{
//	//	//Its invalid
//	//}
//
//	////Convert our result to JSStrig
//	//JSStringRef str = JSValueToStringCopy(caller->js_context(), val, nullptr);
//
//	//// Get UTF-8 C-String from JSString and copy it into our string value 
//	//size_t len = JSStringGetMaximumUTF8CStringSize(str);	// First we determin how large the arreay will be
//	//char* buffer = new char[len];							// Then we create a new char array with our found length
//	//JSStringGetUTF8CString(str, buffer, len);				// Create a UTF8Ctring with all of our found data
//	//std::string result = std::string(buffer, len);			// Copy it into ur STD::String
//	//delete[] buffer;										// Delete Allocated Memory
//	//JSStringRelease(str);									// Release the JSString from memory
//
//	////Find the begining of <tr to find where to remove new lines from 
//	//auto new_it = result.begin() + result.find("<tr");
//	//result.erase(std::remove(result.begin(), new_it, '\n'), new_it);
//
//	////Find the end capsule of tbody to clean up the junk text near the end, From the beigning to hopefully the end as a perfect little tbody capsule
//	//result = "<tbody>\n" + result.substr(7, result.find("</tbody>")) + "\n</tbody>";
//
//	////Make it a valid HTML
//	//result = "<!DOCTYPE html>\n<html>\n<table>\n" + result + "\n</table>\n</html>";
//
//	////Create an output for testing
//	//std::ofstream out("result.txt");
//	//out << result;
//	//out.close();
//}

const char* htmlString() {
  return R"(
    <html>
      <head>
        <style type="text/css">
          body {
            margin: 0;
            padding: 0;
            overflow: hidden;
            color: black;
            font-family: Arial;
            background: linear-gradient(-45deg, #acb4ff, #f5d4e2);
          }
          div {
            width: 350px;
            height: 350px;
            text-align: center;
            margin: 25px;
            border-radius: 25px;
            background: linear-gradient(-45deg, #e5eaf9, #f9eaf6);
            box-shadow: 0 7px 18px -6px #8f8ae1;
          }
          h1 {
            padding: 1em;
          }
          p {
            background: white;
            padding: 2em;
            margin: 40px;
            border-radius: 25px;
          }
        </style>
      </head>
      <body>
        <div>
          <h1>Hello World!</h1>
          <p>Welcome to Ultralight!</p>
        </div>
      </body>
    </html>
    )";
}
