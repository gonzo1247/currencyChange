
#include <string>
#include <iostream>
#include <fstream>
#include <format>

#include "CurrencyChange.h"
#include "TimeMgr.h"

CurrencyChange::CurrencyChange(std::shared_ptr<MySQLConnection> connection, MoneyPvE moneyPvE, MoneyPvP moneyPvP) : _connection(connection), _moneyPvE(moneyPvE), _moneyPvP(moneyPvP)
{
}

std::uint32_t CurrencyChange::LoadCurrencyDataFromDatabase(LoadCurrency currency /*= LoadCurrency::LOAD_CURRENCY_ALL*/)
{
	std::uint16_t _loadFirstCurrencyID = 0;
	std::uint16_t _loadSecondCurrencyID = 0;
	std::uint16_t _justiceCurrencyID = 0; // default 0: if the justice points are not required for select, the value remains 0
	std::uint16_t _honorCurrencyID = 0; // default 0: if the honor points are not required for select, the value remains 0
	SelectCurrencyIDData(_loadFirstCurrencyID, _loadSecondCurrencyID, _justiceCurrencyID, _honorCurrencyID, currency);

	auto selectStmt = _connection->GetUniquePreparedStatement(DB_CHARS_SELECT_CURRENCY);
	selectStmt->setUInt(1, _loadFirstCurrencyID);
	selectStmt->setUInt(2, _loadSecondCurrencyID);
	selectStmt->setUInt(3, _justiceCurrencyID);
	selectStmt->setUInt(4, _honorCurrencyID);

	auto selectResult = _connection->ExecuteSelect(std::move(selectStmt));

	//				1			2			3
	// "SELECT CharacterGuid, Currency, Quantity FROM character_currency WHERE Currency = ? OR Currency = ?

	if (!selectResult)
		return 0;

	while (selectResult->next())
	{
		std::uint64_t _characterGUID = selectResult->getUInt64(1);
		CurrencyID _currencyID = static_cast<CurrencyID>(selectResult->getUInt(2));
		std::uint32_t _quantity = selectResult->getUInt(3);

		CurrencyData& data = _currencyMapData[_characterGUID];

		switch (_currencyID)
		{
			case CurrencyID::CURRENCY_ID_VALOR_POINTS:
				data._quantityValor = _quantity;
				break;
			case CurrencyID::CURRENCY_ID_JUSTICE_POINTS:
				data._quantityJustice = _quantity;
				break;
			case CurrencyID::CURRENCY_ID_CONQUEST_POINTS:
				data._quantityConquest = _quantity;
				break;
			case CurrencyID::CURRENCY_ID_HONOR_POINTS:
				data._quantityHonor = _quantity;
				break;
			default:
				break;
		}
	}

	return _currencyMapData.size();
}

ConvertData CurrencyChange::ConvertCurrency()
{
	ConvertData _convert;
	for (auto& entry : _currencyMapData)
	{
		if (entry.second._quantityValor > 1)
			ConvertValorPoints(entry.second, _convert);

		if (entry.second._quantityConquest > 1)
			ConvertConquestPoints(entry.second, _convert);
	}

	return _convert;
}

