
/*!
 *  @file	coreconfigurator.c
 *  @brief
 *  @author
 *  @sa		coreconfigurator.h
 *  @date	1 Mar 2012
 *  @version	1.0
 */


/*****************************************************************************/
/* File Includes                                                             */
/*****************************************************************************/
#include "coreconfigurator.h"
/*****************************************************************************/
/* Constants                                                                 */
/*****************************************************************************/
#define INLINE static __inline
#define PHY_802_11n
#define MAX_CFG_PKTLEN     1450
#define MSG_HEADER_LEN     4
#define QUERY_MSG_TYPE     'Q'
#define WRITE_MSG_TYPE     'W'
#define RESP_MSG_TYPE      'R'
#define WRITE_RESP_SUCCESS 1
#define INVALID         255
#define MAC_ADDR_LEN    6
#define TAG_PARAM_OFFSET	(MAC_HDR_LEN + TIME_STAMP_LEN + \
							BEACON_INTERVAL_LEN + CAP_INFO_LEN)

/*****************************************************************************/
/* Function Macros                                                           */
/*****************************************************************************/


/*****************************************************************************/
/* Type Definitions                                                          */
/*****************************************************************************/

/* Basic Frame Type Codes (2-bit) */
typedef enum {
	FRAME_TYPE_CONTROL     = 0x04,
	FRAME_TYPE_DATA        = 0x08,
	FRAME_TYPE_MANAGEMENT  = 0x00,
	FRAME_TYPE_RESERVED    = 0x0C,
	FRAME_TYPE_FORCE_32BIT = 0xFFFFFFFF
} tenuBasicFrmType;

/* Frame Type and Subtype Codes (6-bit) */
typedef enum {
	ASSOC_REQ             = 0x00,
	ASSOC_RSP             = 0x10,
	REASSOC_REQ           = 0x20,
	REASSOC_RSP           = 0x30,
	PROBE_REQ             = 0x40,
	PROBE_RSP             = 0x50,
	BEACON                = 0x80,
	ATIM                  = 0x90,
	DISASOC               = 0xA0,
	AUTH                  = 0xB0,
	DEAUTH                = 0xC0,
	ACTION                = 0xD0,
	PS_POLL               = 0xA4,
	RTS                   = 0xB4,
	CTS                   = 0xC4,
	ACK                   = 0xD4,
	CFEND                 = 0xE4,
	CFEND_ACK             = 0xF4,
	DATA                  = 0x08,
	DATA_ACK              = 0x18,
	DATA_POLL             = 0x28,
	DATA_POLL_ACK         = 0x38,
	NULL_FRAME            = 0x48,
	CFACK                 = 0x58,
	CFPOLL                = 0x68,
	CFPOLL_ACK            = 0x78,
	QOS_DATA              = 0x88,
	QOS_DATA_ACK          = 0x98,
	QOS_DATA_POLL         = 0xA8,
	QOS_DATA_POLL_ACK     = 0xB8,
	QOS_NULL_FRAME        = 0xC8,
	QOS_CFPOLL            = 0xE8,
	QOS_CFPOLL_ACK        = 0xF8,
	BLOCKACK_REQ          = 0x84,
	BLOCKACK              = 0x94,
	FRAME_SUBTYPE_FORCE_32BIT  = 0xFFFFFFFF
} tenuFrmSubtype;

/* Element ID  of various Information Elements */
typedef enum {
	ISSID               = 0,   /* Service Set Identifier         */
	ISUPRATES           = 1,   /* Supported Rates                */
	IFHPARMS            = 2,   /* FH parameter set               */
	IDSPARMS            = 3,   /* DS parameter set               */
	ICFPARMS            = 4,   /* CF parameter set               */
	ITIM                = 5,   /* Traffic Information Map        */
	IIBPARMS            = 6,   /* IBSS parameter set             */
	ICOUNTRY            = 7,   /* Country element                */
	IEDCAPARAMS         = 12,  /* EDCA parameter set             */
	ITSPEC              = 13,  /* Traffic Specification          */
	ITCLAS              = 14,  /* Traffic Classification         */
	ISCHED              = 15,  /* Schedule                       */
	ICTEXT              = 16,  /* Challenge Text                 */
	IPOWERCONSTRAINT    = 32,  /* Power Constraint               */
	IPOWERCAPABILITY    = 33,  /* Power Capability               */
	ITPCREQUEST         = 34,  /* TPC Request                    */
	ITPCREPORT          = 35,  /* TPC Report                     */
	ISUPCHANNEL         = 36,  /* Supported channel list         */
	ICHSWANNOUNC        = 37,  /* Channel Switch Announcement    */
	IMEASUREMENTREQUEST = 38,  /* Measurement request            */
	IMEASUREMENTREPORT  = 39,  /* Measurement report             */
	IQUIET              = 40,  /* Quiet element Info             */
	IIBSSDFS            = 41,  /* IBSS DFS                       */
	IERPINFO            = 42,  /* ERP Information                */
	ITSDELAY            = 43,  /* TS Delay                       */
	ITCLASPROCESS       = 44,  /* TCLAS Processing               */
	IHTCAP              = 45,  /* HT Capabilities                */
	IQOSCAP             = 46,  /* QoS Capability                 */
	IRSNELEMENT         = 48,  /* RSN Information Element        */
	IEXSUPRATES         = 50,  /* Extended Supported Rates       */
	IEXCHSWANNOUNC      = 60,  /* Extended Ch Switch Announcement*/
	IHTOPERATION        = 61,  /* HT Information                 */
	ISECCHOFF           = 62,  /* Secondary Channel Offeset      */
	I2040COEX           = 72,  /* 20/40 Coexistence IE           */
	I2040INTOLCHREPORT  = 73,  /* 20/40 Intolerant channel report*/
	IOBSSSCAN           = 74,  /* OBSS Scan parameters           */
	IEXTCAP             = 127, /* Extended capability            */
	IWMM                = 221, /* WMM parameters                 */
	IWPAELEMENT         = 221, /* WPA Information Element        */
	INFOELEM_ID_FORCE_32BIT  = 0xFFFFFFFF
} tenuInfoElemID;


