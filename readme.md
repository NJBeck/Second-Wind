# Second Wind

Just a start of a simple 2D in OpenGL mostly to practice some design principles.
So far I've only started with some data oriented design for the few entity components/systems. This idea is that an entity is just an ID which is registered with various component handler objects who do they actual work of storing/manipulating the data.

Currently only basic movement of a sprite with WASD, and exiting with ESC are implemented.

A binary can be found in the /bin folder.

To compile you'll need to use MSVC because of SDL's libs.
And of course make sure the SDL2.dll is with the executable.
I've included a premake script to build for other buildtools as well.