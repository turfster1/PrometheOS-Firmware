#include "keyboardScene.h"
#include "sceneManager.h"
#include "launchScene.h"
#include "removeScene.h"
#include "flashScene.h"
#include "audioSettingsScene.h"
#include "mainScene.h"

#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\settingsManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\xboxInternals.h"
#include "..\theme.h"

keyboardScene::keyboardScene(const char* text)
{
	mSelectedControl = 0;
	mCapitals = false;
	mSymbols = false;
	mText = strdup(text);
	mSceneResult = sceneResultNone;
	mCounter = 0;
}

keyboardScene::~keyboardScene()
{
	free(mText);
}


void keyboardScene::update()
{
	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		mSceneResult = sceneResultCancelled;
	}

	// Delete Action

	if (inputManager::buttonPressed(ButtonX))
	{
		char* newText = stringUtility::substr(mText, 0, max(strlen(mText) - 1, 0));
		free(mText);
		mText = newText;
	}

	if (inputManager::buttonPressed(ButtonLeftThumb) || inputManager::buttonPressed(ButtonTriggerLeft))
	{
		mCapitals = !mCapitals;
		if (mCapitals == true)
		{
			mSymbols = false;
		}
	}

	if (inputManager::buttonPressed(ButtonRightThumb) || inputManager::buttonPressed(ButtonTriggerRight))
	{
		mSymbols = !mSymbols;
		if (mSymbols == true)
		{
			mCapitals = false;
		}
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		char* keys = "0123456789abcdefghijklmnopqrstuvwxyz ";
		if (mCapitals == true)
		{
			keys = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
		}
		else if (mSymbols == true)
		{
			keys = "!@#$%^&*()~`-_+=[]{}\\|;:'\",.<>?/ ";
		}

		if (mSelectedControl == 0)
		{
			mCapitals = !mCapitals;
			if (mCapitals == true)
			{
				mSymbols = false;
			}
		}
		else if (mSelectedControl >= 1 && mSelectedControl <= 10)
		{
			char* newText = stringUtility::formatString("%s%c", mText, keys[mSelectedControl - 1]);
			free(mText);
			mText = newText;
		}
		else if (mSelectedControl == 11)
		{
			mSymbols = !mSymbols;
			if (mSymbols == true)
			{
				mCapitals = false;
			}
		}
		else if (mSelectedControl >= 12 && mSelectedControl <= 21)
		{
			char* newText = stringUtility::formatString("%s%c", mText, keys[mSelectedControl - 2]);
			free(mText);
			mText = newText;
		}
		else if (mSelectedControl == 22)
		{
			char* newText = stringUtility::substr(mText, 0, max(strlen(mText) - 1, 0));
			free(mText);
			mText = newText;
		}
		else if (mSelectedControl >= 23 && mSelectedControl <= 32)
		{
			char* newText = stringUtility::formatString("%s%c", mText, keys[mSelectedControl - 3]);
			free(mText);
			mText = newText;
		}
		else if (mSelectedControl == 33)
		{
			mSceneResult = sceneResultDone;
		}
		else if (mSelectedControl >= 34 && mSelectedControl <= 40)
		{
			char* newText = stringUtility::formatString("%s%c", mText, keys[mSelectedControl - 4]);
			free(mText);
			mText = newText;
		}
	}
	
	// Left Actions

	if (inputManager::buttonPressed(ButtonDpadLeft))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 10)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : 10; 
		}
		else if (mSelectedControl >= 11 && mSelectedControl <= 21)
		{
			mSelectedControl = mSelectedControl > 11 ? mSelectedControl - 1 : 21; 
		}
		else if (mSelectedControl >= 22 && mSelectedControl <= 32)
		{
			mSelectedControl = mSelectedControl > 22 ? mSelectedControl - 1 : 32; 
		}
		else if (mSelectedControl >= 33 && mSelectedControl <= 40)
		{
			mSelectedControl = mSelectedControl > 33 ? mSelectedControl - 1 : 40; 
		}
	}

	// Right Actions

	if (inputManager::buttonPressed(ButtonDpadRight))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 10)
		{
			mSelectedControl = mSelectedControl < 10 ? mSelectedControl + 1 : 0;
		}
		else if (mSelectedControl >= 11 && mSelectedControl <= 21)
		{
			mSelectedControl = mSelectedControl < 21 ? mSelectedControl + 1 : 11;
		}
		else if (mSelectedControl >= 22 && mSelectedControl <= 32)
		{
			mSelectedControl = mSelectedControl < 32 ? mSelectedControl + 1 : 22;
		}
		else if (mSelectedControl >= 33 && mSelectedControl <= 40)
		{
			mSelectedControl = mSelectedControl < 40 ? mSelectedControl + 1 : 33;
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 10)
		{
			mSelectedControl += 11;
		}
		else if (mSelectedControl >= 11 && mSelectedControl <= 21)
		{
			mSelectedControl += 11;
		}
		else if (mSelectedControl >= 22 && mSelectedControl <= 32)
		{
			mSelectedControl = min(mSelectedControl + 11, 40);
		}
		else if (mSelectedControl >= 33 && mSelectedControl <= 40)
		{
			mSelectedControl = mSelectedControl - 33;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mSelectedControl >= 0 && mSelectedControl <= 10)
		{
			mSelectedControl = 33 + min(mSelectedControl, 7);
		}
		else if (mSelectedControl >= 11 && mSelectedControl <= 21)
		{
			mSelectedControl -= 11;
		}
		else if (mSelectedControl >= 22 && mSelectedControl <= 32)
		{
			mSelectedControl -= 11;
		}
		else if (mSelectedControl >= 33 && mSelectedControl <= 40)
		{
			mSelectedControl -= 11;
		}
	}
}

