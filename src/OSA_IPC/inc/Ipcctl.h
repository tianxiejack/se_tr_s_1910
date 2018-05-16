#ifndef __IPCCTL_H_
#define __IPCCTL_H_

#if 0
#define _PATH0_ "/trackmsg0"
#define _PATH1_ "/trackmsg1"
#define _PATH2_ "/trackmsg2"
#define _PATH3_ "/trackmsg3"
#else
#define _PATH0_ "/"
#define _PATH1_ "/"
#define _PATH2_ "/"
#define _PATH3_ "/"
#endif

#define SHMEMSTATUSSIZE 200
#define SHMEMFRAMESIZE 10485760
#define MMTNUM 10
#define PARAMLEN 12

typedef enum 
{
    sensor,  /*0*/
    pinp, /*1*/
    trk, /*2*/
    sectrk, /*3*/
    enh, /*4*/
    mtd, /*5*/
    trkdoor, /*6*/
    posmove, /*7*/
    zoom, /*8*/
    autocheck, /*9*/
    mmt,/*10*/
    mmtselect,/*11*/
    axismove,/*12*/
    read_shm_trkpos,/*13*/
    exit_img,
    invalid
}CMD_ID;


typedef enum
{
    TRK_MODE,
    LOCK_MODE,
    SCAN_MODE,
    SMALL_AREA_MODE,
    FOLLOW_MODE,
    SEARCH_TRK_MODE
}to_osd_work_mode_enum;

typedef enum
{
    SINGLE_RANGE_MODE,
    CONTINUE_RANGE_MODE,
    MARK_RANGE_MODE,
    EXPOSURE_MODE
}to_osd_laser_work_mode_enum;

typedef enum 
{
    IPC_TOIMG_MSG,  		// SERVER TO CLIENT
    IPC_FRIMG_MSG,		// CLIENT TO SERCER
    IPC_SHA,		
    IPC_SEM,
    IPC_MAX
}IPC_MTYPE;

typedef enum 
{
    IPC_Class_MSG,  
    IPC_Class_SHA,
    IPC_Class_SEM,
    IPC_ClassMAX
}IPC_Class;

typedef enum {
	IPC_eSen_TV0	= 0x00,
	IPC_eSen_TV1	= 0x01,
	IPC_eSen_TV2   = 0x02,
	IPC_eSen_TV3 	= 0x03,
	IPC_eSen_FR1   = 0x04,
	IPC_eSen_Max   = 0x05
}IPC_eSenserStat;

typedef enum img_zoom
{
    zoom_0 = 0x00,
    zoom_2 = 0x02,
    zoom_4 = 0x04,
    zoom_8 = 0x08
}ipc_img_zoom;

typedef enum ipc_Dram_TrkAim
{
	IPC_AIM_NO1		= 0x00,
	IPC_AIM_NO2		= 0x01,
	IPC_AIM_NO3		= 0x02,
	IPC_AIM_NO4		= 0x03,
	IPC_AIM_NO5		= 0x04,
	IPC_AIM_MAX		= 0x05,
} ipc_eTrkAim;

typedef enum {
	ipc_ePicp_top_left = 0x00,
	ipc_ePicp_top_right = 0x01,
	ipc_ePicp_bot_left = 0x02,
	ipc_ePicp_bot_right = 0x03,
}ipc_ePicpPosStat;

typedef enum ipc_Dram_SysMode
{
	IPC_INIT_MODE           = 0x00,
	IPC_CHECK_MODE      = 0x01,
	IPC_AXIS_MODE           = 0x02,
	IPC_NORMAL_MODE     = 0x03,
} ipc_eSysMode;

typedef enum ipc_Dram_TrkType
{
	IPC_AUTO_MODE		= 0x00,
	IPC_CONTRAST_MODE	= 0x01,
	IPC_CENTRE_MODE		= 0x02,
} ipc_eTrkType;

typedef enum ipc_Dram_zoomCtrl
{
	ipc_eZoom_Ctrl_Pause    = 0x00,
	ipc_eZoom_Ctrl_SCAL2    = 0x01,
	ipc_eZoom_Ctrl_SCAL4    = 0x02,
	ipc_eZoom_Ctrl_ADD      = 0x01,
	ipc_eZoom_Ctrl_SUB      = 0x02,
} ipc_eZoomCtrl;

typedef enum ipc_Dram_ImgAlg
{
	ipc_eImgAlg_Disable     = 0x00,
	ipc_eImgAlg_Enable      = 0x01,
} ipc_eImgAlgStat;  // use by img zoom/enh/stb/mtd/rst

