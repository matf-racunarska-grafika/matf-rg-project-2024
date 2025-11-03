# Cat spinning around

436/2019 - Ana Bolovic  
Interactive 3d scene generated using OpenGL-a, where Cat is spinning around in space, illuminated by directional and point lights

## Controls

W / A / S / D -> Move camera forward / left / backward / right
SPACE / LEFT CTRL -> Move camera up / down
Mouse -> Rotate camera
Scroll - >Zoom in/out
1 / 2 / 3 -> Change point light color (red / green / blue)
I / J / K / L / U / O -> Move point light along X/Y/Z axes
F1 -> Toggle cursor visibility
G -> Trigger event sequence
4 / 5 / 6 -> Switch post-processing effect (grayscale / invert / normal)

## Features

### Fundamental:

[x] Model with lighting
[x] Two types of lighting with customizable colors and movement through GUI or ACTIONS
[x] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

### Group A:

[x] Frame-buffers with post-processing   
[ ] Instancing  
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:
[ ] Bloom with the use of HDR  
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

Added support for creating and rendering bloom FBOs
Implemented OpenGL::set_clear_color() function
Modular shader loading through ResourcesController

## Models:

Cat - I used it in almost every project I created for this course over the years (legacy)
Three and Bird - Free3d.com

## Textures

Diffuse and bump maps for the cat
Diffuse texture for tree foliage and trunk
Normal map for parallax mapping test surface

