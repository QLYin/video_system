#include "globalconfig.h"

int GlobalConfig::IconMain = 0xf072;
int GlobalConfig::IconMenu = 0xf0d7;
int GlobalConfig::IconMin = 0xf068;
int GlobalConfig::IconMax = 0xf2d2;
int GlobalConfig::IconNormal = 0xf2d0;
int GlobalConfig::IconClose = 0xf00d;

#ifdef __arm__
QString GlobalConfig::FontName = "WenQuanYi Micro Hei";
int GlobalConfig::FontSize = 18;
#else
QString GlobalConfig::FontName = "Microsoft Yahei";
int GlobalConfig::FontSize = 12;
#endif

QString GlobalConfig::TextColor = "#000000";
QString GlobalConfig::PanelColor = "#F0F0F0";
QString GlobalConfig::BorderColor = "#000000";
QString GlobalConfig::NormalColorStart = "#F0F0F0";
QString GlobalConfig::NormalColorEnd = "#F0F0F0";
QString GlobalConfig::DarkColorStart = "#F0F0F0";
QString GlobalConfig::DarkColorEnd = "#F0F0F0";
QString GlobalConfig::HighColor = "#00BB9E";

bool GlobalConfig::HoverCoverSelected = false;
QString GlobalConfig::HoverBgColor = "#F0F0F0";
QString GlobalConfig::SelectBgColor = "#F0F0F0";
