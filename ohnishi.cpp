#include <Windows.h>
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <iostream>
#include <string>
#include <vector>
#include "base.hpp"

SDL_Window*     window = nullptr;
SDL_Renderer*   renderer = nullptr;
bool            running = false;
SDL_Texture*    LoadImageFromPath(std::string Path);
int             currentSelectedButton = 0;
int             mousePosX, mousePosY;
SDL_Texture*    map;
SDL_Rect        mapRect { 0, 125, 600, 575 };
SDL_Color       white { 0,0,0, 255 };
SDL_Texture*    bg;
SDL_Rect        bgRect { 0, 0, 1000, 700 };
SDL_Texture*    dict;
SDL_Rect        dictRect { -25, 0, 650, 700 };
SDL_Texture*    LeftArrow;
SDL_Rect        LeftArrowRect { 50, 600, 50, 50 };
SDL_Texture*    RightArrow;
SDL_Rect        RightArrowRect { 500, 600, 50, 50 };
SDL_Texture*    noEntriesText ;
int             noEntriesW, noEntriesH;
SDL_Rect        noEntriesRect;
SDL_Texture*    pageNumber = nullptr;
SDL_Rect        pageNumberRect { 250, 600, 100, 50 };
SDL_Texture*    entryName = nullptr;
SDL_Rect        entryNameRect { 305, 150, 200, 50 };
SDL_Rect        entryDescRect { 305, 200, 200, 50 };
SDL_Rect        entryDesc2Rect { 305, 250, 200, 50 };
SDL_Rect        entryLocNameRect { 305, 300, 200, 50 };
SDL_Color       buttonEnabledTint {0,0,0,0};
SDL_Color       buttonDisabledTint {0,0,0,150};
int             currentSelectedEntry;
struct Entry
{
    std::string Name        = "";
    std::string photoPath   = "";
    std::string desc        = "";
    std::string desc2       = "";
    std::string LocName     = "";
    int         LocX = 0;
    int         LocY = 0;
};
Entry*          currentEntry;
Entry*          CreateEntry(std::string name, std::string photoPath, std::string desc, std::string desc2, std::string LocName, float LocX, float LocY)
{
    Entry* entry = new Entry;
    entry->Name = name;
    entry->photoPath = photoPath;
    entry->desc = desc;
    entry->desc2 = desc2;
    entry->LocName = LocName;
    entry->LocX = LocX;
    entry->LocY = LocY;
    return entry;
}
std::vector<Entry*> Entries;
namespace CoreMenu
{
    SDL_Texture*    homeIcon = nullptr;
    SDL_Rect        homeIconRect { 0, 700, 200, 200 };
    SDL_Texture*    mapIcon = nullptr;
    SDL_Rect        mapIconRect { 200, 700, 200, 200 };
    SDL_Texture*    dictIcon = nullptr;
    SDL_Rect        dictIconRect { 400, 700, 200, 200 };
    SDL_Rect        menuBGRect { 0, 700, 600, 200 };
    SDL_Rect        menuSelectBGRect { 0, 700, 200, 250};
    SDL_Color       titleTextColor { 0, 0, 0, 255 };
    SDL_Rect        titleTextRect;
    int             titleTextw, titleTexth;
    SDL_Texture*    titleText;
    TTF_Font*       font64 = nullptr;
    TTF_Font*       font44 = nullptr;
    TTF_Font*       font24 = nullptr;
    TTF_Font*       font14 = nullptr;
    void            LoadMenuAssets()
    {
        homeIcon = LoadImageFromPath("homeicon.png");
        mapIcon  = LoadImageFromPath("mapicon.png");
        dictIcon = LoadImageFromPath("dicticon.png");
        font64 = TTF_OpenFont("font.ttf", 64);
        font44 = TTF_OpenFont("font2.ttf", 44);
        font24 = TTF_OpenFont("font2.ttf", 24);
        font14 = TTF_OpenFont("font2.ttf", 14);
        if (font64 == NULL || font44 == NULL || font24 == NULL || font14 == NULL)
        {
            std::cerr << "[ERROR] APP::FONT Cannot open font." << std::endl;
            state == State_Quit;
            running = false;
            return;
        }
        if (homeIcon == NULL || mapIcon == NULL || dictIcon == NULL)
        {
            std::cerr << "[ERROR] APP::MRES Cannot open core icons." << std::endl;
            state == State_Quit;
            running = false;
            return;
        }
    }
    void            ReloadMenuAssets()
    {
        std::string titleTextContents;
        switch (state)
        {
            case State_Home:
                titleTextContents = "Home";
                break;
            case State_Dict:
                titleTextContents = "Dictionary";
                break;
            case State_Map:
                titleTextContents = "Map";
                break;
            case State_NewEntry:
                titleTextContents = "New Entry";
                break;
        }
        titleText = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(CoreMenu::font44, titleTextContents.c_str(), titleTextColor));
        SDL_QueryTexture(titleText, NULL, NULL, &titleTextw, &titleTexth);
        titleTextRect = { 50, 50, titleTextw, titleTexth };
    }
    void            FreeMenuAssets()
    {
        SDL_DestroyTexture(homeIcon);
        SDL_DestroyTexture(mapIcon);
        SDL_DestroyTexture(dictIcon);
        SDL_DestroyTexture(titleText);
        TTF_CloseFont(font64);
        TTF_CloseFont(font44);
        TTF_CloseFont(font24);
        TTF_CloseFont(font14);
    }
    void            RenderMenu()
    {
        switch (state)
        {
            case State_Home:
                menuSelectBGRect = { 0, 700, 200, 250 };
                break;
            case State_NewEntry:
                menuSelectBGRect = { 0, 700, 200, 250 };
                break;
            case State_Map:
                menuSelectBGRect = { 200, 700, 200, 250 };
                break;
            case State_Dict:
                menuSelectBGRect = { 400, 700, 200, 250 };
                break;
        }
        SDL_RenderCopy(renderer, titleText, NULL, &titleTextRect);
        if (state != State_NewEntry)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(renderer, &menuBGRect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &menuSelectBGRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_RenderCopy(renderer, homeIcon, NULL, &homeIconRect);
            SDL_RenderCopy(renderer, mapIcon, NULL, &mapIconRect);
            SDL_RenderCopy(renderer, dictIcon, NULL, &dictIconRect);
        }
    }
    
}
SDL_Texture* LoadImageFromPath(std::string Path)
{
    SDL_Surface* surface = IMG_Load(Path.c_str());
    if (surface == NULL)
    {
        std::cerr << "[ERROR] APP::RESLOAD Failed to load resource " << Path << std::endl << IMG_GetError() << std::endl;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        std::cerr << "[ERROR] APP::RESLOAD Failed to load resource " << Path << std::endl << IMG_GetError() << std::endl;
    }
    SDL_FreeSurface(surface);
    return texture;
}
SDL_Texture* RenderText_Old(const char* text, const SDL_Color& color = CoreMenu::titleTextColor, int size = 64)
{
    TTF_Font* font = size == 64 ? CoreMenu::font64 : CoreMenu::font44;
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, color, 0);
    if (surface == NULL)
    {
        std::cerr << "[ERROR] APP::RESLOAD Failed to render text" << std::endl << TTF_GetError() << std::endl;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        std::cerr << "[ERROR] APP::RESLOAD_TEXT Failed to render text." << std::endl << TTF_GetError() << std::endl;
    }
    return texture;
}
SDL_Texture* RenderText(const char* text, const SDL_Rect& rect, const SDL_Color& color = CoreMenu::titleTextColor, int size = 64)
{
    TTF_Font* font = size == 64 ? CoreMenu::font64 : CoreMenu::font44;
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, color, 0);
    if (surface == NULL)
    {
        std::cerr << "[ERROR] APP::RESLOAD Failed to render text" << std::endl << TTF_GetError() << std::endl;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        std::cerr << "[ERROR] APP::RESLOAD_TEXT Failed to render text." << std::endl << TTF_GetError() << std::endl;
    }
    SDL_FreeSurface(surface);
    if (color.a != 255)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    if (color.a != 255)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    SDL_DestroyTexture(texture);
    return texture;
}

