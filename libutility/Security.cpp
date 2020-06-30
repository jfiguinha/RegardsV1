// Security.cpp: implementation of the CSecurity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Security.h"
#include "base64.h"
#include <wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSecurity::CSecurity()
{

}

CSecurity::~CSecurity()
{

}

int CSecurity::HashData(BYTE *pbBuffer,const DWORD &dwBufferLen, string &pHashBase64)
{
	BYTE                         *pbHashedBlob;

	DWORD                        cbHashedBlob;
											// Length of the hash BLOB
	CRYPT_ALGORITHM_IDENTIFIER   AlgId;     // Data structure to hold the
			
	CRYPT_HASH_MESSAGE_PARA      HashPara;  // Data structure used to hash                 
											// a message

	const BYTE*                  rgpbToBeHashed[1];     
	DWORD                        rgcbToBeHashed[1]; 
	// hash algorithm identifier

	//  Initialize the CRYPT_ALGORITHM_IDENTIFIER data structure.


	rgpbToBeHashed[0] = pbBuffer;
	rgcbToBeHashed[0] = dwBufferLen;

	AlgId.pszObjId=szOID_OIWSEC_sha1;
	AlgId.Parameters.cbData=0;

	//--------------------------------------------------------------------
	//  Initialize the CRYPT_HASH_MESSAGE_PARA data structure.

	HashPara.cbSize = sizeof(CRYPT_HASH_MESSAGE_PARA);
	HashPara.dwMsgEncodingType=MY_ENCODING_TYPE;
	HashPara.hCryptProv=NULL;
	HashPara.HashAlgorithm=AlgId;
	HashPara.pvHashAuxInfo= NULL;

	//--------------------------------------------------------------------
	// Calculate the size of the hashed and encoded message. 

	
	if(CryptHashMessage(
		  &HashPara,
		  TRUE,
		  1,
		  rgpbToBeHashed,
		  rgcbToBeHashed,
		  NULL,
		  NULL,
		  NULL,
		  &cbHashedBlob))
	{
		 //printf("The size of the hashed, encoded message is %d.\n",
		//	  cbHashedBlob);
	}
	else
	{
		 //HandleError("The size of the hash could not be determined.");

		return -1;
	}

	if(pbHashedBlob = (BYTE*)malloc(cbHashedBlob))
	{
		 //printf("Memory allocated for the hashed, encoded BLOB.\n");
	}
	else
	{
		 //HandleError("Memory allocation failed.");

		 return -1;
	};

	//--------------------------------------------------------------------
	// Hash and encode the message.

	if(CryptHashMessage(
		   &HashPara,
		   TRUE,
		   1,
		   rgpbToBeHashed,
		   rgcbToBeHashed,
		   NULL,
		   NULL,
		   pbHashedBlob,
		   &cbHashedBlob))
	{
		 //printf("The message has been hashed and encoded.\n");
	}
	else
	{
		 //HandleError("The message was not hashed. ");
		 return -1;
	}

	Base64 m_Base64;
	pHashBase64 = m_Base64.encode(pbHashedBlob,cbHashedBlob);

	free(pbHashedBlob);

	return 0;
}