void CurrencyChange::CreateSQLUpdateOutput(std::uint32_t mailID)
{
	// set Valor and Conquest Points to 0
	std::string sqlQuerryUpdateValorConquestPoints = std::format("UPDATE character_currency SET Quantity = 0 WHERE Currency IN ({}, {});", static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_VALOR_POINTS),
		static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_CONQUEST_POINTS));

	// open file for write
	std::ofstream outFile("currency_updates.txt");

	if (outFile.is_open())
	{
		outFile << sqlQuerryUpdateValorConquestPoints << "\n";

		std::uint32_t _currentMailID = mailID;

		std::unique_ptr<TimeMgr> _timeMgr = std::make_unique<TimeMgr>();

		for (auto& entry : _currencyMapData)
		{
			// Update Justice Points
			std::string JusticePointUpdate = std::format("UPDATE character_currency SET Quantity = {} WHERE Currency = {} AND CharacterGuid = {}", entry.second._quantityJustice, 
				static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_JUSTICE_POINTS), entry.first);

			outFile << "-- Justice Point Update" << std::endl;
			outFile << JusticePointUpdate << "\n";

			// Update Honor Points
			std::string HonorPointUpdate = std::format("UPDATE character_currency SET Quantity = {} WHERE Currency = {} AND CharacterGuid = {}", entry.second._quantityHonor,
				static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_HONOR_POINTS), entry.first);

			outFile << "-- Honor Point Update" << std::endl;
			outFile << HonorPointUpdate << "\n";

			// Create Money Mail if needed
			if (entry.second._moneyUpdatePvE > 1)
			{
				MoneyMail _mail = ConvertCopperToMoney(entry.second._moneyUpdatePvE);
				std::string MoneyPvEUpdate = std::format("INSERT INTO `mail` (`id`, `messageType`, `stationery`, `mailTemplateId`, `sender`, `receiver`, `subject`, `body`, `expire_time`, `deliver_time`, `money`) VALUES"
					"({}, 0, 61, 0, 48668, {} 'Umwandlung Tapferkeitspunkte', 'Hallo,$B$B eure Tapferkeitspunkte wurden umgewandelt in Gerechtigkeitspunkte. Da ihr jedoch über das Limit von 4000 Gerechtigkeitspunkten "
					" gekommen seid, wurden die überschüssigen Punkte in Gold umgewandelt. $B$B Ihr erhaltet daher: {} Gold, {} Silber, {} Kupfer. $B$B"
					"Grüße, $B$B Euer RoA Team.' , {}, {}, {});",
					_currentMailID, entry.first, _mail._gold, _mail._silver, _mail._copper, _timeMgr->GetThirtyDaysInFutureTimestamp(), _timeMgr->GetUnixTime(), entry.second._moneyUpdatePvE);

				outFile << "-- Valor / Justice Point Money Convert" << std::endl;
				outFile << MoneyPvEUpdate << "\n";
			}

			if (entry.second._moneyUpdatePvP > 1)
			{
				MoneyMail _mail = ConvertCopperToMoney(entry.second._moneyUpdatePvP);
				std::string MoneyPvPUpdate = std::format("INSERT INTO `mail` (`id`, `messageType`, `stationery`, `mailTemplateId`, `sender`, `receiver`, `subject`, `body`, `expire_time`, `deliver_time`, `money`) VALUES"
					"({}, 0, 61, 0, 48668, {}, 'Umwandlung Ehrenpunkte', 'Hallo,$B$B die nächste Arena Season steht vor der Tür. Daher wurden nun eure Ehrenpunkte, welche über dem Maximum von 4000 Ehrenpunkte liegen in Gold umgewandelt. "
					" $B$B Ihr erhaltet daher: {} Gold, {} Silber, {} Kupfer. $B$B"
					"Grüße, $B$B Euer RoA Team.' , {}, {}, {});",
					_currentMailID, entry.first, _mail._gold, _mail._silver, _mail._copper, _timeMgr->GetThirtyDaysInFutureTimestamp(), _timeMgr->GetUnixTime(), entry.second._moneyUpdatePvP);

				outFile << "-- Conquest / Honor Point Money Convert" << std::endl;
				outFile << MoneyPvPUpdate << "\n";
			}

			outFile << "-- ---------------------------------------------------------------------------------------" << std::endl << std::endl;
		}

		outFile.close();
	}
	else
	{
		// send warning!
	}
}

