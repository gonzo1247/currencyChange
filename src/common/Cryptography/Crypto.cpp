/*
Copyright 2023 - 2024 by Severin

*/

#include "Crypto.h"

Crypto::Crypto() : rsaKey(nullptr)
{
	generateKey();
}

Crypto::~Crypto()
{
	if (rsaKey) 
	{
		RSA_free(rsaKey);
		rsaKey = nullptr;
	}
}

bool Crypto::generateKey(int bits /*= 2048*/)
{
	rsaKey = RSA_generate_key(bits, RSA_F4, nullptr, nullptr);
	return rsaKey != nullptr;
}

std::string Crypto::encryptRSA(const std::string& plaintext) const
{
	if (!rsaKey)
		return "";

	int rsaSize = RSA_size(rsaKey);
	std::vector<unsigned char> ciphertext(rsaSize);

	size_t plaintextLength = plaintext.length();
	int result;

	if (plaintextLength <= static_cast<size_t>(std::numeric_limits<int>::max()))
		result = RSA_public_encrypt(static_cast<int>(plaintextLength),
			reinterpret_cast<const unsigned char*>(plaintext.c_str()),
			ciphertext.data(), rsaKey, RSA_PKCS1_PADDING);
	else
		result = -1;

	if (result == -1)
	{
		ERR_print_errors_fp(stderr);
		return "";
	}

	return std::string(reinterpret_cast<char*>(ciphertext.data()), result);
}

std::string Crypto::decryptRSA(const std::string& ciphertext)
{
	if (!rsaKey)
		return "";

	int rsaSize = RSA_size(rsaKey);
	std::vector<unsigned char> plaintext(rsaSize);

	size_t ciphertextLength = ciphertext.length();
	int result;

	if (ciphertextLength <= static_cast<size_t>(std::numeric_limits<int>::max())) 
		result = RSA_private_decrypt(static_cast<int>(ciphertextLength),
			reinterpret_cast<const unsigned char*>(ciphertext.c_str()),
			plaintext.data(), rsaKey, RSA_PKCS1_PADDING);
	else
		result = -1;

	if (result == -1)
	{
		ERR_print_errors_fp(stderr);
		return "";
	}

	return std::string(reinterpret_cast<char*>(plaintext.data()), result);
}

std::string Crypto::removeTrailingNulls(const std::string& str)
{
	size_t nullPos = str.find('\0');
	if (nullPos != std::string::npos)
		return str.substr(0, nullPos);

	return str;
}

std::string Crypto::removeCharacters(const std::string& str)
{
	std::string result = str;
	for (char ch : charactersToRemove)
		result.erase(std::remove(result.begin(), result.end(), ch), result.end());

	return result;
}

std::string Crypto::removeToMoreCharacters(const std::string str, std::uint32_t maxLength)
{
	if (str.length() >= maxLength)
	{
		std::string returnString = str.substr(0, maxLength);
		return returnString;
	}

	return str;
}

std::string Crypto::EncryptNumberWithRadomValue(std::uint32_t number)
{
	// Generate random number
	std::random_device rd;
	std::mt19937 gen(rd());
    std::uniform_int_distribution<std::uint32_t> dist(0, 31); // Only use 0 to 31 so as not to exceed 32 bits
    std::uint32_t randomValue = dist(gen);

	// Convert number into a string
	std::bitset<32> shiftedBits(number);

	// Shift number to the left
	shiftedBits <<= randomValue;

	// Append the random number to the end of the string
	std::string shiftedString = shiftedBits.to_string() + '|' + std::to_string(randomValue);

	return shiftedString;
}

