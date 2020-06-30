

#include <windows.h>
#include <string>
using namespace std;

#pragma pack(1)

typedef void (*POSTPROCFUNC)();

//   ASPI global definitions
#define SENSE_LEN					14
#define SRB_POSTING					0x01
#define SRB_DIR_IN					0x08
#define SRB_DIR_OUT					0x10
#define SRB_EVENT_NOTIFY			0x40

//   definitions of SRB_Status
#define SS_PENDING					0x00	//   Process is working
#define SS_COMP						0x01	//   Process is completed
#define SS_ABORTED					0x02	//   Process is aborted

//   definitions of ASPI command
#define SC_HA_INQUIRY		0x00        //   Inquiry
#define SC_GET_DEV_TYPE		0x01        //   Get device information
#define SC_EXEC_SCSI_CMD	0x02        //   Execute command

typedef unsigned char	Ucbit;
typedef unsigned char	u_char;

#define	MP_P_CODE			\
	Ucbit	p_code		: 6;	\
	Ucbit	p_res		: 1;	\
	Ucbit	parsave		: 1


//   structure of host adapter inquiry
typedef struct tSRB_HAInquiry               // Offset
{
	BYTE	SRB_Cmd;			//   SC_HA_INQUIRY
	BYTE	SRB_Status;
	BYTE	SRB_Ha;
	BYTE	SRB_Flags;
	DWORD	Reserved1;
	BYTE	HA_Count;
	BYTE	HA_SCSI_ID;
	BYTE	HA_ManagerId[16];
	BYTE	HA_Identifier[16];
	BYTE	HA_Unique[16];
	WORD	Reserved2;
} SRB_HAInquiry;

//   structure of get device type
typedef struct tSRB_GDEVBlock
{
	BYTE	SRB_Cmd;		//   SC_GET_DEV_TYPE
	BYTE	SRB_Status;
	BYTE	SRB_Ha;
	BYTE	SRB_Flags;
	DWORD	Reserved1;
	BYTE	SRB_Tgt;
	BYTE	SRB_Lun;
	BYTE	SRB_DeviceType;
	BYTE	Reserved2;
} SRB_GDEVBlock;

//   structure of execute scsi command
typedef struct SRB_ExecSCSICmd
{
	BYTE	SRB_Cmd;				//   SC_EXEC_SCSI_CMD
	BYTE	SRB_Status;
	BYTE	SRB_Ha;
	BYTE	SRB_Flags;
	DWORD	Reserved1;
	BYTE	SRB_Tgt;
	BYTE	SRB_Lun;
	WORD	Reserved2;
	DWORD	SRB_BufLen;
	BYTE	*SRB_BufPointer;
	BYTE	SRB_SenseLen;
	BYTE	SRB_CDBLen;
	BYTE	SRB_HaStat;
	BYTE	SRB_TgtStat;
	VOID	*SRB_PostProc;
	BYTE	Reserved3[20];
	BYTE	CDBByte[16];
	BYTE	SenseArea[32+2];
} SRB_ExecSCSICmd;

//   structure of aspibuff
typedef struct tASPI32BUFF                   // Offset
{
	PBYTE	BufPointer;
	DWORD	BufLen;
	DWORD	ZeroFill;
	DWORD	Reserved;
} ASPI32BUFF;

//   type definitions
typedef void *LPSRB;

//   definitions of functions type
typedef DWORD (*fGetASPI32SupportInfo)(void);
typedef DWORD (*fSendASPI32Command)(LPSRB);
typedef BOOL  (*fGetASPI32Buffer)(ASPI32BUFF *ab);
typedef BOOL  (*fFreeASPI32Buffer)(ASPI32BUFF *pab);
typedef BOOL  (*fTranslateASPI32Address)(DWORD *path,DWORD *node);
typedef DWORD (*fGetASPI32DLLVersion)(void);


