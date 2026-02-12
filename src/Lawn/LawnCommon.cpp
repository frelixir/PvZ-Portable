#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include "Board.h"
#include "../LawnApp.h"
#include "LawnCommon.h"
#include "../Resources.h"
#include "../Sexy.TodLib/TodCommon.h"
#include "graphics/Font.h"
#include "widget/Dialog.h"
#include "widget/Checkbox.h"

int gLawnEditWidgetColors[][4] = {
    { 0,   0,   0,   0 },
    { 0,   0,   0,   0 },
    { 240, 240, 255, 255 },
    { 255, 255, 255, 255 },
    { 0,   0,   0,   255 },
};

// 判断在 [theNumber - theRange, theNumber + theRange] 区间内是否存在 theMod 的整数倍数
bool ModInRange(int theNumber, int theMod, int theRange)
{
	theRange = abs(theRange);
	for (int i = theNumber - theRange; i <= theNumber + theRange; i++)
		if (i % theMod == 0) return true;
	return false;
}

// 判断点 (x1, y1) 是否位于点 (x2, y2) 周围的 (theRangeX, theRangeY) 范围内
bool GridInRange(int x1, int y1, int x2, int y2, int theRangeX, int theRangeY)
{
	return x1 >= x2 - theRangeX && x1 <= x2 + theRangeX && y1 >= y2 - theRangeY && y1 <= y2 + theRangeY;
}

void TileImageHorizontally(Graphics* g, Image* theImage, int theX, int theY, int theWidth)
{
	while (theWidth > 0)
	{
		int aImageWidth = std::min(theWidth, theImage->GetWidth());
		g->DrawImage(theImage, theX, theY, Rect(0, 0, aImageWidth, theImage->GetHeight()));
		theX += aImageWidth;
		theWidth -= aImageWidth;
	}
}

void TileImageVertically(Graphics* g, Image* theImage, int theX, int theY, int theHeight)
{
	while (theHeight > 0)
	{
		int aImageHeight = std::min(theHeight, theImage->GetHeight());
		g->DrawImage(theImage, theX, theY, Rect(0, 0, theImage->GetWidth(), aImageHeight));
		theY += aImageHeight;
		theHeight -= aImageHeight;
	}
}

LawnEditWidget::LawnEditWidget(int theId, EditListener* theListener, Dialog* theDialog) : EditWidget(theId, theListener)
{
    mDialog = theDialog;
    mAutoCapFirstLetter = true;
}

//0x456700
LawnEditWidget::~LawnEditWidget()
{
}

//0x456720
void LawnEditWidget::KeyDown(KeyCode theKey)
{
    EditWidget::KeyDown(theKey);
    if (theKey == KeyCode::KEYCODE_ESCAPE)
        mDialog->KeyDown(KeyCode::KEYCODE_ESCAPE);
}

//0x456760
void LawnEditWidget::KeyChar(char theChar)
{
    if (mAutoCapFirstLetter && isalpha(theChar))
    {
        theChar = toupper(theChar);
        mAutoCapFirstLetter = false;
    }

    EditWidget::KeyChar(theChar);
}

//0x4567B0
LawnEditWidget* CreateEditWidget(int theId, EditListener* theListener, Dialog* theDialog)
{
    LawnEditWidget* aEditWidget = new LawnEditWidget(theId, theListener, theDialog);
    aEditWidget->SetFont(Sexy::FONT_BRIANNETOD16);
    aEditWidget->SetColors(gLawnEditWidgetColors, EditWidget::NUM_COLORS);
    aEditWidget->mBlinkDelay = 14;

    return aEditWidget;
}

void DrawEditBox(Graphics* g, EditWidget* theWidget)
{
    Rect aDest(theWidget->mX - 8, theWidget->mY - 4, theWidget->mWidth + 16, theWidget->mHeight + 8);
    g->DrawImageBox(aDest, IMAGE_EDITBOX);
}

//0x456860
Checkbox* MakeNewCheckbox(int theId, CheckboxListener* theListener, bool theDefault)
{
    Checkbox* aCheckbox = new Checkbox(Sexy::IMAGE_OPTIONS_CHECKBOX0, Sexy::IMAGE_OPTIONS_CHECKBOX1, theId, theListener);
    aCheckbox->mChecked = theDefault;
    aCheckbox->mHasAlpha = true;
    aCheckbox->mHasTransparencies = true;

    return aCheckbox;
}

std::string GetSavedGameName(GameMode theGameMode, int theProfileId)
{
    return GetAppDataPath(StrFormat("userdata/game%d_%d.v4", theProfileId, static_cast<int>(theGameMode)));
}

//0x4568D0
// GOTY @Patoke: 0x45A200
std::string GetLegacySavedGameName(GameMode theGameMode, int theProfileId)
{
    return GetAppDataPath(StrFormat("userdata/game%d_%d.dat", theProfileId, static_cast<int>(theGameMode)));
}

// Original version from 0x456980 is modified to C++20 standard
int GetCurrentDaysSince2000()
{
    return GetCurrentDaysSince2000(std::time(nullptr));
}

int GetCurrentDaysSince2000(std::time_t theTime)
{
    using namespace std::chrono;

    const std::tm aTM = *std::localtime(&theTime);
    const year_month_day aYMD = year{aTM.tm_year + 1900} / month{static_cast<unsigned>(aTM.tm_mon + 1)} /
                               day{static_cast<unsigned>(aTM.tm_mday)};
    // 1-based
    return static_cast<int>((sys_days{aYMD} - sys_days{2000y / January / 1}).count() + 1);
}
