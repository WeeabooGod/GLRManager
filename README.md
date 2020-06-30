# Weeb's Greenluma Reborn Manager
A C++ variant of the Greenluma Reborn Manager. Uses Imgui for Gui with OpenGL. Integrated with Ultralight, a Chromium based Headless browser for use of scraping Steam.DB.

Inspired off of **ImaniiTy's** variant of the manager, I find I avoid a lot more traps and pitfalls that it falls into, making it easier and more reliable to use.


(VCRUNTIME140_1.dll missing)
Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019 is required to run this program. 
You can download the redistrubutable here: https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads

x64 seems to be what is required.

# How to use

If you come from using ImaniiTy Greenluma Reborn Manager, you might find it to function quite similarily, just no longer with python.

Simply search for your Game or DLC within the search bar on the left side. After 1-3 seconds a list should pop up. Select your games and bottom left press "Add Games". You can use SHIFT to select more than one, though my implementation of it is wonky.

On the right is the profile manager. Create profiles and add games to the list. There will be a number to indicate if you go over 171, as well as a warning if you try to generate with more than 171 games.

# How to build

I use premake5 to generate visual studio files. You should be able to just run the GenerateBuildFiles.bat and create a build folder for your version of visual studio.




There will be bugs that I havn't found. Please just let me know!
