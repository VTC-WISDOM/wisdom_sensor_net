# wisdom_sensor_net

WISDOM team RPi Pico based sensor network codebase

Maintainers:
- Evan Morse (emm08060@vermontstate.edu OR emorse@tech.dev)
- Amelia Vlahogiannis (agv07030@vermontstate.edu or amelia@ag2v.com)

## Cloning the Repo
Repo should live at: ~/pico/wisdom_sensor_net  
Because we use submodules in our rep, you need to clone recursively:

`git clone --recursive -j8 git@github.com:VTC-WISDOM/wisdom_sensor_net.git`

NOTE: You'll need proper SSH access to the repo before this will work properly.  
Contact me (Evan Morse) or ping me on the team Discord to configure access.

## Subprojects
Since our work is really a collection of many projects, our repo is also split out  
into many seperately buidable subprojects. There is a template subproject which can  
be copied to hit the ground running with all the files needed to build code for  
the RP2040.

### Creating a New Subproject
(from repo root)
1. `cp -r template subproject_name ; cd subproject_name`
2. Edit "target_name" in this line in wisdom_config.cmake (something more meaningful):
```CMake
set(target "target_name")
```
3. Begin editing src/main.c
4. Pet nearest cat

### Building Subproject
Assuming you have properly configured your wisdom_config.cmake file, the build process  
is very simple:
1. Navigate to root folder of subproject you wish to build.
2. `make build`
3. (only if using picotool) `make load`

If not using picotool, you'll have to copy the generated .uf2 onto the pico bootloader  
manually.

### Applying Licensing
Every source file created (.c or .h) should contain this licensing preamble under  
a comment which contains the source file name and a brief description of what is  
contained in the source file:

```C
// source.c
// Brief description

//	Copyright (C) 2024 
//	Evan Morse
//	Amelia Vlahogiannis
//	Noelle Steil
//	Jordan Allen
//	Sam Cowan
//	Rachel Cleminson

//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.

//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.

//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
