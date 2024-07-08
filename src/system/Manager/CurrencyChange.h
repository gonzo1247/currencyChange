#pragma once

#include <vector>
#include <map>

#include "MySQLConnection.h"
#include "SharedDefines.h"

struct CurrencyData
{
	std::uint64_t _playerGUID;
	std::uint32_t _quantityValor;
	std::uint32_t _quantityJustice;
	std::uint32_t _quantityConquest;
	std::uint32_t _quantityHonor;
	std::uint64_t _moneyUpdatePvE;
	std::uint64_t _moneyUpdatePvP;

	CurrencyData() :
		_playerGUID(0),
		_moneyUpdatePvE(0), _moneyUpdatePvP(0),
		_quantityValor(0), _quantityJustice(0),
		_quantityConquest(0), _quantityHonor(0) {}
};

struct MoneyPvE
{
	std::uint32_t _gold = 0;
	std::uint32_t _silver = 47;
	std::uint32_t _copper = 0;
};

struct MoneyPvP
{
	std::uint32_t _gold = 0;
	std::uint32_t _silver = 35;
	std::uint32_t _copper = 0;
};

struct MoneyMail
{
	std::uint32_t _gold = 0;
	std::uint32_t _silver = 35;
	std::uint32_t _copper = 0;
};

struct ConvertData
{
	std::uint32_t _pveConvert = 0;
	std::uint32_t _pvpConvert = 0;
	std::uint32_t _moneyConvert = 0;
};

class CurrencyChange
{
public:
	CurrencyChange(std::shared_ptr<MySQLConnection> connection, MoneyPvE moneyPvE, MoneyPvP moneyPvP);

	std::uint32_t LoadCurrencyDataFromDatabase(LoadCurrency currency = LoadCurrency::LOAD_CURRENCY_ALL);

	ConvertData ConvertCurrency();
	void CreateSQLUpdateOutput(std::uint32_t mailID);

private:
	void SelectCurrencyIDData(std::uint16_t& firstID, std::uint16_t& secondID, std::uint16_t& justiceID, std::uint16_t& honorID, LoadCurrency& currency);
	void ConvertValorPoints(CurrencyData& data, ConvertData& convertData);
	void ConvertConquestPoints(CurrencyData& data, ConvertData& convertData);

	std::uint32_t CalculateCopperValuePvE(std::uint32_t points);
	std::uint32_t CalculateCopperValuePvP(std::uint32_t points);

	MoneyMail ConvertCopperToMoney(std::uint32_t copper);

	// for (const auto& entry : characterDataMap)
	std::map<std::uint64_t, CurrencyData> _currencyMapData;
	MoneyPvE _moneyPvE;
	MoneyPvP _moneyPvP;

	std::shared_ptr<MySQLConnection> _connection;
};


/*
// Print the results to verify
	for (const auto& entry : characterDataMap) {
		std::cout << "CharacterGUID: " << entry.first << std::endl;
		std::cout << "Currency 1: ID=" << entry.second.currency1ID << " Quantity=" << entry.second.quantity1 << std::endl;
		std::cout << "Currency 2: ID=" << entry.second.currency2ID << " Quantity=" << entry.second.quantity2 << std::endl;
		std::cout << "Currency 3: ID=" << entry.second.currency3ID << " Quantity=" << entry.second.quantity3 << std::endl;
		std::cout << "Currency 4: ID=" << entry.second.currency4ID << " Quantity=" << entry.second.quantity4 << std::endl;
	}


*/

