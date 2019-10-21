# WinPolicyLicense
Fetch Windows License Policy Name and Value

The key is HKLM\System\CurrentControlSet\ProductOptions
The value is a binary data: ProductPolicy

It's format is listed as below:

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

I just use a char array with memcpy to fetch the name and value, and convert to unicode string to ascii string.