typedef struct {
	char *pcRespBuffer;
	s32 s32MaxRespBuffLen;
	s32 s32BytesRead;
	bool bRespRequired;
} tstrConfigPktInfo;



/*****************************************************************************/
/* Extern Variable Declarations                                              */
/*****************************************************************************/


/*****************************************************************************/
/* Extern Function Declarations                                              */
/*****************************************************************************/
extern s32 SendRawPacket(s8 *ps8Packet, s32 s32PacketLen);
extern void NetworkInfoReceived(u8 *pu8Buffer, u32 u32Length);
extern void GnrlAsyncInfoReceived(u8 *pu8Buffer, u32 u32Length);
extern void host_int_ScanCompleteReceived(u8 *pu8Buffer, u32 u32Length);
/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/
static struct semaphore SemHandleSendPkt;
static struct semaphore SemHandlePktResp;


static tstrConfigPktInfo gstrConfigPktInfo;

/* WID Switches */
static tstrWID gastrWIDs[] = {
	{WID_FIRMWARE_VERSION,          WID_STR},
	{WID_PHY_VERSION,               WID_STR},
	{WID_HARDWARE_VERSION,          WID_STR},
	{WID_BSS_TYPE,                  WID_CHAR},
	{WID_QOS_ENABLE,                WID_CHAR},
	{WID_11I_MODE,                  WID_CHAR},
	{WID_CURRENT_TX_RATE,           WID_CHAR},
	{WID_LINKSPEED,                 WID_CHAR},
	{WID_RTS_THRESHOLD,             WID_SHORT},
	{WID_FRAG_THRESHOLD,            WID_SHORT},
	{WID_SSID,                      WID_STR},
	{WID_BSSID,                     WID_ADR},
	{WID_BEACON_INTERVAL,           WID_SHORT},
	{WID_POWER_MANAGEMENT,          WID_CHAR},
	{WID_LISTEN_INTERVAL,           WID_CHAR},
	{WID_DTIM_PERIOD,               WID_CHAR},
	{WID_CURRENT_CHANNEL,           WID_CHAR},
	{WID_TX_POWER_LEVEL_11A,        WID_CHAR},
	{WID_TX_POWER_LEVEL_11B,        WID_CHAR},
	{WID_PREAMBLE,                  WID_CHAR},
	{WID_11G_OPERATING_MODE,        WID_CHAR},
	{WID_MAC_ADDR,                  WID_ADR},
	{WID_IP_ADDRESS,                WID_ADR},
	{WID_ACK_POLICY,                WID_CHAR},
	{WID_PHY_ACTIVE_REG,            WID_CHAR},
	{WID_AUTH_TYPE,                 WID_CHAR},
	{WID_REKEY_POLICY,              WID_CHAR},
	{WID_REKEY_PERIOD,              WID_INT},
	{WID_REKEY_PACKET_COUNT,        WID_INT},
	{WID_11I_PSK,                   WID_STR},
	{WID_1X_KEY,                    WID_STR},
	{WID_1X_SERV_ADDR,              WID_IP},
	{WID_SUPP_USERNAME,             WID_STR},
	{WID_SUPP_PASSWORD,             WID_STR},
	{WID_USER_CONTROL_ON_TX_POWER,  WID_CHAR},
	{WID_MEMORY_ADDRESS,            WID_INT},
	{WID_MEMORY_ACCESS_32BIT,       WID_INT},
	{WID_MEMORY_ACCESS_16BIT,       WID_SHORT},
	{WID_MEMORY_ACCESS_8BIT,        WID_CHAR},
	{WID_SITE_SURVEY_RESULTS,       WID_STR},
	{WID_PMKID_INFO,                WID_STR},
	{WID_ASSOC_RES_INFO,            WID_STR},
	{WID_MANUFACTURER,              WID_STR}, /* 4 Wids added for the CAPI tool*/
	{WID_MODEL_NAME,                WID_STR},
	{WID_MODEL_NUM,                 WID_STR},
	{WID_DEVICE_NAME,               WID_STR},
	{WID_SSID_PROBE_REQ,            WID_STR},

#ifdef MAC_802_11N
	{WID_11N_ENABLE,                WID_CHAR},
	{WID_11N_CURRENT_TX_MCS,        WID_CHAR},
	{WID_TX_POWER_LEVEL_11N,        WID_CHAR},
	{WID_11N_OPERATING_MODE,        WID_CHAR},
	{WID_11N_SMPS_MODE,             WID_CHAR},
	{WID_11N_PROT_MECH,             WID_CHAR},
	{WID_11N_ERP_PROT_TYPE,         WID_CHAR},
	{WID_11N_HT_PROT_TYPE,          WID_CHAR},
	{WID_11N_PHY_ACTIVE_REG_VAL,    WID_INT},
	{WID_11N_PRINT_STATS,           WID_CHAR},
	{WID_11N_AUTORATE_TABLE,        WID_BIN_DATA},
	{WID_HOST_CONFIG_IF_TYPE,       WID_CHAR},
	{WID_HOST_DATA_IF_TYPE,         WID_CHAR},
	{WID_11N_SIG_QUAL_VAL,          WID_SHORT},
	{WID_11N_IMMEDIATE_BA_ENABLED,  WID_CHAR},
	{WID_11N_TXOP_PROT_DISABLE,     WID_CHAR},
	{WID_11N_SHORT_GI_20MHZ_ENABLE, WID_CHAR},
	{WID_SHORT_SLOT_ALLOWED,        WID_CHAR},
	{WID_11W_ENABLE,                WID_CHAR},
	{WID_11W_MGMT_PROT_REQ,         WID_CHAR},
	{WID_2040_ENABLE,               WID_CHAR},
	{WID_2040_COEXISTENCE,          WID_CHAR},
	{WID_USER_SEC_CHANNEL_OFFSET,   WID_CHAR},
	{WID_2040_CURR_CHANNEL_OFFSET,  WID_CHAR},
	{WID_2040_40MHZ_INTOLERANT,     WID_CHAR},
	{WID_HUT_RESTART,               WID_CHAR},
	{WID_HUT_NUM_TX_PKTS,           WID_INT},
	{WID_HUT_FRAME_LEN,             WID_SHORT},
	{WID_HUT_TX_FORMAT,             WID_CHAR},
	{WID_HUT_BANDWIDTH,             WID_CHAR},
	{WID_HUT_OP_BAND,               WID_CHAR},
	{WID_HUT_STBC,                  WID_CHAR},
	{WID_HUT_ESS,                   WID_CHAR},
	{WID_HUT_ANTSET,                WID_CHAR},
	{WID_HUT_HT_OP_MODE,            WID_CHAR},
	{WID_HUT_RIFS_MODE,             WID_CHAR},
	{WID_HUT_SMOOTHING_REC,         WID_CHAR},
	{WID_HUT_SOUNDING_PKT,          WID_CHAR},
	{WID_HUT_HT_CODING,             WID_CHAR},
	{WID_HUT_TEST_DIR,              WID_CHAR},
	{WID_HUT_TXOP_LIMIT,            WID_SHORT},
	{WID_HUT_DEST_ADDR,             WID_ADR},
	{WID_HUT_TX_PATTERN,            WID_BIN_DATA},
	{WID_HUT_TX_TIME_TAKEN,         WID_INT},
	{WID_HUT_PHY_TEST_MODE,         WID_CHAR},
	{WID_HUT_PHY_TEST_RATE_HI,      WID_CHAR},
	{WID_HUT_PHY_TEST_RATE_LO,      WID_CHAR},
	{WID_HUT_TX_TEST_TIME,          WID_INT},
	{WID_HUT_LOG_INTERVAL,          WID_INT},
	{WID_HUT_DISABLE_RXQ_REPLENISH, WID_CHAR},
	{WID_HUT_TEST_ID,               WID_STR},
	{WID_HUT_KEY_ORIGIN,            WID_CHAR},
	{WID_HUT_BCST_PERCENT,          WID_CHAR},
	{WID_HUT_GROUP_CIPHER_TYPE,     WID_CHAR},
	{WID_HUT_STATS,                 WID_BIN_DATA},
	{WID_HUT_TSF_TEST_MODE,         WID_CHAR},
	{WID_HUT_SIG_QUAL_AVG,          WID_SHORT},
	{WID_HUT_SIG_QUAL_AVG_CNT,      WID_SHORT},
	{WID_HUT_TSSI_VALUE,            WID_CHAR},
	{WID_HUT_MGMT_PERCENT,          WID_CHAR},
	{WID_HUT_MGMT_BCST_PERCENT,     WID_CHAR},
	{WID_HUT_MGMT_ALLOW_HT,         WID_CHAR},
	{WID_HUT_UC_MGMT_TYPE,          WID_CHAR},
	{WID_HUT_BC_MGMT_TYPE,          WID_CHAR},
	{WID_HUT_UC_MGMT_FRAME_LEN,     WID_SHORT},
	{WID_HUT_BC_MGMT_FRAME_LEN,     WID_SHORT},
	{WID_HUT_11W_MFP_REQUIRED_TX,   WID_CHAR},
	{WID_HUT_11W_MFP_PEER_CAPABLE,  WID_CHAR},
	{WID_HUT_11W_TX_IGTK_ID,        WID_CHAR},
	{WID_HUT_FC_TXOP_MOD,           WID_CHAR},
	{WID_HUT_FC_PROT_TYPE,          WID_CHAR},
	{WID_HUT_SEC_CCA_ASSERT,        WID_CHAR},
#endif /* MAC_802_11N */
};

