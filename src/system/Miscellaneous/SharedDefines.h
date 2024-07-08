﻿/*
Copyright 2023 - 2024 by Severin

*/

#pragma once

#include <string>
#include <map>

#include "Util.h"
#include "TranslateText.h"

enum class ErrorCodes : std::uint64_t
{
	ERROR_NO_ERRORS									= 0x000000000001,
	ERROR_NO_MACHINE_NAME							= 0x000000000002,
	ERROR_NO_FIRST_NAME								= 0x000000000004,
	ERROR_NO_LAST_NAME								= 0x000000000008,
	ERROR_UNK_ERROR									= 0x000000000010,
	ERROR_FILE_NAME_TO_LONG							= 0x00000000000B,
	ERROR_NO_SAVE_PATH								= 0x00000000000C,
	ERROR_NO_MYSQL_CONNECTION						= 0x00000000000D,
	ERROR_NO_STYLE									= 0x00000000000E,
	ERROR_NO_SETTING_FILE_FOUND						= 0x00000000000F,
	WARNING_PASSWORD_NOT_MATCHED					= 0x000000000016,
	WARNING_PASSWORD_OR_USER_WRONG					= 0x000000000017,
	WARNING_USER_NAME_TO_SHORT						= 0x000000000018,
	WARNING_PASSWORD_TO_SHORT						= 0x000000000019,
	WARNING_USER_ALREADY_EXIST						= 0x00000000001A,
	ERROR_USER_WAS_NOT_CREATED_INTERN				= 0x00000000001B,
	WARNING_OLD_AND_NEW_PASSWORD_SAME				= 0x00000000001C,
	ERROR_PASSWORD_CHANGE_DID_NOT_WORK				= 0x00000000001D,
	ERROR_MYSQL_ERROR								= 0x00000000001E,
	ERROR_NO_LOG_PATH_FOUND							= 0x00000000001F,
	ERROR_LOG_FILE_NOT_OPEN_OR_CREATED				= 0x000000000020,
	ERROR_EXECUTE_SQL_PROBLEM						= 0x000000000021,
	ERROR_SSH_HOSTNAME_ERROR						= 0x000000000022,
	ERROR_SSH_PORT_ERROR							= 0x000000000023,
	ERROR_SSH_USERNAME_ERROR						= 0x000000000024,
	ERROR_SSH_PASSWORD_ERROR						= 0x000000000025,
	ERROR_SSH_PROGRAMM_PATH_ERROR					= 0x000000000026,
	ERROR_SSH_IS_ACTIVE_ERROR						= 0x000000000027,
	ERROR_SSH_LOCAL_PORT_ERROR						= 0x000000000028,
	ERROR_NO_USERNAME_IN_FIELD						= 0x000000000029,
	ERROR_NO_PASSWORD_IN_FIELD						= 0x00000000002A,
	WARNING_NO_EMAIL_ADDRESS_ADDED					= 0x00000000002B,
	WARNING_NO_VALIDE_EMAIL_ADDRESS					= 0x00000000002C,
	WARNING_EMAIL_ADDRESS_ALREADY_USED				= 0x00000000002D,
	WARNING_SOUND_GENERAL_WARNING					= 0x00000000002E,
	WARNING_CURRENT_LANGUAGE						= 0x00000000002F,
	WARNING_USERNAME_ALREADY_IN_USE					= 0x000000000030,
	WARNING_EMAIL_ADDRESS_IN_USE					= 0x000000000031,
	WARNING_EMAIL_NOT_CHANGED						= 0x000000000032,
	WARNING_ACCESS_RIGHT_NOT_CHANGED				= 0x000000000033,
	WARNING_NO_CHIP_ID								= 0x000000000034,
	WARNING_BOX_ALREADY_EXIST						= 0x000000000035,
	WARNING_BOX_NAME_UPDATE_FAILED					= 0x000000000036,
	WARNING_ARTICLE_POSITION_ALREADY_EXIST			= 0x000000000037,
	WARNING_ARTICLE_POSITION_INSERT_ERROR			= 0x000000000038,
	WARNING_ARTICLE_POSITION_BOX_UPDATE				= 0x000000000039,
	WARNING_ARTICLE_OUTSOURCE_ERROR_GENERAL			= 0x00000000003A,
	WARNING_ARTICLE_OUTSOURCE_TO_LOW_COUNT			= 0x00000000003B,
	WARNING_ARTICLE_OUTSOURCE_NO_DATA				= 0x00000000003C,
	WARNING_ARTICLE_EDIT_NO_ROW_SELECTED			= 0x00000000003D,
	WARNING_ARTICLE_EDIT_MULTIPLE_ROWS				= 0x00000000003E,
	WARNING_ARTICLE_INSERT_ARTICLE_EXIST			= 0x00000000003F,
	WARNING_ARTICLE_INSERT_MISSING_NAME				= 0x000000000040,
	WARNING_ARTICLE_INSERT_MISSING_NUMBER			= 0x000000000041,
	WARNING_ARTICLE_INSERT_MISSING_MANUFACTURER		= 0x000000000042,
	WARNING_ARTICLE_POSITION_NOT_SET				= 0x000000000043,
	WARNING_ARTICLE_POSITION_NOT_UPDATED			= 0x000000000044,
	WARNING_ARTICLE_BOX_NOT_EXIST					= 0x000000000045,
	WARNING_ARTICLE_NOT_DELETED						= 0x000000000046,
	WARNING_ARTICLE_BOX_POSITION_IN_USE				= 0x000000000047,
	WARNING_ARTICLE_BOX_NAME_EXIST_ADD_NEW			= 0x000000000048,
	WARNING_ARTICLE_MISSING_POSITION_AISLE			= 0x000000000049,
	WARNING_ARTICLE_MISSING_POSITION_SIDE			= 0x00000000004A,
	WARNING_ARTICLE_MISSING_POSITION_COLUMN			= 0x00000000004B,
	WARNING_ARTICLE_MISSING_POSITION_ROW			= 0x00000000004C,
	WARNING_ARTICLE_MISSING_BOX_NAME				= 0x00000000004D,
	WARNING_ARTICLE_BOX_NAME_TO_SHORT				= 0x00000000004E,
	WARNING_ARTICLE_BOX_UPDATE_FAILED				= 0x00000000004F,
	WARNING_ARTICLE_BOX_ADD_FAILED					= 0x000000000050,
	WARNING_ARTICLE_BOX_REALY_DELETE				= 0x000000000051,
	WARNING_USER_OLD_PASSWORD_REQUIRED				= 0x000000000052,
	ERROR_USER_CAN_NOT_CHANGE_PASSWORD_OTHER_USER	= 0x000000000053,
	WARNING_USER_PASSWORD_NOT_FOUND					= 0x000000000054,
	WARNING_USER_OLD_PASSWORD_WRONG					= 0x000000000055,
	ERROR_USER_CAN_NOT_CHANGE_OTHER_USER_MAIL		= 0x000000000056,
	ERROR_USER_CAN_NOT_CHANGE_OTHER_USER_RFID		= 0x000000000057,
	WARNING_RFID_DATA_IN_USE						= 0x000000000058,
	WARNING_RFID_CAN_NOT_BE_CHANGED					= 0x000000000059,
	ERROR_USER_CAN_NOT_CHANGE_OTHER_USER_RIGHTS		= 0x00000000005A,
	WARNING_ARTICLE_BARCODE_ALREADY_IN_USE			= 0x00000000005B,
	WARNING_ARTICLE_BOX_ARTICLE_NO_UPDATED			= 0x00000000005C,
	WARNING_SETTINGS_FULL_SCREEN					= 0x00000000005D,
	WARNING_ADMIN_CHANGE_USER_MAIL					= 0x00000000005E,
	WARNING_ADMIN_CHANGE_USER_PASSWORD				= 0x00000000005F,
	WARNING_ADMIN_CHANGE_RFID						= 0x000000000060,
	WARNING_PASSWORD_EMPTY							= 0x000000000061,
	WARNING_COST_UNIT_NEED_TO_SELECTED				= 0x000000000062,
	WARNING_COST_UNIT_CORRECT_SELECTED				= 0x000000000063,
	ERROR_NOT_ENOUGH_RIGHTS							= 0x000000000064,
	WARNING_ARTICLE_STORE_NEED_QUANTITY				= 0x000000000065,
	WARNING_MESSAGE_PARTIAL_DELIVERY				= 0x000000000066,
	WARNING_DELETE_BOX_MUST_UPDATE_POSITION			= 0x000000000067,
	WARNING_ARTICLE_STORE_MORE_AS_ORDERED			= 0x000000000068,
	WARNING_DELETE_BARCODE_CONFIRM					= 0x000000000069,
	WARNING_NO_IMAGE_TO_PRINT						= 0x000000000070,
	ERROR_NO_SHOW_ERROR_OR_WARNING					= 0x000000000071,
	WARNING_BACK_WITHOUT_SAVE						= 0x000000000072,
};