typedef enum ipc_Dram_MMTSelect
{
	ipc_eMMT_No     = 0x00,
	ipc_eMMT_Next       = 0x01,
	ipc_eMMT_Prev       = 0x02,
	ipc_eMMT_Select = 0x03,
} ipc_eMMTSelect;

typedef enum ipc_Dram_DispGradeStat
{
	ipc_eDisp_hide      = 0x00,
	ipc_eDisp_show_rect     = 0x01,
	ipc_eDisp_show_text     = 0x02,
	ipc_eDisp_show_dbg      = 0x04,
} ipc_eDispGrade;

typedef enum ipc_Dram_DispGradeColor
{
	ipc_ecolor_Default  = 0x0,
	ipc_ecolor_Black = 0x1,
	ipc_ecolor_White    = 0x2,
	ipc_ecolor_Red = 0x03,
	ipc_ecolor_Yellow = 0x04,
	ipc_ecolor_Blue = 0x05,
	ipc_ecolor_Green = 0x06,
} ipc_eOSDColor;

typedef enum
{
	ipc_eTrk_Acq        = 0x00,
	ipc_eTrk_Normal = 0x01,
	ipc_eTrk_Assi       = 0x02,
	ipc_eTrk_Lost       = 0x03,
} ipc_eTrkStat;

typedef enum ipc_Dram_trkMode
{
	// mode cmd
	ipc_eTrk_mode_acq       = 0x00,
	ipc_eTrk_mode_target    = 0x01,
	ipc_eTrk_mode_mtd       = 0x02,
	ipc_eTrk_mode_sectrk    = 0x03,
	ipc_eTrk_mode_search    = 0x04,
} ipc_eTrkMode;

typedef enum ipc_Dram_trkRefine
{
	ipc_eTrk_ref_no     = 0x00,
	ipc_eTrk_ref_left   = 0x01,
	ipc_eTrk_ref_right  = 0x02,
	ipc_eTrk_ref_up     = 0x01,
	ipc_eTrk_ref_down   = 0x02,
} ipc_eTrkRefine;

typedef enum ipc_Dram_saveMode
{
	ipc_eSave_Disable       = 0x00,
	ipc_eSave_Enable        = 0x01,
	ipc_eSave_Cancel        = 0x02,
} ipc_eSaveMode;

typedef struct{
	char *name;
	int Identify;
	int Class;
	int IPCID;
}IPC_Handl;

typedef struct{
    volatile unsigned char unitFaultStat;/*0:FR ok,TV ok; 1:FR ok,TV err; 2:FR err,TV ok; 3:FR err,TV err*/
    volatile unsigned char ComStat;/*\u6d93\u5a5a\u20ac\u6c2b\u4fca\u9418\u8235\u20ac\u4f8a\u7d1d0\u59dd\uff45\u7236\u951b?\u93c1\u5474\u6bb0*/
    volatile unsigned char RomdeviceStat;/*\u701b\u6a3a\u504d\u7481\u60e7\ue62c\u9418\u8235\u20ac\u4f8a\u7d300\u59dd\uff45\u7236\u951b?\u93c1\u5474\u6bb0*/
    volatile unsigned char GPUStat;/*GPU\u9418\u8235\u20ac\u4f8a\u7d300\u59dd\uff45\u7236\u951b?\u93c1\u5474\u6bb0*/
}CMD_AUTOCHECK;


typedef struct{
	volatile unsigned char SensorStat;
}CMD_SENSOR;

typedef struct{
	volatile unsigned char ImgPicp;
	volatile unsigned char PicpSensorStat;
	volatile unsigned char PicpZoomStat;//pinp zoom
}CMD_PinP;

typedef struct{
	volatile unsigned char AvtTrkStat;// eTrkMode 
    	volatile unsigned char TrkCmd;//only for ack trk mod
}CMD_TRK;
	
typedef struct{
	volatile unsigned char SecAcqStat;// eSecTrkMode
	volatile  int ImgPixelX; //SecTrk X
	volatile  int ImgPixelY; //SecTrk Y
}CMD_SECTRK;

typedef struct{
    volatile unsigned char ImgEnhStat;// eImgAlgStat enh 0:close 1:open
}CMD_ENH;

typedef struct{
	volatile unsigned char ImgMmtStat;// eImgAlgStat mtd 0:close 1:open
}CMD_MMT;

typedef struct{
	volatile unsigned char ImgMtdStat;// eImgAlgStat mtd 0:close 1:open
}CMD_MTD;

typedef struct{
	volatile unsigned char ImgMmtSelect;// eMMTSelect or range 0-MTD_TARGET_NUM
}CMD_MMTSELECT;

