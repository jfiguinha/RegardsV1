// Aspi.cpp: CAspi クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "Aspi.h"
#include "scsireg.h"
#include "scsidefs.h"
#include <strsafe.h>
#define PGB(a) ((BYTE *)(((DWORD)(a) + 0x0f) & ~0x0f))
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

inline int toInt16(unsigned char input[2]){
	return (input[0]<<8)+input[1];
}

inline int toInt32(unsigned char input[4]){
	return (input[0]<<24)+(input[1]<<16)+
		(input[2]<<8)+input[3];
}
#define	a_to_u_3_byte(a)	((unsigned long) \
				((((BYTE*) a)[2]       & 0xFF) | \
				 (((BYTE*) a)[1] << 8  & 0xFF00) | \
				 (((BYTE*) a)[0] << 16 & 0xFF0000)))

CAspi::CAspi()
{
	bAtapi = TRUE;
}

CAspi::~CAspi()
{
}

int CAspi::InfosMediaErasable()
{
	unsigned char szBuffer[0x100];
	SRB_ExecSCSICmd srbExec;
	memset ( &srbExec, 0, sizeof ( SRB_ExecSCSICmd ) );
	srbExec.SRB_Cmd = SC_EXEC_SCSI_CMD;
	srbExec.SRB_Flags = SRB_DIR_IN;
	srbExec.SRB_BufLen = sizeof(disk_info);
	srbExec.SRB_BufPointer = szBuffer;
	srbExec.SRB_SenseLen = 18;
	srbExec.SRB_CDBLen = 10;
	srbExec.CDBByte [ 0 ] = 0x51;
	srbExec.CDBByte [ 8 ] = 0xff;

	ExecuteCommand(srbExec);

	if ( srbExec.SRB_Status != SS_COMP )
	{
		return 0;
	}
	else
	{
		disk_info *p=(disk_info *)(szBuffer);
		return p->erasable;
	}

	return 0;

}

int CAspi::InfosMedia(int &iNbTrack)
{
	string szReturnString;
	unsigned char szBuffer[0x100];
	SRB_ExecSCSICmd srbExec;
	memset ( &srbExec, 0, sizeof ( SRB_ExecSCSICmd ) );
	srbExec.SRB_Cmd = SC_EXEC_SCSI_CMD;
	srbExec.SRB_Flags = SRB_DIR_IN;
	srbExec.SRB_BufLen = sizeof(disk_info);
	srbExec.SRB_BufPointer = szBuffer;
	srbExec.SRB_SenseLen = 18;
	srbExec.SRB_CDBLen = 10;
	srbExec.CDBByte [ 0 ] = 0x51;
	srbExec.CDBByte [ 8 ] = 0xff;

	ExecuteCommand(srbExec);

	if ( srbExec.SRB_Status != SS_COMP )
	{
		szReturnString="Read Track Info error.";
		return -1;
	}
	else
	{
		disk_info *p=(disk_info *)(szBuffer);
		char s[80];
		
		szReturnString="Disk Status: ";
		switch(p->disk_status)
		{
			case 0:			szReturnString+="Empty\n";
							break;
			case 1:			szReturnString+="Incomplete\n";
							break;
			case 2:			szReturnString+="Complete\n";
							break;
			default:		szReturnString+="Other\n";
							break;
		}

		iNbTrack = p->numsess;
		return p->disk_status;

	}

	return -1;

	//return szReturnString;
}

bool CAspi::ItCanWrite()
{
	BYTE *Buffer,B[256+15];
	DWORD DataPoint;
	BYTE *mp;
	BYTE PageLen;

	Buffer = PGB(B);

	DataPoint = ModeSense(Buffer,256,1,5);	//   get default setting
	if(DataPoint == 0)
	{
		return false;
	}

	return true;
}

bool CAspi::EraseMedia(bool FastErase)
{
	if(InfosMediaErasable())
	{
		BYTE m_SK;
		BYTE m_ASC;
		BYTE m_ASCQ;


		SRB_ExecSCSICmd cmd;
		memset(&cmd,0,sizeof(cmd));

		cmd.SRB_BufLen = 0;
		cmd.SRB_BufPointer = NULL;
		cmd.SRB_CDBLen = 12;
	//	cmd.SRB_Flags      = SRB_DIR_OUT;
		cmd.CDBByte[ 0] = 0xA1;		//   BLANK

		if(FastErase){
			cmd.CDBByte[ 1] = 1;
		}

		ExecuteCommand(cmd);
		if(cmd.SRB_Status == SS_COMP){
			return true;
		}

		m_SK = cmd.SenseArea[2] & 0x0f;
		m_ASC = cmd.SenseArea[12];
		m_ASCQ = cmd.SenseArea[13];
		
	}
	return false;
}

