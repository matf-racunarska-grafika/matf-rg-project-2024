# Campfire Night Scene

mi22027 - Jovana Tasovac
This project represents a atmospheric camp scene set in a forest at night. A campfire provides warm lighting while a
lantern offers adjustable spotlight functionality. The scene features dynamic lighting controls and an interactive event
sequence. Enhancing the nighttime atmosphere, bloom effects add vibrant glow to the fire and lantern lights. The forest
ground is textured with realistic materials, and trees are efficiently rendered using GPU instancing.

## Controls

KEY_W -> camera moves forward  
KEY_S -> camera moves backward  
KEY_A -> camera moves left  
KEY_D -> camera moves right  
KEY_Q -> camera moves down  
KEY_E -> camera moves up  
KEY_UP -> camera moves forward  
KEY_DOWN -> camera moves backward  
KEY_LEFT -> camera moves left  
KEY_RIGHT -> camera moves right  
KEY_SHIFT -> sprint movement  
KEY_H -> display help with all controls  
KEY_ESC -> close program  
KEY_I -> increase campfire intensity  
KEY_O -> decrease campfire intensity  
KEY_U -> increase moonlight intensity  
KEY_J -> decrease moonlight intensity  
KEY_P -> toggle lantern spotlight  
KEY_M -> increase spotlight intensity  
KEY_N -> decrease spotlight intensity  
KEY_L -> toggle lantern on/off  
KEY_F — AFTER_3_SECONDS — campfire intensity boosts (EVENT_A) — AFTER_8_SECONDS — tent color changes to red (EVENT_B) ->
This event sequence can be triggered when standing near the campfire. After completion, the scene resets to normal.

## Features

### Fundamental:

[yes] Model with lighting  
[yes] Two types of lighting with customizable colors and movement through GUI or ACTIONS --> In this project, I used
directional (moonlight), point (campfire, lantern), and spotlight (lantern). All light intensities and colors can be
modified in real-time through keyboard controls. The campfire has animated flickering and the lantern can be toggled
on/off.  
[yes] (KEY_F) — AFTER_3_SECONDS—Triggers---> (Campfire intensity boost) --> AFTER_8_SECONDS—Triggers---> (Tent color
changes to red)

### Group A:

[ ] Frame-buffers with post-processing
[yes] Instancing
[ ] Off-screen Anti-Aliasing
[ ] Parallax Mapping

### Group B:

[yes] Bloom with the use of HDR
[ ] Deferred Shading
[ ] Point Shadows
[ ] SSAO

### Engine improvement:

[yes] BloomController - Custom engine component for HDR bloom post-processing

## Models:

[link1] https://free3d.com/3d-model/low-poly-fir-72272.html
[link2] https://free3d.com/3d-model/stone-130691.html
[link3] https://sketchfab.com/3d-models/campfire-947ef3c08560486c88406af9bd2769ef
[link4] https://sketchfab.com/3d-models/dead-trunk-8ac9cc4207a1430ba9fb87a853d8f63f  
[link5] https://free3d.com/3d-model/lantern-v1--935990.html
[link6] https://sketchfab.com/3d-models/tent-bb41d971b17c4133b652efac55111552

## Textures

[link1] https://polyhaven.com/a/forrest_ground_01

(OTHER-RESOURCES)
Skybox: https://polyhaven.com/a/dikhololo_night