# Weeb's Greenluma Reborn Manager
A C++ variant of the Greenluma Reborn Manager. Uses Imgui for Gui with OpenGL GLFW. Integrated with [Ultralight](https://ultralig.ht/), a Chromium based Headless browser for use of scraping Steam.DB. 

Inspired off of **ImaniiTy's** manager, since it no longer gets updated. I find I avoid a lot more traps and pitfalls that the original falls into, making it easier and more reliable to use (so far).

There will be bugs that I havn't found. Please just let me know!

# (VCRUNTIME140_1.dll missing)
Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017 and 2019 is required to run this program. You can download the redistrubutable here: 

https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads

x64 seems to be what is required based on testing.

# How to install
Simply unzip the release into any manner of folder/directory.

# How to use
If you come from using ImaniiTy Greenluma Reborn Manager, you might find it to function quite similarily, just no longer with python.

Simply search for your Game or DLC within the search bar on the left side. After 1-3 seconds a list should pop up. Select your games and bottom left press "Add Games". You can use SHIFT to select more than one, though my implementation of it is not the smoothest.

On the right is the profile manager. Create profiles and add games to the list. There will be a number to indicate if you go over what ever is the latest green luma limit, as well as a warning if you try to generate with more than that limit.

# How to build
I use premake5 to generate visual studio files. You should be able to just run the GenerateBuildFiles.bat and create a build folder for your version of visual studio.

Afterwards you should be able to load it up in Visual Studio Community (Or Others) with the solution file and build it.