u16 g_num_total_switches = (sizeof(gastrWIDs) / sizeof(tstrWID));
/*****************************************************************************/
/* Static Function Declarations                                              */
/*****************************************************************************/



/*****************************************************************************/
/* Functions                                                                 */
/*****************************************************************************/

/* This function extracts the beacon period field from the beacon or probe   */
/* response frame.                                                           */
INLINE u16 get_beacon_period(u8 *data)
{
	u16 bcn_per = 0;

	bcn_per  = data[0];
	bcn_per |= (data[1] << 8);

	return bcn_per;
}

INLINE u32 get_beacon_timestamp_lo(u8 *data)
{
	u32 time_stamp = 0;
	u32 index    = MAC_HDR_LEN;

	time_stamp |= data[index++];
	time_stamp |= (data[index++] << 8);
	time_stamp |= (data[index++] << 16);
	time_stamp |= (data[index]   << 24);

	return time_stamp;
}

INLINE u32 get_beacon_timestamp_hi(u8 *data)
{
	u32 time_stamp = 0;
	u32 index    = (MAC_HDR_LEN + 4);

	time_stamp |= data[index++];
	time_stamp |= (data[index++] << 8);
	time_stamp |= (data[index++] << 16);
	time_stamp |= (data[index]   << 24);

	return time_stamp;
}

