#include "flashScene.h"
#include "sceneManager.h"
#include "audioSettingsScene.h"
#include "videoSettingsScene.h"
#include "mainScene.h"

#include "..\settingsManager.h"
#include "..\context.h"
#include "..\drawing.h"
#include "..\component.h"
#include "..\ssfn.h"
#include "..\inputManager.h"
#include "..\hdmiDevice.h"
#include "..\xenium.h"
#include "..\stringUtility.h"
#include "..\driveManager.h"
#include "..\fileSystem.h"
#include "..\pointerVector.h"
#include "..\settingsManager.h"
#include "..\theme.h"

pointerVector* flashScene::getFileInfoDetails()
{
	if (strlen(mCurrentPath) == 0)
	{
		pointerVector* fileInfoDetails = new pointerVector(false);
		for (uint32_t i = 0; i < mMountedDrives->count(); i++)
		{
			char* mountPoint = (char*)mMountedDrives->get(i);
			fileSystem::FileInfoDetail* fileInfoDetail = new fileSystem::FileInfoDetail();
			fileInfoDetail->path = stringUtility::formatString("%s:", mountPoint);
			fileInfoDetail->isDirectory = true;
			fileInfoDetail->isFile = false;
			fileInfoDetails->add(fileInfoDetail);
		}
		return fileInfoDetails;
	}

	pointerVector* result = fileSystem::fileGetFileInfoDetails(mCurrentPath);
	if (result != NULL)
	{
		for (int32_t i = (int32_t)result->count() - 1; i >= 0; i--)
		{
			fileSystem::FileInfoDetail* fileInfoDetail = (fileSystem::FileInfoDetail*)result->get(i);
			if (fileInfoDetail->isFile && (fileInfoDetail->size <= 0 || fileInfoDetail->size > (1024 * 1024) || fileInfoDetail->size % (1024 * 256) != 0))
			{
				result->remove(i);
			}
		}
	}

	return result;
}

flashScene::flashScene()
{
	driveManager::mountAllDrives();

	mSelectedControl = 0;
	mScrollPosition = 0;
	mMountedDrives = driveManager::getMountedDrives();
	mCurrentPath = strdup("");
	mFileInfoDetails = getFileInfoDetails();
	mSceneResult = sceneResultNone;
	mFilePath = NULL;
}

flashScene::~flashScene()
{
	delete(mMountedDrives);
	free(mCurrentPath);
	delete(mFileInfoDetails);
	free(mFilePath);
}

void flashScene::update()
{
	// Cancel Action

	if (inputManager::buttonPressed(ButtonB))
	{
		mSceneResult = sceneResultCancelled;
	}

	// Parent Path

	if (inputManager::buttonPressed(ButtonX))
	{
		char* newPath = fileSystem::getDirectory(mCurrentPath);
		free(mCurrentPath);
		mCurrentPath = newPath;

		delete(mFileInfoDetails);
		mFileInfoDetails = getFileInfoDetails();
		mSelectedControl = 0;
		mScrollPosition = 0;
	}

	// Select Actions

	if (inputManager::buttonPressed(ButtonA))
	{
		if (mFileInfoDetails != NULL)
		{
			for (uint32_t i = 0; i < mFileInfoDetails->count(); i++)
			{
				if (mSelectedControl != i)
				{
					continue;
				}
				
				fileSystem::FileInfoDetail* fileInfoDetail = (fileSystem::FileInfoDetail*)mFileInfoDetails->get(i);
				if (fileInfoDetail->isDirectory)
				{
					char* newPath = strdup(fileInfoDetail->path);
					free(mCurrentPath);
					mCurrentPath = newPath;
			
					delete(mFileInfoDetails);
					mFileInfoDetails = getFileInfoDetails();
					mSelectedControl = 0;
					mScrollPosition = 0;
					break;
				}
				else if (fileInfoDetail->isFile)
				{
					mFilePath = strdup(fileInfoDetail->path);
					mSceneResult = sceneResultDone;
					break;
				}
			}
		}
	}

	// Down Actions

	if (inputManager::buttonPressed(ButtonDpadDown))
	{
		if (mFileInfoDetails != NULL)
		{
			mSelectedControl = mSelectedControl < (int)(mFileInfoDetails->count() - 1) ? mSelectedControl + 1 : 0;
		}
	}

	// Up Actions

	if (inputManager::buttonPressed(ButtonDpadUp))
	{
		if (mFileInfoDetails != NULL)
		{
			mSelectedControl = mSelectedControl > 0 ? mSelectedControl - 1 : (int)(mFileInfoDetails->count() - 1); 
		}
	}
}

void flashScene::render()
{
	drawing::clearBackground();
	component::panel(theme::getPanelFillColor(), theme::getPanelStrokeColor(), 16, 16, 688, 448);
	drawing::drawBitmapStringAligned(context::getBitmapFontMedium(), "Please select a BIOS to flash...", theme::getTitleTextColor(), theme::getHeaderAlign(), 40, theme::getHeaderY(), 640);

	if (mFileInfoDetails != NULL)
	{
		int32_t maxItems = 7;

		int32_t start = 0;
		if ((int32_t)mFileInfoDetails->count() >= maxItems)
		{
			start = min(max(mSelectedControl - (maxItems / 2), 0), (int32_t)mFileInfoDetails->count() - maxItems);
		}

		int32_t itemCount = min(start + maxItems, (int32_t)mFileInfoDetails->count()) - start; 
		if (itemCount > 0)
		{
			int32_t yPos = (context::getBufferHeight() - ((itemCount * 40) - 10)) / 2;
			yPos += theme::getCenterOffset();

			for (int32_t i = 0; i < itemCount; i++)
			{
				uint32_t index = start + i;
				fileSystem::FileInfoDetail* fileInfoDetail = (fileSystem::FileInfoDetail*)mFileInfoDetails->get(index);
				char* fileName = fileSystem::getFileName(fileInfoDetail->path);
				component::fileButton(mSelectedControl == index, false, fileInfoDetail->isFile, fileName, 40, yPos, 640, 30);
				free(fileName);
				yPos += 40;
			}
		}
		else
		{
			int yPos = ((context::getBufferHeight() - 44) / 2);
			yPos += theme::getCenterOffset();

			component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
		}
	}
	else
	{
		int yPos = ((context::getBufferHeight() - 44) / 2);
		yPos += theme::getCenterOffset();

		component::textBox("No items", false, false, horizAlignmentCenter, 40, yPos, 640, 44);
	}

	drawing::drawBitmapString(context::getBitmapFontSmall(), "\xC2\xA1 Select \xC2\xA3 Parent", theme::getFooterTextColor(), 40, theme::getFooterY());
	drawing::drawBitmapStringAligned(context::getBitmapFontSmall(), "\xC2\xA2 Cancel", theme::getFooterTextColor(), horizAlignmentRight, 40, theme::getFooterY(), 640);
}

char* flashScene::getFilePath()
{
	return strdup(mFilePath);
}

sceneResult flashScene::getSceneResult()
{
	return mSceneResult;
}