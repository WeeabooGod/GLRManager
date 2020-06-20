#include "CHBrowserManager.h"
#include <sstream>

using namespace ultralight;
HeadlessBrowserManager::HeadlessBrowserManager()
{
	Config config;
    config.device_scale_hint = 2.0;
    config.font_family_standard = "Arial";
	Platform::instance().set_config(config);
	
	GLRBrowserRenderer = Renderer::Create();
	//GLRBrowserView = GLRBrowserRenderer->CreateView(1200, 3000, false);

	GLRBrowserView->set_load_listener(this);
}

HeadlessBrowserManager::~HeadlessBrowserManager()
{
	GLRBrowserRenderer = nullptr;
	GLRBrowserView = nullptr;
}

void HeadlessBrowserManager::SearchSteamDB(const std::string& SearchWord)
{
	//Base URL
	std::string URL = "https://steamdb.info/search/?a=app&q=";

	//Create a vector of words in our search
	std::vector<std::string> SearchWords;
	std::istringstream iss(SearchWord);
	for (std::string s; iss >> s;)
		SearchWords.push_back(s);
	
	//Append Search to URL
	for (const auto& Word : SearchWords)
	{
		URL += Word + "+";
	}
	
	GLRBrowserView->LoadURL(URL.c_str());
}

void HeadlessBrowserManager::Run()
{
	while(!IsDone)
		GLRBrowserRenderer->Update();

	//We are done, create the list
	//CreateList();
}


//void HeadlessBrowserManager::CreateList()
//{
//	//How large is our list?
//	JSValueRef result = GLRBrowserView->EvaluateScript("document.getElementById('table-sortable').getElementsByTagName('tbody')[0].getElementsByTagName('tr').length");
//	int ListSize = static_cast<int>(JSValueToNumber(GLRBrowserView->js_context(), result, nullptr));
//}

void HeadlessBrowserManager::OnFinishLoading(View* caller)
{
	//Force the page to load all elements up to 1000
	caller->EvaluateScript("$('select[name=table-sortable_length]').val('1000').change()");
	GLRBrowserRenderer->Render();
	GLRBrowserView->bitmap()->WritePNG("result.png");

	//We are done, and most likely OnDOMReady will be called afterwards
	IsDone = true;
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