void NewEntryWizard()
{
    CoreMenu::ReloadMenuAssets();
    SDL_Event event;
    std::string newEntry_Name       = "Name";
    std::string newEntry_Path       = "Path";
    std::string newEntry_LocName    = "Location Name";
    std::string newEntry_Desc       = "Desc";
    std::string newEntry_Desc2      = "Desc Line.2";
    std::string newEntry_LocX = "LocX";
    std::string newEntry_LocY = "LocY";
    int currentFocusTextField = -1; // 0=Name, 1=Desc, 2=Path, 3=LocName, 4=LocX, 5=LocY
    int errorTextField = -1;
    SDL_Color normalTextFieldColor { 0, 0, 0, 100 };
    SDL_Color focusedTextFieldColor { 0, 0, 0, 200 };

    SDL_Texture* nameField;
    SDL_Rect nameFieldRect      { 50, 200, 500, 44 };
    SDL_Rect nFTR               { 50, 200, 500, 44 };
    int nFRx;
    SDL_Texture* pathField;
    SDL_Rect PathFieldRect      { 50, 350, 500, 44 };
    SDL_Rect pFTR               { 50, 350, 500, 44 };
    int pFRx;
    SDL_Texture* locNameField;
    SDL_Rect LocNameFieldRect   { 50, 400, 500, 44 };
    SDL_Rect lnFTR               { 50, 400, 500, 44 };
    int lnFRx;
    SDL_Texture* descField;
    SDL_Rect DescFieldRect      { 50, 250, 500, 44 };
    SDL_Rect dFTR               { 50, 250, 500, 44 };
    int dFRx;
    SDL_Texture* desc2Field;
    SDL_Rect Desc2FieldRect     { 50, 300, 500, 44 };
    SDL_Rect d2FTR              { 50, 300, 500, 44 };
    int d2FRx;
    SDL_Texture* LocXField;
    SDL_Rect LocXFieldRect      { 50, 450, 200, 44 };
    SDL_Rect lxFTR              { 50, 450, 200, 44 };
    int lxFRx;
    SDL_Texture* LocYField;
    SDL_Rect LocYFieldRect      { 350, 450, 200, 44 };
    SDL_Rect lyFTR              { 350, 450, 200, 44 };
    int lyFRx;

    while (state == State_NewEntry)
    {
        while (SDL_PollEvent(&event))
        {
            SDL_GetMouseState(&mousePosX, &mousePosY);
            if (event.type == SDL_QUIT)
            {
                state == State_Quit;
                running = false;
                return;
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (currentFocusTextField)
                {
                    case 0:
                        if (newEntry_Name == "Name")
                        {
                            newEntry_Name = "";
                        }
                        break;
                    case 1:
                        if (newEntry_Desc == "Desc")
                        {
                            newEntry_Desc = "";
                        }
                        break;
                    case 2:
                        if (newEntry_Desc2 == "Desc Line.2")
                        {
                            newEntry_Desc2 = "";
                        }
                        break;
                    case 3:
                        if (newEntry_Path == "Path")
                        {
                            newEntry_Path = "";
                        }
                        break;
                    case 4:
                        if (newEntry_LocName == "Location Name")
                        {
                            newEntry_LocName = "";
                        }
                        break;
                    case 5:
                        if (newEntry_LocX == "LocX")
                        {
                            newEntry_LocX = "0";
                        }
                        break;
                    case 6:
                        if (newEntry_LocY == "LocY")
                        {
                            newEntry_LocY = "0";
                        }
                        break;
                }
                switch (event.key.keysym.sym)
                {
                case SDLK_TAB:
                  if (currentFocusTextField < 6) currentFocusTextField++;
                  break;
                    case SDLK_a:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "a";
                                break;
                            case 1:
                                newEntry_Desc += "a";
                                break;
                            case 2:
                                newEntry_Desc2 += "a";
                                break;
                            case 3:
                                newEntry_Path += "a";
                                break;
                            case 4:
                                newEntry_LocName += "a";
                                break;
                        }
                        break;
                    case SDLK_b:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "b";
                                break;
                            case 1:
                                newEntry_Desc += "b";
                                break;
                            case 2:
                                newEntry_Desc2 += "b";
                                break;
                            case 3:
                                newEntry_Path += "b";
                                break;
                            case 4:
                                newEntry_LocName += "b";
                                break;
                        }
                        break;
                    case SDLK_c:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "c";
                                break;
                            case 1:
                                newEntry_Desc += "c";
                                break;
                            case 2:
                                newEntry_Desc2 += "c";
                                break;
                            case 3:
                                newEntry_Path += "c";
                                break;
                            case 4:
                                newEntry_LocName += "c";
                                break;
                        }
                        break;
                    case SDLK_d:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "d";
                                break;
                            case 1:
                                newEntry_Desc += "d";
                                break;
                            case 2:
                                newEntry_Desc2 += "d";
                                break;
                            case 3:
                                newEntry_Path += "d";
                                break;
                            case 4:
                                newEntry_LocName += "d";
                                break;
                        }
                        break;
                    case SDLK_e:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "e";
                                break;
                            case 1:
                                newEntry_Desc += "e";
                                break;
                            case 2:
                                newEntry_Desc2 += "e";
                                break;
                            case 3:
                                newEntry_Path += "e";
                                break;
                            case 4:
                                newEntry_LocName += "e";
                                break;
                        }
                        break;
                    case SDLK_f:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "f";
                                break;
                            case 1:
                                newEntry_Desc += "f";
                                break;
                            case 2:
                                newEntry_Desc2 += "f";
                                break;
                            case 3:
                                newEntry_Path += "f";
                                break;
                            case 4:
                                newEntry_LocName += "f";
                                break;
                        }
                        break;
                    case SDLK_g:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "g";
                                break;
                            case 1:
                                newEntry_Desc += "g";
                                break;
                            case 2:
                                newEntry_Desc2 += "g";
                                break;
                            case 3:
                                newEntry_Path += "g";
                                break;
                            case 4:
                                newEntry_LocName += "g";
                                break;
                        }
                        break;
                    case SDLK_h:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "h";
                                break;
                            case 1:
                                newEntry_Desc += "h";
                                break;
                            case 2:
                                newEntry_Desc2 += "h";
                                break;
                            case 3:
                                newEntry_Path += "h";
                                break;
                            case 4:
                                newEntry_LocName += "h";
                                break;
                        }
                        break;
                    case SDLK_i:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "i";
                                break;
                            case 1:
                                newEntry_Desc += "i";
                                break;
                            case 2:
                                newEntry_Desc2 += "i";
                                break;
                            case 3:
                                newEntry_Path += "i";
                                break;
                            case 4:
                                newEntry_LocName += "i";
                                break;
                        }
                        break;
                    case SDLK_j:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "j";
                                break;
                            case 1:
                                newEntry_Desc += "j";
                                break;
                            case 2:
                                newEntry_Desc2 += "j";
                                break;
                            case 3:
                                newEntry_Path += "j";
                                break;
                            case 4:
                                newEntry_LocName += "j";
                                break;
                        }
                        break;
                    case SDLK_k:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "k";
                                break;
                            case 1:
                                newEntry_Desc += "k";
                                break;
                            case 2:
                                newEntry_Desc2 += "k";
                                break;
                            case 3:
                                newEntry_Path += "k";
                                break;
                            case 4:
                                newEntry_LocName += "k";
                                break;
                        }
                        break;
                    case SDLK_l:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "l";
                                break;
                            case 1:
                                newEntry_Desc += "l";
                                break;
                            case 2:
                                newEntry_Desc2 += "l";
                                break;
                            case 3:
                                newEntry_Path += "l";
                                break;
                            case 4:
                                newEntry_LocName += "l";
                                break;
                        }
                        break;
                    case SDLK_m:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "m";
                                break;
                            case 1:
                                newEntry_Desc += "m";
                                break;
                            case 2:
                                newEntry_Desc2 += "m";
                                break;
                            case 3:
                                newEntry_Path += "m";
                                break;
                            case 4:
                                newEntry_LocName += "m";
                                break;
                        }
                        break;
                    case SDLK_n:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "n";
                                break;
                            case 1:
                                newEntry_Desc += "n";
                                break;
                            case 2:
                                newEntry_Desc2 += "n";
                                break;
                            case 3:
                                newEntry_Path += "n";
                                break;
                            case 4:
                                newEntry_LocName += "n";
                                break;
                        }
                        break;
                    case SDLK_o:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "o";
                                break;
                            case 1:
                                newEntry_Desc += "o";
                                break;
                            case 2:
                                newEntry_Desc2 += "o";
                                break;
                            case 3:
                                newEntry_Path += "o";
                                break;
                            case 4:
                                newEntry_LocName += "o";
                                break;
                        }
                        break;
                    case SDLK_p:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "p";
                                break;
                            case 1:
                                newEntry_Desc += "p";
                                break;
                            case 2:
                                newEntry_Desc2 += "p";
                                break;
                            case 3:
                                newEntry_Path += "p";
                                break;
                            case 4:
                                newEntry_LocName += "p";
                                break;
                        }
                        break;
                    case SDLK_q:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "q";
                                break;
                            case 1:
                                newEntry_Desc += "q";
                                break;
                            case 2:
                                newEntry_Desc2 += "q";
                                break;
                            case 3:
                                newEntry_Path += "q";
                                break;
                            case 4:
                                newEntry_LocName += "q";
                                break;
                        }
                        break;
                    case SDLK_r:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "r";
                                break;
                            case 1:
                                newEntry_Desc += "r";
                                break;
                            case 2:
                                newEntry_Desc2 += "r";
                                break;
                            case 3:
                                newEntry_Path += "r";
                                break;
                            case 4:
                                newEntry_LocName += "r";
                                break;
                        }
                        break;
                    case SDLK_s:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "s";
                                break;
                            case 1:
                                newEntry_Desc += "s";
                                break;
                            case 2:
                                newEntry_Desc2 += "s";
                                break;
                            case 3:
                                newEntry_Path += "s";
                                break;
                            case 4:
                                newEntry_LocName += "s";
                                break;
                        }
                        break;
                    case SDLK_t:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "t";
                                break;
                            case 1:
                                newEntry_Desc += "t";
                                break;
                            case 2:
                                newEntry_Desc2 += "t";
                                break;
                            case 3:
                                newEntry_Path += "t";
                                break;
                            case 4:
                                newEntry_LocName += "t";
                                break;
                        }
                        break;
                    case SDLK_u:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "u";
                                break;
                            case 1:
                                newEntry_Desc += "u";
                                break;
                            case 2:
                                newEntry_Desc2 += "u";
                                break;
                            case 3:
                                newEntry_Path += "u";
                                break;
                            case 4:
                                newEntry_LocName += "u";
                                break;
                        }
                        break;
                    case SDLK_v:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "v";
                                break;
                            case 1:
                                newEntry_Desc += "v";
                                break;
                            case 2:
                                newEntry_Desc2 += "v";
                                break;
                            case 3:
                                newEntry_Path += "v";
                                break;
                            case 4:
                                newEntry_LocName += "v";
                                break;
                        }
                        break;
                    case SDLK_w:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "w";
                                break;
                            case 1:
                                newEntry_Desc += "w";
                                break;
                            case 2:
                                newEntry_Desc2 += "w";
                                break;
                            case 3:
                                newEntry_Path += "w";
                                break;
                            case 4:
                                newEntry_LocName += "w";
                                break;
                        }
                        break;
                    case SDLK_x:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "x";
                                break;
                            case 1:
                                newEntry_Desc += "x";
                                break;
                            case 2:
                                newEntry_Desc2 += "x";
                                break;
                            case 3:
                                newEntry_Path += "x";
                                break;
                            case 4:
                                newEntry_LocName += "x";
                                break;
                        }
                        break;
                    case SDLK_y:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "y";
                                break;
                            case 1:
                                newEntry_Desc += "y";
                                break;
                            case 2:
                                newEntry_Desc2 += "y";
                                break;
                            case 3:
                                newEntry_Path += "y";
                                break;
                            case 4:
                                newEntry_LocName += "y";
                                break;
                        }
                        break;
                    case SDLK_z:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "z";
                                break;
                            case 1:
                                newEntry_Desc += "z";
                                break;
                            case 2:
                                newEntry_Desc2 += "z";
                                break;
                            case 3:
                                newEntry_Path += "z";
                                break;
                            case 4:
                                newEntry_LocName += "z";
                                break;
                        }
                        break;
                    case SDLK_BACKSPACE:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                if (!newEntry_Name.empty()) newEntry_Name.pop_back();
                                break;
                            case 1:
                                if (!newEntry_Desc.empty()) newEntry_Desc.pop_back();
                                break;
                            case 2:
                                if (!newEntry_Desc2.empty()) newEntry_Desc2.pop_back();
                                break;
                            case 3:
                                if (!newEntry_Path.empty()) newEntry_Path.pop_back();
                                break;
                            case 4:
                                if (!newEntry_LocName.empty()) newEntry_LocName.pop_back();
                                break;
                            case 5:
                                if (!newEntry_LocX.empty()) newEntry_LocX.pop_back();
                                break;
                            case 6:
                                if (!newEntry_LocY.empty()) newEntry_LocY.pop_back();
                                break;
                        }
                        break;
                    case SDLK_PERIOD:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += ".";
                                break;
                            case 1:
                                newEntry_Desc += ".";
                                break;
                            case 2:
                                newEntry_Desc2 += ".";
                                break;
                            case 3:
                                newEntry_Path += ".";
                                break;
                            case 4:
                                newEntry_LocName += ".";
                                break;
                        }
                        break;

                    case SDLK_1:
                        switch (currentFocusTextField)
                        {
                            case 0:
                                newEntry_Name += "1";
                                break;
                            case 1:
                                newEntry_Desc += "1";
                                break;
                            case 2:
                                newEntry_Desc2 += "1";
                                break;
                            case 3:
                                newEntry_Path += "1";
                                break;
                            case 4:
                                newEntry_LocName += "1";
                                break;
                            case 5:
                                newEntry_LocX += "1";
                                break;
                            case 6:
                                newEntry_LocY += "1";
                                break;
                        }
                        break;
                    case SDLK_2:
                      switch (currentFocusTextField)
                      {
                      case 0:
                        newEntry_Name += "2";
                        break;
                      case 1:
                        newEntry_Desc += "2";
                        break;
                      case 2:
                        newEntry_Desc2 += "2";
                        break;
                      case 3:
                        newEntry_Path += "2";
                        break;
                      case 4:
                        newEntry_LocName += "2";
                        break;
                      case 5:
                        newEntry_LocX += "2";
                        break;
                      case 6:
                        newEntry_LocY += "2";
                        break;
                      }
                      break;
                    case SDLK_3:
                      switch (currentFocusTextField)
                      {
                      case 0:
                        newEntry_Name += "3";
                        break;
                      case 1:
                        newEntry_Desc += "3";
                        break;
                      case 2:
                        newEntry_Desc2 += "3";
                        break;
                      case 3:
                        newEntry_Path += "3";
                        break;
                      case 4:
                        newEntry_LocName += "3";
                        break;
                      case 5:
                        newEntry_LocX += "3";
                        break;
                      case 6:
                        newEntry_LocY += "3";
                        break;
                      }
                      break;
                    case SDLK_4:
                      switch (currentFocusTextField)
                      {
                      case 0:
                        newEntry_Name += "4";
                        break;
                      case 1:
                        newEntry_Desc += "4";
                        break;
                      case 2:
                        newEntry_Desc2 += "4";
                        break;
                      case 3:
                        newEntry_Path += "4";
                        break;
                      case 4:
                        newEntry_LocName += "4";
                        break;
                      case 5:
                        newEntry_LocX += "4";
                        break;
                      case 6:
                        newEntry_LocY += "4";
                        break;
                      }
                      break;
                    case SDLK_RETURN:
                        if (newEntry_Name == "")
                        {
                            errorTextField = 0;
                            break;
                        }
                        else if (newEntry_Desc == "")
                        {
                            errorTextField = 1;
                            break;
                        }
                        else if (newEntry_Desc2 == "")
                        {
                            errorTextField = 2;
                            break;
                        }
                        else if (newEntry_Path == "")
                        {
                            errorTextField = 3;
                            break;
                        }
                        else if (newEntry_LocName == "")
                        {
                            errorTextField = 4;
                            break;
                        }
                        else if (newEntry_LocX == "LocX")
                        {
                            errorTextField = 5;
                            break;
                        }
                        else if (newEntry_LocY == "LocY")
                        {
                            errorTextField = 6;
                            break;
                        }
                        Entries.push_back(CreateEntry(newEntry_Name, newEntry_Path, newEntry_Desc, newEntry_Desc2, newEntry_LocName, std::stoi(newEntry_LocX), std::stoi(newEntry_LocY)));
                        state=State_Dict;
                        break;
                    case SDLK_ESCAPE:
                        state = State_Home;
                        currentSelectedButton = 0;
                        return;
                        break;
                }
            }
        }

        
        SDL_SetRenderDrawColor(renderer, 245, 245, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        if (errorTextField == 0) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 150);
        SDL_RenderFillRect(renderer, &nameFieldRect);
        if (errorTextField == 0) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        if (errorTextField == 1) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 150);
        SDL_RenderFillRect(renderer, &DescFieldRect);
        if (errorTextField == 1) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        if (errorTextField == 2) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 150);
        SDL_RenderFillRect(renderer, &Desc2FieldRect);
        if (errorTextField == 2) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
         
        if (errorTextField == 3) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 150);
        SDL_RenderFillRect(renderer, &PathFieldRect);
        if (errorTextField == 3) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        if (errorTextField == 4) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 150);
        SDL_RenderFillRect(renderer, &LocNameFieldRect);
        if (errorTextField == 4) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        SDL_RenderFillRect(renderer, &LocXFieldRect);
        SDL_RenderFillRect(renderer, &LocYFieldRect);

        if (mousePosX <= 50+500 && mousePosX >= 50 && mousePosY <= 200+44 && mousePosY >= 200)
        {
            currentFocusTextField = 0;
            nameField = RenderText(newEntry_Name.c_str(), nFTR, focusedTextFieldColor);
        }
        else
        {
            nameField = RenderText(newEntry_Name.c_str(), nFTR, normalTextFieldColor);
        }

        if (mousePosX <= 50+500 && mousePosX >= 50 && mousePosY <= 250+44 && mousePosY >= 250)
        {
            currentFocusTextField = 1;
            descField = RenderText(newEntry_Desc.c_str(), dFTR, focusedTextFieldColor);
        }
        else
        {
            descField = RenderText(newEntry_Desc.c_str(), dFTR, normalTextFieldColor);
        }

        if (mousePosX <= 50+500 && mousePosX >= 50 && mousePosY <= 300+44 && mousePosY >= 300)
        {
            currentFocusTextField = 2;
            desc2Field = RenderText(newEntry_Desc2.c_str(), d2FTR, focusedTextFieldColor);
        }
        else
        {
            desc2Field = RenderText(newEntry_Desc2.c_str(), d2FTR, normalTextFieldColor);
        }

        if (mousePosX <= 50+500 && mousePosX >= 50 && mousePosY <= 350+44 && mousePosY >= 350)
        {
            currentFocusTextField = 3;
            pathField = RenderText(newEntry_Path.c_str(), pFTR, focusedTextFieldColor);
        }
        else
        {
            pathField = RenderText(newEntry_Path.c_str(), pFTR, normalTextFieldColor);
        }

        if (mousePosX <= 50+500 && mousePosX >= 50 && mousePosY <= 400+44 && mousePosY >= 400)
        {
            currentFocusTextField = 4;
            locNameField = RenderText(newEntry_LocName.c_str(), lnFTR, focusedTextFieldColor);
        }
        else
        {
            locNameField = RenderText(newEntry_LocName.c_str(), lnFTR, normalTextFieldColor);
        }

        if (mousePosX <= 50+200 && mousePosX >= 50 && mousePosY <= 450+44 && mousePosY >= 450)
        {
            currentFocusTextField = 5;
            LocXField = RenderText(newEntry_LocX.c_str(), lxFTR, focusedTextFieldColor);
        }
        else
        {
            LocXField = RenderText(newEntry_LocX.c_str(), lxFTR, normalTextFieldColor);
        }

        if (mousePosX <= 350+200 && mousePosX >= 350 && mousePosY <= 450+44 && mousePosY >= 450)
        {
            currentFocusTextField = 6;
            LocYField = RenderText(newEntry_LocY.c_str(), lyFTR, focusedTextFieldColor);
        }
        else
        {
            LocYField = RenderText(newEntry_LocY.c_str(), lyFTR, normalTextFieldColor);
        }

        CoreMenu::RenderMenu();
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);
    }
}

