# Shooting range

mi22032 - Bogdan Micić, Shooting range features a 3D scene with one wooden cabin and shooting targets
scattered over the scene in forest like surrounding (including forest terrain and trees),
with fps camera and rifle with crosshair, where you can activate the targets and shoot them down. Shooting is done
detecting intersections between raycast from camera and bounding box around model when left mouse is
pressed

## Controls

W -> move forward

S -> move backwards

D -> move right

A -> move left

SPACE -> jump

L_SHIFT -> run

MOUSE_LEFT_CLICK -> fire

V -> turn on/off spotlight

1 -> weak spotlight

2 -> strong spotlight

F1 -> lock/unlock mouse cursor

P -> starts shooting practice

ESC -> quit (window close)

## Features

### Fundamental:

[x] Model with lighting

- One directional light for the scene and one spotlight, also additional directional
  light only for rifle, so rifle lighting would be consistent

[x] Two types of lighting with customizable colors and movement through GUI or ACTIONS

- spotlight behaviour is customisable via keyboard actions, where you can turn it on/off, or change intensity and
  attenuation

[x] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

- P is pressed -> after approximately 2 seconds -> Targets go up and are ready to be shot
- When crosshair is inside bounding-box of target -> MOUSE LEFT_CLICK -> after approximately 2 seconds -> target goes
  down

### Group A:

[ ] Frame-buffers with post-processing   
[x] Instancing  
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:

[ ] Bloom with the use of HDR  
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

Added instancing, fps_camera movement with jumping and running, support for calculating bounding-box
for each mesh of model that can be used for calculating bounding-box of the model itself

## Models:

Links to all models can be found inside designated model directory in form of link.txt or licence.txt

Some models are modified in blender so they can be converted to the .obj format that can be loaded via assimp

## Textures

Link to ground texture: https://ambientcg.com/view?id=Ground037
The diffuse texture is only used in this project

## Skyboxes

Link to skybox_night: https://drive.google.com/drive/folders/1Gg80-Y1jAPTSsM0-lZI6ak4-5t7Fb5jM
