#ifndef BASE_H
#define BASE_H

enum StateType
{
    State_Home,
    State_NewEntry,
    State_Map,
    State_Dict,
    State_Quit
};

extern StateType state;

#endif