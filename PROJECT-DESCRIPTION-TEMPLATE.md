# {NAME-OF-THE-PROJECT}

43/2022 - Filip Nedeljković  
Ostrvo u svemiru oko kog rotiraju rotiraju sunce i mesec.
Na ostrvu se nalazi park sa drvećem, žbunjem, stazom, klupama, lampama.
Sunce i mesec osvetljavaju ostrvo.
Lampe se pale i gase u zavisnosti od pozicije sunca i meseca.
U meniju je moguće nameštati boju svetla lampi i brzinu revolucije sunca i meseca.

## Controls

W -> Pravo\
A -> Levo\
S -> Dole\
D -> Desno\
Space -> Gore\
L_Shift -> Dole\
M -> Otvaranje menija

## Features

### Fundamental:

[X] Model with lighting\
[X] Two types of lighting with customizable colors and movement through GUI or ACTIONS\
[X] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

### Group A:

[ ] Frame-buffers with post-processing   
[ ] Instancing  
[X] Off-screen Anti-Aliasing  
[ ] Parallax Mapping

### Group B:

[ ] Bloom with the use of HDR  
[ ] Deferred Shading  
[X] Point Shadows  
[ ] SSAO

### Engine improvement:

[X] Dodata klasa Framebuffer u engine/graphics koja sadrži podatke za offscreen-aa.
U GraphicsController dodate metode pomoću kojih se implementira antialiasing u projektu.
U engine/graphics dodata klasa PointShadowCaster, implementacije metoda klase su u engine/src.
U GraphicsController dodate metode pomoću kojih je moguće baratati nizom point svetala sa senkama.

## Models:

(Svi modeli sa ovog linka)\
https://drive.google.com/drive/folders/1vMCZej9C5V0uc4RgKrinMHS6OM1IaY2g

## Textures

(Skybox)\
https://ar.inspiredpencil.com/pictures-2023/stars-skybox-unity
