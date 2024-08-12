# Spectrum Asylum

Spectrum Asylum is a first-person 3D game made from scratch. This project was a school assignment, with the goal of creating a level inspired by [The Spectrum Retreat](https://store.steampowered.com/app/763250/The_Spectrum_Retreat/) in three weeks.

## Libraries

Math: [mathlib](https://github.com/torrra/mathlib) (legacy version)  
Sound: [irrKlang](https://www.ambiera.com/irrklang/)  
<br>
Graphics API: [OpenGL 3.3](https://www.opengl.org/)  
Windowing: [GLFW 3](https://www.glfw.org/)  
OpenGL loader: [glad](https://glad.dav1d.de/)


## Contributors

Team lead & main rendering rendering programmer : [Torra](https://github.com/torrra)  
Main physics programmer : [MLev29](https://github.com/MLev29)  
Main gameplay programmer : [MetaXGhost](https://github.com/MetaXGhost)

## How to build

This project uses CMake as its build system. To build it you will need:  
* Windows
* Git
* CMake 3.25 or higher
* Visual studio 2022 version 17

### Step 1 : clone the repo

Open CMD and move open the directory in which you wish to install the repo. You can either press your Win key and type out 'cmd', then use 'cd <target_folder>", or open windows file explorer and right click inside to open in terminal. You can now enter the following command:

```bash
git clone git@github.com:torrra/spectrum-retreat-clone.git
```

### Step 2 : generate visual studio solution

Once you have downloaded the repo, you need to open the folder with `cd spectrum-asylum-clone`, and run CMake with either:

```bash
cd build
cmake ..
```

or

```bash
cmake -B build -S .
```

### Step 3 : open Visual Studio

After CMake has successfully configured the VS solution, you can open it with your file explorer, or by using `cmake --open .` (or  
`cmake --open build` if you ran the second CMake command shown in the previous step). Once Visual Studio has finished starting up, you can select Debug or Release and click on 'Local Windows Debugger' to compile everything. The game will start automatically.