/* This function extracts the 'frame type and sub type' bits from the MAC    */
/* header of the input frame.                                                */
/* Returns the value in the LSB of the returned value.                       */
INLINE tenuFrmSubtype get_sub_type(u8 *header)
{
	return ((tenuFrmSubtype)(header[0] & 0xFC));
}

/* This function extracts the 'to ds' bit from the MAC header of the input   */
/* frame.                                                                    */
/* Returns the value in the LSB of the returned value.                       */
INLINE u8 get_to_ds(u8 *header)
{
	return (header[1] & 0x01);
}

/* This function extracts the 'from ds' bit from the MAC header of the input */
/* frame.                                                                    */
/* Returns the value in the LSB of the returned value.                       */
INLINE u8 get_from_ds(u8 *header)
{
	return ((header[1] & 0x02) >> 1);
}

/* This function extracts the MAC Address in 'address1' field of the MAC     */
/* header and updates the MAC Address in the allocated 'addr' variable.      */
INLINE void get_address1(u8 *pu8msa, u8 *addr)
{
	memcpy(addr, pu8msa + 4, 6);
}

/* This function extracts the MAC Address in 'address2' field of the MAC     */
/* header and updates the MAC Address in the allocated 'addr' variable.      */
INLINE void get_address2(u8 *pu8msa, u8 *addr)
{
	memcpy(addr, pu8msa + 10, 6);
}

/* This function extracts the MAC Address in 'address3' field of the MAC     */
/* header and updates the MAC Address in the allocated 'addr' variable.      */
INLINE void get_address3(u8 *pu8msa, u8 *addr)
{
	memcpy(addr, pu8msa + 16, 6);
}

/* This function extracts the BSSID from the incoming WLAN packet based on   */
/* the 'from ds' bit, and updates the MAC Address in the allocated 'addr'    */
/* variable.                                                                 */
INLINE void get_BSSID(u8 *data, u8 *bssid)
{
	if (get_from_ds(data) == 1)
		get_address2(data, bssid);
	else if (get_to_ds(data) == 1)
		get_address1(data, bssid);
	else
		get_address3(data, bssid);
}

/* This function extracts the SSID from a beacon/probe response frame        */
INLINE void get_ssid(u8 *data, u8 *ssid, u8 *p_ssid_len)
{
	u8 len = 0;
	u8 i   = 0;
	u8 j   = 0;

	len = data[MAC_HDR_LEN + TIME_STAMP_LEN + BEACON_INTERVAL_LEN +
		   CAP_INFO_LEN + 1];
	j   = MAC_HDR_LEN + TIME_STAMP_LEN + BEACON_INTERVAL_LEN +
		CAP_INFO_LEN + 2;

	/* If the SSID length field is set wrongly to a value greater than the   */
	/* allowed maximum SSID length limit, reset the length to 0              */
	if (len >= MAX_SSID_LEN)
		len = 0;

	for (i = 0; i < len; i++, j++)
		ssid[i] = data[j];

	ssid[len] = '\0';

	*p_ssid_len = len;
}

/* This function extracts the capability info field from the beacon or probe */
/* response frame.                                                           */
INLINE u16 get_cap_info(u8 *data)
{
	u16 cap_info = 0;
	u16 index    = MAC_HDR_LEN;
	tenuFrmSubtype st;

	st = get_sub_type(data);

	/* Location of the Capability field is different for Beacon and */
	/* Association frames.                                          */
	if ((st == BEACON) || (st == PROBE_RSP))
		index += TIME_STAMP_LEN + BEACON_INTERVAL_LEN;

	cap_info  = data[index];
	cap_info |= (data[index + 1] << 8);

	return cap_info;
}

