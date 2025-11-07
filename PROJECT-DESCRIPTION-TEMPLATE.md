# matf-rg-project

mi22005 - Matija Radulovic  
Noćna scena napustenog parka u šumi. Scena je okružena bujnom travom. U vazduhu se povremeno pojavljuju i trepere svitci. 
Implementiran bloom i deferred shading za svetlašca, instancing za crtanje trave i dodata podrška za emisivne materijale.

## Controls

F1 -> Toggle mouse capture
F2 -> Toggle GUI 
ESC -> Exit application
Space -> Start timer that triggers glowing worms that move around scene for set duration
W,A,S,D -> Move the camera forward/back/left/right

## Features

### Fundamental:

[X] Model with lighting
[X] Two types of lighting with customizable colors and movement through GUI or ACTIONS
[X] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

### Group A:

[ ] Frame-buffers with post-processing   
[X] Instancing  
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:
[X] Bloom with the use of HDR  
[X] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

[ ] Added support for emissive materials in the engine
[ ] Implemented a system for timed actions that can trigger events after a set duration
[ ] Added support for custom frame buffers
[ ] Implemented instancing for rendering large amounts of models
[ ] Implemented support for multiple lights  

## Models:

https://free3d.com/3d-models/
https://sketchfab.com/
https://free3d.io/
https://tools.wwwtyro.net/space-3d/index.html

## Textures

No external textures were used.