bool CAspi::IsCDReady(void)
{
	//   test unit ready
	if(TestUnitReady() != 0)
	{
		return false;
	}

	//   check media info
	if(!CheckDisc()){
		return false;
	}
	return true;
}

int CAspi::TestUnitReady()
{
	SRB_ExecSCSICmd cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.SRB_BufLen = 0;
	cmd.SRB_BufPointer = NULL;
	cmd.SRB_CDBLen = 6;
//	cmd.SRB_Flags      = SRB_DIR_IN;
	cmd.CDBByte[ 0] = 0x00;		//   TEST UNIT READY

	ExecuteCommand(cmd);

	if(cmd.SRB_Status != SS_COMP){
		return 1;		//   bad status
	}

	if((cmd.SenseArea[2] & 0x0f) == 2){
		return 2;		//   not ready
	}

	if((cmd.SenseArea[2] & 0x0f) == 6){
		return 2;		//   illegal request
	}

	return 0;
}

long CAspi::GetLeadOutPos(void)
{
	BYTE Buffer[400];

	memset(Buffer,0,400);
	if(ReadATIP(Buffer)){
		m_LeadOutPos = ((Buffer[12] * 60) + Buffer[13]) * 75 + Buffer[14];
	} else {
		m_LeadOutPos = 0;
	}

	return m_LeadOutPos;
}

bool CAspi::ReadATIP(BYTE * Buffer)
{
	DWORD BufferSize;
	SRB_ExecSCSICmd cmd;

	memset(&cmd,0,sizeof(cmd));
	BufferSize = 400;

	cmd.SRB_BufLen = BufferSize;
	cmd.SRB_BufPointer = Buffer;
	cmd.SRB_CDBLen = 10;
	cmd.SRB_Flags      = SRB_DIR_IN;
	cmd.CDBByte[ 0] = 0x43;		//   READ TOC/PMA/ATIP
	cmd.CDBByte[ 1] = 2;		//   READ ATIP
	cmd.CDBByte[ 2] = 4;		//   READ ATIP
	cmd.CDBByte[ 7] = (BYTE)((BufferSize / 0x100) & 0xff);
	cmd.CDBByte[ 8] = (BYTE)(BufferSize & 0xff);

	ExecuteCommand(cmd);

	if(cmd.SRB_Status == SS_COMP){
		return true;
	}

	m_SK = cmd.SenseArea[2] & 0x0f;
	m_ASC = cmd.SenseArea[12];
	m_ASCQ = cmd.SenseArea[13];

	return false;
}

int CAspi::CheckDisc()
{
	BYTE * Buffer = new BYTE[256];
	SRB_ExecSCSICmd cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.SRB_BufLen = 256;
	cmd.SRB_BufPointer = Buffer;
	cmd.SRB_CDBLen = 10;
	cmd.SRB_Flags      = SRB_DIR_IN;
	cmd.CDBByte[ 0] = 0x51;		//   READ DISC INFORMATION
	cmd.CDBByte[ 7] = (BYTE)(255 >> 8);
	cmd.CDBByte[ 8] = (BYTE)(255 & 0xff);

	ExecuteCommand(cmd);

	int m_LeadInLBA = 0;
	int m_LeadInSize = 0;

	if(cmd.SRB_Status != SS_COMP){
		delete[] Buffer;
		return false;
	}

	if((Buffer[2] & 0x03) != 0){
		delete[] Buffer;
		return false;
	}

	m_LeadInLBA = Buffer[19] + 75*(Buffer[18] + 60*(Buffer[17]));
	m_LeadInSize = 450000 - (Buffer[19] + 75*(Buffer[18] + 60*(Buffer[17])));

	delete[] Buffer;
	return true;
}