/* This function extracts the capability info field from the Association */
/* response frame.                                                                       */
INLINE u16 get_assoc_resp_cap_info(u8 *data)
{
	u16 cap_info = 0;

	cap_info  = data[0];
	cap_info |= (data[1] << 8);

	return cap_info;
}

/* This funcion extracts the association status code from the incoming       */
/* association response frame and returns association status code            */
INLINE u16 get_asoc_status(u8 *data)
{
	u16 asoc_status = 0;

	asoc_status = data[3];
	asoc_status = (asoc_status << 8) | data[2];

	return asoc_status;
}

/* This function extracts association ID from the incoming association       */
/* response frame							                                     */
INLINE u16 get_asoc_id(u8 *data)
{
	u16 asoc_id = 0;

	asoc_id  = data[4];
	asoc_id |= (data[5] << 8);

	return asoc_id;
}

/**
 *  @brief              initializes the Core Configurator
 *  @details
 *  @return     Error code indicating success/failure
 *  @note
 *  @author	mabubakr
 *  @date		1 Mar 2012
 *  @version		1.0
 */

s32 CoreConfiguratorInit(void)
{
	s32 s32Error = WILC_SUCCESS;

	PRINT_D(CORECONFIG_DBG, "CoreConfiguratorInit()\n");

	sema_init(&SemHandleSendPkt, 1);
	sema_init(&SemHandlePktResp, 0);


	memset((void *)(&gstrConfigPktInfo), 0, sizeof(tstrConfigPktInfo));
	return s32Error;
}

u8 *get_tim_elm(u8 *pu8msa, u16 u16RxLen, u16 u16TagParamOffset)
{
	u16 u16index = 0;

	/*************************************************************************/
	/*                       Beacon Frame - Frame Body                       */
	/* --------------------------------------------------------------------- */
	/* |Timestamp |BeaconInt |CapInfo |SSID |SupRates |DSParSet |TIM elm   | */
	/* --------------------------------------------------------------------- */
	/* |8         |2         |2       |2-34 |3-10     |3        |4-256     | */
	/* --------------------------------------------------------------------- */
	/*                                                                       */
	/*************************************************************************/

	u16index = u16TagParamOffset;

	/* Search for the TIM Element Field and return if the element is found */
	while (u16index < (u16RxLen - FCS_LEN)) {
		if (pu8msa[u16index] == ITIM)
			return &pu8msa[u16index];
		else
			u16index += (IE_HDR_LEN + pu8msa[u16index + 1]);
	}

	return NULL;
}

/* This function gets the current channel information from
 * the 802.11n beacon/probe response frame */
u8 get_current_channel_802_11n(u8 *pu8msa, u16 u16RxLen)
{
	u16 index;

	index = TAG_PARAM_OFFSET;
	while (index < (u16RxLen - FCS_LEN)) {
		if (pu8msa[index] == IDSPARMS)
			return pu8msa[index + 2];
		else
			/* Increment index by length information and header */
			index += pu8msa[index + 1] + IE_HDR_LEN;
	}

	/* Return current channel information from the MIB, if beacon/probe  */
	/* response frame does not contain the DS parameter set IE           */
	/* return (mget_CurrentChannel() + 1); */
	return 0;  /* no MIB here */
}

u8 get_current_channel(u8 *pu8msa, u16 u16RxLen)
{
#ifdef PHY_802_11n
#ifdef FIVE_GHZ_BAND
	/* Get the current channel as its not set in */
	/* 802.11a beacons/probe response            */
	return (get_rf_channel() + 1);
#else /* FIVE_GHZ_BAND */
	/* Extract current channel information from */
	/* the beacon/probe response frame          */
	return get_current_channel_802_11n(pu8msa, u16RxLen);
#endif /* FIVE_GHZ_BAND */
#else
	return 0;
#endif /* PHY_802_11n */
}

/**
 *  @brief                      parses the received 'N' message
 *  @details
 *  @param[in]  pu8MsgBuffer The message to be parsed
 *  @param[out]         ppstrNetworkInfo pointer to pointer to the structure containing the parsed Network Info
 *  @return             Error code indicating success/failure
 *  @note
 *  @author		mabubakr
 *  @date			1 Mar 2012
 *  @version		1.0
 */
