#pragma once
#include <Windows.h>
#include <wincrypt.h>
#include<iostream>
using namespace std;

HCRYPTPROV	m_hCryptProv;
HCRYPTKEY	m_hKey;
HCRYPTHASH	m_hHash;

#define KEYCONTAINER _T("BenTestKeyContainer")
#define KEYLENGTH  0x00800000


BOOL CreateCryptSession(LPTSTR tszPassword)
{
	//---------------------------------------------------------------
	// Get the handle to the default provider. KEYCONTAINER, MS_ENHANCED_PROV
	if (m_hCryptProv == NULL)
	{
		if (!CryptAcquireContext(&m_hCryptProv, KEYCONTAINER, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
		{
			if (GetLastError() == NTE_BAD_KEYSET)
			{
				if (!CryptAcquireContext(&m_hCryptProv, KEYCONTAINER, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}
	// Create a hash object. 
	if (m_hHash == NULL)
	{
		if (!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &m_hHash))
		{
			return FALSE;
		}
	}
	// Hash in the password data. 
	if (m_hKey)
	{
		CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}
	if (!CryptHashData(m_hHash, (BYTE *)tszPassword, _tcslen(tszPassword) * sizeof(TCHAR), 0))
	{
		return FALSE;
	}
	// Derive a session key from the hash object. RC4 is a encryption algorithm.You can 
	// use DES and AES to replace it.
	// CryptDeriveKey(m_hCryptProv, CALG_RC4, m_hHash, KEYLENGTH, &m_hKey)
	bool bRet = CryptDeriveKey(m_hCryptProv, CALG_DES, m_hHash, CRYPT_EXPORTABLE, &m_hKey);
	if (!bRet)
	{
		cout << "错误码：" << GetLastError() << endl;
		return FALSE;
	}
	return TRUE;
}


BOOL DestroyCryptSession()
{
	// Destroy session key. 
	if (m_hKey)
	{
		CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}
	// Destroy hash object. 
	if (m_hHash)
	{
		CryptDestroyHash(m_hHash);
		m_hHash = NULL;
	}
	// Release provider handle. 
	if (m_hCryptProv)
	{
		CryptReleaseContext(m_hCryptProv, 0);
		m_hCryptProv = NULL;
	}
	return TRUE;
}


BOOL EncryptData(PBYTE pbBuffer, DWORD dwBufferLen, LPDWORD lpdwCount, BOOL bFinal = FALSE)
{
	if (m_hKey == NULL || pbBuffer == NULL || dwBufferLen == 0 || lpdwCount == NULL)
	{
		return FALSE;
	}
	// Encrypt data. 
	return CryptEncrypt(m_hKey, NULL, bFinal, 0, pbBuffer, lpdwCount, dwBufferLen);
}

// 解密数据
BOOL DecryptData(PBYTE pbBuffer, LPDWORD lpdwCount, BOOL bFinal = FALSE)
{
	if (m_hKey == NULL || pbBuffer == NULL || lpdwCount == NULL)
	{
		return FALSE;
	}
	// Decrypt the block of data. 
	if (!CryptDecrypt(m_hKey, 0, bFinal, 0, pbBuffer, lpdwCount))
	{
		return FALSE;
	}
	return TRUE;
}


int example_one()
{
	char a[100] = "hulixin!";
	LPTSTR tszPassword = TEXT("c105109");
	// 加密 数据 
	if (!CreateCryptSession(tszPassword))
	{
		cout << "CreateCryptSession Failed.. " << endl;
		return FALSE;
	}

	DWORD dwCount = sizeof(char)*(strlen(a)+1);
	if (!EncryptData((BYTE*)a, 100, &dwCount,true))
	{
		cout << "Encrypt Data Failed: " <<GetLastError()<< endl;
		return FALSE;
	}
	else
	{
		cout << "Encrypt Data Successed:" << a << endl;
	}

	DestroyCryptSession();  // 注意这句的调用与前面 CreateCryptSession 成对
							//-----------------------------------------------------------

	{	// =======================================================
		// ====        解密部分  =============
		if (CreateCryptSession(tszPassword) == FALSE)
		{
			return FALSE;
		}

		// 解密
		//	DWORD dwDecryptCount = sizeof(decryptText);
		if (DecryptData((PBYTE)a, &dwCount))
		{
			cout << "Decrypt Data Successed:" << a << endl;
		}
		else
		{
			cout << "DeCrypt Data Failed... " << endl;
		}

		DestroyCryptSession();
		// =======================================================
	}

	return 0;
}