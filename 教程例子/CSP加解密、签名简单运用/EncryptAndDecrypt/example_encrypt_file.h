#pragma once
//本程序通过调用CSP实现文件AES_128加解密功能
#include "stdafx.h"
#include <windows.h>
#include <wincrypt.h>

#define BLOCK_SIZE 1024
#define ALG_SYM_ALGO CALG_3DES

/*****************************************************
*函数名：EncryptFile
*功  能：加密文件
*入  参：PCHAR szSource,		待加密的文件名
PCHAR szDestination,	加密后的文件名
PCHAR passwd);         加密口令
*出  参：无
*返回值：BOOL，TRU为加密成功，FALSE为加密失败
******************************************************/
BOOL EncryptFile(PCHAR szSource, PCHAR szDestination, PCHAR passwd);

/*****************************************************
*函数名：DecryptFile
*功  能：解密文件
*入  参：PCHAR szSource,		待加密的文件名
PCHAR szDestination,	加密后的文件名
PCHAR passwd);         解密口令
*出  参：无
*返回值：BOOL，TRU为加密成功，FALSE为加密失败
******************************************************/
BOOL DecryptFile(PCHAR szSource, PCHAR szDestination, PCHAR passwd);

int example_three()
{
	CHAR szEncSource[] = "E:\\vs2017project\\EncryptAndDecrypt\\Debug\\test.txt";
	CHAR szEncDes[] = "test_enc.txt";

	CHAR szDecSource[] = "test_enc.txt";
	CHAR szDecDes[] = "test_dec.txt";

	CHAR * passwd = "123456";

	BOOL bRet = EncryptFile(szEncSource, szEncDes, passwd);
	if (bRet)
		printf("EncryptFile success!\n");

	bRet = DecryptFile(szDecSource, szDecDes, passwd);
	if (bRet)
		printf("DecryptFile success!\n");
	getchar();
	return 0;
}

BOOL EncryptFile(PCHAR szSource, PCHAR szDestination, PCHAR passwd)
{
	//声明变量
	FILE* fSource = NULL;
	FILE* fDestination = NULL;

	if ((fSource = fopen(szSource, "rb")) == NULL)
	{
		return FALSE;
	}

	if ((fDestination = fopen(szDestination, "wb")) == NULL)
	{
		return FALSE;
	}

	//一、获得一个CSP句柄
	HCRYPTPROV hCryptProv;
	BOOL bRet = CryptAcquireContext(
		&hCryptProv,
		NULL,			//密钥容器名，NULL表示使用默认容器
		NULL,			//CSP_NAME
		PROV_RSA_AES,
		0
	);
	if (!bRet)
	{
		bRet = CryptAcquireContext(&hCryptProv, NULL, NULL,	PROV_RSA_AES, CRYPT_NEWKEYSET);
		if (!bRet)
		{
			printf("CryptAcquireContext fail!");
			return FALSE;
		}
	}

	//创建一个会话密钥（session key），会话密钥也叫对称密钥
	// 一个Session是指从调用函数CryptAcquireContext到调用函数CryptReleaseContext 期间的阶段。
	//会话密钥只能存在于一个会话过程
	HCRYPTHASH hCryptHash;
	//创建hash对象
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hCryptHash);
	if (!bRet)
	{
		printf("CryptCreateHash fail!");
		return FALSE;
	}
	//用输入的密码作哈稀散列
	bRet = CryptHashData(hCryptHash, (BYTE*)passwd, strlen(passwd), 0);
	if (!bRet)
	{
		printf("CryptHashData fail!");
		return FALSE;
	}

	//用哈稀散列生成会话密钥
	HCRYPTKEY hCryptKey;
	bRet = CryptDeriveKey(hCryptProv, ALG_SYM_ALGO, hCryptHash, CRYPT_EXPORTABLE, &hCryptKey);
	CryptDestroyHash(hCryptHash);
	if (!bRet)
	{
		printf("CryptDeriveKey fail!");
		return FALSE;
	}

	//对文件进行加密,hCryptKey已经与加密算法相关联了CALG_AES_128
	while (TRUE)
	{
		//分组加密，分组大小1024
		CHAR buf[BLOCK_SIZE] = { 0 };
		DWORD nReadLen = fread(buf, 1, BLOCK_SIZE, fSource);
		if (nReadLen <= 0)
			break;

		CryptEncrypt(hCryptKey,
			NULL,//如果数据同时进行散列和加密，这里传入一个散列对象
			feof(fSource),//如果是最后一个块为TRUE
			0,
			(BYTE*)buf,//输入被加密的数据，输出加密数据
			&nReadLen,//输入输入数据长度，输出加密后数据长度
			BLOCK_SIZE//buf的大小
		);
		fwrite(buf, 1, nReadLen, fDestination);
	}

	//加密完成，释放
	fclose(fSource);
	fclose(fDestination);

	if (hCryptKey)
		CryptDestroyKey(hCryptKey);

	if (hCryptHash)
		CryptDestroyHash(hCryptHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);

	return TRUE;
}


