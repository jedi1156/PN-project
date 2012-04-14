#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "menu.h"
#include "game.h"
#include "loading.h"

void* load_level(ALLEGRO_THREAD *thread, void *argument){
    printf("Loading... level\n");

    struct GameSharedData *Data = (struct GameSharedData*) argument;

    printf("Loading finished\n");
    return NULL;
};


int abs(int a){
    return a<0 ? -a : a;
}


void menu_elem_init(struct menu_elem*Item,
                    enum menu_elem_type NewType,
                    char *NewName,
                    void *NewActivate){
    Item->Type = NewType;
    Item->Name = NewName;
    Item->Activate = NewActivate;
}

int main(){

    if(!al_init()){
        fprintf(stderr, "Problems when initilizing Allegro");
        return -1;
    }

    struct GameSharedData Data;
    Data.Display = NULL;
    Data.GameState = gsMENU;
    Data.CloseNow = false;
    Data.DrawCall = false;

    /**
        Menu initialization
        */
    //this needs to be somehow compressed... macroes?

    struct menu_elem MainMenu[1 + abs(MAIN_MENU_SIZE)],
                     OptionsMenu[1 + abs(OPTIONS_MENU_SIZE)],
                     HighScoresMenu[1 + abs(HIGH_SCORES_MENU_SIZE)],
                     GraphicMenu[1 + abs(GRAPHIC_MENU_SIZE)],
                     SoundMenu[1 + abs(SOUND_MENU_SIZE)],
                     ControlsMenu[1 + abs(CONTROLS_MENU_SIZE)];

    menu_elem_init(&MainMenu[mmeDESCRIPTOR],
                   MAIN_MENU_SIZE,
                   "MAIN MENU",
                   &MainMenu);
    menu_elem_init(&MainMenu[mmeNEWGAME],
                   metACTIVATE,
                   "NEW GAME",
                   (void*) new_game_activate);
    menu_elem_init(&MainMenu[mmeOPTIONS],
                   metSUBMENU,
                   "OPTIONS",
                   (void*) &OptionsMenu);
    menu_elem_init(&MainMenu[mmeHIGHSCORES],
                   metSUBMENU,
                   "HIGH SCORES",
                   (void*) &HighScoresMenu);
    menu_elem_init(&MainMenu[mmeEXIT],
                   metACTIVATE,
                   "EXIT",
                   (void*) exit_activate);

    menu_elem_init(&OptionsMenu[omeDESCRIPTOR],
                   OPTIONS_MENU_SIZE,
                   "OPTIONS",
                   &MainMenu);
    menu_elem_init(&OptionsMenu[omeGRAPHIC],
                   metSUBMENU,
                   "GRAPHIC",
                   &GraphicMenu);
    menu_elem_init(&OptionsMenu[omeSOUND],
                   metSUBMENU,
                   "SOUND",
                   &SoundMenu);
    menu_elem_init(&OptionsMenu[omeCONTROLS],
                   metSUBMENU,
                   "CONTROLS",
                   &ControlsMenu);
    menu_elem_init(&OptionsMenu[omeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &MainMenu);

    menu_elem_init(&GraphicMenu[gmeDESCRIPTOR],
                   GRAPHIC_MENU_SIZE,
                   "GRAPHIC",
                   &OptionsMenu);
    menu_elem_init(&GraphicMenu[gmeRESOLUTION],
                   metUPDOWN,
                   "RESOLUTION",
                   &resolution_activate);
    menu_elem_init(&GraphicMenu[gmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &OptionsMenu);

    menu_elem_init(&SoundMenu[smeDESCRIPTOR],
                   SOUND_MENU_SIZE,
                   "SOUND",
                   &OptionsMenu);
    menu_elem_init(&SoundMenu[smeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &OptionsMenu);

    menu_elem_init(&ControlsMenu[cmeDESCRIPTOR],
                   CONTROLS_MENU_SIZE,
                   "CONTROLS",
                   &OptionsMenu);
    menu_elem_init(&ControlsMenu[cmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &OptionsMenu);


    menu_elem_init(&HighScoresMenu[hsmeDESCRIPTOR],
                   HIGH_SCORES_MENU_SIZE,
                   "HIGH SCORES",
                   &MainMenu);
    menu_elem_init(&HighScoresMenu[hsmeRETURN],
                   metSUBMENU,
                   "RETURN",
                   &MainMenu);


    Data.Menu.CurrentMenu = &MainMenu[0];
    Data.Menu.Current = 1;

    /**
        Initializing fonts
        */
    al_init_font_addon();
    al_init_ttf_addon();

    /**
        Getting resolution
        */

    Data.MaxResolutionIndex = al_get_num_display_modes() - 1;
    Data.ChosenResolution = Data.MaxResolutionIndex;

    al_get_display_mode(Data.ChosenResolution, &Data.DisplayData);
    Data.InMenuDisplayData = Data.DisplayData;
    Data.ChosenInMenu = Data.ChosenResolution;

    /**
        Creating display
        */

    al_set_new_display_flags(ALLEGRO_FULLSCREEN); //ALLEGRO_WINDOWED
    Data.Display = al_create_display(Data.DisplayData.width, Data.DisplayData.height);

    if(!Data.Display){
        fprintf(stderr, "Problems when creating the display");
        return -1;
    }


    /**
        Setting font size accordingly to resolution
        */
    scale_fonts(&Data);

    /**
        Initializing keyboard
        */
    if(!al_install_keyboard()) {
        fprintf(stderr, "failed to initialize the keyboard!\n");
        printf("Nie możesz grać w tą grę bez klawiatury gościu...");
        return -1;
    }

    /**
        Initializing mouse
        */
    if(!al_install_mouse()) {
        fprintf(stderr, "failed to initialize the mouse!\n");
        Data.MouseWorking = false;
    }
    else{
        Data.MouseWorking = true;
    }

    /**
        Initializing event_queue
        */
    Data.MainEventQueue = NULL;
    Data.MainEventQueue = al_create_event_queue();
    if(!Data.MainEventQueue) {
        fprintf(stderr, "failed to create main_event_queue!\n");
        al_destroy_display(Data.Display);
        return -1;
    }
    al_register_event_source(Data.MainEventQueue, al_get_keyboard_event_source());
    al_register_event_source(Data.MainEventQueue, al_get_display_event_source(Data.Display));
    al_register_event_source(Data.MainEventQueue, al_get_mouse_event_source());

    /**
        Initializing data
        */

    Data.Level.LevelNumber = 0;
    Data.Level.NumberOfMovableObjects = 0;
    Data.Level.NumberOfFixedObjects = 0;
    Data.Level.Fixed = NULL;
    Data.Level.Movable = NULL;

    /**
        First draw
        */
    draw_menu(&Data);

    /**
        Main loop
        */
    while(1){
        al_wait_for_event(Data.MainEventQueue, &Data.LastEvent);
        //printf("event type#%d\n", ev.type);

        switch(Data.GameState){
            case gsGAME: handle_event_game(&Data); break;
            case gsMENU: handle_event_menu(&Data); break;
            case gsPAUSE: break;
            case gsLOADING: handle_event_loading(&Data); break;
        }

        if(Data.CloseNow){
            break;
        }

        if(Data.DrawCall){
            switch(Data.GameState){
                case gsGAME: break;
                case gsLOADING: draw_loading(&Data); break;
                case gsMENU: draw_menu(&Data); break;
                case gsPAUSE: break;
            }
        }
    }



    /**
        Clean-up
        */
    al_destroy_display(Data.Display);
    al_destroy_event_queue(Data.MainEventQueue);

    return 0;
}
