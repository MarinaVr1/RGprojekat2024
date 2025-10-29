# {Underwater Bus Station}

{026/2022} - {Marina Vracaric}  
{This project represents a bus—or rather, a submarine—station located at the
bottom of the sea. A few animals are waiting at the station for their ride.
When a specific button is pressed, a submarine approaches from one side to pick up the
passengers. Enhancing the underwater atmosphere, several jellyfish cast soft blue and
purple light through the water. A bloom effect applied to the jellyfish adds a vibrant
glow to their colors. The sandy ground is rendered using parallax mapping,
giving it a wavy and realistic appearance.}

## Controls

{KEY_A} -> {camera moves to the left}  
{KEY_S} -> {camera is moving away from model}  
{KEY_D} -> {camera moves to the right}
{KEY_W} -> {camera is moving closer to model}
{KEY_G} -> {turning GUI on/off}
{KEY_UP} -> {camera is going up}
{KEY_DOWN} -> {camera is moving down}
{KEY_ESC} -> {closing program}
{KEY_LEFT} --- AFTER_1_SECOND --- {submarine arrives on station and
Gary enters (he is not visible on screen anymore)} --- AFTER_5_SECONDS --- {submarine leaves}
--> When this key is pressed again, submarine comes in different direction and Gary is again visible.
This event can be done any number of times, and depending on how many times the button has been pressed,
the submarine will approach from a different side.

## Features

### Fundamental:

[ ] Model with lighting
[ ] Two types of lighting with customizable colors and movement through GUI or ACTIONS
--> In this project, I used directional and 4 point light represented by jellyfishes.
Color of directional light can be modified through GUI.
Point lights are changing colors from purple to blue.
In GUI, attenuation of point lights can be modified, and also their positions.
[ ] {ACTION_X} --- AFTER_M_SECONDS---Triggers---> {EVENT_A} ---> AFTER_N_SECONDS---Triggers---> {EVENT_B}

### Group A:

[ ] Frame-buffers with post-processing   
[ ] Instancing  
[ ] Off-screen Anti-Aliasing  
[ yes ] Parallax Mapping

### Group B:

[ yes ] Bloom with the use of HDR  
[ ] Deferred Shading  
[ ] Point Shadows  
[ ] SSAO

### Engine improvement:

[ ] ...

## Models:

[link1] https://sketchfab.com/3d-models/underwater-stop-8052aae0cffd48758496eb6fc54fde39  
[link2] https://sketchfab.com/3d-models/gary-spongebob-b230a3f6ff634a8e94fa91febdc923cd
[link3] https://sketchfab.com/3d-models/submarine-4f3f4f26894d46c2a98a0c0396f4a245
[link4] https://sketchfab.com/3d-models/blue-jellyfish-c7bd4ef6fed94eb6a303dc4f9486028e
[link5] https://sketchfab.com/3d-models/square-plane-91724d1f1db7489dac6483bc1384a425

## Textures

[link1]  https://3dtextures.me/2020/02/21/sand-006/
[link2] Cant find link to skybox, but I left in program all 6 pictures
needed

{OTHER-RESOURCES}
