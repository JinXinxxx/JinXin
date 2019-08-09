// Rsa.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include"加密.h"

int main()
{
	RSA Rsa;
	char filename[1024], pub_name[1024], pri_name[1024], output[1024];
	int tag;
	__int64 n;
	while (1)
	{
		puts("1.生成钥匙\n2.加密\n3.解密");
		scanf_s("%d", &tag);
		time_t start;
		switch (tag)
		{
		case 1:
			puts("请输入存放公钥的文件名:");
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> pub_name;
			puts("请输入存放私钥的文件名:");
			std::cin >> pri_name;
			start = time(0);
			Rsa.Create_Key(pub_name, pri_name);
			//printf("%g\n",difftime(time(0),start));
			break;
		case 2:
			puts("请输入存放公钥的文件名:");
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> pub_name;
			//puts("输入要加密的整数");
			//scanf("%I64d",&n);
			//printf("加密结果为：%d\n",RSA::Encrypt_Int16(pub_name,n));
			puts("请输入要加密的文件名:");
			std::cin >> filename;
			puts("请输入输出的文件名:");
			std::cin >> output;
			start = time(0);
			Rsa.Encrypt(filename, output, pub_name);
			//	printf("%g\n",difftime(time(0),start));

			break;
		case 3:
			puts("请输入存放私钥的文件名:");
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin >> pri_name;
			//puts("输入要解密的整数");
			//scanf("%I64d",&n);
			//printf("解密结果为：%d\n",RSA::DeEncrypt_Int16(pri_name,n));
			puts("请输入要解密的文件名:");
			std::cin >> filename;
			puts("请输入输出的文件名:");
			std::cin >> output;
			start = time(0);
			Rsa.DeEncrypt(filename, output, pri_name);
			//	printf("%g\n",difftime(time(0),start));

			break;
		}
	}
	return 0;
}