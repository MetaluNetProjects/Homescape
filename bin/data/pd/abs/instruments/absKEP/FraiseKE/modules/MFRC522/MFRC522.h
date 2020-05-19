#ifndef MFRC522_h
#define MFRC522_h

//#include <Arduino.h>
//#include <SPI.h>

	/////////////////////////////////////////////////////////////////////////////////////
	// Functions for setting up the Arduino
	/////////////////////////////////////////////////////////////////////////////////////
	/*MFRC522(byte chipSelectPin, byte resetPowerDownPin);*/
	void MFRC522_setSPIConfig();
	
	/////////////////////////////////////////////////////////////////////////////////////
	// Basic interface functions for communicating with the MFRC522
	/////////////////////////////////////////////////////////////////////////////////////
	void MFRC522_PCD_WriteRegister(byte reg, byte value);
	void MFRC522_PCD_WriteRegister(byte reg, byte count, byte *values);
	byte MFRC522_PCD_ReadRegister(byte reg);
	void MFRC522_PCD_ReadRegister(byte reg, byte count, byte *values, byte rxAlign /*= 0*/);
	void MFRC522_setBitMask(unsigned char reg, unsigned char mask);
	void MFRC522_PCD_SetRegisterBitMask(byte reg, byte mask);
	void MFRC522_PCD_ClearRegisterBitMask(byte reg, byte mask);
	byte MFRC522_PCD_CalculateCRC(byte *data, byte length, byte *result);
	
	/////////////////////////////////////////////////////////////////////////////////////
	// Functions for manipulating the MFRC522
	/////////////////////////////////////////////////////////////////////////////////////
	void MFRC522_PCD_Init();
	void MFRC522_PCD_Reset();
	void MFRC522_PCD_AntennaOn();
	void MFRC522_PCD_AntennaOff();
	byte MFRC522_PCD_GetAntennaGain();
	void MFRC522_PCD_SetAntennaGain(byte mask);
	bool MFRC522_PCD_PerformSelfTest();
	
	/////////////////////////////////////////////////////////////////////////////////////
	// Functions for communicating with PICCs
	/////////////////////////////////////////////////////////////////////////////////////
	byte MFRC522_PCD_TransceiveData(byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits /*= NULL*/, byte rxAlign /*= 0*/, bool checkCRC /* = false*/);
	byte MFRC522_PCD_CommunicateWithPICC(byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData /*= NULL*/, byte *backLen /*= NULL*/, byte *validBits /*= NULL*/, byte rxAlign /*= 0*/, bool checkCRC /*= false*/);
	byte MFRC522_PICC_RequestA(byte *bufferATQA, byte *bufferSize);
	byte MFRC522_PICC_WakeupA(byte *bufferATQA, byte *bufferSize);
	byte MFRC522_PICC_REQA_or_WUPA(byte command, byte *bufferATQA, byte *bufferSize);
	byte MFRC522_PICC_Select(Uid *uid, byte validBits = 0);
	byte MFRC522_PICC_HaltA();
	
	/////////////////////////////////////////////////////////////////////////////////////
	// Functions for communicating with MIFARE PICCs
	/////////////////////////////////////////////////////////////////////////////////////
	byte MFRC522_PCD_Authenticate(byte command, byte blockAddr, MIFARE_Key *key, Uid *uid);
	void MFRC522_PCD_StopCrypto1();
	byte MFRC522_MIFARE_Read(byte blockAddr, byte *buffer, byte *bufferSize);
	byte MFRC522_MIFARE_Write(byte blockAddr, byte *buffer, byte bufferSize);
	byte MFRC522_MIFARE_Decrement(byte blockAddr, long delta);
	byte MFRC522_MIFARE_Increment(byte blockAddr, long delta);
	byte MFRC522_MIFARE_Restore(byte blockAddr);
	byte MFRC522_MIFARE_Transfer(byte blockAddr);
	byte MFRC522_MIFARE_Ultralight_Write(byte page, byte *buffer, byte bufferSize);
	byte MFRC522_MIFARE_GetValue(byte blockAddr, long *value);
	byte MFRC522_MIFARE_SetValue(byte blockAddr, long value);
	
	/////////////////////////////////////////////////////////////////////////////////////
	// Support functions
	/////////////////////////////////////////////////////////////////////////////////////
	byte MFRC522_PCD_MIFARE_Transceive(byte *sendData, byte sendLen, bool acceptTimeout /*= false*/);
	// old function used too much memory, now name moved to flash; if you need char, copy from flash to memory
	//const char *GetStatusCodeName(byte code);
	//const __FlashStringHelper *GetStatusCodeName(byte code);
	byte MFRC522_PICC_GetType(byte sak);
	// old function used too much memory, now name moved to flash; if you need char, copy from flash to memory
	//const char *PICC_GetTypeName(byte type);
	const __FlashStringHelper *MFRC522_PICC_GetTypeName(byte type);
	void MFRC522_PICC_DumpToSerial(Uid *uid);
	void MFRC522_PICC_DumpMifareClassicToSerial(Uid *uid, byte piccType, MIFARE_Key *key);
	void MFRC522_PICC_DumpMifareClassicSectorToSerial(Uid *uid, MIFARE_Key *key, byte sector);
	void MFRC522_PICC_DumpMifareUltralightToSerial();
	void MFRC522_MIFARE_SetAccessBits(byte *accessBitBuffer, byte g0, byte g1, byte g2, byte g3);
	bool MFRC522_MIFARE_OpenUidBackdoor(bool logErrors);
	bool MFRC522_MIFARE_SetUid(byte *newUid, byte uidSize, bool logErrors);
	bool MFRC522_MIFARE_UnbrickUidSector(bool logErrors);
	
	/////////////////////////////////////////////////////////////////////////////////////
	// Convenience functions - does not add extra functionality
	/////////////////////////////////////////////////////////////////////////////////////
	bool MFRC522_PICC_IsNewCardPresent();
	bool MFRC522_PICC_ReadCardSerial();
	
/*private:
	byte _chipSelectPin;		// Arduino pin connected to MFRC522's SPI slave select input (Pin 24, NSS, active low)
	byte _resetPowerDownPin;	// Arduino pin connected to MFRC522's reset and power down input (Pin 6, NRSTPD, active low)*/
	byte MFRC522_MIFARE_TwoStepHelper(byte command, byte blockAddr, long data);
//};

#endif