s32 ParseNetworkInfo(u8 *pu8MsgBuffer, tstrNetworkInfo **ppstrNetworkInfo)
{
	s32 s32Error = WILC_SUCCESS;
	tstrNetworkInfo *pstrNetworkInfo = NULL;
	u8 u8MsgType = 0;
	u8 u8MsgID = 0;
	u16 u16MsgLen = 0;

	u16 u16WidID = (u16)WID_NIL;
	u16 u16WidLen  = 0;
	u8  *pu8WidVal = NULL;

	u8MsgType = pu8MsgBuffer[0];

	/* Check whether the received message type is 'N' */
	if ('N' != u8MsgType) {
		PRINT_ER("Received Message format incorrect.\n");
		WILC_ERRORREPORT(s32Error, WILC_FAIL);
	}

	/* Extract message ID */
	u8MsgID = pu8MsgBuffer[1];

	/* Extract message Length */
	u16MsgLen = MAKE_WORD16(pu8MsgBuffer[2], pu8MsgBuffer[3]);

	/* Extract WID ID */
	u16WidID = MAKE_WORD16(pu8MsgBuffer[4], pu8MsgBuffer[5]);

	/* Extract WID Length */
	u16WidLen = MAKE_WORD16(pu8MsgBuffer[6], pu8MsgBuffer[7]);

	/* Assign a pointer to the WID value */
	pu8WidVal  = &pu8MsgBuffer[8];

	/* parse the WID value of the WID "WID_NEWORK_INFO" */
	{
		u8  *pu8msa = NULL;
		u16 u16RxLen = 0;
		u8 *pu8TimElm = NULL;
		u8 *pu8IEs = NULL;
		u16 u16IEsLen = 0;
		u8 u8index = 0;
		u32 u32Tsf_Lo;
		u32 u32Tsf_Hi;

		pstrNetworkInfo = kmalloc(sizeof(tstrNetworkInfo), GFP_KERNEL);
		if (!pstrNetworkInfo)
			return -ENOMEM;

		memset((void *)(pstrNetworkInfo), 0, sizeof(tstrNetworkInfo));

		pstrNetworkInfo->s8rssi = pu8WidVal[0];

		/* Assign a pointer to msa "Mac Header Start Address" */
		pu8msa = &pu8WidVal[1];

		u16RxLen = u16WidLen - 1;

		/* parse msa*/

		/* Get the cap_info */
		pstrNetworkInfo->u16CapInfo = get_cap_info(pu8msa);
		#ifdef WILC_P2P
		/* Get time-stamp [Low only 32 bit] */
		pstrNetworkInfo->u32Tsf = get_beacon_timestamp_lo(pu8msa);
		PRINT_D(CORECONFIG_DBG, "TSF :%x\n", pstrNetworkInfo->u32Tsf);
		#endif

		/* Get full time-stamp [Low and High 64 bit] */
		u32Tsf_Lo = get_beacon_timestamp_lo(pu8msa);
		u32Tsf_Hi = get_beacon_timestamp_hi(pu8msa);

		pstrNetworkInfo->u64Tsf = u32Tsf_Lo | ((u64)u32Tsf_Hi << 32);

		/* Get SSID */
		get_ssid(pu8msa, pstrNetworkInfo->au8ssid, &(pstrNetworkInfo->u8SsidLen));

		/* Get BSSID */
		get_BSSID(pu8msa, pstrNetworkInfo->au8bssid);

		/* Get the current channel */
		pstrNetworkInfo->u8channel = get_current_channel(pu8msa, (u16RxLen + FCS_LEN));

		/* Get beacon period */
		u8index = (MAC_HDR_LEN + TIME_STAMP_LEN);

		pstrNetworkInfo->u16BeaconPeriod = get_beacon_period(pu8msa + u8index);

		u8index += BEACON_INTERVAL_LEN + CAP_INFO_LEN;

		/* Get DTIM Period */
		pu8TimElm = get_tim_elm(pu8msa, (u16RxLen + FCS_LEN), u8index);
		if (pu8TimElm != NULL)
			pstrNetworkInfo->u8DtimPeriod = pu8TimElm[3];
		pu8IEs = &pu8msa[MAC_HDR_LEN + TIME_STAMP_LEN + BEACON_INTERVAL_LEN + CAP_INFO_LEN];
		u16IEsLen = u16RxLen - (MAC_HDR_LEN + TIME_STAMP_LEN + BEACON_INTERVAL_LEN + CAP_INFO_LEN);

		if (u16IEsLen > 0) {
			pstrNetworkInfo->pu8IEs = kmalloc(u16IEsLen, GFP_KERNEL);
			if (!pstrNetworkInfo->pu8IEs)
				return -ENOMEM;

			memset((void *)(pstrNetworkInfo->pu8IEs), 0, u16IEsLen);

			memcpy(pstrNetworkInfo->pu8IEs, pu8IEs, u16IEsLen);
		}
		pstrNetworkInfo->u16IEsLen = u16IEsLen;

	}

	*ppstrNetworkInfo = pstrNetworkInfo;

ERRORHANDLER:
	return s32Error;
}

/**
 *  @brief              Deallocates the parsed Network Info
 *  @details
 *  @param[in]  pstrNetworkInfo Network Info to be deallocated
 *  @return             Error code indicating success/failure
 *  @note
 *  @author		mabubakr
 *  @date		1 Mar 2012
 *  @version		1.0
 */
s32 DeallocateNetworkInfo(tstrNetworkInfo *pstrNetworkInfo)
{
	s32 s32Error = WILC_SUCCESS;

	if (pstrNetworkInfo != NULL) {
		if (pstrNetworkInfo->pu8IEs != NULL) {
			kfree(pstrNetworkInfo->pu8IEs);
			pstrNetworkInfo->pu8IEs = NULL;
		} else {
			s32Error = WILC_FAIL;
		}

		kfree(pstrNetworkInfo);
		pstrNetworkInfo = NULL;

	} else {
		s32Error = WILC_FAIL;
	}

	return s32Error;
}

