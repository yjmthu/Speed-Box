﻿#ifndef DIALOGWALLPAPER_H
#define DIALOGWALLPAPER_H

#include "wallpaper.h"

class DialogWallpaper : public Wallpaper
{
    Q_OBJECT

public:
    void setWallhaven() override;
    void setBing() override;
    void setNative() override;
    void setRandom() override;
    void setAdvance() override;
    void startWork() override;
};

#endif // DIALOGWALLPAPER_H
