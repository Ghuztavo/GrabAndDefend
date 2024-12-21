# GrabAndDefend
Simple game coded with c++ using raylib, where you control a left hand with the keyboard and the right hand with the mouse and you have to kill the enemies before they get to the chest.
The more enemies you kill, the more score you get, and the only objective is to get as much score as you can before losing all your health points.

![title_screen](https://github.com/user-attachments/assets/5bac6900-8f55-4e4b-958e-5519583aaf0b)

#How to install
To run the code you will need to instal the raylib library from github, instal this version: raylib-5.0_win64_msvc16.
Then you need to modify the properties of the project to add the library.

#Controls

You control the left hand with W, A, S, D to move and use space to slam and destroy the enemies that collision with the slam.

![slam](https://github.com/user-attachments/assets/13f5a483-0ce7-4c8f-89f6-6384011d1e7c)

Use the mouse to control the right hand, you can use it to grab the chest and reposition it so the enemies cannot reach it, 
but if you grab the chest for too long it will drop the chest and it will enter in a cooldown.

![grab](https://github.com/user-attachments/assets/407c3a7f-248d-4b49-b3fa-0c3cf15ad18b)

You lose if the enemies reach the chest and down your health bar to 0.

#Main issues
The main problem of the code is that the window is not scalable, the code is not entirely dynamic so if you change the size of the widow the game will break.