/**
 *  @brief                      parses the received Association Response frame
 *  @details
 *  @param[in]  pu8Buffer The Association Response frame to be parsed
 *  @param[out]         ppstrConnectRespInfo pointer to pointer to the structure containing the parsed Association Response Info
 *  @return             Error code indicating success/failure
 *  @note
 *  @author		mabubakr
 *  @date			2 Apr 2012
 *  @version		1.0
 */
s32 ParseAssocRespInfo(u8 *pu8Buffer, u32 u32BufferLen,
			       tstrConnectRespInfo **ppstrConnectRespInfo)
{
	s32 s32Error = WILC_SUCCESS;
	tstrConnectRespInfo *pstrConnectRespInfo = NULL;
	u16 u16AssocRespLen = 0;
	u8 *pu8IEs = NULL;
	u16 u16IEsLen = 0;

	pstrConnectRespInfo = kmalloc(sizeof(tstrConnectRespInfo), GFP_KERNEL);
	if (!pstrConnectRespInfo)
		return -ENOMEM;

	memset((void *)(pstrConnectRespInfo), 0, sizeof(tstrConnectRespInfo));

	/* u16AssocRespLen = pu8Buffer[0]; */
	u16AssocRespLen = (u16)u32BufferLen;

	/* get the status code */
	pstrConnectRespInfo->u16ConnectStatus = get_asoc_status(pu8Buffer);
	if (pstrConnectRespInfo->u16ConnectStatus == SUCCESSFUL_STATUSCODE) {

		/* get the capability */
		pstrConnectRespInfo->u16capability = get_assoc_resp_cap_info(pu8Buffer);

		/* get the Association ID */
		pstrConnectRespInfo->u16AssocID = get_asoc_id(pu8Buffer);

		/* get the Information Elements */
		pu8IEs = &pu8Buffer[CAP_INFO_LEN + STATUS_CODE_LEN + AID_LEN];
		u16IEsLen = u16AssocRespLen - (CAP_INFO_LEN + STATUS_CODE_LEN + AID_LEN);

		pstrConnectRespInfo->pu8RespIEs = kmalloc(u16IEsLen, GFP_KERNEL);
		if (!pstrConnectRespInfo->pu8RespIEs)
			return -ENOMEM;

		memset((void *)(pstrConnectRespInfo->pu8RespIEs), 0, u16IEsLen);

		memcpy(pstrConnectRespInfo->pu8RespIEs, pu8IEs, u16IEsLen);
		pstrConnectRespInfo->u16RespIEsLen = u16IEsLen;
	}

	*ppstrConnectRespInfo = pstrConnectRespInfo;


	return s32Error;
}

/**
 *  @brief                      Deallocates the parsed Association Response Info
 *  @details
 *  @param[in]  pstrNetworkInfo Network Info to be deallocated
 *  @return             Error code indicating success/failure
 *  @note
 *  @author		mabubakr
 *  @date			2 Apr 2012
 *  @version		1.0
 */
s32 DeallocateAssocRespInfo(tstrConnectRespInfo *pstrConnectRespInfo)
{
	s32 s32Error = WILC_SUCCESS;

	if (pstrConnectRespInfo != NULL) {
		if (pstrConnectRespInfo->pu8RespIEs != NULL) {
			kfree(pstrConnectRespInfo->pu8RespIEs);
			pstrConnectRespInfo->pu8RespIEs = NULL;
		} else {
			s32Error = WILC_FAIL;
		}

		kfree(pstrConnectRespInfo);
		pstrConnectRespInfo = NULL;

	} else {
		s32Error = WILC_FAIL;
	}

	return s32Error;
}

#ifndef CONNECT_DIRECT
s32 ParseSurveyResults(u8 ppu8RcvdSiteSurveyResults[][MAX_SURVEY_RESULT_FRAG_SIZE],
			       wid_site_survey_reslts_s **ppstrSurveyResults,
			       u32 *pu32SurveyResultsCount)
{
	s32 s32Error = WILC_SUCCESS;
	wid_site_survey_reslts_s *pstrSurveyResults = NULL;
	u32 u32SurveyResultsCount = 0;
	u32 u32SurveyBytesLength = 0;
	u8 *pu8BufferPtr;
	u32 u32RcvdSurveyResultsNum = 2;
	u8 u8ReadSurveyResFragNum;
	u32 i;
	u32 j;

	for (i = 0; i < u32RcvdSurveyResultsNum; i++) {
		u32SurveyBytesLength = ppu8RcvdSiteSurveyResults[i][0];


		for (j = 0; j < u32SurveyBytesLength; j += SURVEY_RESULT_LENGTH) {
			u32SurveyResultsCount++;
		}
	}

	pstrSurveyResults = kmalloc_array(u32SurveyResultsCount,
				sizeof(wid_site_survey_reslts_s), GFP_KERNEL);
	if (!pstrSurveyResults)
		return -ENOMEM;

	memset((void *)(pstrSurveyResults), 0, u32SurveyResultsCount * sizeof(wid_site_survey_reslts_s));

	u32SurveyResultsCount = 0;

	for (i = 0; i < u32RcvdSurveyResultsNum; i++) {
		pu8BufferPtr = ppu8RcvdSiteSurveyResults[i];

		u32SurveyBytesLength = pu8BufferPtr[0];

		/* TODO: mostafa: pu8BufferPtr[1] contains the fragment num */
		u8ReadSurveyResFragNum = pu8BufferPtr[1];

		pu8BufferPtr += 2;

		for (j = 0; j < u32SurveyBytesLength; j += SURVEY_RESULT_LENGTH) {
			memcpy(&pstrSurveyResults[u32SurveyResultsCount], pu8BufferPtr, SURVEY_RESULT_LENGTH);
			pu8BufferPtr += SURVEY_RESULT_LENGTH;
			u32SurveyResultsCount++;
		}
	}

ERRORHANDLER:
	*ppstrSurveyResults = pstrSurveyResults;
	*pu32SurveyResultsCount = u32SurveyResultsCount;

	return s32Error;
}


