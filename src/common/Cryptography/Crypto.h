/*
Copyright 2023 - 2024 by Severin

*/

#pragma once

extern "C" {
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/blowfish.h>
}

#include <algorithm>
#include <fstream>
#include <cstring>
#include <string>
#include <bitset>
#include <random>
#include <iostream>
#include <vector>

class Crypto
{
public:
	Crypto();
	Crypto(const std::string& key);
	~Crypto();


	bool generateKey(int bits = 2048);

	std::string encryptRSA(const std::string& plaintext) const;
	std::string decryptRSA(const std::string& ciphertext);

	std::string removeTrailingNulls(const std::string& str);
	std::string removeCharacters(const std::string& str);
	std::string removeToMoreCharacters(const std::string str, std::uint32_t maxLength);

	std::string EncryptNumberWithRadomValue(std::uint32_t number);
	std::uint32_t DecryptNumberWithRandomValue(const std::string& shiftedString);

	// AES-256 Encryption
	std::vector<unsigned char> EncryptData(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
	// AES-256 Decryption
	std::vector<unsigned char> DecryptData(const std::vector<unsigned char>& encryptedData, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

	void SaveKeyAndIV(const std::string& filename, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
	void LoadKeyAndIV(const std::string& filename, std::vector<unsigned char>& key, std::vector<unsigned char>& iv);
	void GenerateRandomKeyAndIV(std::vector<unsigned char>& key, std::vector<unsigned char>& iv, size_t keySize, size_t ivSize);

private:
	std::vector<unsigned char> generateIV(int size = 16); // default value for a 128-Bit-IV
	

	std::string keyTest = "aK827V62sR21aF71"; // 16 Bytes for AES-128

	std::string charactersToRemove = "\x01\x02\x03\x04\x05\x06\x07\x0F\x1E";

	RSA* rsaKey;
};

