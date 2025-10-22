# Christmas in Space


59/2022 - Matija Đorđević \
Snežno ostrvo u svemiru  na kome se nalazi vikendica, jelka na kojoj je zvezda koja treperi i pokloni ispod nje.
Takođe su prisutne pahuljice, Sneško Belić i sanke Deda Mraza. Svetlost iz svemira, zajedno sa zvezdom i logorskom vatrom
obasjavaju scenu pri čemu je na zvezdi primetan bloom efekat.
U meniju je moguće podešavati boju zvezde, kao i jačinu i prisutnost bloom efekta kao i nivo izloženosti (exposure).

## Controls

W  -> Pravo\
A  -> Levo\
S  -> Nazad\
D  -> Desno\
F2 -> Otvaranje/zatvaranje menija\
Q  -> Smanji exposure\
E  -> Povećaj exposure\
J  -> Smanji jačinu bloom-a\
K  -> Povećaj jačinu bloom-a\
F  -> Paljenje/gašenje bloom-a


## Features

### Fundamental:

[X] Model with lighting\
[X] Two types of lighting with customizable colors and movement through GUI or ACTIONS\
[X] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

### Group A:

[ ] Frame-buffers with post-processing   
[X] Instancing  
[ ] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:
[X] Bloom with the use of HDR  
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

[X] U okviru Mesha dodana mogućnost crtanja istanciranih modela, uokvireno u Modelu za korišćenje.
Napravljena klasa PostProcessing u engine/include/engine/graphics a implemntaicja u engine/src.
U njoj je omogućeno korišćenje HDR i bloom efekta. U GraphicsController-u  su dodate i izmenjene metode,
kako bi bilo moguće koristiti HDR i bloom u App delu preko njega.


## Models:

zvezda: [https://sketchfab.com/3d-models/christmas-star-de23837809ab4afa92e73bc3244100d4 ]  
oblak: [https://sketchfab.com/3d-models/cloud-from-super-mario-galaxy-927910b2729a4041bfbf6005d902a8fb ]\
pahuljica: [https://sketchfab.com/3d-models/snowflake-5cb68fa2bd1a43eca4f0fc7f5c676a8d ]  
kuca: [https://sketchfab.com/3d-models/snowy-wooden-hut-857c02ab61834df8a8d125ccce6fec2a ]\
svi ostali modeli: [https://drive.google.com/drive/folders/1vMCZej9C5V0uc4RgKrinMHS6OM1IaY2g]

## Textures

Skybox: [https://drive.google.com/drive/folders/1vMCZej9C5V0uc4RgKrinMHS6OM1IaY2g]
