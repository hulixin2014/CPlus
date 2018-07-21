#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <iostream>
using namespace std;


void example_two() {
	HCRYPTPROV hProv;
	BYTE* puBuffer = (BYTE*)"data hash and sign.";
	DWORD dwBufferLen = strlen((char*)puBuffer) + 1;
	HCRYPTHASH hHash;
	HCRYPTKEY hKey;    //ǩ����Կ���
	HCRYPTKEY hPubKey;
	BYTE* pbKeyBlob;  //������Կblob������ָ��
	BYTE* pbSignature;
	DWORD dwSigLen;
	DWORD dwBlobLen;
	DWORD i;
	if (CryptAcquireContext(&hProv, _T("test"), NULL, PROV_RSA_FULL, 0))
		printf("�򿪾���ɹ�\n");
	else {
		if (!CryptAcquireContext(&hProv, _T("test"), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			printf("����ʧ�ܡ�\n");
	}
	if (CryptGetUserKey(hProv, AT_SIGNATURE, &hKey))
		printf("���ǩ����Կ�ɹ���\n");
	else {
		printf("��ȡʧ�ܣ����ڴ����µ�RSA��Կ�ԡ�\n");
		if (!CryptAcquireContext(&hProv, _T("test"), NULL, PROV_RSA_FULL, 0))
			printf("��ȡCSP���ʧ��\n");
		if (!CryptGenKey(hProv, 2, CRYPT_EXPORTABLE | 0X04000000, &hKey))
			printf("CryptGenKey error.\n");
	}
	if (CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, NULL, &dwBlobLen))
		printf("we get the length of the public key.\n");
	else
		printf("CryptExportKey erro.\n");
	if (pbKeyBlob = (BYTE*)malloc(dwBlobLen))
		printf("we get the memory.\n");
	else
		printf("malloc erro.\n");
	if (CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, pbKeyBlob, &dwBlobLen))
		printf("export the public key.\n");
	else
		printf("CryptExportKeya error.\n");
	if (CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
		printf("CreateHash succeed.\n");
	else
		printf("CreatHash error.\n");
	if (CryptHashData(hHash, puBuffer, dwBufferLen, 0))
		printf("HashData succeed.\n ");
	else
		printf("HashData error.\n");
	dwSigLen = 0;
	if (CryptSignHash(hHash, AT_SIGNATURE, NULL, 0, NULL, &dwSigLen))
		printf("Get the length of signature.\n");
	else
		printf("CryptSignHash error.\n");
	if (pbSignature = (BYTE*)malloc(dwSigLen))
		printf("get the memory.\n");
	else
		printf("memory error.\n");
	if (CryptSignHash(hHash, AT_SIGNATURE, NULL, 0, pbSignature, &dwSigLen))
		printf("signature succeed.\n");
	else
		printf("Signature error.\n");
	printf("Signature: \n");
	for (i = 0; i < dwSigLen; i++) {
		if ((i == 0) && (i != 0))
			printf("\n");
		printf("%2.2x", pbSignature[i]);
	}
	printf("\n");
	printf("OK.\n");
	if (hHash)
		CryptDestroyHash(hHash);
	if (CryptImportKey(hProv, pbKeyBlob, dwBlobLen, 0, 0, &hPubKey))
		printf("Import the key.\n");
	else
		printf("erro");
	if (CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
		printf("������ϣ����ɹ� \n");
	else
		printf("����CryptCreateHashʧ��");
	if (CryptHashData(hHash, puBuffer, dwBufferLen, 0))
		printf("���ݹ�ϣ���.\n");
	else
		printf("����CryptHashDataʧ��");
	if (CryptVerifySignature(hHash, pbSignature, dwSigLen, hPubKey, NULL, 0))
		printf("��֤ǩ���ɹ���\n");
	else
		printf("ǩ����֤ʧ�ܣ�ǩ����Ч");
	if (pbSignature)
		free(pbSignature);
	if (hHash)
		CryptDestroyHash(hHash);
	if (hProv)
		CryptReleaseContext(hProv, 0);
	system("pause");
}