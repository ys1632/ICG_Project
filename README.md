# ICG_Project
Interactive Computer Graphics Term Project (Spring 2014) <br>
C++ and OpenGL <br>
This project contains four parts. We add some new futures to previous part getting the new part. The fourth part is the final version of the whole project that contains all features.
##Installation Instructions for OpenGL (Windows OS)
###What you need to download
>1. VC++ 20xx Express Edition, is available [here](https://www.visualstudio.com/en-US/products/visual-studio-express-vs)
2. GLUT packages, available to download [here] (http://cis.poly.edu/cs653/glut-3.7.6-bin.zip)
3. GLEW packages, available to download [here] (http://glew.sourceforge.net/index.html)

After unzipping the GLUT and GLEW packages, put the following files to the specified folders as below:

1. glut.h, glew.h, wglew.h: to the "include/gl" folder of either VC++ or SDK (e.g., "C:\Program Files\Microsoft SDKs\Windows\v6.1\Include\gl", or "C:\Program Files\Microsoft Visual Studio 8\VC\include\gl\"), where the OpenGL header files "gl.h" and "glu.h" are already there. (Search for the folder containing "gl.h" and "glu.h", and put glut.h, glew.h, wglew.h there.)

2. glut32.lib, glew32.lib: to the "Lib" folder of either VC++ or SDK (e.g., "C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib" or "C:\Program Files\Microsoft Visual Studio 8\VC\lib"), where the OpenGL library files "opengl32.lib" and "glu32.lib" are already there. (Search for the folder containing "opengl32.lib" and "glu32.lib", and put glut32.lib, glew32.lib there.)

3. glut32.dll, glew32.dll: to the systems folder (e.g., "C:\Windows\System32" for a 32-bit OS, or "C:\Windows\SysWOW64" for a 64-bit OS), where the OpenGL .DLL files "opengl32.dll" and "glu32.dll" are already there. (Search for the folder containing "opengl32.dll" and "glu32.dll", and put glut32.dll, glew32.dll there.)

##Compilation Instructions
1. Start VC++ from the start menu.
2. File -> New a project with template Win32 Console Application (note: not Win32 Project), give a project name. In the next pop-up window, click Application Settings, then choose Empty project (so that the two items chosen are Console application and Empty project), and click Finish. A new folder with the project name is created. (If you want to use an existing folder containing all source files, in the first pop-up window give this folder name as the "Name" and its parent folder location as the "Location").
3. If you want to open an existing project, instead of New use Open -> Project/Solution.
4. Move all source (.c or .cpp) files, header (.h) files, and input data files to the project folder (either the new folder created in Step 2 or the existing folder mentioned in Step 2). 
In the left sub-window of VC++, right click Source Files and select Add -> Existing Item to include the source (.c or .cpp) files, one by one, into the project as source files. Similarly include the .h files (if any) as Header Files. (For shader files *.glsl, just put them to the same project folder without including them.)
5. For OpenGL projects that use GLEW (such as those involving GLSL), it is necessary to declare the dependencies for the GLEW library ("glew32.lib") being used: 
a. Click on Project -> [project name] Properties. 
b. In the left window, click Configuration Properties to expand it. On the very top left, the Configuration drop-down, select All Configurations. 
c. Below Configuration Properties, select Linker -> Input. 
d. In the right window, click Additional Dependencies to activate this field. Type in 
"glew32.lib" . Then click OK.
6. To compile, click on Build -> Build [project name] (or Rebuild [project name]). 
To run, click on Debug -> Start Without Debugging (or Ctrl+F5).

Remark on step d: Also type in "glut32.lib opengl32.lib glu32.lib" in case they cannot be automatically found during linking.


##For Mac or Linux (not recommended)