# ICG_Project
CS6533 Interactive Computer Graphics Term Project (Spring 2014)
##Installation Instructions for OpenGL
###What you need to download
>1. VC++ 20xx Express Edition, is available [here](https://www.visualstudio.com/en-US/products/visual-studio-express-vs)
2. GLUT packages, available to download [here] (http://cis.poly.edu/cs653/glut-3.7.6-bin.zip)
3. GLEW packages, available to download [here] (http://glew.sourceforge.net/index.html)

After unzipping the GLUT and GLEW packages, put the following files to the specified folders as below:

1. glut.h, glew.h, wglew.h: to the "include/gl" folder of either VC++ or SDK (e.g., "C:\Program Files\Microsoft SDKs\Windows\v6.1\Include\gl", or "C:\Program Files\Microsoft Visual Studio 8\VC\include\gl\"), where the OpenGL header files "gl.h" and "glu.h" are already there. (Search for the folder containing "gl.h" and "glu.h", and put glut.h, glew.h, wglew.h there.)

2. glut32.lib, glew32.lib: to the "Lib" folder of either VC++ or SDK (e.g., "C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib" or "C:\Program Files\Microsoft Visual Studio 8\VC\lib"), where the OpenGL library files "opengl32.lib" and "glu32.lib" are already there. (Search for the folder containing "opengl32.lib" and "glu32.lib", and put glut32.lib, glew32.lib there.)

3. glut32.dll, glew32.dll: to the systems folder (e.g., "C:\Windows\System32" for a 32-bit OS, or "C:\Windows\SysWOW64" for a 64-bit OS), where the OpenGL .DLL files "opengl32.dll" and "glu32.dll" are already there. (Search for the folder containing "opengl32.dll" and "glu32.dll", and put glut32.dll, glew32.dll there.)