// CD Cap / mech status 
typedef struct SCSICDMODEPAGE2A_TAG
{		
	MP_P_CODE;					// parsave & pagecode				(0)
	u_char	p_len;				// 0x14 = 20 Bytes					(1)

	Ucbit	cd_r_read	: 1;	// Reads CD-R  media				(2)
	Ucbit	cd_rw_read	: 1;	// Reads CD-RW media		     
	Ucbit	method2		: 1;	// Reads fixed packet method2 media  
	Ucbit	dvd_rom_read: 1;	// Reads DVD ROM media		     
	Ucbit	dvd_r_read	: 1;	// Reads DVD-R media		     
	Ucbit	dvd_ram_read: 1;	// Reads DVD-RAM media		     
	Ucbit	res_2_67	: 2;	// Reserved			     

	Ucbit	cd_r_write	: 1;	// Supports writing CD-R  media		(3)
	Ucbit	cd_rw_write	: 1;	// Supports writing CD-RW media	     
	Ucbit	test_write	: 1;	// Supports emulation write	     
	Ucbit	res_3_3		: 1;	// Reserved			     
	Ucbit	dvd_r_write	: 1;	// Supports writing DVD-R media	     
	Ucbit	dvd_ram_write: 1;	// Supports writing DVD-RAM media    
	Ucbit	res_3_67	: 2;	// Reserved			     

	Ucbit	audio_play	: 1;	// Supports Audio play operation	(4)    
	Ucbit	composite	: 1;	// Deliveres composite A/V stream
	Ucbit	digital_port_2: 1;	// Supports digital output on port 2 
	Ucbit	digital_port_1: 1;	// Supports digital output on port 1 
	Ucbit	mode_2_form_1: 1;	// Reads Mode-2 form 1 media (XA)    
	Ucbit	mode_2_form_2: 1;	// Reads Mode-2 form 2 media	     
	Ucbit	multi_session: 1;	// Reads multi-session media	     
	Ucbit	res_4		: 1;	// Reserved			     

	Ucbit	cd_da_supported: 1;	// Reads audio data with READ CD cmd 
	Ucbit	cd_da_accurate: 1;	// READ CD data stream is accurate   
	Ucbit	rw_supported: 1;	// Reads R-W sub channel information 
	Ucbit	rw_deint_cor: 1;	// Reads de-interleved R-W sub chan  
	Ucbit	c2_pointers	: 1;	// Supports C2 error pointers	     
	Ucbit	ISRC		: 1;	// Reads ISRC information	     
	Ucbit	UPC			: 1;	// Reads media catalog number (UPC)  
	Ucbit	read_bar_code: 1;	// Supports reading bar codes	     

	Ucbit	lock		: 1;	// PREVENT/ALLOW may lock media	     (5)
	Ucbit	lock_state	: 1;	// Lock state 0=unlocked 1=locked    
	Ucbit	prevent_jumper: 1;	// State of prev/allow jumper 0=pres 
	Ucbit	eject		: 1;	// Ejects disc/cartr with STOP LoEj  
	Ucbit	res_6_4		: 1;	// Reserved			     
	Ucbit	loading_type: 3;	// Loading mechanism type	     

	Ucbit	sep_chan_vol: 1;	// Vol controls each channel separat (6)
	Ucbit	sep_chan_mute: 1;	// Mute controls each channel separat
	Ucbit	disk_present_rep:1;	// Changer supports disk present rep 
	Ucbit	sw_slot_sel:1;		// Load empty slot in changer	     
	Ucbit	res_7	: 4;		// Reserved			     

	BYTE	ReadSpeedH;			// Max. read speed in KB/s				(7)
	BYTE	ReadSpeedL;			// Max. read speed in KB/s				(7)

	u_char	num_vol_levels[2];	// # of supported volume levels			(9)

	u_char	buffer_size[2];		// Buffer size for the data in KB		(11)
	u_char	cur_read_speed[2];	// Current read speed in KB/s			(13)	     
	u_char	res_16;				// Reserved								(14)
	
	Ucbit	res_17_0: 1;		// Reserved								(15)
	Ucbit	BCK		: 1;		// Data valid on falling edge of BCK 
	Ucbit	RCK		: 1;		// Set: HIGH high LRCK=left channel  
	Ucbit	LSBF		: 1;	// Set: LSB first Clear: MSB first   
	Ucbit	length		: 2;	// 0=32BCKs 1=16BCKs 2=24BCKs 3=24I2c
	Ucbit	res_17		: 2;	// Reserved			     
	
	u_char	max_write_speed[2];	// Max. write speed supported in KB/s	(17)

	u_char	cur_write_speed[2];	// Current write speed in KB/s			(19)

} SCSICDMODEPAGE2A;

