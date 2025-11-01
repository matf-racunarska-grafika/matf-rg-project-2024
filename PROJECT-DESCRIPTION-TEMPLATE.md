# Mesec nebo zvezdice

{154/2020} - {ALEKSANDRA ZELJIC}  
Kratak opis: Projekat za računarsku grafiku – scena sa mesecom, cvetom, zvezdanim nebom (skybox) i zvezdama. Implementirano je osvetljenje sa dva tipa svetla (Directional + Point) koje je moguće podešavati kroz GUI, kao i lanac događaja (ACTION → EVENT_A → EVENT_B → EVENT_C) i bloom post‑proces.

## Controls

- WASD: kretanje kamere  
- Miš: rotacija, točkić: zoom  
- F2: uklj/isklj GUI  
- B: uklj/isklj Bloom (ACTION_B)  
- SPACE: pokretanje sekvence događaja (ACTION_X)  
- A: trenutno pojača point svetlo i prebaci ga u roze nijansu (EVENT_A instant test)  
- C: prikaže/sakrije tulip (EVENT_C instant test)

## Features

### Fundamental:

- [x] Model with lighting (Phong, albedo iz texture_diffuse1)  
- [x] Two types of lighting with customizable colors and movement through GUI or ACTIONS  
- [x] {ACTION_X} --- AFTER_5_SECONDS ---> {EVENT_A (promena svetla)} ---> AFTER_3_SECONDS ---> {EVENT_B (pomeraj meseca)} ---> AFTER_4_SECONDS ---> {EVENT_C (sakrij/prikaži tulip)}

### Group A:

- [x] Frame-buffers with post-processing (korišćeno u bloom pipeline‑u)
- [ ] Instancing  
- [ ] Off-screen Anti-Aliasing  
- [ ] Parallax Mapping

### Group B:
- [x] Bloom with the use of HDR (ping‑pong blur + tone mapping)  
- [ ] Deferred Shading  
- [ ] Point Shadows  
- [ ] SSAO

### Engine improvement:

- [x] SpriteController (billboard sprite sa blending‑om, depth mask OFF tokom crtanja)  
- [x] Interni engine target (`engine_lib`) – projekat je samostalan  
- [x] Robusno učitavanje skybox‑a (relativna putanja prema trenutnom radnom direktorijumu)

## Models:

- resources/models/moon/Moon2k.obj  
- resources/models/flower/tulip.obj

## Textures

- resources/textures/star.png (sprite)  
- resources/skyboxes/skybox/{right,left,top,bottom,front,back}.jpeg

## Napomene

- Osvetljenje: `basic.glsl` – DirLight + PointLight (ambient/diffuse/specular, attenuation), shininess iz GUI‑ja.  
- Događaji: SPACE pokreće sekvencu (A na 5s, B na 8s, C na 12s; reset na 18s). A/C imaju i direktne test tastere.  
- Bloom: `BloomController` (HDR FBO + ping‑pong blur + final tone mapping), toggle taster B.
