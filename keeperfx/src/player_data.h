/******************************************************************************/
// Free implementation of Bullfrog's Dungeon Keeper strategy game.
/******************************************************************************/
/** @file player_data.h
 *     Header file for player_data.c.
 * @par Purpose:
 *     Player data structures definitions.
 * @par Comment:
 *     Just a header file - #defines, typedefs, function prototypes etc.
 * @author   Tomasz Lis
 * @date     10 Nov 2009 - 20 Jan 2010
 * @par  Copying and copyrights:
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 */
/******************************************************************************/
#ifndef DK_PLYR_DATA_H
#define DK_PLYR_DATA_H

#include "bflib_basics.h"
#include "globals.h"
#include "engine_camera.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
#define PLAYERS_COUNT           5

#define INVALID_PLAYER (&bad_player)
typedef long PlayerNumber;

#ifdef __cplusplus
#pragma pack(1)
#endif

struct Wander {
    unsigned char field_0[424];
};


#define SIZEOF_PlayerInfo 0x4EF
struct PlayerInfo {
    unsigned char field_0;
    unsigned char field_1;
    unsigned char field_2; //seems to be never used
    unsigned char field_3;
    unsigned char field_4;
    unsigned char field_5;
    unsigned char field_6;
    unsigned char *field_7;
    unsigned char packet_num; // index of packet slot associated with this player
    long field_C;
unsigned int field_10;
unsigned char field_14;
    char field_15[20]; //size may be shorter
    unsigned char victory_state;
    unsigned char field_2A;
    unsigned char index;
    unsigned char field_2C;
    unsigned char field_2D[2];
    short field_2F;
    long field_31;
    short thing_under_hand;
    unsigned char field_37;
    struct Camera *acamera;  // Pointer to the currently active camera
    struct Camera cameras[4];
    unsigned short field_E4;
    unsigned short field_E6;
char field_E8[2];
    struct Wander wandr1;
    struct Wander wandr2;
    short field_43A;
char field_43C[2];
    short field_43E;
    long field_440;
    short engine_window_width;
    short engine_window_height;
    short engine_window_x;
    short engine_window_y;
    short mouse_x;
    short mouse_y;
    unsigned short minimap_zoom;
    unsigned char view_type;
    unsigned char work_state;
    unsigned char field_454;
    unsigned char field_455;
    unsigned char field_456;
char field_457[8];
char field_45F;
short field_460;
char field_462;
    char strfield_463[64];
    unsigned char field_4A3;
    unsigned char field_4A4;
    char field_4A5;
    char field_4A6;
    char field_4A7[4];
    short field_4AB;
    short field_4AD;
    unsigned char field_4AF;
    unsigned char instance_num;
    unsigned long field_4B1;
    char field_4B5;
    long field_4B6;
    char field_4BA[3];
    long field_4BD;
    long field_4C1;
    long field_4C5;
    unsigned char *palette;
    long field_4CD;
    char field_4D1;
    long field_4D2;
    long field_4D6;
    char field_4DA;
    long field_4DB;
    long field_4DF;
    long field_4E3;
    long field_4E7;
    long field_4EB;
    };

#ifdef __cplusplus
#pragma pack()
#endif
/******************************************************************************/
extern struct PlayerInfo bad_player;
/******************************************************************************/
struct PlayerInfo *get_player(long plyr_idx);
TbBool player_invalid(struct PlayerInfo *player);
TbBool is_my_player(struct PlayerInfo *player);
TbBool is_my_player_number(PlayerNumber plyr_num);
/******************************************************************************/
#ifdef __cplusplus
}
#endif
#endif