char *fillbytes(void *tov, int cnt, char val);


typedef struct SCISMODEHEADER_TAG {
	Ucbit	sense_data_len	: 8;
	u_char	medium_type;
	Ucbit	res2		: 4;
	Ucbit	cache		: 1;
	Ucbit	res		: 2;
	Ucbit	write_prot	: 1;
	BYTE	nBlockLen;
} SCISMODEHEADER;


typedef struct SCSIMODEHDR_6_TAG {
	BYTE		btModeDataLen;					// 0
	BYTE		btMediumType;					// 1
	BYTE		btDevSpecificParam;				// 2
	BYTE		btBlkDescrLen;					// 3
} SCSIMODEHDR_6;


typedef struct SCSIMODEHDR_10_TAG {
	BYTE		btModeDataLenH;					// 0
	BYTE		btModeDataLenL;					// 1
	BYTE		btMediumType;					// 2
	BYTE		btDevSpecificParam;				// 3
	BYTE		btReserved1;					// 4
	BYTE		btReserved2;					// 5 
	BYTE		btBlkDescrLenH;					// 6
	BYTE		btBlkDescrLenL;					// 7
} SCSIMODEHDR_10;

typedef struct SCSIBLOCKDESCRIPTOR_TAG {
	BYTE		btDensity;						// 0
	BYTE		btNumberOfBlocksH;				// 1
	BYTE		btNumberOfBlocksM;				// 2
	BYTE		btNumberOfBlocksL;				// 3
	BYTE		btReserved;						// 4
	BYTE		btBlockLenH;					// 5 
	BYTE		btBlockLenM;					// 6
	BYTE		btBlockLenL;					// 7
} SCSIBLOCKDESCRIPTOR;



#pragma pack()

class CAspi  
{
public:
	CAspi();
	virtual ~CAspi();

	long GetLeadOutPos();
	int CheckDisc();
	string SCSICDRScan( );
	bool ItCanWrite();
	bool EraseMedia(bool FastErase);
	int InfosMedia(int &iNbTrack);
	int ReadTrackInfo(int track,int &iTrackBegin,int &iTrackEnding);
	long CDROMCapacity(long &NbBlocks, long &sizeBlocks);
	string DriveMediaSupport();
	int InfosMediaErasable();
	bool IsCDReady();
	BOOL EjectCD(BOOL bEject);
	BYTE IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen,LPBYTE lpBuffer,int nBufLen,BYTE btAdapterID,BYTE btTargetID,BYTE btLunID);
	BYTE IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen);
	BYTE IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen,LPBYTE lpBuffer,int nBufLen);
	virtual DWORD ModeSense(int nPage,BYTE* pbtBuffer,int nBufSize);
	virtual bool ModeSelect(BYTE *Buffer, DWORD BufLen);
	virtual DWORD ModeSense(BYTE *Buffer, DWORD BufLen, BYTE PCFlag, BYTE PageCode);
	virtual void ExecuteCommand(SRB_ExecSCSICmd &cmd);
	virtual DWORD GetVersion(void);
	virtual BOOL IsActive(void);
	virtual void Initialize(void);
	virtual void InitialAsync(void);
	virtual void FinalizeAsync(void);
	virtual bool ExecuteCommandAsync(SRB_ExecSCSICmd &cmd);
	virtual int GetDeviceCount(void);
	virtual void GetDeviceString(string & Vendor, string & Product, string & Revision, string & BusAddress);
	virtual void SetDevice(int DeviceNo);
	virtual int GetCurrentDevice(void);

	BYTE GetHaId()
	{
		return m_Ha;
	}

	BYTE GetTarget()
	{
		return m_Tgt;
	}

	BYTE GetLunId()
	{
		return m_Lun;
	}

protected:

	bool ReadATIP(BYTE * Buffer);
	int TestUnitReady();
	int mode_sense(BYTE * dp,int cnt,int page,int pcf);

	BYTE m_SK;
	BYTE m_ASC;
	BYTE m_ASCQ;
	BYTE m_Ha;
	BYTE m_Tgt;
	BYTE m_Lun;
	BOOL bAtapi;
	long m_LeadOutPos;
};