string CAspi::SCSICDRScan( ) 
{


	string szReturnString;

	BYTE mode[100];
	int	len;
	int	secsize = -1;

	/* XXX Quick and dirty, musz verallgemeinert werden !!! */

	ZeroMemory(mode, sizeof(mode));


	len = sizeof(scsi_mode_header) + sizeof(scsi_mode_blockdesc);
	/*
	 * Wenn wir hier get_mode_params() nehmen bekommen wir die Warnung:
	 * Warning: controller returns wrong page 1 for All pages page (3F).
	 */

	int current = 1;

	if (ModeSense(mode, len, 0x3F, current?0:2) < 0) 
	{
		ZeroMemory(mode, sizeof(mode));
		if (ModeSense(mode, len, 0, 0) < 0)	{/* VU (block desc) */
			return "";
		}
	}
	if (mode[3] == 8) 
	{
			printf("Density: 0x%X\n", mode[4]);
			printf("Blocks:  %ld\n", a_to_u_3_byte(&mode[5]));
			printf("Blocklen:%ld\n", a_to_u_3_byte(&mode[9]));
			secsize = a_to_u_3_byte(&mode[9]);
	}

	ZeroMemory(mode, sizeof(mode));

	if (ModeSense(mode, 0xFE, 0x3F, current?0:2) >= 0) {	/* All Pages */

		BYTE * ep = mode+mode[0];	/* Points to last byte of data */
		BYTE * p = &mode[4];
		p += mode[3];
		printf("Pages: ");
		while (p < ep) {
			printf("0x%X ", *p&0x3F);
			p += p[1]+2;
		}
		printf("\n");
	}
	return szReturnString;
}

long CAspi::CDROMCapacity(long &NbBlocks, long &sizeBlocks) 
{
	string szReturnString;
	unsigned char szBuffer[40];
	SRB_ExecSCSICmd cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.SRB_BufLen = 36;
	cmd.SRB_BufPointer = szBuffer;
	cmd.SRB_SenseLen = SENSE_LEN;
	cmd.SRB_CDBLen = 10;
	cmd.CDBByte [ 0 ] = SCSI_READCDCAP;
	ExecuteCommand(cmd);

	NbBlocks = sizeBlocks = 0;

	if ( cmd.SRB_Status != SS_COMP )
	{
		szReturnString="Inquiry error.";
		return 0;
	}
	else{
		char s[80];
		NbBlocks = (szBuffer[0]<<24)+(szBuffer[1]<<16)+(szBuffer[2]<<8)+szBuffer[3]+1;
		sizeBlocks = (szBuffer[4]<<24)+(szBuffer[5]<<16)+(szBuffer[6]<<8)+szBuffer[7];

		return NbBlocks * sizeBlocks;
	}
	return 0;
}

//--------------------------------------------------------------------------

int  CAspi::ReadTrackInfo(int track,int &iTrackBegin,int &iTrackEnding)
{
	unsigned char szBuffer[0x100];

	SRB_ExecSCSICmd cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.SRB_BufLen = sizeof(track_info);
	cmd.SRB_BufPointer = szBuffer;
	cmd.SRB_SenseLen = 18;
	cmd.SRB_CDBLen = 10;
	cmd.CDBByte [ 0 ] = 0x52;
	cmd.CDBByte [ 1 ] = 0x01;
	cmd.CDBByte [ 5 ] = track<1?0xff:track;
	cmd.CDBByte [ 8 ] = 0xff;
	ExecuteCommand(cmd);

	/*
	SRB_ExecSCSICmd srbExec;
	memset ( &srbExec, 0, sizeof ( SRB_ExecSCSICmd ) );
	srbExec.SRB_Cmd = SC_EXEC_SCSI_CMD;
	srbExec.SRB_HaId = m_HA;
	srbExec.SRB_Flags = SRB_DIR_IN;
	srbExec.SRB_Target = m_ID;
	srbExec.SRB_Lun = m_LUN;
	srbExec.SRB_BufLen = sizeof(track_info);
	srbExec.SRB_BufPointer = szBuffer;
	srbExec.SRB_SenseLen = 18;
	srbExec.SRB_CDBLen = 10;
	srbExec.CDBByte [ 0 ] = 0x52;
	srbExec.CDBByte [ 1 ] = 0x01;	
	srbExec.CDBByte [ 5 ] = track<1?0xff:track;
	srbExec.CDBByte [ 8 ] = 0xff;
	*/

	//pfnSendASPI32Command ( ( LPSRB ) &srbExec );
	//while ( srbExec.SRB_Status == SS_PENDING );

	if ( cmd.SRB_Status != SS_COMP )
	{
		return -1;
	}

	else
	{
		track_info *p=(track_info *)szBuffer;


		string szReturnString,s;
		/*
		s.Format("Track Number:%d,Session Number:%d\n",
			p->track_number_LSB,p->session_number_LSB);
		szReturnString+=s;*/

		switch(p->track_mode&0xD)
		{
		case 4:			
			szReturnString+="Data Track,Uninteruppted\n";
			break;
		case 5:			
			szReturnString+="Data Track,Incremental\n";
			break;
		}
		szReturnString+=p->damage?"Damaged,":"Not Damaged\n";
		if(p->damage)	
			szReturnString+=p->NWA_V?"Writting allowed\n":"No writting allowed\n";

		szReturnString+=p->RT?"Reserved\n":"Not Reserved\n";
		szReturnString+=p->blank?"Blank\n":"Not Blank\n";
		szReturnString+=p->packet?"Packet":"Not Packet\n";

		if(p->packet)	
			szReturnString+=p->fp?"(Fixed Size)\n":"(Variable)\n";

		switch(p->data_mode)
		{
			case 1:			
				szReturnString+="Mode1\n";
				break;
			case 2:			
				szReturnString+="Mode2\n";
				break;
		}

		iTrackBegin = toInt32(p->track_start_addr);
		int iTrackSize = toInt32(p->track_size);
		//s.Format("Track start address: %Xh\n",SCSI2::toInt32(p->track_start_addr));
		szReturnString+=s;
		if(p->LRA_V)
		{	
			//s.Format("Last Record address: %Xh\n",SCSI2::toInt32(p->last_record_addr));
			szReturnString+=s;
		}
		else
			szReturnString+="Last Record address: Not Available\n";

		iTrackEnding = toInt32(p->next_writtable_addr);
		int iNext = toInt32(p->next_writtable_addr);
		//s.Format("Track size: %Xh\n",SCSI2::toInt32(p->track_size));
		szReturnString+=s;
		if(p->NWA_V)
		{
			//s.Format("Next writable address: %Xh\n",SCSI2::toInt32(p->next_writtable_addr));
			szReturnString+=s;
		}
		else
			szReturnString+="Next Writable address: Not Available\n";

		return 0;	
	}
}