void CurrencyChange::SelectCurrencyIDData(std::uint16_t& firstID, std::uint16_t& secondID, std::uint16_t& justiceID, std::uint16_t& honorID, LoadCurrency& currency)
{
	switch (currency)
	{
		case LoadCurrency::LOAD_CURRENCY_ALL:
			firstID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_VALOR_POINTS);
			secondID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_CONQUEST_POINTS);
			justiceID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_JUSTICE_POINTS);
			honorID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_HONOR_POINTS);
			break;
		case LoadCurrency::LOAD_CURRENCY_VALOR:
			firstID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_VALOR_POINTS);
			justiceID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_JUSTICE_POINTS);
			break;
		case LoadCurrency::LOAD_CURRENCY_CONQUEST:
			firstID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_CONQUEST_POINTS);
			honorID = static_cast<std::uint16_t>(CurrencyID::CURRENCY_ID_HONOR_POINTS);
			break;
		default:
			break;
	}
}

void CurrencyChange::ConvertValorPoints(CurrencyData& data, ConvertData& convertData)
{
	bool _maximumJusticeReached = data._quantityJustice >= CurrencyMaximumQuantity;

	if (!_maximumJusticeReached)
	{
		std::uint32_t _newPoints = data._quantityJustice + data._quantityValor;
		std::uint32_t _excessPoints = 0;

		if (_newPoints > CurrencyMaximumQuantity)
			_excessPoints = _newPoints - CurrencyMaximumQuantity;

		data._quantityValor = 0;
		data._quantityJustice = _newPoints;

		convertData._pveConvert += 1;

		if (_excessPoints > 1)
		{
			data._moneyUpdatePvE = CalculateCopperValuePvE(_excessPoints);
			convertData._moneyConvert += 1;
		}
	}
	else // we have too many Justice Points right at the start
	{
		// have to convert all Valor Points into gold
		data._moneyUpdatePvE = CalculateCopperValuePvE(data._quantityValor);
		data._quantityValor = 0;

		convertData._pvpConvert += 1;
		convertData._moneyConvert += 1;
	}
}

void CurrencyChange::ConvertConquestPoints(CurrencyData& data, ConvertData& convertData)
{
	bool _maximumHonorReached = data._quantityHonor >= CurrencyMaximumQuantity;

	if (!_maximumHonorReached)
	{
		std::uint32_t _newPoints = data._quantityHonor + data._quantityConquest;
		std::uint32_t _excessPoints = 0;

		if (_newPoints > CurrencyMaximumQuantity)
			_excessPoints = _newPoints - CurrencyMaximumQuantity;

		data._quantityHonor = 0;
		data._quantityConquest = _newPoints;

		convertData._pvpConvert += 1;

		if (_excessPoints > 1)
		{
			data._moneyUpdatePvP = CalculateCopperValuePvP(_excessPoints);
			convertData._moneyConvert += 1;
		}
	}
	else // we have too many Honor Points right at the start
	{
		// have to convert all Conquest Points into gold
		data._moneyUpdatePvP = CalculateCopperValuePvP(data._quantityConquest);
		data._quantityConquest = 0;

		convertData._pvpConvert += 1;
		convertData._moneyConvert += 1;
	}
}

std::uint32_t CurrencyChange::CalculateCopperValuePvE(std::uint32_t points)
{
	// Divide points by 100
	std::uint32_t adjustedPoints = points / 100;

	// Calculation of the copper value
	std::uint32_t totalCopper = adjustedPoints * (_moneyPvE._gold * 10000 + _moneyPvE._silver * 100 + _moneyPvE._copper);

	return totalCopper;
}

std::uint32_t CurrencyChange::CalculateCopperValuePvP(std::uint32_t points)
{
	// Divide points by 100
	std::uint32_t adjustedPoints = points / 100;

	// Calculation of the copper value
	std::uint32_t totalCopper = adjustedPoints * (_moneyPvP._gold * 10000 + _moneyPvP._silver * 100 + _moneyPvP._copper);

	return totalCopper;
}

MoneyMail CurrencyChange::ConvertCopperToMoney(std::uint32_t copper)
{
	MoneyMail money;
	money._gold = copper / 10000;
	copper %= 10000;
	money._silver = copper / 100;
	copper %= 100;
	money._copper = copper;
	return money;
}

