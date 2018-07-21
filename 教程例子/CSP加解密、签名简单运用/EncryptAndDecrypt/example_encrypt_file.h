#pragma once
//������ͨ������CSPʵ���ļ�AES_128�ӽ��ܹ���
#include "stdafx.h"
#include <windows.h>
#include <wincrypt.h>

#define BLOCK_SIZE 1024
#define ALG_SYM_ALGO CALG_3DES

/*****************************************************
*��������EncryptFile
*��  �ܣ������ļ�
*��  �Σ�PCHAR szSource,		�����ܵ��ļ���
PCHAR szDestination,	���ܺ���ļ���
PCHAR passwd);         ���ܿ���
*��  �Σ���
*����ֵ��BOOL��TRUΪ���ܳɹ���FALSEΪ����ʧ��
******************************************************/
BOOL EncryptFile(PCHAR szSource, PCHAR szDestination, PCHAR passwd);

/*****************************************************
*��������DecryptFile
*��  �ܣ������ļ�
*��  �Σ�PCHAR szSource,		�����ܵ��ļ���
PCHAR szDestination,	���ܺ���ļ���
PCHAR passwd);         ���ܿ���
*��  �Σ���
*����ֵ��BOOL��TRUΪ���ܳɹ���FALSEΪ����ʧ��
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
	//��������
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

	//һ�����һ��CSP���
	HCRYPTPROV hCryptProv;
	BOOL bRet = CryptAcquireContext(
		&hCryptProv,
		NULL,			//��Կ��������NULL��ʾʹ��Ĭ������
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

	//����һ���Ự��Կ��session key�����Ự��ԿҲ�жԳ���Կ
	// һ��Session��ָ�ӵ��ú���CryptAcquireContext�����ú���CryptReleaseContext �ڼ�Ľ׶Ρ�
	//�Ự��Կֻ�ܴ�����һ���Ự����
	HCRYPTHASH hCryptHash;
	//����hash����
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hCryptHash);
	if (!bRet)
	{
		printf("CryptCreateHash fail!");
		return FALSE;
	}
	//���������������ϡɢ��
	bRet = CryptHashData(hCryptHash, (BYTE*)passwd, strlen(passwd), 0);
	if (!bRet)
	{
		printf("CryptHashData fail!");
		return FALSE;
	}

	//�ù�ϡɢ�����ɻỰ��Կ
	HCRYPTKEY hCryptKey;
	bRet = CryptDeriveKey(hCryptProv, ALG_SYM_ALGO, hCryptHash, CRYPT_EXPORTABLE, &hCryptKey);
	CryptDestroyHash(hCryptHash);
	if (!bRet)
	{
		printf("CryptDeriveKey fail!");
		return FALSE;
	}

	//���ļ����м���,hCryptKey�Ѿ�������㷨�������CALG_AES_128
	while (TRUE)
	{
		//������ܣ������С1024
		CHAR buf[BLOCK_SIZE] = { 0 };
		DWORD nReadLen = fread(buf, 1, BLOCK_SIZE, fSource);
		if (nReadLen <= 0)
			break;

		CryptEncrypt(hCryptKey,
			NULL,//�������ͬʱ����ɢ�кͼ��ܣ����ﴫ��һ��ɢ�ж���
			feof(fSource),//��������һ����ΪTRUE
			0,
			(BYTE*)buf,//���뱻���ܵ����ݣ������������
			&nReadLen,//�����������ݳ��ȣ�������ܺ����ݳ���
			BLOCK_SIZE//buf�Ĵ�С
		);
		fwrite(buf, 1, nReadLen, fDestination);
	}

	//������ɣ��ͷ�
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
	//��������
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

	//һ�����һ��CSP���
	HCRYPTPROV hCryptProv;
	BOOL bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, 0);
	if (!bRet)
	{
		bRet = CryptAcquireContext(
			&hCryptProv,
			NULL,			//��Կ��������NULL��ʾʹ��Ĭ������
			NULL,			//�û���¼��
			PROV_RSA_AES,
			CRYPT_NEWKEYSET //������Կ����
		);
		if (!bRet)
		{
			printf("CryptAcquireContext fail!");
			return FALSE;
		}
	}

	//����һ���Ự��Կ��session key�����Ự��ԿҲ�жԳ���Կ
	// һ��Session��ָ�ӵ��ú���CryptAcquireContext�����ú���CryptReleaseContext �ڼ�Ľ׶Ρ�
	//�Ự��Կֻ�ܴ�����һ���Ự����
	HCRYPTHASH hCryptHash;
	//����hash����
	bRet = CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hCryptHash);
	if (!bRet)
	{
		printf("CryptCreateHash fail!");
		return FALSE;
	}
	//���������������ϡɢ��
	bRet = CryptHashData(hCryptHash, (BYTE*)passwd, strlen(passwd), 0);
	if (!bRet)
	{
		printf("CryptHashData fail!");
		return FALSE;
	}

	//�ù�ϡɢ�����ɻỰ��Կ
	HCRYPTKEY hCryptKey;
	bRet = CryptDeriveKey(hCryptProv, ALG_SYM_ALGO, hCryptHash, CRYPT_EXPORTABLE, &hCryptKey);
	if (!bRet)
	{
		printf("CryptDeriveKey fail!");
		return FALSE;
	}

	//���ļ����н���,hCryptKey�Ѿ�������㷨�������CALG_AES_128
	while (TRUE)
	{
		//������ܣ������С1024
		CHAR buf[BLOCK_SIZE] = { 0 };
		DWORD nReadLen = fread(buf, 1, BLOCK_SIZE, fSource);
		if (nReadLen <= 0)
			break;

		CryptDecrypt(hCryptKey,
			NULL,//�������ͬʱ����ɢ�кͼ��ܣ����ﴫ��һ��ɢ�ж���
			feof(fSource),//��������һ����ΪTRUE
			0,
			(BYTE*)buf,//���뱻���ܵ����ݣ������������
			&nReadLen//�����������ݳ��ȣ�������ܺ����ݳ���
		);
		fwrite(buf, 1, nReadLen, fDestination);
	}

	//������ɣ��ͷ�
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