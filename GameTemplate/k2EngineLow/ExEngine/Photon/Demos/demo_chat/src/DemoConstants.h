#pragma once

#include <stdlib.h>

static const ExitGames::Common::JString APP_ID = L"<no-app-id>";
static const ExitGames::Common::JString APP_VERSION = L"1.0";

static const ExitGames::Common::JString SUBSCRIBE_CHANNELS[] = {L"a", L"b", L"c"};
const int USER_COUNT = 512;
static const ExitGames::Common::JString USER_PREFIX = L"user";
static const ExitGames::Common::JString DEFAULT_USER_ID = USER_PREFIX+abs(GETTIMEMS())%USER_COUNT;