/*

File manager from NK's Snes 9x port to GP2x and adapted to Caprice GP2x by KaosOverride

Menu System for CapriceGP2x by KaosOverride based on NK's file manager GUI

*/
void carga_menu();
void icomenu_eval (SDL_Event icoevent);
void icomenu_vis (bool pausecreen);
void dibujamenu ();
void dibujaopcion (int opci);
void descarga_menu();
void intro_cap(bool splash);
void show_keys();
void unload_keyboard();
void load_keyboard();
void eval_keyboard(SDL_Event kevent);
void CPC_BootStartTape();