std::uint32_t Crypto::DecryptNumberWithRandomValue(const std::string& shiftedString)
{
	// Find the position of the separator
	std::size_t separatorPos = shiftedString.find_last_of('|');
	if (separatorPos == std::string::npos) 
	{
		// Separator not found, handle the error
		std::cerr << "Error: Separator not found in the string." << std::endl;
		return 0;
	}

	// Extract the original bits (without the separator) and convert to uint32_t
	std::bitset<32> shiftedBits(shiftedString.substr(0, separatorPos));
	std::uint32_t originalNumber = shiftedBits.to_ulong();

	// Extract the random value from the string
	std::uint32_t randomValue = std::stoi(shiftedString.substr(separatorPos + 1));

	// Shift the original number back to the right
	shiftedBits >>= randomValue;
	originalNumber = shiftedBits.to_ulong();

	return originalNumber;
}

std::vector<unsigned char> Crypto::EncryptData(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	std::vector<unsigned char> encryptedData(data.size() + EVP_MAX_BLOCK_LENGTH);

	// Initialization of the encryption process
	EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());

	// Encrypting the data
	int encryptedLength;
	EVP_EncryptUpdate(ctx, encryptedData.data(), &encryptedLength, data.data(), static_cast<int>(data.size()));

	// Completing the encryption process
	int finalLength;
	EVP_EncryptFinal_ex(ctx, encryptedData.data() + encryptedLength, &finalLength);

	EVP_CIPHER_CTX_free(ctx);

	// Adjust the size of the encrypted text
	encryptedData.resize(encryptedLength + finalLength);

	return encryptedData;
}

std::vector<unsigned char> Crypto::DecryptData(const std::vector<unsigned char>& encryptedData, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	std::vector<unsigned char> decryptedData(encryptedData.size());

	// Initialization of the encryption process
	EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());

	// Decrypting the data
	int decryptedLength;
	EVP_DecryptUpdate(ctx, decryptedData.data(), &decryptedLength, encryptedData.data(), static_cast<int>(encryptedData.size()));

	// Completing the decryption process
	int finalLength;
	EVP_DecryptFinal_ex(ctx, decryptedData.data() + decryptedLength, &finalLength);

	EVP_CIPHER_CTX_free(ctx);

	// Adjust the size of the decrypted text
	decryptedData.resize(decryptedLength + finalLength);

	return decryptedData;
}

void Crypto::SaveKeyAndIV(const std::string& filename, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
	std::ofstream file(filename, std::ios::binary);
	file.write(reinterpret_cast<const char*>(key.data()), key.size());
	file.write(reinterpret_cast<const char*>(iv.data()), iv.size());
	file.close();
}

void Crypto::LoadKeyAndIV(const std::string& filename, std::vector<unsigned char>& key, std::vector<unsigned char>& iv)
{
	std::ifstream file(filename, std::ios::binary);
	key.clear();
	iv.clear();

	size_t keySize = 32;  // Example: AES-256
	size_t ivSize = 16;   // Example: AES-256

	key.resize(keySize);
	iv.resize(ivSize);

	file.read(reinterpret_cast<char*>(key.data()), keySize);
	file.read(reinterpret_cast<char*>(iv.data()), ivSize);

	file.close();
}

std::vector<unsigned char> Crypto::generateIV(int size /*= 16*/)
{
	// Seed for random number generation
	std::random_device rd;
	std::mt19937 gen(rd());

	// Distribution for bytes (0 to 255)
	std::uniform_int_distribution<> dis(0, 255);

	// create vector for iv
	std::vector<unsigned char> iv(size);

	// Fill the vector with random bytes
	for (int i = 0; i < size; ++i)
		iv[i] = static_cast<unsigned char>(dis(gen));

	return iv;
}

void Crypto::GenerateRandomKeyAndIV(std::vector<unsigned char>& key, std::vector<unsigned char>& iv, size_t keySize, size_t ivSize)
{
	key.resize(keySize);
	iv.resize(ivSize);

	// Generate random bytes for the key and the IV
	RAND_bytes(key.data(), static_cast<int>(keySize));
	RAND_bytes(iv.data(), static_cast<int>(ivSize));
}
