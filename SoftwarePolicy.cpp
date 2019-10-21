#include <Windows.h>
#include <stdio.h>
#include <slpublic.h>

typedef struct policyHeader {
	DWORD cbSize;			// struct size
	DWORD cbDataSize;
	DWORD cbEndMarker;
	DWORD reserved1;
	DWORD reserved2;
} policyHeader;

typedef struct policyValue {
	WORD		cbSize;			// struct size
	WORD		cbName;			// key name size, unicode string
	WORD		sldType;
	WORD		cbData;			// value size
	DWORD		reserved1;
	DWORD		reserved2;
} policyValue;

const char* UnicodeToAscii(const wchar_t* str, int& len) {
	const char* ret_str;
	int ansiiLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	len = ansiiLen;
	char* pAssii = (char*)malloc(sizeof(char) * ansiiLen);
	WideCharToMultiByte(CP_ACP, 0, str, -1, pAssii, ansiiLen, NULL, NULL);
	ret_str = pAssii;
	return ret_str;
}

int main() {
	HKEY hKey;
	LONG lRet;
	DWORD regType;
	BYTE buf[65536] = { '\0' };
	BYTE dstBuf[4096] = { '\0' };

	DWORD cbSize;
	DWORD cbEndMarker;

	CONST CHAR* szName;
	DWORD szBuf = sizeof buf;
	INT pos = 0;
	INT lastPos = 0;
	WORD valueSize;
	WORD nameSize;
	WORD sldType;
	WORD dataSize;
	INT nameLen;

	RegOpenKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\ProductOptions", &hKey);
	RegQueryValueEx(hKey, L"ProductPolicy", NULL, &regType, buf, &szBuf);

	// read header
	memcpy(&cbSize, buf, 4);
	memcpy(&cbEndMarker, buf + sizeof policyHeader::cbSize + sizeof policyHeader::cbDataSize, 4);
	pos += sizeof policyHeader;
	lastPos = pos;

	while (pos < cbSize - cbEndMarker) {
		// read Value
		memcpy(&valueSize, buf + pos, sizeof(policyValue::cbSize));
		pos += sizeof(policyValue::cbSize);
		memcpy(&nameSize, buf + pos, sizeof(policyValue::cbName));
		pos += sizeof(policyValue::cbName);
		memcpy(&sldType, buf + pos, sizeof(policyValue::sldType));
		pos += sizeof(policyValue::sldType);
		memcpy(&dataSize, buf + pos, sizeof(policyValue::cbData));
		pos += sizeof(policyValue::cbData);
		pos += sizeof(policyValue::reserved1);
		pos += sizeof(policyValue::reserved2);

		// key name
		memcpy(dstBuf, buf + pos, nameSize);
		szName = UnicodeToAscii((wchar_t*)dstBuf, nameLen);
		printf("%s\n", szName);
		ZeroMemory(dstBuf, sizeof dstBuf);
		memset(dstBuf, 0x2d, nameLen - 1);
		printf("%s\n", dstBuf);
		ZeroMemory(dstBuf, sizeof dstBuf);
		free((void*)szName);
		pos += nameSize;
		ZeroMemory(dstBuf, sizeof dstBuf);

		// value
		memcpy(dstBuf, buf + pos, dataSize);
		switch (sldType) {
			int v;
		case SL_DATA_NONE:
			break;
		case SL_DATA_SZ:
			szName = UnicodeToAscii((wchar_t*)dstBuf, nameLen);
			printf("%s", szName);
			free((void*)szName);
			break;
		case SL_DATA_DWORD:
			memcpy(&v, dstBuf, 4);
			printf("%d", v);
			break;
		case SL_DATA_BINARY:
			for (int i = 0; i < dataSize; i++) {
				printf("%x", dstBuf[i]);
			}
			break;
		}

		ZeroMemory(dstBuf, sizeof dstBuf);
		pos += dataSize;
		lastPos += valueSize;
		pos = lastPos;
		printf("\n\n");
	}

	getchar();

	return 0;
}