void keyboardScene::render()
{
	if (mCounter == 0)
	{
		mCounter = 60;
	}
	else 
	{
		mCounter--;
	}

	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please enter a bank name...", theme::getTitleTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	char* keys = "0123456789abcdefghijklmnopqrstuvwxyz ";
	if (mCapitals == true)
	{
		keys = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	}
	else if (mSymbols == true)
	{
		keys = "!@#$%^&*()~`-_+=[]{}\\|;:'\",.<>?/ ";
	}

	int32_t keyboardX = 38;
	int32_t keyboardY = (context::getBufferHeight() - (((4 * 48) - 4) + 30 + 64)) / 2;
	keyboardY += theme::getCenterOffset();

	drawing::drawBitmapString(context::getBitmapFontSmall(), "Bank Name:", theme::getTextColor(), 40, keyboardY);

	keyboardY += 30;

	char* tempText = mCounter < 30 ? stringUtility::formatString("%s\xC2\xAF", mText) : stringUtility::formatString("%s\xC2\xB0", mText);
	component::textBox(tempText, false, false, horizAlignmentLeft, 40, keyboardY, 640, 44);
	free(tempText);

	keyboardY += 64;

	component::button(mSelectedControl == 0, mCapitals, "Caps", keyboardX, keyboardY + (48 * 0), 104, 44);

	keyboardButton(mSelectedControl == 1, false, keys[0], keyboardX + 108 + (54 * 0), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 2, false, keys[1], keyboardX + 108 + (54 * 1), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 3, false, keys[2], keyboardX + 108 + (54 * 2), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 4, false, keys[3], keyboardX + 108 + (54 * 3), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 5, false, keys[4], keyboardX + 108 + (54 * 4), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 6, false, keys[5], keyboardX + 108 + (54 * 5), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 7, false, keys[6], keyboardX + 108 + (54 * 6), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 8, false, keys[7], keyboardX + 108 + (54 * 7), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 9, false, keys[8], keyboardX + 108 + (54 * 8), keyboardY + (48 * 0), 50, 44);
	keyboardButton(mSelectedControl == 10, false, keys[9], keyboardX + 108 + (54 * 9), keyboardY + (48 * 0), 50, 44);

	component::button(mSelectedControl == 11, mSymbols, "Symbol", keyboardX, keyboardY + (48 * 1), 104, 44);

	keyboardButton(mSelectedControl == 12, false, keys[10], keyboardX + 108 + (54 * 0), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 13, false, keys[11], keyboardX + 108 + (54 * 1), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 14, false, keys[12], keyboardX + 108 + (54 * 2), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 15, false, keys[13], keyboardX + 108 + (54 * 3), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 16, false, keys[14], keyboardX + 108 + (54 * 4), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 17, false, keys[15], keyboardX + 108 + (54 * 5), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 18, false, keys[16], keyboardX + 108 + (54 * 6), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 19, false, keys[17], keyboardX + 108 + (54 * 7), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 20, false, keys[18], keyboardX + 108 + (54 * 8), keyboardY + (48 * 1), 50, 44);
	keyboardButton(mSelectedControl == 21, false, keys[19], keyboardX + 108 + (54 * 9), keyboardY + (48 * 1), 50, 44);

	component::button(mSelectedControl == 22, false, "Delete", keyboardX, keyboardY + (48 * 2), 104, 44);

	keyboardButton(mSelectedControl == 23, false, keys[20], keyboardX + 108 + (54 * 0), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 24, false, keys[21], keyboardX + 108 + (54 * 1), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 25, false, keys[22], keyboardX + 108 + (54 * 2), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 26, false, keys[23], keyboardX + 108 + (54 * 3), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 27, false, keys[24], keyboardX + 108 + (54 * 4), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 28, false, keys[25], keyboardX + 108 + (54 * 5), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 29, false, keys[26], keyboardX + 108 + (54 * 6), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 30, false, keys[27], keyboardX + 108 + (54 * 7), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 31, false, keys[28], keyboardX + 108 + (54 * 8), keyboardY + (48 * 2), 50, 44);
	keyboardButton(mSelectedControl == 32, false, keys[29], keyboardX + 108 + (54 * 9), keyboardY + (48 * 2), 50, 44);

	component::button(mSelectedControl == 33, false, "Done", keyboardX, keyboardY + (48 * 3), 104, 44);

	keyboardButton(mSelectedControl == 34, false, keys[30], keyboardX + 108 + (54 * 0), keyboardY + (48 * 3), 50, 44);
	keyboardButton(mSelectedControl == 35, false, keys[31], keyboardX + 108 + (54 * 1), keyboardY + (48 * 3), 50, 44);

	if (mSymbols)
	{
		component::button(mSelectedControl == 36, false, "Space", keyboardX + 108 + (54 * 2), keyboardY + (48 * 3), (54 * 8) - 4, 44);
	}
	else
	{
		keyboardButton(mSelectedControl == 36, false, keys[32], keyboardX + 108 + (54 * 2), keyboardY + (48 * 3), 50, 44);
		keyboardButton(mSelectedControl == 37, false, keys[33], keyboardX + 108 + (54 * 3), keyboardY + (48 * 3), 50, 44);
		keyboardButton(mSelectedControl == 38, false, keys[34], keyboardX + 108 + (54 * 4), keyboardY + (48 * 3), 50, 44);
		keyboardButton(mSelectedControl == 39, false, keys[35], keyboardX + 108 + (54 * 5), keyboardY + (48 * 3), 50, 44);
		component::button(mSelectedControl == 40, false, "Space", keyboardX + 108 + (54 * 6), keyboardY + (48 * 3), 212, 44);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

void keyboardScene::keyboardButton(bool selected, bool active, char key, int x, int y, int width, int height)
{
	char* keyString = stringUtility::formatString("%c", key);
	component::button(selected, active, keyString, x, y, width, height);
	free(keyString);
}

char* keyboardScene::getText()
{
	return strdup(mText);
}

sceneResult keyboardScene::getSceneResult()
{
	return mSceneResult;
}