string CAspi::DriveMediaSupport()
{
	string szReturnString;
	unsigned char szBuffer[100];

	SRB_ExecSCSICmd cmd;

	memset(&cmd,0,sizeof(cmd));

	cmd.SRB_BufLen = 100;
	cmd.SRB_BufPointer = szBuffer;
	cmd.SRB_SenseLen = 18;
	cmd.SRB_CDBLen = 6;
	cmd.SRB_Flags      = SRB_DIR_IN;
	cmd.CDBByte[ 0] = SCSI_MODE_SEN6;		//   READ DISC INFORMATION
	cmd.CDBByte [ 1 ] = 0x08;
	cmd.CDBByte [ 2 ] = 0x2A;
	cmd.CDBByte [ 4 ] = 100;

	ExecuteCommand(cmd);

	if ( cmd.SRB_Status != SS_COMP )
		szReturnString="Mode Sense error.";
	else{
		cd_mode_page_2A *p=(cd_mode_page_2A *)(szBuffer+4);
		//BYTE* page=szBuffer+4;
		if(p->cd_r_write)
			szReturnString="CDR Writing:Yes\n";
		else
			szReturnString="CDR Writing:No\n";
		if(p->cd_rw_write)
			szReturnString+="CDRW Writing:Yes\n";
		else
			szReturnString+="CDRW Writing:No\n";
		if(p->multi_session)
			szReturnString+="Multisession:Yes\n";
		else
			szReturnString+="Multisession:No\n";
		char s[80];
		StringCchPrintf(s,80,"Buffer Size: %dKB\n",toInt16(p->buffer_size));
		szReturnString+=s;
		
	}
	return szReturnString;
}

BOOL CAspi::EjectCD(BOOL bEject)
{
	BYTE cmd[6] = {0x1B, 0, 0, 0, 0, 0};
	
	LONG lSector = 0;

	WORD wNumSectors= 0;

	cmd [1]=m_Lun<<5;

	if (bEject)
		cmd [4]=0x02;
	else
		cmd [4]=0x03;

	return (BOOL)IssueScsiCmd(SRB_DIR_OUT,cmd,sizeof(cmd));
}

BYTE CAspi::IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen)
{
	return IssueScsiCmd(bFlags,lpcbData,ncbLen,NULL,0,m_Ha,m_Tgt,m_Lun);
}

BYTE CAspi::IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen,LPBYTE lpBuffer,int nBufLen)
{
	return IssueScsiCmd(bFlags,lpcbData,ncbLen,lpBuffer,nBufLen,m_Ha,m_Tgt,m_Lun);
}

