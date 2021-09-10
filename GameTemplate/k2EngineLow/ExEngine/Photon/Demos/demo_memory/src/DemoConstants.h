#pragma once

#include "Common-cpp/inc/Common.h"

static const ExitGames::Common::JString DEFAULT_USER_NAME  = ExitGames::Common::JString(L"u")+(unsigned int)GETTIMEMS()%1000;
static const ExitGames::Common::JString APP_ID = L"<no-app-id>";
static const ExitGames::Common::JString APP_VERSION = L"1.0";

static const int BOARD_SIZE = 4;
static const int TILE_COUNT = (BOARD_SIZE * BOARD_SIZE);
static const int TILES_TYPE_COUNT = (BOARD_SIZE * BOARD_SIZE / 2);
static const int MAX_PLAYERS = 2;
static const int END_OF_TURN_DELAY = 3000;
static const int NOT_FLIPPED_TILE = 255;

static const ExitGames::Common::JString PROP_TURN = L"turn";
static const ExitGames::Common::JString PROP_NAMES  = L"names";