enum class SuccessfullCodes : std::uint64_t
{
	MESSAGE_CODE_NO_DATA								= 0xDEAD000,
	MESSAGE_USER_SUCCESSFULLY_CREATED					= 0xDEAD001,
	MESSAGE_ACCESS_RIGHTS_SUCCESSFULLY_CHANGED			= 0xDEAD002,
	MESSAGE_PASSWORD_SUCCESSFULLY_CHANGED				= 0xDEAD003,
	MESSAGE_EMAIL_SUCCESSFULLY_CHANGED					= 0xDEAD004,
	MESSAGE_ARTICLE_SUCCESSFULLY_OUTSOURCE				= 0xDEAD005,
	MESSAGE_ARTICLE_SUCCESSFULLY_STORED					= 0xDEAD006,
	MESSAGE_ARTICLE_POSITION_IN_BOX_SUCCESSFULLY_UPDATE	= 0xDEAD007,
	MESSAGE_ARTICLE_POSITION_UPDATED					= 0xDEAD008,
	MESSAGE_ARTICLE_SUCCESSFULLY_UPDATED				= 0xDEAD009,
	MESSAGE_ARTICLE_SUCCESSFULLY_DELETED				= 0xDEAD010,
	MESSAGE_ARTICLE_BOX_SUCCESSFULLY_UPDATED			= 0xDEAD011,
	MESSAGE_ARTICLE_BOX_SUCCESSFULLY_ADDED				= 0xDEAD012,
	MESSAGE_ARTICLE_BOX_DELETED							= 0xDEAD013,
	MESSAGE_USER_SUCCESSFULLY_CHANGED					= 0xDEAD014,
	MESSAGE_USER_RFID_SUCCESSFULLY_CHANGED				= 0xDEAD015,
	MESSAGE_ERROR_NO_DATA								= 0xDEAD016,
	MESSAGE_SETTINGS_SUCCESSFULLY_SAVED					= 0xDEAD017,
	MESSAGE_ARTICLE_SUCCESSFULLY_ADDED					= 0xDEAD018,
	MESSAGE_ORDER_SUCCESSFULLY_AND_COMPLETE_STORED		= 0xDEAD019,
	MESSAGE_ORDER_SUCCESSFULLY_PARTIAL_STORED			= 0xDEAD020,
};