SDL_Color black { 0,0,0, 255 };
SDL_Texture* newEntry;
int newEntryw, newEntryh;
SDL_Rect newEntryRect;
SDL_Texture* contentText;
int contentTextw, contentTexth;
SDL_Rect contentTextRect;

void LoadHome()
{
    newEntry = RenderText_Old("+");
    SDL_QueryTexture(newEntry, NULL, NULL, &newEntryw, &newEntryh);
    newEntryRect = { 500, 36, newEntryw, newEntryh };
    
    contentText = RenderText_Old("This is a \n newline?");
    SDL_QueryTexture(contentText, NULL, NULL, &contentTextw, &contentTexth);
    contentTextRect = { 50, 150, contentTextw, contentTexth};
}

void GoHome()
{
    CoreMenu::ReloadMenuAssets();
    
    SDL_Event event;
    while (state == State_Home)
    {
        while (SDL_PollEvent(&event))
        {
            SDL_GetMouseState(&mousePosX, &mousePosY);
            if (event.type == SDL_QUIT)
            {
                state == State_Quit;
                running = false;
                return;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if ((mousePosX <= 200 && mousePosX >= 0) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 0;
                }
                if ((mousePosX <= 400 && mousePosX > 200) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 1;
                }
                if ((mousePosX <= 600 && mousePosX > 400) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 2;
                }
                if ((mousePosX <= (500 + newEntryw) && mousePosX > 500) && (mousePosY <= (25 + newEntryh) && mousePosX > 25))
                {
                    currentSelectedButton = 3;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_PAGEUP:
                        if (currentSelectedButton > 0)
                        {
                            currentSelectedButton--;
                        }
                        break;
                    case SDLK_PAGEDOWN:
                        if (currentSelectedButton < 2)
                        {
                            currentSelectedButton++;
                        }
                        break;
                    case SDLK_ESCAPE:
                        state = State_Quit;
                        running = false;
                        return;
                        break;
                }
            }
        }

        switch (currentSelectedButton)
        {
            case 0:
            state=State_Home;
            break;
            case 1:
            state=State_Map;
            break;
            case 2:
            state=State_Dict;
            break;
            case 3:
            state=State_NewEntry;
            break;
        }

        SDL_SetRenderDrawColor(renderer, 245, 245, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, newEntry, NULL, &newEntryRect);
        SDL_RenderCopy(renderer, contentText, NULL, &contentTextRect);

        CoreMenu::RenderMenu();
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);
    }
}