s32 DeallocateSurveyResults(wid_site_survey_reslts_s *pstrSurveyResults)
{
	s32 s32Error = WILC_SUCCESS;

	if (pstrSurveyResults != NULL) {
		kfree(pstrSurveyResults);
	}

	return s32Error;
}
#endif

/**
 *  @brief              Deinitializes the Core Configurator
 *  @details
 *  @return     Error code indicating success/failure
 *  @note
 *  @author	mabubakr
 *  @date		1 Mar 2012
 *  @version	1.0
 */

s32 CoreConfiguratorDeInit(void)
{
	s32 s32Error = WILC_SUCCESS;

	PRINT_D(CORECONFIG_DBG, "CoreConfiguratorDeInit()\n");


	return s32Error;
}

/*Using the global handle of the driver*/
extern wilc_wlan_oup_t *gpstrWlanOps;
/**
 *  @brief              sends certain Configuration Packet based on the input WIDs pstrWIDs
 *  using driver config layer
 *
 *  @details
 *  @param[in]  pstrWIDs WIDs to be sent in the configuration packet
 *  @param[in]  u32WIDsCount number of WIDs to be sent in the configuration packet
 *  @param[out]         pu8RxResp The received Packet Response
 *  @param[out]         ps32RxRespLen Length of the received Packet Response
 *  @return     Error code indicating success/failure
 *  @note
 *  @author	mabubakr
 *  @date		1 Mar 2012
 *  @version	1.0
 */
s32 SendConfigPkt(u8 u8Mode, tstrWID *pstrWIDs,
			  u32 u32WIDsCount, bool bRespRequired, u32 drvHandler)
{
	s32 counter = 0, ret = 0;

	if (gpstrWlanOps == NULL) {
		PRINT_D(CORECONFIG_DBG, "Net Dev is still not initialized\n");
		return 1;
	} else {
		PRINT_D(CORECONFIG_DBG, "Net Dev is initialized\n");
	}
	if (gpstrWlanOps->wlan_cfg_set == NULL ||
	    gpstrWlanOps->wlan_cfg_get == NULL)	{
		PRINT_D(CORECONFIG_DBG, "Set and Get is still not initialized\n");
		return 1;
	} else {
		PRINT_D(CORECONFIG_DBG, "SET is initialized\n");
	}
	if (u8Mode == GET_CFG) {
		for (counter = 0; counter < u32WIDsCount; counter++) {
			PRINT_INFO(CORECONFIG_DBG, "Sending CFG packet [%d][%d]\n", !counter,
				   (counter == u32WIDsCount - 1));
			if (!gpstrWlanOps->wlan_cfg_get(!counter,
							pstrWIDs[counter].u16WIDid,
							(counter == u32WIDsCount - 1), drvHandler)) {
				ret = -1;
				printk("[Sendconfigpkt]Get Timed out\n");
				break;
			}
		}
		/**
		 *      get the value
		 **/
		counter = 0;
		for (counter = 0; counter < u32WIDsCount; counter++) {
			pstrWIDs[counter].s32ValueSize = gpstrWlanOps->wlan_cfg_get_value(
					pstrWIDs[counter].u16WIDid,
					pstrWIDs[counter].ps8WidVal, pstrWIDs[counter].s32ValueSize);

		}
	} else if (u8Mode == SET_CFG) {
		for (counter = 0; counter < u32WIDsCount; counter++) {
			PRINT_D(CORECONFIG_DBG, "Sending config SET PACKET WID:%x\n", pstrWIDs[counter].u16WIDid);
			if (!gpstrWlanOps->wlan_cfg_set(!counter,
							pstrWIDs[counter].u16WIDid, pstrWIDs[counter].ps8WidVal,
							pstrWIDs[counter].s32ValueSize,
							(counter == u32WIDsCount - 1), drvHandler)) {
				ret = -1;
				printk("[Sendconfigpkt]Set Timed out\n");
				break;
			}
		}
	}

	return ret;
}