enum class Styles
{
	STYLE_LIGHT								= 0,
	STYLE_DARK								= 1,
	STYLE_LIGHT_DARK						= 2,
	STYLE_DARK_FLAT							= 3,
	STYLE_NEW_2								= 4,
	STYLE_NEW_3								= 5,
	STYLE_NEW_4								= 6,
};

enum class SelectLanguage : std::uint8_t
{
	LANG_EN_GB								= 0,
	LANG_DE_DE								= 1,
	LANG_JA_JP								= 2,
};

inline const std::string stringLang_en_GB = "en_GB";
inline const std::string stringLang_de_DE = "de_DE";
inline const std::string stringLang_ja_JP = "ja_JP";


inline std::string GetSelectLangaugeString(SelectLanguage _language)
{
	switch (_language)
	{
		case SelectLanguage::LANG_EN_GB: return "en_GB";
		case SelectLanguage::LANG_DE_DE: return "de_DE";
		case SelectLanguage::LANG_JA_JP: return "ja_JP";
		default: return "en_GB";
	}
}

inline std::uint8_t GetSelectLanguageID(const std::string& _language)
{
	if (_language == stringLang_en_GB)
		return std::uint8_t(SelectLanguage::LANG_EN_GB);

	if (_language == stringLang_de_DE)
		return std::uint8_t(SelectLanguage::LANG_DE_DE);

	if (_language == stringLang_ja_JP)
		return std::uint8_t(SelectLanguage::LANG_JA_JP);

	return std::uint8_t(SelectLanguage::LANG_EN_GB);
}

enum class LogFilterFlags : std::uint32_t
{
	LOG_FLAG_CREATE_ARTICLE					= 0x00001,
	LOG_FLAG_CHANGE_ARTICLE					= 0x00002,
	LOG_FLAG_DELETE_ARTICLE					= 0x00004,
	LOG_FLAG_CHANGE_ARTICLE_COUNT			= 0x00008,
	LOG_FLAG_CREATE_ARTICLE_BOX				= 0x00010,
	LOG_FLAG_CHANGE_ARTICLE_BOX				= 0x00020,
	LOG_FLAG_DELETE_ARTICLE_BOX				= 0x00040,
	LOG_FLAG_CREATE_USER					= 0x00080,
	LOG_FLAG_CHANGE_USER					= 0x00100,
	LOG_FLAG_DELETE_USER					= 0x00200,
	LOG_FLAG_CHANGE_SETTINGS				= 0x00400,
};

enum class LoadCurrency : std::uint8_t
{
	LOAD_CURRENCY_ALL						= 0x000,
	LOAD_CURRENCY_VALOR						= 0x001,
	LOAD_CURRENCY_CONQUEST					= 0x002,
};

enum class CurrencyID : std::uint16_t
{
	CURRENCY_ID_VALOR_POINTS				= 396,
	CURRENCY_ID_JUSTICE_POINTS				= 395, // Maximum of 400000
	CURRENCY_ID_CONQUEST_POINTS				= 390,
	CURRENCY_ID_HONOR_POINTS				= 392, // Maximum of 400000
};

constexpr auto CurrencyMaximumQuantity = 400000;;

// inline std::map<std::string, std::string> sStoredSettings;