BYTE CAspi::IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen,LPBYTE lpBuffer,int nBufLen,BYTE btAdapterID,BYTE btTargetID,BYTE btLunID)
{
	
// added by Andi, scenalyzer@blackbox.net
	SRB_ExecSCSICmd	mySrb;					// Create SRB_EXECSCSICMD header
	HANDLE			hEvent=NULL;			// Handle to event

// end added

    DWORD			dwASPIEventStatus=0;
    DWORD			dwASPIStatus;

	// Clear all fields
	memset(&mySrb,0x00,sizeof(SRB_ExecSCSICmd));

    // **************************************************
    // Create event for MySRB.  Initial state         
    // non-signaled, manual reset.                    
    // **************************************************

	// Make sure it is a manual reset event!
    if ((hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)) == NULL)
          return SS_ABORTED;

	// Set SRB fields
	mySrb.SRB_Cmd=SC_EXEC_SCSI_CMD;
	mySrb.SRB_Ha =btAdapterID;
	mySrb.SRB_Flags=bFlags|SRB_EVENT_NOTIFY;
	mySrb.SRB_Tgt =btTargetID;
	mySrb.SRB_Lun = btLunID;
    mySrb.SRB_SenseLen = SENSE_LEN;
	mySrb.SRB_CDBLen=ncbLen;
    mySrb.SRB_BufLen=nBufLen;
    mySrb.SRB_BufPointer=lpBuffer;

	LPBYTE lpTest=(LPBYTE)&mySrb;

	// Copy CBDByte data, if available
	if (lpcbData!=NULL)
	{
		memcpy(&mySrb.CDBByte,lpcbData,ncbLen);
	}

	// Clear event handle
	ResetEvent(hEvent);

	if ( ncbLen == 6 )
	{
		printf("Sending ASPI command flag %02X cmd %02X %02X %02X %02X %02X %02X",
				mySrb.SRB_Flags,
				mySrb.CDBByte[0],
				mySrb.CDBByte[1],
				mySrb.CDBByte[2],
				mySrb.CDBByte[3],
				mySrb.CDBByte[4],
				mySrb.CDBByte[5] );
	}
	else
	{
		printf("Sending ASPI command flag %02X cmd %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
				mySrb.SRB_Flags,
				mySrb.CDBByte[0],
				mySrb.CDBByte[1],
				mySrb.CDBByte[2],
				mySrb.CDBByte[3],
				mySrb.CDBByte[4],
				mySrb.CDBByte[5],
				mySrb.CDBByte[6],
				mySrb.CDBByte[7],
				mySrb.CDBByte[8],
				mySrb.CDBByte[9]
				);
	}


	// Send ASPI32 command
	ExecuteCommand(mySrb);

	/*
	 * Check ASPI command status
	 */
	if (mySrb.SRB_Status != SS_COMP)
	{
		//ASSERT(FALSE);
		/*
		printf("Error in IssueSCSICommand with Status %02X Sense flags %02X %02X %02X ",
					mySrb.SRB_Status,
					g_SenseKey.SK,
					g_SenseKey.ASC,
					g_SenseKey.ASCQ );*/

		// Clear event, time out
		//ResetEvent(hEvent);
	}

	// Close the event handle

	// Check aspi function call
	//dwAspiStatus=IsScsiError((LPSRB)&mySrb);

	return mySrb.SRB_Status;
}

void CAspi::Initialize()
{
}

BOOL CAspi::IsActive()
{
	return false;
}

DWORD CAspi::GetVersion()
{
	return 0;
}

void CAspi::ExecuteCommand(SRB_ExecSCSICmd &cmd)
{
}

void CAspi::InitialAsync(void)
{
}

void CAspi::FinalizeAsync(void)
{
}

bool CAspi::ExecuteCommandAsync(SRB_ExecSCSICmd &cmd)
{
	return false;
}