typedef struct{
	volatile unsigned char unitMtdValid;//mmt  0-disable 1-valid 2-unvalid
	volatile unsigned char unitMtdPixelX1;//report mmt x1
	volatile unsigned char unitMtdPixelX2;//report mmt x2
	volatile unsigned char unitMtdPixelY1;//report mmt y1
	volatile unsigned char unitMtdPixelY2;//report mmt y2
}CMD_sMTD;

typedef struct{
	volatile unsigned char unitAimW;//aim w
	volatile unsigned char unitAimH;//aim h
}CMD_AIMWH;

typedef struct{
	volatile unsigned char unitAimX1; //aim x1
	volatile unsigned char unitAimX2; //aim x2
	volatile unsigned char unitAimY1; //aim y1
	volatile unsigned char unitAimY2; //aim y2
}CMD_AIMXY;

typedef struct{
	volatile unsigned char AvtTrkAimSize;
}CMD_TRKDOOR;

typedef struct{
	volatile unsigned char UartSpeedSelect;//sync422 rate
}CMD_SYNC422;

typedef struct{
	volatile unsigned char AvtMoveX;// eTrkRefine (axis or aim) x
	volatile unsigned char AvtMoveY;// eTrkRefine (axis or aim) y
}CMD_POSMOVE;

typedef struct{
	volatile unsigned char unitTrkX1;// for report and osd text x1
	volatile unsigned char unitTrkX2;// for report and osd text x2
	volatile unsigned char unitTrkY1;// for report and osd text y1
	volatile unsigned char unitTrkY2;// for report and osd text y2
}CMD_sTRKXY;

typedef struct{
	volatile unsigned char unitAxisX1;//cross x1
	volatile unsigned char unitAxisX2;//cross x2
	volatile unsigned char unitAxisY1;//cross y1
	volatile unsigned char unitAxisY2;//cross y2
}CMD_CROSSXY;

typedef struct{
    volatile unsigned char ImgZoomStat;
}CMD_ZOOM;

typedef struct{
    volatile unsigned char screenshot;//1:enable 2:disable
    volatile unsigned char g_record;//1:enable record  2:disable record
}CMD_SCREENSHOT;

typedef struct{
    volatile unsigned char screenh265;//1:enable 2:disable
    volatile unsigned char g_record;//1:enable record  2:disable record
}CMD_SCREENH265;

typedef struct{
    volatile unsigned int framerate;
}CMD_FRAMERATE;

typedef struct{
    volatile unsigned int datastream;
}CMD_DATASTREAM;

typedef struct{
    unsigned char cmd_ID;
	unsigned char param[PARAMLEN];
}SENDST;


typedef struct{
    unsigned int m_trackstatus;// acq/trk/assi/lost
     int m_trackpos_x;
     int m_trackpos_y;
     unsigned int unitFaultStat;
}IMGSTATUS;


typedef struct out_frame_angle
{
    volatile unsigned char out_frame_angle_a[3];
    volatile unsigned char out_frame_angle_e[3];
}out_frame_angle_t;

typedef struct in_frame_angle
{
    volatile unsigned char  in_frame_angle_a[3];
    volatile unsigned char  in_frame_angle_e[3];
}in_frame_angle_t;

typedef struct to_osd_time
{
    volatile unsigned char year[2];
    volatile unsigned char month;
    volatile unsigned char day;
    volatile unsigned char hour;
    volatile unsigned char min;
    volatile unsigned char sec;
    volatile unsigned char msec;
}to_osd_time_t;

typedef struct work_mode
{
    volatile unsigned char mode;
}work_mode_t;

typedef struct laser_work_mode
{
    volatile unsigned char laser_mode;
}laser_work_mode_t;

typedef struct laser_range
{
    volatile unsigned char range[2];
}laser_range_t;

typedef struct man_focal_length
{
    volatile unsigned char length[2];
}man_focal_length_t;

typedef struct photo_dect_temp
{
    volatile  char temp;
}photo_dect_temp_t;

typedef struct prevent_jam
{
    volatile unsigned char stat;
    volatile unsigned char level;
}prevent_jam_t;


int ipc_settrack(unsigned int trackstatus, int trackposx, int trackposy);
int ipc_gettrack(unsigned int* trackstatus, int* trackposx, int* trackposy);
int ipc_setSensorstat(unsigned int unitFaultStat);
int ipc_getSensorstat(unsigned int* unitFaultStat);
int ipc_sendmsg(SENDST* Param,int Mesgthe );
int ipc_recvmsg(SENDST* Param,int Mesgthe );
void* recv_msg(SENDST *RS422);
int send_msg(SENDST *RS422);
int  send_msgpth(SENDST * RS422);
void Ipc_init();
void Ipc_create();
void Ipc_uninit();
void  ipc_status_P();
void  ipc_status_V();


#endif