void LoadMap()
{
    map = LoadImageFromPath("mapSample.png");

}

void GoMap()
{
    CoreMenu::ReloadMenuAssets();

    SDL_Event event;
    
    while (state == State_Map)
    {
        while (SDL_PollEvent(&event))
        {
            SDL_GetMouseState(&mousePosX, &mousePosY);
            if (event.type == SDL_QUIT)
            {
                state == State_Quit;
                running = false;
                return;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if ((mousePosX <= 200 && mousePosX >= 0) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 0;
                }
                if ((mousePosX <= 400 && mousePosX > 200) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 1;
                }
                if ((mousePosX <= 600 && mousePosX > 400) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 2;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_PAGEUP:
                        if (currentSelectedButton > 0)
                        {
                            currentSelectedButton--;
                        }
                        break;
                    case SDLK_PAGEDOWN:
                        if (currentSelectedButton < 2)
                        {
                            currentSelectedButton++;
                        }
                        break;
                    case SDLK_ESCAPE:
                        state = State_Quit;
                        running = false;
                        return;
                        break;
                }
            }
        }

        switch (currentSelectedButton)
        {
            case 0:
            state=State_Home;
            break;
            case 1:
            state=State_Map;
            break;
            case 2:
            state=State_Dict;
            break;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, map, NULL, &mapRect);

        for (const auto entry : Entries)
        {
            SDL_Rect rect { entry->LocX, entry->LocY, 50, 50 };
            SDL_RenderCopy(renderer, LoadImageFromPath(entry->photoPath), NULL, &rect);
        }

        CoreMenu::RenderMenu();
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);
    }
};

