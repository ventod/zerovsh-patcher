ZeroVSH Patcher v0.4 - NightStar3 and codestation
Lite mod by Vento

-- Original project  --

Original code can be found here https://github.com/NightStar3/zerovsh-patcher

---- Mod Description ----

Removed funtionalities found in the original plugin:
* PSP GO Calendar/Clock on 2K/3K PSP
* Led Control
* Brightness Control
* CPU Clock Control
* .ini settings file loading
* Support for older FWs < 6.60

Now the plugin expect the PSP to be at least on FW version 6.60 and it loads custom files from "ef0:/PSP/VSH/" or "ms0:/PSP/VSH/" depending if you added "USE_EF0" as compile paramenter.

There is still "a lot" of unused/useless code left around, I will later updated the mod with a cleaner version.

---- Plugin Description ----

ZeroVSH Patcher is a plugin that allows users to customize their PSP to the 
fullest extent without writting to the flash0. ZeroVSH Patcher redirects the
loading of various file types from flash to your memory stick or internal storage, 
thus removing the fear of bricking.

Please scroll down for further information. 

---- How to Use ----

Copy zerovsh_patcher.prx in your seplugins folder.

Just place all the files you want to load from your memory stick or internal storage in the 
folder "ef0:/PSP/VSH/" or "ms0:/PSP/VSH/" depending if you added "USE_EF0" as compile paramenter.

---- Bugs ----

-Does not work on 1k psp

---- For Developers ----

You must have the pspsdk or psptoolchain set up in order to compile the plugin
Just run the script in the trunk corresponding to your operating system, and it should compile fine

The plugin will be compiled to the /bin folder in the trunk.

-- CFW/HEN's supported by this mod --

* ARK (tested only this one)
* PRO
* ME

-- File types supported --

* Fonts - .pgf
* Bitmap Images - .bmp
* Resources - .rco
* Gameboots - .pmf
* Modules - .prx
* default theme - .dat
* (more to come hopefully)