BOOL DecryptFile(PCHAR szSource,
	PCHAR szDestination,
	PCHAR passwd)
{
	//声明变量
	FILE* fSource = NULL;
	FILE* fDestination = NULL;

	if ((fSource = fopen(szSource, "rb")) == NULL)
	{
		return FALSE;
	}

	if ((fDestination = fopen(szDestination, "wb")) == NULL)
	{
		return FALSE;
	}

	//一、获得一个CSP句柄
	HCRYPTPROV hCryptProv;
	BOOL bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, 0);
	if (!bRet)
	{
		bRet = CryptAcquireContext(
			&hCryptProv,
			NULL,			//密钥容器名，NULL表示使用默认容器
			NULL,			//用户登录名
			PROV_RSA_AES,
			CRYPT_NEWKEYSET //创建密钥容器
		);
		if (!bRet)
		{
			printf("CryptAcquireContext fail!");
			return FALSE;
		}
	}

	//创建一个会话密钥（session key），会话密钥也叫对称密钥
	// 一个Session是指从调用函数CryptAcquireContext到调用函数CryptReleaseContext 期间的阶段。
	//会话密钥只能存在于一个会话过程
	HCRYPTHASH hCryptHash;
	//创建hash对象
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hCryptHash);
	if (!bRet)
	{
		printf("CryptCreateHash fail!");
		return FALSE;
	}
	//用输入的密码作哈稀散列
	bRet = CryptHashData(hCryptHash, (BYTE*)passwd, strlen(passwd), 0);
	if (!bRet)
	{
		printf("CryptHashData fail!");
		return FALSE;
	}

	//用哈稀散列生成会话密钥
	HCRYPTKEY hCryptKey;
	bRet = CryptDeriveKey(hCryptProv, ALG_SYM_ALGO, hCryptHash, CRYPT_EXPORTABLE, &hCryptKey);
	if (!bRet)
	{
		printf("CryptDeriveKey fail!");
		return FALSE;
	}

	//对文件进行解密,hCryptKey已经与加密算法相关联了CALG_AES_128
	while (TRUE)
	{
		//分组加密，分组大小1024
		CHAR buf[BLOCK_SIZE] = { 0 };
		DWORD nReadLen = fread(buf, 1, BLOCK_SIZE, fSource);
		if (nReadLen <= 0)
			break;

		CryptDecrypt(hCryptKey,
			NULL,//如果数据同时进行散列和加密，这里传入一个散列对象
			feof(fSource),//如果是最后一个块为TRUE
			0,
			(BYTE*)buf,//输入被加密的数据，输出加密数据
			&nReadLen//输入输入数据长度，输出加密后数据长度
		);
		fwrite(buf, 1, nReadLen, fDestination);
	}

	//加密完成，释放
	fclose(fSource);
	fclose(fDestination);

	if (hCryptKey)
		CryptDestroyKey(hCryptKey);

	if (hCryptHash)
		CryptDestroyHash(hCryptHash);

	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);

	return TRUE;
}