void LoadDict()
{
    bg = LoadImageFromPath("dictbg.png");
    dict = LoadImageFromPath("dictOpen.png");
    LeftArrow = RenderText_Old("<");
    RightArrow = RenderText_Old(">");

    noEntriesText = RenderText_Old("No Entries");
    SDL_QueryTexture(noEntriesText, NULL, NULL, &noEntriesW, &noEntriesH);
    noEntriesRect = { (600-noEntriesW) / 2, (700 - noEntriesH) / 2, noEntriesW, noEntriesH};
}

SDL_Surface* e;
SDL_Texture* e2;
SDL_Texture* entryImage;
SDL_Rect entryImageRect { 90, 150, 200, 400 };

void GoDict()
{
    CoreMenu::ReloadMenuAssets();

    SDL_Event event;
    while (state == State_Dict)
    {
        while (SDL_PollEvent(&event))
        {
            SDL_GetMouseState(&mousePosX, &mousePosY);
            if (event.type == SDL_QUIT)
            {
                state == State_Quit;
                running = false;
                return;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if ((mousePosX <= 200 && mousePosX >= 0) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 0;
                }
                if ((mousePosX <= 400 && mousePosX > 200) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 1;
                }
                if ((mousePosX <= 600 && mousePosX > 400) && (mousePosY <= 900 && mousePosY > 700))
                {
                    currentSelectedButton = 2;
                }
                if ((mousePosX <= 100 && mousePosX > 50) && (mousePosY <= 650 && mousePosY > 600))
                {
                    if (currentSelectedEntry > 0) currentSelectedEntry--;
                }
                if ((mousePosX <= 550 && mousePosX > 500) && (mousePosY <= 650 && mousePosY > 600))
                {
                    if (currentSelectedEntry < Entries.size()-1) currentSelectedEntry++;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_PAGEUP:
                        if (currentSelectedButton > 0)
                        {
                            currentSelectedButton--;
                        }
                        break;
                    case SDLK_PAGEDOWN:
                        if (currentSelectedButton < 2)
                        {
                            currentSelectedButton++;
                        }
                        break;
                    case SDLK_ESCAPE:
                        state = State_Quit;
                        running = false;
                        return;
                        break;
                }
            }
        }

        switch (currentSelectedButton)
        {
            case 0:
            state=State_Home;
            break;
            case 1:
            state=State_Map;
            break;
            case 2:
            state=State_Dict;
            break;
        }

        SDL_SetRenderDrawColor(renderer, 245, 245, 255, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, bg, NULL, &bgRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 175);
        SDL_RenderFillRect(renderer, &bgRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        SDL_RenderCopy(renderer, dict, NULL, &dictRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &LeftArrowRect);
        SDL_RenderFillRect(renderer, &RightArrowRect);
        SDL_RenderCopy(renderer, LeftArrow, NULL, &LeftArrowRect);
        SDL_RenderCopy(renderer, RightArrow, NULL, &RightArrowRect);

        if (Entries.empty())
        {
            SDL_RenderCopy(renderer, noEntriesText, NULL, &noEntriesRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, buttonDisabledTint.r, buttonDisabledTint.g, buttonDisabledTint.b, buttonDisabledTint.a);
            SDL_RenderFillRect(renderer, &LeftArrowRect);
            SDL_RenderFillRect(renderer, &RightArrowRect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
        else if (currentSelectedEntry <= Entries.size())
        {
            if (currentEntry != Entries[currentSelectedEntry])
            {
                if (entryImage != NULL) SDL_DestroyTexture(entryImage);
                currentEntry = Entries[currentSelectedEntry];
                entryImage = LoadImageFromPath(currentEntry->photoPath.c_str());
            }

            RenderText((std::to_string(currentSelectedEntry+1) + "/" + std::to_string(Entries.size())).c_str(), pageNumberRect);
            if (!currentEntry->Name.empty()) RenderText(currentEntry->Name.c_str(), entryNameRect);
            else RenderText("Name...", entryNameRect);
            if (!currentEntry->desc.empty()) RenderText(currentEntry->desc.c_str(), entryDescRect, CoreMenu::titleTextColor, 44);
            else RenderText("Desc...", entryDescRect);
            if (!currentEntry->desc2.empty()) RenderText(currentEntry->desc2.c_str(), entryDesc2Rect, CoreMenu::titleTextColor, 44);
            else RenderText("Desc...", entryDesc2Rect);
            if (!currentEntry->LocName.empty()) RenderText(("Location:\n " + currentEntry->LocName).c_str(), entryLocNameRect, CoreMenu::titleTextColor, 44);
            else RenderText("Loc Name...", entryLocNameRect);
            SDL_RenderCopy(renderer, entryImage, NULL, &entryImageRect);
        }

        CoreMenu::RenderMenu();
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/60);
    }
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    window = SDL_CreateWindow("Ohnishi Nishimizu", 
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        600,
        900,
        SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    CoreMenu::LoadMenuAssets();

    LoadHome();
    LoadMap();
    LoadDict();

    running = true;

    while (running)
    {
        switch (state)
        {
            case State_Home:
                GoHome();
                break;
            case State_NewEntry:
                NewEntryWizard();
                break;
            case State_Map:
                GoMap();
                break;
            case State_Dict:
                GoDict();
                break;
        }
    }

    CoreMenu::FreeMenuAssets();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}