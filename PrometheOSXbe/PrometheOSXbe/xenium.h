#pragma once

#include "xboxinternals.h"
#include "utils.h"

#define LPC_MEMORY_BASE 0xFF000000u
#define XENIUM_SETTINGS_OFFSET (0x1FA000-0x1C0000)
#define XENIUM_REGISTER_BANKING 0x00EF
#define XENIUM_REGISTER_LED 0x00EE

#define XENIUM_MANUF_ID 0x01
#define XENIUM_MANUF_ID1 0xC2
#define XENIUM_DEVICE_ID 0xC4
#define XENIUM_FLASH_NUM_SECTORS 35
#define XENIUM_FLASH_SECTOR_SIZE1 0x10000
#define XENIUM_FLASH_SECTOR_SIZE2 0x2000
#define XENIUM_FLASH_SIZE (1024 * 1024 * 2)

class xenium
{
public:

	enum ledColorEnum
	{ 
		ledColorOff = 0,
		ledColorRed = 1,
		ledColorGreen = 2,
		ledColorAmber = 3,
		ledColorBlue = 4,
		ledColorPurple = 5,
		ledColorTeal = 6,
		ledColorWhite = 7
	}; 

	enum bankEnum
	{ 
		bankInvalid = -1,
		bankTsop = 0,
		bankBootloader = 1,
		bankXeniumOS = 2,
		bank1_256k = 3,
		bank2_256k = 4,
		bank3_256k = 5,
		bank4_256k = 6,
		bank1_512k = 7,
		bank2_512k = 8,
		bank1_1024k = 9,
		bankRecovery = 10
	}; 

	//IO CONTROL INTERFACE
	static uint8_t inputByte(uint16_t port);
	static void outputByte(uint16_t port, uint8_t value);
	static void setBank(bankEnum bank);
	static bankEnum getBank();
	static void setLedColor(ledColorEnum ledColor);

	//FLASH MEMORY INTERFACE
	static void lpcSendByte(uint32_t address, uint8_t data);
	static uint8_t flashReadByte(uint32_t address);
	static void flashReset();
	static void flashReadStream(uint32_t address, uint8_t *data, uint32_t len);
	static uint8_t flashBusy();
	static void sectorErase(uint32_t address);
	static void flashWriteByte(uint32_t address, uint8_t data);
	static uint8_t isDetected();
	static uint32_t sectorToAddress(uint8_t sector);
	static uint32_t sectorToSize(uint8_t sector);
	static bankEnum sectorToBank(uint8_t sector);
	static uint8_t bankToSector(bankEnum bank);
	static uint8_t bankToSectors(bankEnum bank);
	static int32_t getBankSize(bankEnum bank);
	static bankEnum getBankFromIdAndSlots(uint8_t id, uint8_t slots);
	static utils::dataContainer* readBank(bankEnum bank, uint32_t offset, uint32_t length);
	static void eraseBank(bankEnum bank, uint32_t offset, uint32_t size);
	static void writeBank(bankEnum bank, uint32_t offset, utils::dataContainer* dataContainer);
	static bool verifyBank(bankEnum bank, uint32_t offset, utils::dataContainer* dataContainer);
	static void launchBank(uint8_t id, uint8_t slots, ledColorEnum ledColor);
	static void launchTsop();
};
