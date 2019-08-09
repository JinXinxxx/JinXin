#pragma once
#include<string.h>
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<math.h>



class RSA{
public:
	void Init();
	bool IsPrimer(__int64 n);

	__int64 Gcd(__int64 a, __int64 b);

	void Ex_Gcd(__int64 a, __int64 b, __int64 & x, __int64 & y);

	void GetPrimers(__int64 & p, __int64 & q);

	__int64 Mod(unsigned __int64 a, unsigned __int64 b, unsigned __int64 n);

	void Create_Key(char * pub_key_file, char * pri_key_file);

	int Encrypt_Int16(char * pub_key_file, unsigned __int64 in);

	int DeEncrypt_Int16(char * pri_key_file, unsigned __int64 in);

	void Encrypt(char * filename, char * output, char * pub_key_file);

	void DeEncrypt(char * filename, char * output, char * pri_key_file);

};