DWORD CAspi::ModeSense(int nPage,BYTE* pbtBuffer,int nBufSize)
{
	BYTE	btReturn;

	// Clear result buffer
	memset(pbtBuffer,0x00,nBufSize);

	BYTE cmd[12];

	// Init buffers
	memset(cmd,0x00,sizeof(cmd));

	// Common settings
	cmd[1]=m_Lun<<5;				// Lun
	cmd[2]=nPage&0x3F;				// Get page values (current value)

	// ATAPI or SCSI sense ?
	if (bAtapi==TRUE)
	{
		cmd[0]= 0x5A;					// Operation Code
		cmd[7]=	nBufSize>>8;
		cmd[8]=	nBufSize&0xFF;

		// Call SCSI command
		btReturn=IssueScsiCmd(SRB_DIR_IN,cmd,10,pbtBuffer,nBufSize);
//				 IssueScsiCmd(SRB_DIR_IN,cmd,sizeof(cmd),pbtBuffer,sizeof(pbtBuffer));

		int nLen=pbtBuffer[1];
		nLen+=((int)pbtBuffer[0])<<8;

		// Convert header to standard SCSI header
		pbtBuffer[0]=(BYTE)(nLen-4);
		pbtBuffer[1]=pbtBuffer[2];
		pbtBuffer[2]=pbtBuffer[3];
		pbtBuffer[3]=pbtBuffer[7];
		memmove(pbtBuffer+4,pbtBuffer+8,nBufSize-4);

	}
	else
	{

		cmd[0]=0x1A;					// Operation Code
		cmd[8]=	nBufSize;				// Set low word of buffer size

		// Call SCSI command
		btReturn=IssueScsiCmd(SRB_DIR_IN,cmd,6,pbtBuffer,nBufSize);
	}
	return btReturn;
}

DWORD CAspi::ModeSense(BYTE *Buffer, DWORD BufLen, BYTE PCFlag, BYTE PageCode)
{
	BYTE *LocalBuffer;
	SRB_ExecSCSICmd cmd;

	LocalBuffer = (BYTE*)malloc(256*sizeof(BYTE));

	memset(&cmd,0,sizeof(cmd));
	memset(LocalBuffer,0,256);

	cmd.SRB_Flags = SRB_DIR_IN;
	cmd.SRB_BufLen = 256;
	cmd.SRB_BufPointer = (BYTE FAR *)(LocalBuffer);
	cmd.SRB_CDBLen = 0xa;

	cmd.CDBByte[0]		= 0x5a;		// MODE SENSE
	cmd.CDBByte[2]		=(PageCode & 0x3f) | (PCFlag << 6);
	cmd.CDBByte[7]		= (256 >> 8) & 0xff;
	cmd.CDBByte[8]		= 256 & 0xff;

	ExecuteCommand(cmd);

	if(cmd.SRB_Status == SS_COMP){
		DWORD DataLen,BlockDescLen,ModeDataLen,ModeDataPoint;

		DataLen = ((LocalBuffer[0] << 8) | LocalBuffer[1]) + 2;
		BlockDescLen = (LocalBuffer[6] << 8) | LocalBuffer[7];
		ModeDataPoint = BlockDescLen + 8;
		ModeDataLen = LocalBuffer[ModeDataPoint + 1] + 2;

		memcpy(Buffer,LocalBuffer,DataLen);

		free(LocalBuffer);

		return ModeDataPoint;
	}
	memset(Buffer,0,BufLen);

	free(LocalBuffer);

	return 0;
}

bool CAspi::ModeSelect(BYTE *Buffer, DWORD BufLen)
{
	SRB_ExecSCSICmd cmd;
	BYTE * PBuffer = (BYTE *)malloc(BufLen);

	memcpy(PBuffer,Buffer,BufLen);

	Buffer[0] = 0;
	Buffer[1] = 0;
	Buffer[4] = 0;
	Buffer[5] = 0;
	memset(&cmd,0,sizeof(cmd));

	cmd.SRB_Flags = SRB_DIR_OUT;
	cmd.SRB_BufLen = BufLen;
	cmd.SRB_BufPointer = PBuffer;
	cmd.SRB_CDBLen = 0xa;

//	/*
	cmd.CDBByte[0]		= 0x55;		// MODE SELECT
	cmd.CDBByte[1]		= (1 << 4) + 0;
	cmd.CDBByte[7]		= (BYTE)((BufLen >> 8) & 0xff);
	cmd.CDBByte[8]		= (BYTE)(BufLen & 0xff);
//	*/

	ExecuteCommand(cmd);

	free(PBuffer);

	if(cmd.SRB_Status == SS_COMP){
		return true;
	}

	return false;
}

int CAspi::GetDeviceCount(void)
{
	return 0;
}

void CAspi::SetDevice(int DeviceNo)
{
}

void CAspi::GetDeviceString(string & Vendor, string & Product, string & Revision, string & BusAddress)
{
}

int CAspi::GetCurrentDevice(void)
{
	return 0;
}
