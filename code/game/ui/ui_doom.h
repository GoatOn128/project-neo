// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
void UI_DoomInit();
bool UI_DoomOpen(uiMenuCommand_t menu);
bool UI_DoomActive();
bool UI_DoomFullscreen();
void UI_DoomDraw();
void UI_DoomKey(int key, bool down);
void UI_DoomMouse(int dx, int dy);
bool UI_DoomCommand(const char *command);

bool UI_DoomDrawLoading();
