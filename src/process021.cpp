/*
 * process021.cpp
 *
 *  Created on: May 5, 2017
 *      Author: ubuntu
 */
#include <glut.h>
#include "process021.hpp"
#include "vmath.h"
//#include "grpFont.h"


#include "dx_main.h"
#include "msgDriv.h"
#include"app_ctrl.h"
#include"dx.h"
#include"osd_cv.h"
CProcess021 * CProcess021::sThis = NULL;

#define AXIS_FILE		"AxisFile.yml"
CProcess021::CProcess021()
{
	memset(&rcTrackBak, 0, sizeof(rcTrackBak));
	memset(tgBak, 0, sizeof(tgBak));
	memset(&blob_rectBak, 0, sizeof(blob_rectBak));
	memset(&extInCtrl, 0, sizeof(CMD_EXT));
	memset(&extOutAck, 0, sizeof(ACK_EXT));
	prisensorstatus=0;//tv
	m_castTm=0;
	m_bCast=false;
	rememflag=false;
	rememtime=0;
	// default cmd value
	CMD_EXT *pIStuts = &extInCtrl;

	pIStuts->unitAxisX[0]      = 640;
	pIStuts->unitAxisY[0]      = 512;
#ifdef VIDEO1280X1024
	pIStuts->unitAxisX[1]      = 640;
	pIStuts->unitAxisY[1]      = 512;
#else
	pIStuts->unitAxisX[1]      = 320;
	pIStuts->unitAxisY[1]      = 256;
#endif

	pIStuts->unitAxisXtmp[0] = 640;
	pIStuts->unitAxisYtmp[0] = 512;
	
	pIStuts->unitAimW = 64;
	pIStuts->unitAimH = 64;
	pIStuts->unitAimX=640;
	pIStuts->unitAimY=512;
	pIStuts->SensorStat     = eSen_TV;
	pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat = 0xFF;

	pIStuts->DispGrp[0] = 1;//(eDisp_show_rect | eDisp_show_text/* | eDisp_show_dbg*/);
	pIStuts->DispGrp[1] =1;// (eDisp_show_rect | eDisp_show_text/* | eDisp_show_dbg*/);


	pIStuts->DispColor[0]=2;
	pIStuts->DispColor[1]=2;

	extInCtrl.TrkBomenCtrl=1;
	pIStuts->changeSensorFlag = 0;
	pIStuts->unitWorkMode = NORMAL_MODE; // self init check end
	crossBak.x=extInCtrl.unitAxisX[pIStuts->SensorStat ];
	crossBak.y=extInCtrl.unitAxisY[pIStuts->SensorStat ];
	pIStuts->AvtTrkAimSize=2;

	pIStuts->ImgPixelX[0]=640;
	pIStuts->ImgPixelY[0]=512;

	pIStuts->AvtPosXTv=640;
	pIStuts->AvtPosYTv=512;

	pIStuts->FovStat=1;
#ifdef VIDEO1280X1024
	pIStuts->ImgPixelX[1]=800;
	pIStuts->ImgPixelY[1]=800;

	pIStuts->AvtPosXFir=640;
	pIStuts->AvtPosYFir=512;
#else
	pIStuts->ImgPixelX[1]=320;
	pIStuts->ImgPixelY[1]=256;

	pIStuts->AvtPosXFir=320;
	pIStuts->AvtPosYFir=256;
	
#endif
	pIStuts->FrCollimation=2;
	pIStuts->PicpSensorStatpri=2;
	tvcorx=_IMAGE_WIDTH_-100;
	tvcory=_IMAGE_HEIGHT_-100;

	pIStuts->Firpoweron=2;
	memset(secBak,0,sizeof(secBak));

	memset(Osdflag,0,sizeof(Osdflag));

	
	Mmtsendtime=0;

#if 0
	if(ReadAxisFromFile() != 0)
	{
		pIStuts->unitAxisXtmp[0] = 640;
		pIStuts->unitAxisYtmp[0] = 512;
	}
#endif
	//for(i=0;i<4;i++)
	//
	rendpos[0].x=vdisWH[0][0]*2/3;
	rendpos[0].y=vdisWH[0][1]*2/3;
	rendpos[0].w=vdisWH[0][0]/3;
	rendpos[0].h=vdisWH[0][1]/3;

	rendpos[1].x=vdisWH[0][0]*2/3;
	rendpos[1].y=0;
	rendpos[1].w=vdisWH[0][0]/3;
	rendpos[1].h=vdisWH[0][1]/3;

	rendpos[2].x=0;
	rendpos[2].y=0;
	rendpos[2].w=vdisWH[0][0]/3;
	rendpos[2].h=vdisWH[0][1]/3;

	rendpos[3].x=0;
	rendpos[3].y=vdisWH[0][1]*2/3;
	rendpos[3].w=vdisWH[0][0]/3;
	rendpos[3].h=vdisWH[0][1]/3;

	msgextInCtrl=&extInCtrl;
	sThis = this;
	
}

CProcess021::~CProcess021()
{
	sThis=NULL;
}

int  CProcess021::WindowstoPiexlx(int x,int channel)
{
	int ret=0;
	ret= cvRound(x*1.0/vdisWH[0][0]*vcapWH[channel][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vcapWH[channel][0])
	 	{
			ret=vcapWH[channel][0];
	 	}


	  return ret;
}


int  CProcess021::WindowstoPiexly(int y,int channel)
{
	 int ret=0;
	 ret= cvRound(y*1.0/vdisWH[0][1]*vcapWH[channel][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vcapWH[channel][1])
	 	{
			ret=vcapWH[channel][1];
	 	}
	return  ret;
}



int  CProcess021::PiexltoWindowsx(int x,int channel)
{
	int ret=0;
	 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
	 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
	 ret= cvRound(x*1.0/vcapWH[channel][0]*vdisWH[0][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][0])
	 	{
			ret=vdisWH[0][0];
	 	}

	 if(extInCtrl.ImgMmtshow[extInCtrl.SensorStat])
	 	{
			ret =ret*2/3;//-vdisWH[0][0]/6;

	 	}

	  return ret;
}

int  CProcess021::PiexltoWindowsy(int y,int channel)
{
	 int ret=0;
	 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
	 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
	 ret= cvRound(y*1.0/vcapWH[channel][1]*vdisWH[0][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][1])
	 	{
			ret=vdisWH[0][1];
	 	}

	  if(extInCtrl.ImgMmtshow[extInCtrl.SensorStat])
	 	{
			ret =ret*2/3;//-vdisWH[0][1]/6;

	 	}
	
	return  ret;
}

float  CProcess021::PiexltoWindowsxf(float x,int channel)
{
	float ret=0;
	 ret= (x*1.0/vcapWH[channel][0]*vdisWH[0][0]);
	 if(ret<0)
 	{
		ret=0;
 	}
	 else if(ret>=vdisWH[0][0])
 	{
		ret=vdisWH[0][0];
 	}

	  return ret;
}

float  CProcess021::PiexltoWindowsyf(float y,int channel)
{
	 float ret=0;
	 ret= (y*1.0/vcapWH[channel][1]*vdisWH[0][1]);

	  if(ret<0)
 	{
		ret=0;
 	}
	 else if(ret>=vdisWH[0][1])
 	{
		ret=vdisWH[0][1];
 	}
	
	return  ret;
}


int  CProcess021::PiexltoWindowsxzoom(int x,int channel)
{
	int ret=0;
	 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
	 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
	 ret= cvRound(x*1.0/vcapWH[channel][0]*vdisWH[0][0]);
	 if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][0])
	 	{
			ret=vdisWH[0][0];
	 	}

	 if(extInCtrl.ImgMmtshow[extInCtrl.SensorStat])
	 	{
			ret =ret*2/3;//-vdisWH[0][0]/6;

	 	}
//zoom 18.4.6
	 if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0)
	 	{
	 		ret=ret-320;
			ret=2*ret;
	 		;
			//ret=ret*2;
	 	}


	  return ret;
}

int  CProcess021::PiexltoWindowsyzoom(int y,int channel)
{
	 int ret=0;
	 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
	 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
	 ret= cvRound(y*1.0/vcapWH[channel][1]*vdisWH[0][1]);

	  if(ret<0)
	 	{
			ret=0;
	 	}
	 else if(ret>=vdisWH[0][1])
	 	{
			ret=vdisWH[0][1];
	 	}

	  if(extInCtrl.ImgMmtshow[extInCtrl.SensorStat])
	 	{
			ret =ret*2/3;//-vdisWH[0][1]/6;

	 	}
	  //zoom 18.4.6
	 if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0)
	 	{
	 		ret=ret-256;
			ret=2*ret;
	 		;
			//ret=ret*2;
	 	}
	return  ret;
}

int  CProcess021::PiexltoWindowsxzoom_TrkRect(int x,int channel)
{
	int ret=0;

	ret= cvRound(x*1.0/vcapWH[channel][0]*vdisWH[0][0]);
	
	if(ret<0)
	{
		ret=0;
	}
	else if(ret>=vdisWH[0][0])
	{
		ret=vdisWH[0][0];
	}

	//zoom 18.4.6
	//if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0)
	{
		//ret=ret-320;
		//ret=2*ret;
		/*
		if(ret>=vdisWH[0][0]/2)
			ret = (ret-vdisWH[0][0]/2)/2+vdisWH[0][0]/2 ;
		else if(ret<vdisWH[0][0]/2)
			ret = vdisWH[0][0]/2 -(vdisWH[0][0]/2 - ret)/2 ;
		*/
		
	}

	#if 1
	//result to even
	if((ret%2)==0)
		ret = ret;
	else
		ret = ret+1;
	#else

	if((ret>=vdisWH[0][0]/2-1)&&(ret<=vdisWH[0][0]/2+1))
		ret = vdisWH[0][0]/2;

	#endif
	  return ret;
}

int  CProcess021::PiexltoWindowsyzoom_TrkRect(int y,int channel)
{
	 int ret=0;

	 ret= cvRound(y*1.0/vcapWH[channel][1]*vdisWH[0][1]);

	if(ret<0)
 	{
		ret=0;
 	}
	 else if(ret>=vdisWH[0][1])
 	{
		ret=vdisWH[0][1];
 	}

	  //zoom 18.4.6
	//if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0)
 	{
 		//ret=ret-256;
		//ret=2*ret;

		/*
		if(ret>=vdisWH[0][1]/2)
			ret = (ret-vdisWH[0][1]/2)/2+vdisWH[0][1]/2;
		else if(ret<vdisWH[0][1]/2)
			ret = vdisWH[0][1]/2 -(vdisWH[0][1]/2 - ret)/2;
		*/
 	}

	 #if 1
	//result to even
	 if((ret%2)==0)
		ret = ret;
	else
		ret = ret+1;
	#else

	if((ret>=vdisWH[0][1]/2-1)&&(ret<=vdisWH[0][1]/2+1))
		ret = vdisWH[0][1]/2;
	
	#endif
	return  ret;
}
void CProcess021::OnCreate()
{


	osdgraph_init(process_osd);
	MSGDRIV_create();
	MSGAPI_initial();
	//App_dxmain();

	#if 1
	CMD_EXT *pIStuts = &extInCtrl;

	#if 0
	CFGID_FIELD_GET(pIStuts->unitAxisX ,CFGID_IMAGE_AXIX1);
	CFGID_FIELD_GET(pIStuts->unitAxisY ,CFGID_IMAGE_AXIY1);
	if(pIStuts->unitAxisX<0||pIStuts->unitAxisX>1280)
		pIStuts->unitAxisX      = 640;
	if(pIStuts->unitAxisY<0||pIStuts->unitAxisY>1024)
		pIStuts->unitAxisY      = 512;

	pIStuts->unitAimX=pIStuts->unitAxisX ;
	pIStuts->unitAimY=pIStuts->unitAxisY ;
	m_ImageAxisx=pIStuts->unitAxisX ;
	m_ImageAxisy=pIStuts->unitAxisY;
	#endif

	CFGID_FIELD_GET(pIStuts->DispColor[0] ,CFGID_RTS_TV_SEN_COLOR);
	CFGID_FIELD_GET(pIStuts->DispColor[1] ,CFGID_RTS_FR_SEN_COLOR);
	if(pIStuts->DispColor[0]<0||pIStuts->DispColor[0]>6)
		pIStuts->DispColor[0]=2;

	CFGID_FIELD_GET(pIStuts->PicpPosStat,CFGID_SENSOR_TV_PICP_POS);
	if(pIStuts->PicpPosStat<0||pIStuts->PicpPosStat>3)
		pIStuts->PicpPosStat=0;
	//pIStuts->PicpPosStat=0;
#endif

};
void CProcess021::OnDestroy(){};
void CProcess021::OnInit(){};
void CProcess021::OnConfig(){};
void CProcess021::OnRun(){};
void CProcess021::OnStop(){};
void CProcess021::Ontimer(){

	//msgdriv_event(MSGID_EXT_INPUT_VIDEOEN,NULL);
};
bool CProcess021::OnPreProcess(int chId, Mat &frame)
{
	set_graph();

	
	if(m_bCast){
		Uint32 curTm = OSA_getCurTimeInMsec();
		Uint32 elapse = curTm - m_castTm;
		//MSGAPI_AckSnd( AckTrkErr);
		if(elapse < 2000){
			return false;
		}
		else
			{
				m_bCast=false;
			}
	}

	

	return true;
}


int onece=0;
void CProcess021::process_osd(void *pPrm)
{

	int devId=0;
	Mat frame=sThis->m_dc;
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	int winId;
	Text_Param_fb * textParam = NULL;
	Line_Param_fb * lineParam = NULL;
	Text_Param_fb * textParampri = NULL;
	Line_Param_fb * lineParampri = NULL;


	//OSA_printf(" %d:%s devId %d grplevel %d grpcolor %d\n",OSA_getCurTimeInMsec(),__func__,
	//	devId, pIStuts->DispGrp[devId], pIStuts->DispColor[devId]);
	if(sThis->m_display.m_bOsd == false)
		return ;
	//memcpy(frame.d);
//	OSA_printf(" %d:%s devId %d iFrameCnt %d\n",OSA_getCurTimeInMsec(),__func__,
//		devId, grpxChWinPrms.chParam[devId].numWindows);
	for(winId = 0; winId < grpxChWinPrms.chParam[devId].numWindows; winId++)
	{
		textParam = &grpxChWinPrms.chParam[devId].winPrms[winId];
		textParampri = &grpxChWinPrms.chParam[devId].winPrms_pri[winId];
		lineParam = (Line_Param_fb *)&grpxChWinPrms.chParam[devId].winPrms[winId];
		lineParampri = (Line_Param_fb *)&grpxChWinPrms.chParam[devId].winPrms_pri[winId];
		if(onece<ALG_LINK_GRPX_MAX_WINDOWS)
		{
			memcpy(textParampri,textParam,sizeof(Text_Param_fb));
			onece++;
		}

		if(winId==WINID_TV_FOV_CHOOSE_1/2)
		{
			//printf("textParam->enableWin=%d  objType=%d valid=%d\n",textParam->enableWin,textParam->objType,textParam->text_valid);
		}
		//printf("textParam->enableWin=%d  objType=%d valid=%d\n",textParam->enableWin,textParam->objType,textParam->text_valid);
		if(!textParam->enableWin)
			continue;
		EraseDraw_graph_osd(frame,textParam,textParampri);
		Draw_graph_osd(frame,textParam,lineParam);	
		//sThis->updata_osd[0]=true;
		memcpy(textParampri,textParam,sizeof(Text_Param_fb));
	}
	waitKey(1);
	sThis->m_display.UpDateOsd(0);
}

void CProcess021::process_osd1()
{

	int devId=0;
	//Mat frame=sThis->m_dc;//.m_imgOsd;
	Mat frame=sThis->m_dccv;//.m_imgOsd;
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	int winId;
	Text_Param_fb * textParam = NULL;
	Line_Param_fb * lineParam = NULL;
	Text_Param_fb * textParampri = NULL;
	Line_Param_fb * lineParampri = NULL;
	
#if 0
	static int iFrameCnt=0;
	iFrameCnt++;
	OSA_printf(" %d:%s devId %d iFrameCnt %d\n",OSA_getCurTimeInMsec(),__func__,
		devId, iFrameCnt);
#endif

	//OSA_printf(" %d:%s devId %d grplevel %d grpcolor %d\n",OSA_getCurTimeInMsec(),__func__,
	//	devId, pIStuts->DispGrp[devId], pIStuts->DispColor[devId]);
	if(sThis->m_display.m_bOsd == false)
		return ;
	//memcpy(frame.d);
//	OSA_printf(" %d:%s devId %d iFrameCnt %d\n",OSA_getCurTimeInMsec(),__func__,
//		devId, grpxChWinPrms.chParam[devId].numWindows);
	for(winId = 0; winId < grpxChWinPrms.chParam[devId].numWindows; winId++)
	{
		textParam = &grpxChWinPrms.chParam[devId].winPrms[winId];
		textParampri = &grpxChWinPrms.chParam[devId].winPrms_pri[winId];
		lineParam = (Line_Param_fb *)&grpxChWinPrms.chParam[devId].winPrms[winId];
		lineParampri = (Line_Param_fb *)&grpxChWinPrms.chParam[devId].winPrms_pri[winId];
		if(onece<ALG_LINK_GRPX_MAX_WINDOWS)
		{
			memcpy(textParampri,textParam,sizeof(Text_Param_fb));
			onece++;
		}
		

		if(winId==WINID_TV_FOV_CHOOSE_1/2)
			{
				//printf("textParam->enableWin=%d  objType=%d valid=%d\n",textParam->enableWin,textParam->objType,textParam->text_valid);
			}
		//printf("textParam->enableWin=%d  objType=%d valid=%d\n",textParam->enableWin,textParam->objType,textParam->text_valid);
		if(!textParam->enableWin)
			continue;
		EraseDraw_graph_osd(frame,textParam,textParampri);
		Draw_graph_osd(frame,textParam,lineParam);
		
		//sThis->updata_osd[0]=true;
		
		memcpy(textParampri,textParam,sizeof(Text_Param_fb));

	}
	
	//sThis->m_display.UpDateOsd(0);
	
	



}



void CProcess021::osd_mtd_show(TARGET tg[], bool bShow)
{
	int i;
	
	int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
	unsigned char Alpha = (bShow) ? frcolor : 0;
	CvScalar colour=GetcvColour(Alpha);

	for(i=0;i<MAX_TARGET_NUMBER;i++)
	{
		if(tg[i].valid)
		{
			cv::Rect result;
			result.width = 32;
			result.height = 32;
			result.x = ((int)tg[i].cur_x) % _IMAGE_WIDTH_;
			result.y = ((int)tg[i].cur_y ) % _IMAGE_HEIGHT_;
			result.x = result.x - result.width/2;
			result.y = result.y - result.height/2;
			rectangle( m_dccv,
				Point( result.x, result.y ),
				Point( result.x+result.width, result.y+result.height),
				colour, 1, 8);
		}
	}
}

void CProcess021::DrawBlob(BlobRect blobRct,  bool bShow /*= true*/)
{
	int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
	unsigned char Alpha = (bShow) ? frcolor : 0;
	CvScalar colour=GetcvColour(Alpha);
	int startx=0;
	int starty=0;
	int endx=0;
	int endy=0;
	int sensor=0;

	if(bShow)
		{
			sensor=extInCtrl.SensorStat;

		}
	else
		{
			sensor=prisensorstatus;

		}

	if(blobRct.bvalid){
//		RotatedRect box = RotatedRect( blobRct.center, blobRct.size, blobRct.angle);
//		ellipse(m_dc, box, cvScalar(0,0,255, Alpha), 4, CV_AA);
	//	ellipse(m_dc, blobRct.center, blobRct.size*0.5f, blobRct.angle, 0, 360, colour, 4/*, CV_AA*/);

		Point pt1,pt2,center;
		center.x = blobRct.center.x;
		center.y = blobRct.center.y;

	 	startx=PiexltoWindowsx(center.x,sensor);
	 	starty=PiexltoWindowsy(center.y,sensor);
	 	//endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
	  	//endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);
		pt1.x=startx-10;pt1.y=starty;
		pt2.x=startx+10;pt2.y=starty;
		line(m_dccv, pt1, pt2, colour, 1, 8, 0 );
		pt1.x=startx;pt1.y=starty-10;
		pt2.x=startx;pt2.y=starty+10;
		line(m_dccv, pt1, pt2, colour, 1, 8, 0 );

		extInCtrl.TempTvCollX=center.x;
		extInCtrl.TempTvCollY=center.y;
		if(m_bBlobDetect)
		{
			if((extInCtrl.TempTvCollX==0)||(extInCtrl.TempTvCollY==0))
				;
			else					
				;//MSGAPI_AckSnd( AckTvCollErr);
		}
	}
}


void CProcess021::DrawCross(int x,int y,int fcolour ,bool bShow /*= true*/)
{
	
	unsigned char colour = (bShow) ?fcolour : 0;
	Line_Param_fb lineparm;
	lineparm.x=x;
	lineparm.y=y;
	lineparm.width=48;
	lineparm.height=48;
	lineparm.frcolor=colour;


	//Drawcvcross(m_dccv,&lineparm);
	Drawcvcrossaim(m_dccv,&lineparm);
}



int majormmtid=0;
int primajormmtid=0;

void CProcess021::erassdrawmmt(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);

			tempdata=primajormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[primajormmtid].valid)&&(i==0))
						{
							//majormmtid=i;
							result.width = 32;
							result.height = 32;
							tempmmtx=result.x = ((int)tg[primajormmtid].cur_x) % _IMAGE_WIDTH_;
							tempmmty=result.y = ((int)tg[primajormmtid].cur_y ) % _IMAGE_HEIGHT_;


							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;

							
							 startx=PiexltoWindowsx(result.x,prisensorstatus);
							 starty=PiexltoWindowsy(result.y,prisensorstatus);
							 endx=PiexltoWindowsx(result.x+result.width,prisensorstatus);
						 	 endy=PiexltoWindowsy(result.y+result.height,prisensorstatus);

							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
							
						}
						
						else if(tg[tempdata].valid)
							{
								testid++;
								result.width = 32;
								result.height = 32;
								tempmmtx=result.x = ((int)tg[tempdata].cur_x) % _IMAGE_WIDTH_;
								tempmmty=result.y = ((int)tg[tempdata].cur_y ) % _IMAGE_HEIGHT_;

								 startx=PiexltoWindowsx(result.x,prisensorstatus);
								 starty=PiexltoWindowsy(result.y,prisensorstatus);
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",(tempdata+MAX_TARGET_NUMBER-primajormmtid)%MAX_TARGET_NUMBER);
								putText(frame,numbuf,cvPoint(startx+14,starty+14),CV_FONT_HERSHEY_SIMPLEX,1,colour);
								
								
							}
				
				
						tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}


}


#if 1
void CProcess021::drawmmt(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);
			
			//memset(extInCtrl.MtdOffsetXY,0,20);
			for(i=0;i<20;i++)
				{
					extInCtrl.MtdOffsetXY[i]=0;
				}
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(tg[majormmtid].valid==0)
						{
							//majormmtid++;
							majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;

						}
					if(tg[i].valid==1)
						{
							extInCtrl.Mtdtargetnum++;
							

						}

				}

			//OSA_printf("the num detect=%d  majormmtid=%d\n",extInCtrl.Mtdtargetnum,majormmtid);
			primajormmtid=tempdata=majormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[majormmtid].valid)&&(i==0))
						{
							//majormmtid=i;
							result.width = 32;
							result.height = 32;
							tempmmtx=result.x = ((int)tg[majormmtid].cur_x) % _IMAGE_WIDTH_;
							tempmmty=result.y = ((int)tg[majormmtid].cur_y ) % _IMAGE_HEIGHT_;


							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;
							
							//OSA_printf("the num  majormmtid=%d\n",majormmtid);
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;

							 startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
							 starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
							 endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
						 	 endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);

							//OSA_printf("the AvtTrkStat=%d  DispGrp=%d\n",extInCtrl.AvtTrkStat,extInCtrl.DispGrp[extInCtrl.SensorStat]);
							if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3)&&(extInCtrl.SysMode!=9)&&(extInCtrl.SysMode!=2))
								{
							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
								}
								//OSA_printf("******************the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								extInCtrl.MtdOffsetXY[j]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3]=(tempmmty>>8)&0xff;
								//j++;

								//tempdata=(tempdata+1)%MAX_TARGET_NUMBER;
									
							//memcpy(extInCtrl.MtdOffsetXY,tempmmtx,sizeof(tempmmtx));
							//memcpy(extInCtrl.MtdOffsetXY+2,tempmmty,sizeof(tempmmty));
							
						}
						
						else if(tg[tempdata].valid)
							{
								testid++;
								result.width = 32;
								result.height = 32;
								tempmmtx=result.x = ((int)tg[tempdata].cur_x) % _IMAGE_WIDTH_;
								tempmmty=result.y = ((int)tg[tempdata].cur_y ) % _IMAGE_HEIGHT_;

								//OSA_printf("+++++++++++++++the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								//result.x = result.x - result.width/2;
								//result.y = result.y - result.height/2;
								//OSA_printf("the num  majormmtid=%d\n",tempdata);

								 startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
								 starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
									if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3)&&(extInCtrl.SysMode!=9)&&(extInCtrl.SysMode!=2))
								{
								//DrawCross(result.x,result.y,frcolor,bShow);
								//trkimgcross(frame,result.x,result.y,16);
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",(tempdata+MAX_TARGET_NUMBER-majormmtid)%MAX_TARGET_NUMBER);
								putText(frame,numbuf,cvPoint(startx+14,starty+14),CV_FONT_HERSHEY_SIMPLEX,1,colour);
								}
								//memcpy(extInCtrl.MtdOffsetXY+testid*4,tempmmtx,sizeof(tempmmtx));
								//memcpy(extInCtrl.MtdOffsetXY+2+testid*4,tempmmty,sizeof(tempmmty));
								

								extInCtrl.MtdOffsetXY[j+testid*4]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1+testid*4]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2+testid*4]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3+testid*4]=(tempmmty>>8)&0xff;
								//j++;
								
							}
				
				
						tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}

			

			if(Mmtsendtime==0)
				;//MSGAPI_AckSnd( AckMtdInfo);
			Mmtsendtime++;
			if(Mmtsendtime==1)
				{
					Mmtsendtime=0;
				}
			


}
#endif


void CProcess021::erassdrawmmtnew(TARGETDRAW tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);

			primajormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

			
						if((tg[primajormmtid].valid)&&(i==primajormmtid))
						{	
							 startx=tg[primajormmtid].startx;//PiexltoWindowsx(result.x,prisensorstatus);
							 starty=tg[primajormmtid].starty;//PiexltoWindowsy(result.y,prisensorstatus);
							 endx=tg[primajormmtid].endx;//PiexltoWindowsx(result.x+result.width,prisensorstatus);
						 	 endy=tg[primajormmtid].endy;//PiexltoWindowsy(result.y+result.height,prisensorstatus);

							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
							rectangle( frame,
								Point( startx-1, starty-1 ),
								Point( endx+1, endy+1),
								colour, 1, 8);
							
						}
						
					if((tg[i].valid)&&(i!=primajormmtid))
							{
								

								 startx=tg[i].startx;//PiexltoWindowsx(result.x,prisensorstatus);
								 starty=tg[i].starty;//PiexltoWindowsy(result.y,prisensorstatus);
								 endx=tg[i].endx;
								 endy=tg[i].endy;
								 #if 0
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								#else
								rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);

								#endif
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",i+1);
								putText(frame,numbuf,cvPoint(startx,starty-2),CV_FONT_HERSHEY_SIMPLEX,0.8,colour);
							}
				
				

					}


}


void CProcess021::drawmmtnew(TARGET tg[],bool bShow)
{
			int startx=0;
			int starty=0;
			int endx=0;
			int endy=0;
			Mat frame=m_dccv;
			int i=0,j=0;
			cv::Rect result;
			short tempmmtx=0;
			short tempmmty=0;
			int tempdata=0;
			int testid=0;
			extInCtrl.Mtdtargetnum=0;
			char numbuf[3];
			int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			unsigned char Alpha = (bShow) ? frcolor : 0;
			CvScalar colour=GetcvColour(Alpha);
			
			//memset(extInCtrl.MtdOffsetXY,0,20);
			for(i=0;i<20;i++)
				{
					extInCtrl.MtdOffsetXY[i]=0;
				}
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(tg[majormmtid].valid==0)
						{
							//majormmtid++;
							//find mmt major target;
							if(extInCtrl.MMTTempStat==3)
								majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;
							else if(extInCtrl.MMTTempStat==4)
								majormmtid=(majormmtid-1+MAX_TARGET_NUMBER)%MAX_TARGET_NUMBER;
							else
								majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;

						}
					if(tg[i].valid==1)
						{
							//valid mmt num;
							extInCtrl.Mtdtargetnum++;
							

						}
					Mdrawbak[i].valid=0;//reset

				}
			
			primajormmtid=tempdata=majormmtid;
			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

						//if(m_mtd[chId]->tg[i].valid)
						
						if((tg[majormmtid].valid)&&(i==majormmtid))
						{

							if(extInCtrl.SensorStat==0)
								{
									if(extInCtrl.FovCtrl!=5)
										{
											result.width = 32;
											result.height = 32;
										}
									else
										{
											result.width = 16;
											result.height = 16;


										}
								}
							
							else
								{
									result.width = 16;
									result.height = 16;

								}
							tempmmtx=result.x = ((int)tg[majormmtid].cur_x) % _IMAGE_WIDTH_;
							tempmmty=result.y = ((int)tg[majormmtid].cur_y ) % _IMAGE_HEIGHT_;

							
							//mmt track target set
							extInCtrl.unitMtdPixelX=result.x;
							extInCtrl.unitMtdPixelY=result.y;
							extInCtrl.unitMtdValid=1;

							
						
							result.x = result.x - result.width/2;
							result.y = result.y - result.height/2;
							
							 startx=PiexltoWindowsxzoom(result.x,extInCtrl.SensorStat);
							 starty=PiexltoWindowsyzoom(result.y,extInCtrl.SensorStat);
							 endx=PiexltoWindowsxzoom(result.x+result.width,extInCtrl.SensorStat);
						 	 endy=PiexltoWindowsyzoom(result.y+result.height,extInCtrl.SensorStat);
							 //erase param
							 Mdrawbak[i].startx=startx;
							 Mdrawbak[i].starty=starty;
							 Mdrawbak[i].endx=endx;
							 Mdrawbak[i].endy=endy;
							 Mdrawbak[i].valid=1;

							//OSA_printf("the AvtTrkStat=%d  DispGrp=%d\n",extInCtrl.AvtTrkStat,extInCtrl.DispGrp[extInCtrl.SensorStat]);
							if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3)&&(extInCtrl.SysMode!=8)&&(extInCtrl.SysMode!=2))
								{
							rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);
								Osdflag[osdindex]=1;

								rectangle( frame,
								Point( startx-1, starty-1 ),
								Point( endx+1, endy+1),
								colour, 1, 8);
								}
								//OSA_printf("******************the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								tempmmtx  =PiexltoWindowsx(tempmmtx,extInCtrl.SensorStat);
								tempmmty  =PiexltoWindowsy(tempmmty,extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3]=(tempmmty>>8)&0xff;

								//j++;

								//tempdata=(tempdata+1)%MAX_TARGET_NUMBER;
									
							//memcpy(extInCtrl.MtdOffsetXY,tempmmtx,sizeof(tempmmtx));
							//memcpy(extInCtrl.MtdOffsetXY+2,tempmmty,sizeof(tempmmty));
							
						}
						
						if((tg[i].valid)&&(i!=majormmtid))
							{
								testid++;
								if(extInCtrl.SensorStat==0)
								{
								if(extInCtrl.FovCtrl!=5)
										{
											result.width = 32;
											result.height = 32;
										}
								else
									{
											result.width = 16;
											result.height = 16;

									}
								}
								else
								{
									result.width = 16;
									result.height = 16;

								}
								
								tempmmtx=result.x = ((int)tg[i].cur_x) % _IMAGE_WIDTH_;
								tempmmty=result.y = ((int)tg[i].cur_y ) % _IMAGE_HEIGHT_;


								

								//OSA_printf("+++++++++++++++the num  majormmtid=%d x=%d y=%d w=%d h=%d\n",majormmtid,
								//	result.x,result.y,result.width,result.height);
								result.x = result.x - result.width/2;
								result.y = result.y - result.height/2;
								//OSA_printf("the num  majormmtid=%d\n",tempdata);

								 startx=PiexltoWindowsxzoom(result.x,extInCtrl.SensorStat);
								 starty=PiexltoWindowsyzoom(result.y,extInCtrl.SensorStat);
								   endx=PiexltoWindowsxzoom(result.x+result.width,extInCtrl.SensorStat);
						 	 	  endy=PiexltoWindowsyzoom(result.y+result.height,extInCtrl.SensorStat);

								 Mdrawbak[i].startx=startx;
 								 Mdrawbak[i].starty=starty;
								  Mdrawbak[i].endx=endx;
 								 Mdrawbak[i].endy=endy;
 								 Mdrawbak[i].valid=1;
									if((((extInCtrl.AvtTrkStat == eTrk_mode_mtd)||(extInCtrl.AvtTrkStat == eTrk_mode_acq)))&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3)&&(extInCtrl.SysMode!=8)&&(extInCtrl.SysMode!=2))
								{
								//DrawCross(result.x,result.y,frcolor,bShow);
								//trkimgcross(frame,result.x,result.y,16);
								#if 0
								line(frame, cvPoint(startx-16,starty), cvPoint(startx+16,starty), colour, 1, 8, 0 ); 
								line(frame, cvPoint(startx,starty-16), cvPoint(startx,starty+16), colour, 1, 8, 0 ); 
								#else
								rectangle( frame,
								Point( startx, starty ),
								Point( endx, endy),
								colour, 1, 8);


								#endif
								//OSA_printf("******************the num  majormmtid=%d\n",majormmtid);
								sprintf(numbuf,"%d",i+1);
								putText(frame,numbuf,cvPoint(startx,starty-2),CV_FONT_HERSHEY_SIMPLEX,0.8,colour);
								}
								//memcpy(extInCtrl.MtdOffsetXY+testid*4,tempmmtx,sizeof(tempmmtx));
								//memcpy(extInCtrl.MtdOffsetXY+2+testid*4,tempmmty,sizeof(tempmmty));
								

								extInCtrl.MtdOffsetXY[j+testid*4]=tempmmtx&0xff;
								extInCtrl.MtdOffsetXY[j+1+testid*4]=(tempmmtx>>8)&0xff;
								extInCtrl.MtdOffsetXY[j+2+testid*4]=tempmmty&0xff;
								extInCtrl.MtdOffsetXY[j+3+testid*4]=(tempmmty>>8)&0xff;

								extInCtrl.MtdOffsetXY[j+testid*4]    =PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+testid*4],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+1+testid*4]=PiexltoWindowsx(extInCtrl.MtdOffsetXY[j+1+testid*4],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+2+testid*4]=PiexltoWindowsy(extInCtrl.MtdOffsetXY[j+2+testid*4],extInCtrl.SensorStat);
								extInCtrl.MtdOffsetXY[j+3+testid*4]=PiexltoWindowsy(extInCtrl.MtdOffsetXY[j+3+testid*4],extInCtrl.SensorStat);
								//j++;
								
							}

						//mmt show
						tempmmtx=result.x = ((int)tg[i].cur_x) % _IMAGE_WIDTH_;
						tempmmty=result.y = ((int)tg[i].cur_y ) % _IMAGE_HEIGHT_;
						Mmtpos[i].x=tempmmtx-result.width/2;
						Mmtpos[i].y=tempmmty-result.height/2;
						Mmtpos[i].w=result.width;
						Mmtpos[i].h=result.height;
						Mmtpos[i].valid=tg[i].valid;
				
				
					//	tempdata=(tempdata+1)%MAX_TARGET_NUMBER;

					}

			

			if(Mmtsendtime==0)
				;//MSGAPI_AckSnd( AckMtdInfo);
			Mmtsendtime++;
			if(Mmtsendtime==1)
				{
					Mmtsendtime=0;
				}
			
			msgdriv_event(MSGID_EXT_INPUT_MMTSHOWUPDATE, NULL);

}



void CProcess021::DrawMeanuCross(int lenx,int leny,int fcolour , bool bShow ,int centerx,int centery)
{
	int templenx=lenx;
	int templeny=leny;
	int lenw=35;
	unsigned char colour = (bShow) ?fcolour : 0;
	//int centerx=vdisWH[0][0]/2;
	//int centery=vdisWH[0][1]/2;
	Osd_cvPoint start;
	Osd_cvPoint end;

	////v
	start.x=centerx-templenx;
	start.y=centery-templeny;
	end.x=centerx-templenx+lenw;
	end.y=centery-templeny;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	start.x=centerx+templenx-lenw;
	start.y=centery-templeny;
	end.x=centerx+templenx;
	end.y=centery-templeny;
	DrawcvLine(m_dccv,&start,&end,colour,1);


	start.x=centerx-templenx;
	start.y=centery+templeny;
	end.x=centerx-templenx+lenw;
	end.y=centery+templeny;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	start.x=centerx+templenx-lenw;
	start.y=centery+templeny;
	end.x=centerx+templenx;
	end.y=centery+templeny;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	//h
	start.x=centerx-templenx;
	start.y=centery-templeny;
	end.x=centerx-templenx;
	end.y=centery-templeny+lenw;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	start.x=centerx+templenx;
	start.y=centery-templeny;
	end.x=centerx+templenx;
	end.y=centery-templeny+lenw;
	DrawcvLine(m_dccv,&start,&end,colour,1);


	start.x=centerx-templenx;
	start.y=centery+templeny-lenw;
	end.x=centerx-templenx;
	end.y=centery+templeny;
	DrawcvLine(m_dccv,&start,&end,colour,1);

	start.x=centerx+templenx;
	start.y=centery+templeny-lenw;
	end.x=centerx+templenx;
	end.y=centery+templeny;
	DrawcvLine(m_dccv,&start,&end,colour,1);



	


}

void CProcess021::DrawdashCross(int x,int y,int fcolour ,bool bShow /*= true*/)
{

	int startx=0;
	int starty=0;
	int endx=0;
	int endy=0;
	unsigned char colour = (bShow) ?fcolour : 0;
	Line_Param_fb lineparm;

	startx=WindowstoPiexlx(extInCtrl.ImgPixelX[extInCtrl.SensorStat],extInCtrl.SensorStat);
	starty=WindowstoPiexly(extInCtrl.ImgPixelY[extInCtrl.SensorStat],extInCtrl.SensorStat);
	
	lineparm.x=startx;//extInCtrl.ImgPixelX;
	lineparm.y=starty;//extInCtrl.ImgPixelY;
	lineparm.width=50;
	lineparm.height=50;
	lineparm.frcolor=colour;

	int dashlen=2;

	Point start,end;

	// startx=PiexltoWindowsx(result.x,extInCtrl.SensorStat);
	// starty=PiexltoWindowsy(result.y,extInCtrl.SensorStat);
	// endx=PiexltoWindowsx(result.x+result.width,extInCtrl.SensorStat);
 	// endy=PiexltoWindowsy(result.y+result.height,extInCtrl.SensorStat);
	
	//Drawcvcross(m_dc,&lineparm);
	if(!bShow)
		{
				//startx=PiexltoWindowsx(secBak[1].x,extInCtrl.SensorStat);
				//starty=PiexltoWindowsy(secBak[1].y,extInCtrl.SensorStat);
				lineparm.x=secBak[1].x;
				lineparm.y=secBak[1].y;
				DrawcvDashcross(m_dccv,&lineparm,dashlen,dashlen);
				startx=secBak[0].x;//PiexltoWindowsx(secBak[0].x,extInCtrl.SensorStat);
				starty=secBak[0].y;//PiexltoWindowsy(secBak[0].y,extInCtrl.SensorStat);
				endx=secBak[1].x;//PiexltoWindowsx(secBak[1].x,extInCtrl.SensorStat);
 				endy=secBak[1].y;//PiexltoWindowsy(secBak[1].y,extInCtrl.SensorStat);
				
				drawdashlinepri(m_dccv,startx,starty,endx,endy,dashlen,dashlen,colour);
		}

	else if((extInCtrl.SecAcqFlag)&&(extInCtrl.DispGrp[extInCtrl.SensorStat]<3))
		{
				//printf("ImgPixelX=%d,ImgPixelY=%d  extInCtrl.DispGrp[extInCtrl.SensorStat]=%d \n",extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,extInCtrl.DispGrp[extInCtrl.SensorStat]);
				DrawcvDashcross(m_dccv,&lineparm,dashlen,dashlen);
				startx=PiexltoWindowsxzoom(extInCtrl.unitAxisX[extInCtrl.SensorStat ],extInCtrl.SensorStat);
				starty=PiexltoWindowsyzoom(extInCtrl.unitAxisY[extInCtrl.SensorStat ],extInCtrl.SensorStat);
				endx=lineparm.x;//PiexltoWindowsx(extInCtrl.ImgPixelX[extInCtrl.SensorStat],extInCtrl.SensorStat);
 				endy=lineparm.y;//PiexltoWindowsy(extInCtrl.ImgPixelY[extInCtrl.SensorStat],extInCtrl.SensorStat);
				
				drawdashlinepri(m_dccv,startx,starty,endx,endy,dashlen,dashlen,colour);

				secBak[0].x=startx;
				secBak[0].y=starty;
				secBak[1].x=endx;
				secBak[1].y=endy;
				
				Osdflag[osdindex]=1;
				
		      	
				//DrawcvDashliner(m_dc,extInCtrl.unitAxisX,extInCtrl.unitAxisX,extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,4,4,colour);
				//drawdashcross(frame,extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,50,4,4);
				//drawdashline(frame,extInCtrl.unitAxisX,extInCtrl.unitAxisX,extInCtrl.ImgPixelX,extInCtrl.ImgPixelY,4,4);
				
		}
}


void CProcess021::DrawdashRect(int startx,int starty,int endx,int endy,int colour)
{

	int dashlen=3;
	drawdashlinepri(m_dccv,startx,starty,endx,starty,dashlen,dashlen,colour);
	drawdashlinepri(m_dccv,startx,endy,endx,endy,dashlen,dashlen,colour);
	drawdashlinepri(m_dccv,endx,starty,endx,endy,dashlen,dashlen,colour);
	drawdashlinepri(m_dccv,startx,starty,startx,endy,dashlen,dashlen,colour);

}

bool CProcess021::OnProcess(int chId, Mat &frame)
{
	//track
	int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
	int startx=0;
	int starty=0;
	int endx=0;
	int endy=0;
	int crossshiftx=cvRound(vdisWH[0][0]/3);
	int crossshifty=cvRound(vdisWH[0][1]/3);

	int coastRectx,coastRecty;
	
	static int coastCnt = 1;
	static int bDraw = 0;

	static int changesensorCnt = 0;
	//OSA_printf("extInCtrl.changeSensorFlag = %d\n",extInCtrl.changeSensorFlag);
	if(extInCtrl.changeSensorFlag == 1)
		++changesensorCnt;
	if(changesensorCnt == 3){
		extInCtrl.changeSensorFlag =  0;
		changesensorCnt = 0;
	}
	
	if(((++coastCnt)%10) == 0)
	{
		bDraw = !bDraw;
		
		coastCnt = 0;
	}
	
	CvScalar colour=GetcvColour(frcolor);
	static unsigned int countnofresh = 0;
	if((countnofresh ) >= 5)
	{
		countnofresh=0;
		OSA_mutexLock(&osd_mutex);
		process_osd1();
		OSA_mutexUnlock(&osd_mutex);
	}
	countnofresh++;

	osdindex=0;
		//picp cross
	{

			startx=crosspicpBak.x;//PiexltoWindowsx(crossBak.x,extInCtrl.SensorStat);
	 		starty=crosspicpBak.y;//PiexltoWindowsy(crossBak.y,extInCtrl.SensorStat);
	 		if(Osdflag[osdindex]==1)
	 			{
					DrawCross(startx,starty,frcolor,false);
					Osdflag[osdindex]=0;
	 			}
			startx=PiexltoWindowsx(extInCtrl.SensorStat?extInCtrl.AvtPosXTv:extInCtrl.AvtPosXFir,1-extInCtrl.SensorStat);
	 		starty=PiexltoWindowsy(extInCtrl.SensorStat?extInCtrl.AvtPosYTv:extInCtrl.AvtPosYFir,1-extInCtrl.SensorStat);
			//printf("pri the startx=%d  starty=%d\n ",startx,starty);
			switch(extInCtrl.PicpPosStat)
				{
					case 0:
						startx+=crossshiftx;
						starty-=crossshifty;
						break;
					case 1:
						startx+=crossshiftx;
						starty+=crossshifty;
						break;
					case 2:
						startx-=crossshiftx;
						starty+=crossshifty;
						break;
					case 3:
						startx-=crossshiftx;
						starty-=crossshifty;
						break;

					default:
						break;
				}
			
			if(startx<0)
				{
					startx=0;
				}
			else if(startx>vdisWH[0][0])
				{
					startx=0;
				}
			if(starty<0)
				{
					starty=0;
				}
			else if(starty>vdisWH[0][0])
				{
					starty=0;
				}
			if(((extInCtrl.PicpSensorStat==1)||(extInCtrl.PicpSensorStat==0))&&(extInCtrl.FrCollimation!=1)&&	(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3))
				{
					DrawCross(startx,starty,frcolor,true);
					//printf("picp***********lat the startx=%d  starty=%d\n ",startx,starty);
					Osdflag[osdindex]=1;
				}
			crosspicpBak.x=startx;
			crosspicpBak.y=starty;

	}
osdindex++;
	{
		 UTC_RECT_float rcResult = m_rcTrack;
		 UTC_RECT_float rcResult_algRect = m_rcTrack;
		 
		 trackinfo_obj->trackrect=m_rcTrack;
		 trackinfo_obj->TrkStat = extInCtrl.AvtTrkStat;
		 m_SensorStat = extInCtrl.SensorStat;
		 int aimw= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][0];
		 int aimh= trkWinWH[extInCtrl.SensorStat][extInCtrl.AvtTrkAimSize][1];
		 if((extInCtrl.FovCtrl==5)&&(extInCtrl.SensorStat==0))
			{
				aimw=aimw/2;
				aimh=aimh/2;
		 	}

		 #if 0
		 startx=rcTrackBak.x;//PiexltoWindowsx(rcTrackBak.x,extInCtrl.SensorStat);
		 starty=rcTrackBak.y;//PiexltoWindowsy(rcTrackBak.y,extInCtrl.SensorStat);
		 endx=rcTrackBak.x+rcTrackBak.width;//PiexltoWindowsx(rcTrackBak.x+rcTrackBak.width,extInCtrl.SensorStat);
		 endy=rcTrackBak.y+rcTrackBak.height;//PiexltoWindowsy(rcTrackBak.y+rcTrackBak.height,extInCtrl.SensorStat);
		rectangle( m_dccv,
			Point( startx, starty ),
			Point( endx, endy),
			cvScalar(0,0,0, 0), 1, 8 );
		
		 if((m_bTrack)&&(extInCtrl.TrkBomenCtrl==1))
		 {
			 startx=PiexltoWindowsx(rcResult.x,extInCtrl.SensorStat);
			 starty=PiexltoWindowsy(rcResult.y,extInCtrl.SensorStat);
			 endx=PiexltoWindowsx(rcResult.x+rcResult.width,extInCtrl.SensorStat);
		 	 endy=PiexltoWindowsy(rcResult.y+rcResult.height,extInCtrl.SensorStat);

			// printf("the x=%d y=%d w=%f h=%f\n",startx,starty,rcResult.width,rcResult.height);
		 	
			if( m_iTrackStat == 1)
				rectangle( m_dccv,
					Point( startx, starty ),
					Point( endx, endy),
					colour, 1, 8 );
			else
				rectangle( m_dccv,
				Point( startx, starty ),
				Point( endx, endy),
				cvScalar(0,255,0, 255), 1, 8 );
			//rcTrackBak = rcResult;
			rcTrackBak.x=startx;
			rcTrackBak.y=starty;
			rcTrackBak.width=endx-startx;
			rcTrackBak.height=endy-starty;
			extInCtrl.unitAimX=rcResult.x+rcResult.width/2;
			extInCtrl.unitAimY=rcResult.y+rcResult.height/2;
		 }
		  if(m_bTrack)
		 	{
		 	extInCtrl.unitTrkStat=m_iTrackStat;
			if(m_iTrackStat == 1)
				{
					rememflag=false;
				}
			else if(m_iTrackStat == 2)
				{
					if(!rememflag)
						{
							rememflag=true;
							rememtime=OSA_getCurTimeInMsec();
						}
					
					if((OSA_getCurTimeInMsec()-rememtime)>5000)
						{							
							extInCtrl.unitTrkStat=3;
						}
					else
						{

							extInCtrl.unitTrkStat=2;
						}
				}
		 	 if((extInCtrl.unitTrkStat == 1)||(extInCtrl.unitTrkStat == 2))
		 	 	{
		 	 		//rememflag=false;
					extInCtrl.trkerrx=extInCtrl.unitTrkX =rcResult.x+rcResult.width/2;
					extInCtrl.trkerry=extInCtrl.unitTrkY = rcResult.y+rcResult.height/2;
					MSGAPI_AckSnd( AckTrkErr);
		 	 	}
		if(extInCtrl.unitTrkStat!=extInCtrl.unitTrkStatpri)
		{
			extInCtrl.unitTrkStatpri=extInCtrl.unitTrkStat;
			MSGAPI_AckSnd( AckTrkType);
		}

		 	}
		 else
		 	{
				rememflag=false;

		 	}
		 
		// printf("rcResult.x =%f rcResult.y=%f w=%f h=%f\n",rcResult.x,rcResult.y,rcResult.width,rcResult.height);
		 #else

		 
		if(Osdflag[osdindex]==1)
 		{
			rectangle( m_dccv,
				Point( rcTrackBak.x, rcTrackBak.y ),
				Point( rcTrackBak.x+rcTrackBak.width, rcTrackBak.y+rcTrackBak.height),
				cvScalar(0,0,0, 0), 1, 8 );
			#if 1//dft alg reply x y w h
			if(algOsdRect == true)
				rectangle( m_dccv,
					Point( resultTrackBak.x, resultTrackBak.y ),
					Point( resultTrackBak.x+resultTrackBak.width, resultTrackBak.y+resultTrackBak.height),
					cvScalar(0,0,0,0), 1, 8 );
			#endif			
			Osdflag[osdindex]=0;
		}
		 if((m_bTrack)&&(extInCtrl.TrkBomenCtrl==1))
		 {

			//for display  20180419
			extInCtrl.unitTrkXtmp =rcResult.x+rcResult.width/2;
			extInCtrl.unitTrkYtmp = rcResult.y+rcResult.height/2;
			coastRectx = extInCtrl.unitAxisX[0];
			coastRecty = extInCtrl.unitAxisY[0];
			if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0){
				if(extInCtrl.unitTrkXtmp>=vdisWH[0][0]/2)
					extInCtrl.unitTrkXtmp = (extInCtrl.unitTrkXtmp-vdisWH[0][0]/2)*2+vdisWH[0][0]/2 ;
				else if(extInCtrl.unitTrkXtmp<vdisWH[0][0]/2)
					extInCtrl.unitTrkXtmp = vdisWH[0][0]/2 -(vdisWH[0][0]/2 - extInCtrl.unitTrkXtmp)*2 ;

				if(extInCtrl.unitTrkYtmp>=vdisWH[0][1]/2)
					extInCtrl.unitTrkYtmp = (extInCtrl.unitTrkYtmp-vdisWH[0][1]/2)*2+vdisWH[0][1]/2;
				else if(extInCtrl.unitTrkYtmp<vdisWH[0][1]/2)
					extInCtrl.unitTrkYtmp = vdisWH[0][1]/2 -(vdisWH[0][1]/2 - extInCtrl.unitTrkYtmp)*2;

				startx=PiexltoWindowsxzoom_TrkRect(extInCtrl.unitTrkXtmp-aimw,extInCtrl.SensorStat);			
				starty=PiexltoWindowsyzoom_TrkRect(extInCtrl.unitTrkYtmp-aimh ,extInCtrl.SensorStat);
				endx=PiexltoWindowsxzoom_TrkRect(extInCtrl.unitTrkXtmp+aimw,extInCtrl.SensorStat);
				endy=PiexltoWindowsyzoom_TrkRect(extInCtrl.unitTrkYtmp+aimh ,extInCtrl.SensorStat);
			}
			else{

				 startx=PiexltoWindowsxzoom_TrkRect(rcResult.x+rcResult.width/2-aimw/2,extInCtrl.SensorStat);			
				 starty=PiexltoWindowsyzoom_TrkRect(rcResult.y+rcResult.height/2-aimh/2 ,extInCtrl.SensorStat);
				 endx=PiexltoWindowsxzoom_TrkRect(rcResult.x+rcResult.width/2+aimw/2,extInCtrl.SensorStat);
			 	 endy=PiexltoWindowsyzoom_TrkRect(rcResult.y+rcResult.height/2+aimh/2 ,extInCtrl.SensorStat);
			}
			 //OSA_printf("startxy=(%d,%d) endXY=(%d,%d)resultXY(%f,%f)\n",startx,starty,endx,endy,rcResult.x+rcResult.width/2-aimw/2,rcResult.y+rcResult.height/2-aimh/2);
			#if 1//dft alg reply x y w h			
			if(algOsdRect == true)
			{
				rcResult_algRect.x = PiexltoWindowsx(rcResult_algRect.x,extInCtrl.SensorStat);
				rcResult_algRect.y = PiexltoWindowsy(rcResult_algRect.y,extInCtrl.SensorStat);
				rcResult_algRect.width = PiexltoWindowsx(rcResult_algRect.width,extInCtrl.SensorStat);
				rcResult_algRect.height = PiexltoWindowsy(rcResult_algRect.height,extInCtrl.SensorStat);
			}
			#endif
			if( m_iTrackStat == 1)
			{
				rectangle( m_dccv,
					Point( startx, starty ),
					Point( endx, endy),
					colour, 1, 8 );
				#if 1 //dft alg reply x y w h
				if(algOsdRect == true)
					rectangle( m_dccv,
						Point( rcResult_algRect.x, rcResult_algRect.y ),
						Point( rcResult_algRect.x+rcResult_algRect.width, rcResult_algRect.y+rcResult_algRect.height),
						cvScalar(0,255,0,255), 1, 8 );
				#endif
			}
			else
			{


				//for display  20180419
				if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0){
					
					startx=PiexltoWindowsxzoom_TrkRect(coastRectx-aimw,extInCtrl.SensorStat);			
					starty=PiexltoWindowsyzoom_TrkRect(coastRecty-aimh ,extInCtrl.SensorStat);
					endx=PiexltoWindowsxzoom_TrkRect(coastRectx+aimw,extInCtrl.SensorStat);
					endy=PiexltoWindowsyzoom_TrkRect(coastRecty+aimh ,extInCtrl.SensorStat);

				}else{
					startx=PiexltoWindowsxzoom(extInCtrl.unitAxisX[extInCtrl.SensorStat]-aimw/2,extInCtrl.SensorStat);			
					starty=PiexltoWindowsyzoom(extInCtrl.unitAxisY[extInCtrl.SensorStat]-aimh/2 ,extInCtrl.SensorStat);
					endx=PiexltoWindowsxzoom(extInCtrl.unitAxisX[extInCtrl.SensorStat]+aimw/2,extInCtrl.SensorStat);
					endy=PiexltoWindowsyzoom(extInCtrl.unitAxisY[extInCtrl.SensorStat]+aimh/2 ,extInCtrl.SensorStat);
					}
				if(bDraw != 0){
					#if 0
					rectangle( m_dccv,
						Point( startx, starty),
						Point( endx, endy),
						colour, 1, 8 );
					#else
					DrawdashRect(startx,starty,endx,endy,frcolor);
					#endif
					
				}
				#if 1 //dft alg reply x y w h
				if(algOsdRect == true)
					rectangle( m_dccv,
						Point( rcResult_algRect.x, rcResult_algRect.y ),
						Point( rcResult_algRect.x+rcResult_algRect.width, rcResult_algRect.y+rcResult_algRect.height),
						cvScalar(0,255,0,255), 1, 8 );		
				#endif
			}

			Osdflag[osdindex]=1;
			rcTrackBak = rcResult;
			rcTrackBak.x=startx;
			rcTrackBak.y=starty;
			rcTrackBak.width=endx-startx;
			rcTrackBak.height=endy-starty;

			#if 1 //dft alg reply x y w h
			if(algOsdRect == true)
			{
				resultTrackBak = rcResult_algRect;
				resultTrackBak.x = rcResult_algRect.x;
				resultTrackBak.y = rcResult_algRect.y;
				resultTrackBak.width = rcResult_algRect.width;
				resultTrackBak.height = rcResult_algRect.height;
			}
			#endif
			
			extInCtrl.unitAimX=rcResult.x+rcResult.width/2;
			extInCtrl.unitAimY=rcResult.y+rcResult.height/2;
			extInCtrl.unitAimW=rcResult.width;
			extInCtrl.unitAimH=rcResult.height;
			//printf("the errx=%d error y=%d\n",extInCtrl.unitAimX,extInCtrl.unitAimY);
		 }
		 
		 if(m_bTrack)
		 	{
		 	extInCtrl.unitTrkStat=m_iTrackStat;
			if(m_iTrackStat == 1)
				{
					rememflag=false;
				}
			else if(m_iTrackStat == 2)
				{
					if(!rememflag)
						{
							rememflag=true;
							rememtime=OSA_getCurTimeInMsec();
						}
					
					if((OSA_getCurTimeInMsec()-rememtime)>5000)
						{							
							extInCtrl.unitTrkStat=3;
						}
					else
						{

							//printf("rcResult.xy =(%f,%f)   wh=(%f,%f)\n",rcResult.x,rcResult.y,rcResult.width,rcResult.height);
							extInCtrl.unitTrkStat=2;
						}
				}
		 	 if((extInCtrl.unitTrkStat == 1)||(extInCtrl.unitTrkStat == 2))
		 	 {


		 	 		//rememflag=false;
		 	 		
					extInCtrl.unitTrkX =rcResult.x+rcResult.width/2;
					extInCtrl.unitTrkY = rcResult.y+rcResult.height/2;
					////for replay  20180419
					if(extInCtrl.FovCtrl==5&&extInCtrl.SensorStat==0){
						if(extInCtrl.unitTrkX>=vdisWH[0][0]/2)
							extInCtrl.unitTrkX = (extInCtrl.unitTrkX-vdisWH[0][0]/2)*2+vdisWH[0][0]/2 ;
						else if(extInCtrl.unitTrkX<vdisWH[0][0]/2)
							extInCtrl.unitTrkX = vdisWH[0][0]/2 -(vdisWH[0][0]/2 - extInCtrl.unitTrkX)*2 ;

						if(extInCtrl.unitTrkY>=vdisWH[0][1]/2)
							extInCtrl.unitTrkY = (extInCtrl.unitTrkY-vdisWH[0][1]/2)*2+vdisWH[0][1]/2;
						else if(extInCtrl.unitTrkY<vdisWH[0][1]/2)
							extInCtrl.unitTrkY = vdisWH[0][1]/2 -(vdisWH[0][1]/2 - extInCtrl.unitTrkY)*2;
					}
					
					//OSA_printf("transferbefore ********* trkxy(%f,%f)\n",extInCtrl.unitTrkX,extInCtrl.unitTrkY);
					extInCtrl.trkerrx=(PiexltoWindowsxf(extInCtrl.unitTrkX ,extInCtrl.SensorStat))*10;
					extInCtrl.trkerry=(PiexltoWindowsyf(extInCtrl.unitTrkY ,extInCtrl.SensorStat))*10;
					//OSA_printf("transferafter ********* trkxy(%d,%d)\n",extInCtrl.trkerrx,extInCtrl.trkerry);
					if(extInCtrl.unitTrkStat == 2)
					{
							extInCtrl.trkerrx=(PiexltoWindowsx(m_ImageAxisx ,extInCtrl.SensorStat))*10;
							extInCtrl.trkerry=(PiexltoWindowsy(m_ImageAxisy ,extInCtrl.SensorStat))*10;
	
					}

					//MSGAPI_AckSnd( AckTrkErr);
					extInCtrl.TrkErrFeedback = 1;
		 	 }
			 else
			 	extInCtrl.TrkErrFeedback = 0;
			 
			if(extInCtrl.unitTrkStat!=extInCtrl.unitTrkStatpri)
			{
				extInCtrl.unitTrkStatpri=extInCtrl.unitTrkStat;
				//MSGAPI_AckSnd( AckTrkType);
			}

		 }
		 else
	 	{
			rememflag=false;
			extInCtrl.TrkErrFeedback = 0;
	 	}



		 #endif
	}


	
	//mtd
osdindex++;
	{
		//osd_mtd_show(tgBak, false);
		if(Osdflag[osdindex]==1)
	 			{
					erassdrawmmtnew(Mdrawbak, false);
					Osdflag[osdindex]=0;
				}
		 if(m_bMtd){
			//osd_mtd_show(m_mtd[chId]->tg, true);
			drawmmtnew(m_mtd[chId]->tg, true);
			//memcpy(tgBak, m_mtd[chId]->tg, sizeof(TARGET)*MAX_TARGET_NUMBER);
			
		 }

		// drawmmt(chId,false);
	}

osdindex++;
	// blob detect
	{
	if(Osdflag[osdindex]==1)
	 			{
					DrawBlob(blob_rectBak, false);
					Osdflag[osdindex]=0;
				}
		if(m_bBlobDetect&&(extInCtrl.SensorStat==0)){
			DrawBlob(m_blobRect, true);
			memcpy(&blob_rectBak, &m_blobRect, sizeof(BlobRect));
			Osdflag[osdindex]=1;
		}
	}
osdindex++;
//return true;

	{
		if(Osdflag[osdindex]==1)
			{
				DrawdashCross(0,0,frcolor,false);
				Osdflag[osdindex]=0;
			}
		DrawdashCross(0,0,frcolor,true);
		
	}
osdindex++;
	
	//cross aim
	{
		startx=crossBak.x;//PiexltoWindowsx(crossBak.x,extInCtrl.SensorStat);
	 	starty=crossBak.y;//PiexltoWindowsy(crossBak.y,extInCtrl.SensorStat);
	 	if(Osdflag[osdindex]==1)
			{
				DrawCross(startx,starty,frcolor,false);
				Osdflag[osdindex]=0;
	 		}
	

			startx=PiexltoWindowsx(extInCtrl.unitAxisX[extInCtrl.SensorStat],extInCtrl.SensorStat);
		 	starty=PiexltoWindowsy(extInCtrl.unitAxisY[extInCtrl.SensorStat ],extInCtrl.SensorStat);
			//OSA_printf("unitAxisX=%d  unitAxisY=%d sensor=%d  prix=%d  prix=%d\n",startx,starty,extInCtrl.SensorStat,extInCtrl.unitAxisX[extInCtrl.SensorStat],extInCtrl.unitAxisY[extInCtrl.SensorStat ]);
		
		//OSA_printf("unitAxisX=%d  unitAxisY=%d sensor=%d  prix=%d  prix=%d\n",startx,starty,extInCtrl.SensorStat,extInCtrl.unitAxisX[extInCtrl.SensorStat],extInCtrl.unitAxisY[extInCtrl.SensorStat ]);
		if(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3)
			{
				DrawCross(startx,starty,frcolor,true);
				Osdflag[osdindex]=1;
			}


			crossBak.x=PiexltoWindowsx(extInCtrl.unitAxisX[extInCtrl.SensorStat ],extInCtrl.SensorStat);
			crossBak.y=PiexltoWindowsy(extInCtrl.unitAxisY[extInCtrl.SensorStat ],extInCtrl.SensorStat);

		//OSA_printf("unitAxisX=%d  unitAxisY=%d\n",extInCtrl.unitAxisX,extInCtrl.unitAxisY);
		//memcpy(&crossBak, &m_blobRect, sizeof(BlobRect));
	}


///jiaozhou

osdindex++;
	

	{
		if(extInCtrl.FrCollimation==1)
			{
				
				//CFGID_FIELD_GET(tvcorx ,CFGID_TRACK_TV_AXIX);
				//CFGID_FIELD_GET(tvcory ,CFGID_TRACK_TV_AXIY);
				tvcorx=extInCtrl.CollPosXFir;
				tvcory=extInCtrl.CollPosYFir;
				//printf("fcorpri*******the frcol is x=%d y=%d\n",tvcorx,tvcory);
				tvcorx=tvcorx-crossshiftx;
				tvcory=tvcory-crossshifty;
				if((tvcorx>vdisWH[0][0]*1/3)||(tvcorx<0))
					{
						tvcorx=cvRound(vdisWH[0][0]*1.0/6);
					}
				if((tvcory<0)||(tvcory>vdisWH[0][1]*1/3))
					{
						tvcory=cvRound(vdisWH[0][1]*1.0/6);
					}

				startx=tvcorx;
				starty=tvcory;
				//startx=PiexltoWindowsx(tvcorx,extInCtrl.SensorStat);
	 			//starty=PiexltoWindowsy(tvcory,extInCtrl.SensorStat);

				//printf("fcor*******the frcol is x=%d y=%d\n",startx,starty);
				if(Osdflag[osdindex]==1)
					{
						DrawCross(freezecrossBak.x,freezecrossBak.y,frcolor,false);
						Osdflag[osdindex]=0;
					}
				DrawCross(startx,starty,frcolor,true);
				freezecrossBak.x=startx;
				freezecrossBak.y=starty;
				
				Osdflag[osdindex]=1;
				//if((tvcorx+1280/3)
				//DrawCross(1280*5/6,1024*1/6,frcolor,true);
				
			}
		else
			{
				startx=PiexltoWindowsx(tvcorx,extInCtrl.SensorStat);
	 			starty=PiexltoWindowsy(tvcory,extInCtrl.SensorStat);
				if(Osdflag[osdindex]==1)
				{
					DrawCross(freezecrossBak.x,freezecrossBak.y,frcolor,false);
					Osdflag[osdindex]=0;
				}
				//DrawCross(1280*5/6,1024*1/6,frcolor,false);
			}




	}
///fov
	osdindex++;

	if(Osdflag[osdindex]==1)
		{
			DrawMeanuCross(rectfovBak[0].x,rectfovBak[0].y,frcolor,false,rectfovBak[1].x,rectfovBak[1].y);
			Osdflag[osdindex]=0;
		}
	if(extInCtrl.DispGrp[extInCtrl.SensorStat]<=3)
		{
			int fovw=0;
			int fovh=0;
			double ratiox=0.0;
			double ratioy=0.0;
			#if 0
			switch(extInCtrl.FovStat)
				{
				case 1:
					fovw=FOVw[extInCtrl.SensorStat][extInCtrl.FovStat-1][0]/2;
					fovh=FOVw[extInCtrl.SensorStat][extInCtrl.FovStat-1][1]/2;
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default :
					break;

				}
			#endif
			if(extInCtrl.FovStat<5)
				{
					ratiox=FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat)%5][0]/FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat-1)%5][0];
					ratioy=FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat)%5][1]/FOVw[extInCtrl.SensorStat][(extInCtrl.FovStat-1)%5][1];
					fovw=vdisWH[0][0]/2*ratiox;
					fovh=vdisWH[0][1]/2*ratioy;
					Fovpri[extInCtrl.SensorStat]=extInCtrl.FovStat;

					int centerx=vdisWH[0][0]/2;
					int centery=vdisWH[0][1]/2;
					 if(extInCtrl.ImgMmtshow[extInCtrl.SensorStat])
					 	{
							//ret =ret*2/3;//-vdisWH[0][0]/6;
							centerx=centerx*2/3;
							centery=centery*2/3;
							fovw=fovw*2/3;
							fovh=fovh*2/3;

					 	}
					DrawMeanuCross(fovw,fovh,frcolor,true,centerx,centery);
					Osdflag[osdindex]=1;
					rectfovBak[0].x=fovw;
					rectfovBak[0].y=fovh;
					rectfovBak[1].x=centerx;
					rectfovBak[1].y=centery;
				}
		}
	
	//process_osd_test(NULL);
	
	
	prisensorstatus=extInCtrl.SensorStat;

	static unsigned int count = 0;
	if((count & 1) == 1)
		OSA_semSignal(&(sThis->m_display.tskdisSemmain));
	count++;
	//sThis->m_display.UpDateOsd(1);
	return true;
}

static inline void my_rotate(GLfloat result[16], float theta)
{
	float rads = float(theta/180.0f) * CV_PI;
	const float c = cosf(rads);
	const float s = sinf(rads);

	memset(result, 0, sizeof(GLfloat)*16);

	result[0] = c;
	result[1] = -s;
	result[4] = s;
	result[5] = c;
	result[10] = 1.0f;
	result[15] = 1.0f;
}

void CProcess021::OnMouseLeftDwn(int x, int y){};
void CProcess021::OnMouseLeftUp(int x, int y){};
void CProcess021::OnMouseRightDwn(int x, int y){};
void CProcess021::OnMouseRightUp(int x, int y){};

void CProcess021::OnKeyDwn(unsigned char key)
{
	CMD_EXT *pIStuts = &extInCtrl;

	if(key == 'a' || key == 'A')
	{
		pIStuts->SensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
		msgdriv_event(MSGID_EXT_INPUT_SENSOR, NULL);
	}

	if(key == 'b' || key == 'B')
	{
		pIStuts->PicpSensorStat = (pIStuts->PicpSensorStat + 1) % (eSen_Max+1);
		msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
	}

	if(key == 'c'|| key == 'C')
	{
		if(pIStuts->AvtTrkStat)
			pIStuts->AvtTrkStat = eTrk_mode_acq;
		else
			pIStuts->AvtTrkStat = eTrk_mode_target;
		msgdriv_event(MSGID_EXT_INPUT_TRACK, NULL);
	}

	if(key == 'd'|| key == 'D')
	{
	
		if(pIStuts->ImgMtdStat[pIStuts->SensorStat])
			pIStuts->ImgMtdStat[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgMtdStat[pIStuts->SensorStat] = eImgAlg_Enable;
		msgdriv_event(MSGID_EXT_INPUT_ENMTD, NULL);
	}

	if (key == 'e' || key == 'E')
	{
		if(pIStuts->ImgEnhStat[pIStuts->SensorStat])
			pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Enable;
		msgdriv_event(MSGID_EXT_INPUT_ENENHAN, NULL);
	}

	if (key == 'o' || key == 'O')
	{
		if(pIStuts->ImgBlobDetect[pIStuts->SensorStat])
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Enable;
		msgdriv_event(MSGID_EXT_INPUT_ENBDT, NULL);
	}

	if (key == 't' || key == 'T')
		{
			if(pIStuts->ImgVideoTrans[pIStuts->SensorStat])
				pIStuts->ImgVideoTrans[pIStuts->SensorStat] = eImgAlg_Disable;
			else
				pIStuts->ImgVideoTrans[pIStuts->SensorStat] = eImgAlg_Enable;
			msgdriv_event(MSGID_EXT_INPUT_RST_THETA, NULL);
		}
	if (key == 'f' || key == 'F')
		{
			if(pIStuts->ImgFrezzStat[pIStuts->SensorStat])
				pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Disable;
			else
				pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Enable;
			
			msgdriv_event(MSGID_EXT_INPUT_ENFREZZ, NULL);
		}
	if (key == 'p'|| key == 'P')
		{
			
			pIStuts->PicpPosStat=(pIStuts->PicpPosStat+1)%4;
			msgdriv_event(MSGID_EXT_INPUT_PICPCROP, NULL);
		}
	if (key == 'g'|| key == 'G')
		{
			
			
			msgdriv_event(MSGID_EXT_INPUT_COAST, NULL);
		}
	if (key == 'z'|| key == 'Z')
		{
			
			pIStuts->ImgZoomStat[0]=(pIStuts->ImgZoomStat[0]+1)%2;
			pIStuts->ImgZoomStat[1]=(pIStuts->ImgZoomStat[1]+1)%2;
			msgdriv_event(MSGID_EXT_INPUT_ENZOOM, NULL);
		}
	if(key == 'w'|| key == 'W')
		{
			if(pIStuts->ImgMmtshow[pIStuts->SensorStat])
				pIStuts->ImgMmtshow[pIStuts->SensorStat] = eImgAlg_Disable;
			else
				pIStuts->ImgMmtshow[pIStuts->SensorStat] = eImgAlg_Enable;
			
			msgdriv_event(MSGID_EXT_INPUT_MMTSHOW, NULL);
			OSA_printf("MSGID_EXT_INPUT_MMTSHOW\n");
		}

	if (key == 'y'|| key == 'Y')
	{		
		if(moveDetectRect == true)
		{
			moveDetectRect = false;
			rectangle( m_dccv,
				Point( preAcpSR.x, preAcpSR.y ),
				Point( preAcpSR.x+preAcpSR.width, preAcpSR.y+preAcpSR.height),
				cvScalar(0,0,0,0), 1, 8 );
		}
		else
			moveDetectRect = true;
	}
	
}


void CProcess021::msgdriv_event(MSG_PROC_ID msgId, void *prm)
{
	int tempvalue=0;
	CMD_EXT *pIStuts = &extInCtrl;
	CMD_EXT *pInCmd = NULL;
//printf("*************x=%d y=%d\n",pIStuts->unitAxisX[extInCtrl.SensorStat ],pIStuts->unitAxisY[extInCtrl.SensorStat ]);
	if(msgId == MSGID_EXT_INPUT_SENSOR || msgId == MSGID_EXT_INPUT_ENPICP)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->SensorStat = pInCmd->SensorStat;
			pIStuts->PicpSensorStat = pInCmd->PicpSensorStat;
		}
		int itmp;
		//chage acq;
		m_rcAcq.x=pIStuts->unitAxisX[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
		m_rcAcq.y=pIStuts->unitAxisY[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
		m_rcAcq.width=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
		m_rcAcq.height=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];

		OSA_printf("recv   the rctrack x=%f y=%f w=%f h=%f  sensor=%d picpsensor=%d\n",m_rcAcq.x,m_rcAcq.y,m_rcAcq.width,m_rcAcq.height,pIStuts->SensorStat
			,pIStuts->PicpSensorStat);
		
		itmp = pIStuts->SensorStat;
		dynamic_config(VP_CFG_MainChId, itmp, NULL);

#if 1//change the sensor picp change too
		if(pIStuts->PicpSensorStat==0||pIStuts->PicpSensorStat==1)
			{
				if(pIStuts->SensorStat==0)
					{
						pIStuts->PicpSensorStat=1;
						
					//	pIStuts->ImgPicp[pIStuts->SensorStat^1]=0;
					//	pIStuts->ImgPicp[pIStuts->SensorStat]=1;
					}
				else
					{
						pIStuts->PicpSensorStat=0;
					//	pIStuts->ImgPicp[pIStuts->SensorStat^1]=0;
					//	pIStuts->ImgPicp[pIStuts->SensorStat]=1;
					}
				if(pIStuts->ImgPicp[pIStuts->SensorStat]==1||pIStuts->ImgPicp[pIStuts->SensorStat^1]==1)
					{
					pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat;
					pIStuts->ImgPicp[pIStuts->SensorStat^1]=0;
					pIStuts->ImgPicp[pIStuts->SensorStat]=1;
					OSA_printf("the picp sensorstatpir=%d\n",pIStuts->PicpSensorStatpri);
					}
				
				
			}
#endif

		itmp = pIStuts->PicpSensorStat;//freeze change
		dynamic_config(VP_CFG_SubChId, itmp, NULL);
////enhance 

		if(pIStuts->ImgEnhStat[pIStuts->SensorStat^1] ==0x01)
		{
			int ENHStatus=0;
			if(pIStuts->ImgEnhStat[pIStuts->SensorStat] ==0x00)
				{
					
					ENHStatus=pIStuts->ImgEnhStat[pIStuts->SensorStat]=pIStuts->ImgEnhStat[pIStuts->SensorStat^1];
					
					OSA_printf("the enhstaus=%d  pIStuts->SensorStat=%d\n",ENHStatus,pIStuts->SensorStat);
					dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat, &ENHStatus);
				}
				ENHStatus=0;
				dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat^1, &ENHStatus);
				pIStuts->ImgEnhStat[pIStuts->SensorStat^1]=0;
				
		}
		
//sec track sync
		//if( pIStuts->SecAcqStat=)
		if(pIStuts->SensorStat==0)
			{
				pIStuts->ImgPixelX[pIStuts->SensorStat] =PiexltoWindowsx( pIStuts->ImgPixelX[pIStuts->SensorStat^1],pIStuts->SensorStat^1);
				pIStuts->ImgPixelY[pIStuts->SensorStat] =PiexltoWindowsy( pIStuts->ImgPixelY[pIStuts->SensorStat^1],pIStuts->SensorStat^1);
			}
		else
			{

				pIStuts->ImgPixelX[pIStuts->SensorStat] =WindowstoPiexlx( pIStuts->ImgPixelX[pIStuts->SensorStat^1],pIStuts->SensorStat);
				pIStuts->ImgPixelY[pIStuts->SensorStat] =WindowstoPiexly( pIStuts->ImgPixelY[pIStuts->SensorStat^1],pIStuts->SensorStat);
			}


	

 			//pIStuts->ImgPixelX[pIStuts->SensorStat^1] = pIStuts->ImgPixelX[pIStuts->SensorStat];
			//pIStuts->ImgPixelY[pIStuts->SensorStat^1] = pIStuts->ImgPixelY[pIStuts->SensorStat] ;

//sensor 1 rect

		DS_Rect lay_rect;
	#if 1
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = vdisWH[0][0]/2-lay_rect.w/2;
		lay_rect.y = vdisWH[0][1]/2-lay_rect.h/2;
	#endif
		if(pIStuts->PicpSensorStat==1)
			{
			#if 1
				lay_rect.w = vcapWH[1][0]/3;
				lay_rect.h = vcapWH[1][1]/3;
				lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
				lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
			#endif

			}
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
//picp position
//18.3.31
#if 1
		lay_rect=rendpos[pIStuts->PicpPosStat];
		m_ImageAxisx=pIStuts->unitAxisX[extInCtrl.SensorStat ];
		m_ImageAxisy=pIStuts->unitAxisY[extInCtrl.SensorStat ];

		//OSA_printf("%s: lay_rect: %d, %d,  %d x %d\n", __func__, lay_rect.x, lay_rect.y, lay_rect.w, lay_rect.h);
		
		
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &lay_rect);
#endif
///sensor zoom

		if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
			{
				memset(&lay_rect, 0, sizeof(DS_Rect));
				if(pIStuts->SensorStat==0)//just tv zooom
					{
				
					lay_rect.w = vcapWH[pIStuts->SensorStat][0]/2;
					lay_rect.h = vcapWH[pIStuts->SensorStat][1]/2;
					lay_rect.x = vcapWH[pIStuts->SensorStat][0]/4;
					lay_rect.y = vcapWH[pIStuts->SensorStat][1]/4;
					

					}
				
				m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 0, &lay_rect);
				if(pIStuts->PicpSensorStat==1)
					{
						
						lay_rect.w = vcapWH[1][0]/3;
						lay_rect.h = vcapWH[1][1]/3;
						lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
					}
				if(pIStuts->PicpSensorStat==0)
					{
						
						lay_rect.w = vcapWH[0][0]/6;
						lay_rect.h = vcapWH[0][1]/6;
						lay_rect.x = vcapWH[0][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[0][1]/2-lay_rect.h/2;
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
					}


				
			}


//mmt show change
	if(pIStuts->ImgMmtshow[pIStuts->SensorStat^1]==0x01)
		{
			
			int mmtchid=0;
			int chid=pIStuts->SensorStat;
			pIStuts->ImgMmtshow[pIStuts->SensorStat^1]=0;
			pIStuts->ImgMmtshow[pIStuts->SensorStat]=1;
			itmp = chid;
			mmtchid=2;
			dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
			//chid++;
			itmp=chid;
			mmtchid=3;
			dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
			//chid++;
			itmp=chid;
			mmtchid=4;
			dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
			//chid++;
			itmp=chid;
			mmtchid=5;
			dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
			//chid++;
			itmp=chid;
			mmtchid=6;
			dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
			lay_rect.w = vdisWH[0][0]/3*2;
			lay_rect.h = vdisWH[0][1]/3*2;
			lay_rect.x = 0;
			lay_rect.y = vdisWH[0][1]/3;
			m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 0, &lay_rect);
			//m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 7, NULL);
			



		}

		
		
	
	}

	if(msgId == MSGID_EXT_INPUT_TRACK)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->AvtTrkStat = pInCmd->AvtTrkStat;
		}

		char procStr[][10] = {"ACQ", "TARGET", "MTD", "SECTRK", "SEARCH", "ROAM", "SCENE", "IMGTRK"};

		if (pIStuts->AvtTrkStat == eTrk_mode_acq)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);

			pIStuts->AvtTrkAimSize = 2;
			dynamic_config(VP_CFG_TrkEnable, 0);
			pIStuts->unitAimX = pIStuts->unitAxisX[extInCtrl.SensorStat ] ;//- pIStuts->unitAimW/2;
			if(pIStuts->unitAimX<0)
				{
					pIStuts->unitAimX=0;
				}
			
			pIStuts->unitAimY = pIStuts->unitAxisY[extInCtrl.SensorStat ];// - pIStuts->unitAimH/2;
				if(pIStuts->unitAimY<0)
				{
					pIStuts->unitAimY=0;
				}

			return ;
		}

		int iSens = (pIStuts->SensorStat+1)%eSen_Max;
		 if (pIStuts->AvtTrkStat == eTrk_mode_sectrk)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);

			pIStuts->AvtTrkStat = eTrk_mode_sectrk;
			 pIStuts->unitAimX = pIStuts->ImgPixelX[extInCtrl.SensorStat];
		  	 pIStuts->unitAimY = pIStuts->ImgPixelY[extInCtrl.SensorStat] ;
			// pIStuts->unitAxisX[extInCtrl.SensorStat ]=pIStuts->unitAimX ;
		  	// pIStuts->unitAxisY[extInCtrl.SensorStat ]=pIStuts->unitAimY ;
			//pIStuts->unitTrkProc = eTrk_proc_target;

			//return ;
		}

		else if (pIStuts->AvtTrkStat == eTrk_mode_search)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);

		   pIStuts->AvtTrkStat = eTrk_mode_search;
		   pIStuts->unitAimX = pIStuts->ImgPixelX[extInCtrl.SensorStat];
		   pIStuts->unitAimY = pIStuts->ImgPixelY[extInCtrl.SensorStat] ;
		//   pIStuts->unitAxisX[extInCtrl.SensorStat ]=pIStuts->unitAimX ;
		//   pIStuts->unitAxisY[extInCtrl.SensorStat ]=pIStuts->unitAimY ;
		   //pIStuts->unitTrkProc = eTrk_proc_target;

			//return ;
		}

		else if (pIStuts->AvtTrkStat == eTrk_mode_mtd)
		{
			OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);


			pIStuts->AvtTrkStat = eTrk_mode_target;

			//zoom for mtdTrk change xy 
			 if(pIStuts->FovCtrl==5&&pIStuts->SensorStat==0)
		 	{
		 		pIStuts->unitMtdPixelX=pIStuts->unitMtdPixelX-320;
				pIStuts->unitMtdPixelX=2*pIStuts->unitMtdPixelX;

				pIStuts->unitMtdPixelY=pIStuts->unitMtdPixelY-256;
				pIStuts->unitMtdPixelY=2*pIStuts->unitMtdPixelY;
				
				pIStuts->unitAimX=pIStuts->unitMtdPixelX;
				pIStuts->unitAimY=pIStuts->unitMtdPixelY;

		 	}else{
			
				pIStuts->unitAimX=pIStuts->unitMtdPixelX;
				pIStuts->unitAimY=pIStuts->unitMtdPixelY;
		 	}

				if(pIStuts->unitMtdValid)
			{
				tempvalue=pIStuts->unitMtdPixelX;
					//- pIStuts->unitAimW/2;
				
				if(tempvalue<0)
					{
						pIStuts->unitAimX=0;
					}
				else
					{
						pIStuts->unitAimX=tempvalue;

					}
				tempvalue=pIStuts->unitMtdPixelY ;
				//- pIStuts->unitAimH/2;
				if(tempvalue<0)
					{
						pIStuts->unitAimY=0;
					}
				else
					{
						pIStuts->unitAimY=tempvalue;

					}
				
				OSA_printf(" %d:%s set track to x =%f y=%f  mtdx=%d mtdy=%d  w=%d  h=%d\n", OSA_getCurTimeInMsec(), __func__,
						pIStuts->unitAimX,pIStuts->unitAimY, pIStuts->unitMtdPixelX,pIStuts->unitMtdPixelY,pIStuts->unitAimW/2,pIStuts->unitAimH/2);
			}
			else
			{
				pIStuts->unitAimX = pIStuts->unitAxisX[extInCtrl.SensorStat ] - pIStuts->unitAimW/2;
				pIStuts->unitAimY = pIStuts->unitAxisY[extInCtrl.SensorStat ] - pIStuts->unitAimH/2;
			}
			//pIStuts->unitTrkProc = eTrk_proc_target;

			//return ;
		}

		OSA_printf(" %d:%s set track to [%s]\n", OSA_getCurTimeInMsec(), __func__,
					   procStr[pIStuts->AvtTrkStat]);
		UTC_RECT_float rc;
		if((pIStuts->FovCtrl==5)&&(pIStuts->SensorStat==0))
		{
			if(TrkAim43 == true)
			{
				rc.x=pIStuts->unitAimX-trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/4;
				rc.y=pIStuts->unitAimY-trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/4;
				rc.width= trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
				rc.height= trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
			}
			else
			{

				if(pIStuts->unitAimX>=vdisWH[0][0]/2)
					pIStuts->unitAimX = (pIStuts->unitAimX-vdisWH[0][0]/2)/2+vdisWH[0][0]/2 ;
				else if(pIStuts->unitAimX<vdisWH[0][0]/2)
					pIStuts->unitAimX = vdisWH[0][0]/2 -(vdisWH[0][0]/2 - pIStuts->unitAimX)/2 ;

				if(pIStuts->unitAimY>=vdisWH[0][1]/2)
					pIStuts->unitAimY = (pIStuts->unitAimY-vdisWH[0][1]/2)/2+vdisWH[0][1]/2;
				else if(pIStuts->unitAimY<vdisWH[0][1]/2)
					pIStuts->unitAimY = vdisWH[0][1]/2 -(vdisWH[0][1]/2 - pIStuts->unitAimY)/2;

				
				rc.x=pIStuts->unitAimX-trkWinWHZoom[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
				rc.y=pIStuts->unitAimY-trkWinWHZoom[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;

				//zoom need
				rc.width= trkWinWHZoom[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
				rc.height= trkWinWHZoom[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
			}
		}
		else
		{
			if(TrkAim43 == true)
			{
				rc.x=pIStuts->unitAimX-trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
				rc.y=pIStuts->unitAimY-trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
				rc.width= trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
				rc.height= trkWinWH43[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
			}
			else
			{
				rc.x=pIStuts->unitAimX-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
				rc.y=pIStuts->unitAimY-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
				rc.width= trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
				rc.height= trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
			}
		}
			
		OSA_printf("rc. xy(%f,%f),wh(%f,%f)\n",rc.x,rc.y,rc.width,rc.height);
		dynamic_config(VP_CFG_TrkEnable, 1,&rc);
		if((pIStuts->AvtTrkStat == eTrk_mode_sectrk)||(pIStuts->AvtTrkStat == eTrk_mode_search))
			{
				m_intervalFrame=2;
				m_rcAcq=rc;
				pIStuts->AvtTrkStat = eTrk_mode_target;
				
				OSA_printf("***********************set sec track\n ");
				
			}
	//	printf("the rc.x=%d rc.y=%d ,unitAimX=%d  unitAimY=%d \n",rc.x,rc.y,pIStuts->unitAimX,pIStuts->unitAimY);
	//	printf("w=%d h=%d\n",pIStuts->unitAimW,pIStuts->unitAimH);
	//	printf("*************x=%d y=%d\n",pIStuts->unitAxisX[extInCtrl.SensorStat ],pIStuts->unitAxisY[extInCtrl.SensorStat ]);
		
 	}

	if(msgId == MSGID_EXT_INPUT_ENMTD)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgMtdStat[0] = pInCmd->ImgMtdStat[0];
			pIStuts->ImgMtdStat[1] = pInCmd->ImgMtdStat[1];
		}

		int MTDStatus = (pIStuts->ImgMtdStat[pIStuts->SensorStat]&0x01) ;

//		OSA_printf(" %d:%s set mtd enMask %x\n", OSA_getCurTimeInMsec(),__func__,m_mtd_ctrl.un_mtd.enMask);

		if(MTDStatus)
			dynamic_config(VP_CFG_MtdEnable, 1);
		else
			dynamic_config(VP_CFG_MtdEnable, 0);
		//FOR DUMP FRAME
		if(MTDStatus)
			dynamic_config(CDisplayer::DS_CFG_MMTEnable, pIStuts->SensorStat, &MTDStatus);
		else
			dynamic_config(CDisplayer::DS_CFG_MMTEnable, pIStuts->SensorStat, &MTDStatus);
	}

	if(msgId == MSGID_EXT_INPUT_ENENHAN)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgEnhStat[0] = pInCmd->ImgEnhStat[0];
			pIStuts->ImgEnhStat[1] = pInCmd->ImgEnhStat[1];
		}

		int ENHStatus = (pIStuts->ImgEnhStat[pIStuts->SensorStat]&0x01) ;

		OSA_printf(" %d:%s set mtd enMask %d\n", OSA_getCurTimeInMsec(),__func__,ENHStatus);

		if(ENHStatus)
			dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat, &ENHStatus);
		else
			dynamic_config(CDisplayer::DS_CFG_EnhEnable, pIStuts->SensorStat, &ENHStatus);
	}

	if(msgId == MSGID_EXT_INPUT_ENBDT)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgBlobDetect[0] = pInCmd->ImgBlobDetect[0];
			pIStuts->ImgBlobDetect[1] = pInCmd->ImgBlobDetect[1];
		}

		int BlobStatus = (pIStuts->ImgBlobDetect[pIStuts->SensorStat]&0x01) ;

		//		OSA_printf(" %d:%s set mtd enMask %x\n", OSA_getCurTimeInMsec(),__func__,m_mtd_ctrl.un_mtd.enMask);

		if(BlobStatus)
			dynamic_config(VP_CFG_BlobEnable, 1);
		else
			dynamic_config(VP_CFG_BlobEnable, 0);
	}

	if(msgId == MSGID_EXT_INPUT_RST_THETA)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
		}

		int videoTrans = (pIStuts->ImgVideoTrans[pIStuts->SensorStat]&0x01) ;

//		OSA_printf(" %d:%s set mtd enMask %x\n", OSA_getCurTimeInMsec(),__func__,m_mtd_ctrl.un_mtd.enMask);
		GLfloat result[16];

		if(videoTrans)
		{
			my_rotate(result, 45.0);
			dynamic_config(CDisplayer::DS_CFG_VideoTransMat, pIStuts->SensorStat, result);
		}
		else
		{
			my_rotate(result, 0.0);
			dynamic_config(CDisplayer::DS_CFG_VideoTransMat, pIStuts->SensorStat, result);
		}
	}


	if(msgId == MSGID_EXT_INPUT_AIMPOS || msgId == MSGID_EXT_INPUT_AIMSIZE)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->AvtTrkAimSize = pInCmd->AvtTrkAimSize;
			pIStuts->AvtMoveX = pInCmd->AvtMoveX;
			pIStuts->AvtMoveY = pInCmd->AvtMoveY;
		}

		if(pIStuts->AvtTrkAimSize<0||pIStuts->AvtTrkAimSize>4)
			pIStuts->AvtTrkAimSize=2;

		if(pIStuts->AvtTrkStat)
		{

			UTC_RECT_float rc;
			if(msgId == MSGID_EXT_INPUT_AIMSIZE)
			{
				pIStuts->unitAimW = trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
				pIStuts->unitAimH= trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];

				rc.x=pIStuts->unitAimX-pIStuts->unitAimW/2;
				rc.y=pIStuts->unitAimY-pIStuts->unitAimH/2;

				rc.width=pIStuts->unitAimW;
				rc.height=pIStuts->unitAimH;

				//OSA_printf("***xy = (%f,%f)  WH(%f,%f)\n",rc.x,rc.y,rc.width,rc.height);
			}
			else
			{
				moveStat = true;
				//printf("----- XY(%d,%d),WH(%d,%d)\n",pIStuts->unitAimX,pIStuts->unitAimY,pIStuts->unitAimW,pIStuts->unitAimH);
				if(pIStuts->unitAimW > trkWinWH[pIStuts->SensorStat][4][0])
					pIStuts->unitAimW = trkWinWH[pIStuts->SensorStat][4][0];
				if(pIStuts->unitAimH > trkWinWH[pIStuts->SensorStat][4][1])
					pIStuts->unitAimH = trkWinWH[pIStuts->SensorStat][4][1];
				
				rc.width=pIStuts->unitAimW;
				rc.height=pIStuts->unitAimH;
				rc.x=pIStuts->unitAimX-pIStuts->unitAimW/2+pIStuts->AvtMoveX;
				rc.y=pIStuts->unitAimY-pIStuts->unitAimH/2+pIStuts->AvtMoveY;
				

			}
			//dynamic_config(VP_CFG_TrkEnable, 0);
			//OSA_waitMsecs(40);
			//usleep(1);
			//dynamic_config(VP_CFG_TrkEnable, 1,&rc);
			m_intervalFrame=1;
			m_rcAcq=rc;
			OSA_printf(" %d:%s refine move (%d, %d), wh(%f, %f)  aim(%d,%d) rc(%f,%f)\n", OSA_getCurTimeInMsec(), __func__,
						pIStuts->AvtMoveX, pIStuts->AvtMoveY, 
						rc.width, rc.height,pIStuts->unitAimX,pIStuts->unitAimY,rc.x,rc.y);
		}

		return ;
	}

#if 0
	if(msgId == MSGID_EXT_INPUT_CFGSAVE)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->AvtCfgSave = pInCmd->AvtCfgSave;
		}

		OSA_printf(" %d:%s config save %d\n", OSA_getCurTimeInMsec(), __func__, 
					pIStuts->AvtCfgSave);
		if(pIStuts->AvtCfgSave == eSave_Enable){
			WriteAxisToFile();
		}
		/*
		else if(pIStuts->AvtCfgSave == eSave_Cancel){
			ReadAxisFromFile();
			// redraw cross
			m_ImageAxisx = pIStuts->AxisCalibX[pIStuts->SensorStat];
			m_ImageAxisy = pIStuts->AxisCalibY[pIStuts->SensorStat];
		}*/
		return ;
	}
#endif
	if(msgId == MSGID_EXT_INPUT_ENZOOM)
	{
		if(prm != NULL)
		{
			pInCmd = (CMD_EXT *)prm;
			pIStuts->ImgZoomStat[0] = pInCmd->ImgZoomStat[0];
			pIStuts->ImgZoomStat[1] = pInCmd->ImgZoomStat[1];
			pIStuts->DispZoomMultiple[0] = pInCmd->DispZoomMultiple[0];
			pIStuts->DispZoomMultiple[1] = pInCmd->DispZoomMultiple[1];
		}
#if 1
		DS_Rect lay_rect;
		
		if(pIStuts->SensorStat==0)//tv
			{
					memset(&lay_rect, 0, sizeof(DS_Rect));
					if(pIStuts->ImgZoomStat[0])
					{
						lay_rect.w = vdisWH[0][0]/2;
						lay_rect.h = vdisWH[0][1]/2;
						lay_rect.x = vdisWH[0][0]/4;
						lay_rect.y = vdisWH[0][1]/4;
						OSA_printf("MSGID_EXT_INPUT_ENZOOM*********tv  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					
					m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 0, &lay_rect);


					memset(&lay_rect, 0, sizeof(DS_Rect));
					#if 0
					if(pIStuts->ImgZoomStat[0]&&(pIStuts->PicpSensorStat==1))
					{
						
						lay_rect.w = vcapWH[1][0]/6;
						lay_rect.h = vcapWH[1][1]/6;
						lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
						printf("MSGID_EXT_INPUT_ENZOOM*********tv  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					#endif
					//else 
						{

							lay_rect.w = vcapWH[1][0]/3;
							lay_rect.h = vcapWH[1][1]/3;
							lay_rect.x = vcapWH[1][0]/2-lay_rect.w/2;
							lay_rect.y = vcapWH[1][1]/2-lay_rect.h/2;
							
						}
					
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
					
					
			}
		else
			{
		
					memset(&lay_rect, 0, sizeof(DS_Rect));
					#if 0
					if(pIStuts->ImgZoomStat[1])
					{
						//lay_rect.w = vdisWH[0][0]/2;
						//lay_rect.h = vdisWH[0][1]/2;
						//lay_rect.x = vdisWH[0][0]/4;
						//lay_rect.y = vdisWH[0][1]/4;
						lay_rect.w = vcapWH[pIStuts->SensorStat][0]/2;
						lay_rect.h = vcapWH[pIStuts->SensorStat][1]/2;
						lay_rect.x = vcapWH[pIStuts->SensorStat][0]/4;
						lay_rect.y = vcapWH[pIStuts->SensorStat][1]/4;

						printf("*************************fir  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 0, &lay_rect);
					#endif

					memset(&lay_rect, 0, sizeof(DS_Rect));
					if(pIStuts->ImgZoomStat[0]&&(pIStuts->PicpSensorStat==0))
					{
						
						lay_rect.w = vcapWH[0][0]/6;
						lay_rect.h = vcapWH[0][1]/6;
						lay_rect.x = vcapWH[0][0]/2-lay_rect.w/2;
						lay_rect.y = vcapWH[0][1]/2-lay_rect.h/2;
						OSA_printf("MSGID_EXT_INPUT_ENZOOM*********tv  w=%d h=%d x=%d y=%d\n",lay_rect.w,lay_rect.h,lay_rect.x,lay_rect.y);
					}
					else 
						{

							lay_rect.w = vcapWH[0][0]/3;
							lay_rect.h = vcapWH[0][1]/3;
							lay_rect.x = vcapWH[0][0]/2-lay_rect.w/2;
							lay_rect.y = vcapWH[0][1]/2-lay_rect.h/2;
							
						}
					
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 1, &lay_rect);
			}

		
#endif
		return ;
	}
	if(msgId ==MSGID_EXT_INPUT_ENFREZZ)
		{
			int freeze=pIStuts->ImgFrezzStat[pIStuts->SensorStat];
			DS_Rect rendposr;
			//wj20180319
			#if 0
			rendposr.x=vdisWH[0][0]*2/3;
			rendposr.y=vdisWH[0][1]*2/3;
			rendposr.w=vdisWH[0][0]/3;
			rendposr.h=vdisWH[0][1]/3;
			#else
			rendposr.x=0;
			rendposr.y=vdisWH[0][1]*2/3;
			rendposr.w=vdisWH[0][0]/3;
			rendposr.h=vdisWH[0][1]/3;
			
			#endif
			if(freeze)
				m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &rendposr);
			else
				{
				rendposr=rendpos[pIStuts->PicpPosStat];
				m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &rendposr);
				}
			//tui chu freeze
			m_display.dynamic_config(CDisplayer::DS_CFG_FreezeEnable, 1, &freeze);
			m_display.dynamic_config(CDisplayer::DS_CFG_FreezeEnable, 0, &freeze);

		}
	if(msgId ==MSGID_EXT_INPUT_PICPCROP)
		{

			//DS_Rect rendpos;
			//rendposr.x=0;
			//rendpos.y=0;
			//rendpos.w=vdisWH[1][0]/3;
			//rendpos.w=vdisWH[1][1]/3;

			//printf("the x=%d y=%d w=%d h=%d  pIStuts->PicpPosStat=%d\n",rendpos[pIStuts->PicpPosStat].x,rendpos[pIStuts->PicpPosStat].y,
			//rendpos[pIStuts->PicpPosStat].w,rendpos[pIStuts->PicpPosStat].h,pIStuts->PicpPosStat);
		
			
			m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &rendpos[pIStuts->PicpPosStat]);

		}
	if(msgId ==MSGID_EXT_INPUT_COAST)
		{
			m_castTm=OSA_getCurTimeInMsec();
			m_bCast=true;
			

		}
	if(msgId ==MSGID_EXT_INPUT_VIDEOEN)
		{
			int status=pIStuts->unitFaultStat&0x01;
			status^=1;
			m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, 0, &status);
			OSA_printf("MSGID_EXT_INPUT_VIDEOEN status0=%d\n",status);
			 status=(pIStuts->unitFaultStat>1)&0x01;
			 status^=1;
			m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, 1, &status);
			OSA_printf("MSGID_EXT_INPUT_VIDEOEN status1=%d\n",status);
			

		}
	if(msgId ==MSGID_EXT_INPUT_MMTSHOW)
		{
			int itmp=0;
			int mmtchid=0;
			DS_Rect lay_rect;
			//Mat frame;

			//int frcolor=extInCtrl.DispColor[extInCtrl.SensorStat];
			//extInCtrl.DispColor[extInCtrl.SensorStat]=0;
			//OnProcess(pIStuts->SensorStat,frame);
			//extInCtrl.DispColor[extInCtrl.SensorStat]=frcolor;
			if(pIStuts->ImgMmtshow[pIStuts->SensorStat])
				{
					int chid=pIStuts->SensorStat;
					itmp = chid;
					mmtchid=2;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					//chid++;
					itmp=chid;
					mmtchid=3;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					//chid++;
					itmp=chid;
					mmtchid=4;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					//chid++;
					itmp=chid;
					mmtchid=5;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					//chid++;
					itmp=chid;
					mmtchid=6;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					lay_rect.w = vdisWH[0][0]/3*2;
					lay_rect.h = vdisWH[0][1]/3*2;
					lay_rect.x = 0;
					lay_rect.y = vdisWH[0][1]/3;
					m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 0, &lay_rect);
					//m_display.dynamic_config(CDisplayer::DS_CFG_Rendercount, 7, NULL);

					//m_display.m_renderCount
				}
			else
				{
					itmp = 8;
					mmtchid=2;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					itmp=8;
					mmtchid=3;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					itmp=8;
					mmtchid=4;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					itmp=8;
					mmtchid=5;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					itmp=8;
					mmtchid=6;
					dynamic_config(VP_CFG_SubPicpChId, itmp, &mmtchid);
					lay_rect.w = vdisWH[0][0];
					lay_rect.h = vdisWH[0][1];
					lay_rect.x = 0;
					lay_rect.y = 0;
					m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 0, &lay_rect);
					//m_display.dynamic_config(CDisplayer::DS_CFG_Rendercount, 2, NULL);
				}
			
		#if 1	
		lay_rect.w = 30;
		lay_rect.h = 30;
		lay_rect.x = 0;
		lay_rect.y = 0;
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 2, &lay_rect);
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 3, &lay_rect);
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 4, &lay_rect);
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 5, &lay_rect);
		m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, 6, &lay_rect);
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = 0;
		lay_rect.y = 0;
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 2, &lay_rect);
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = vdisWH[0][0]/3;
		lay_rect.y = 0;
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 3, &lay_rect);
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = vdisWH[0][0]/3;
		lay_rect.x=lay_rect.x*2;
		lay_rect.y = 0;
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 4, &lay_rect);
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = vdisWH[0][0]/3;
		lay_rect.x=lay_rect.x*2;
		lay_rect.y = vdisWH[0][1]/3;
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 5, &lay_rect);
		lay_rect.w = vdisWH[0][0]/3;
		lay_rect.h = vdisWH[0][1]/3;
		lay_rect.x = vdisWH[0][0]/3;
		lay_rect.x=lay_rect.x*2;
		lay_rect.y = vdisWH[0][1]/3;
		lay_rect.y=lay_rect.y*2;
		m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 6, &lay_rect);
		#endif

		}

	if(msgId ==MSGID_EXT_INPUT_MMTSHOWUPDATE)
		{

		
		for(int i=0;i<5;i++)
			{
				if(Mmtpos[i].valid)
					{
						//m_display.m_renders[i+2].videodect=1;
						m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, i+2, &Mmtpos[i].valid);
						m_display.dynamic_config(CDisplayer::DS_CFG_CropRect, i+2, &Mmtpos[i]);
					}
				else
					{
						//m_display.m_renders[i+2].videodect=0;
						//OSA_printf("the id=%d valid =%d\n",i+2,Mmtpos[i].valid);
						m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, i+2, &Mmtpos[i].valid);
						
					}
				//int valid=0;
				//if(i==4)
				//	m_display.dynamic_config(CDisplayer::DS_CFG_VideodetEnable, i+2, &valid);
				//if()
				
			}

		}
	
	/*if(msgId == MSGID_EXT_INPUT_DISPGRADE)
	{
		if(pIStuts->DispGrp[0] == eDisp_hide)
			pThis->m_display.m_bOsd = false;
		else
			pThis->m_display.m_bOsd = true;
	}*/
}


/////////////////////////////////////////////////////
//int majormmtid=0;

 int  CProcess021::MSGAPI_initial()
{
   MSGDRIV_Handle handle=&g_MsgDrvObj;
    assert(handle != NULL);
    memset(handle->msgTab, 0, sizeof(MSGTAB_Class) * MAX_MSG_NUM);
//MSGID_EXT_INPUT_MTD_SELECT
    MSGDRIV_attachMsgFun(handle,    MSGID_SYS_INIT,           				   MSGAPI_init_device,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_SENSOR,           	   MSGAPI_inputsensor,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_PICPCROP,      		   MSGAPI_croppicp,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_TRACK,          		   MSGAPI_inputtrack,     		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENMTD,                       MSGAPI_inpumtd,       		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_MTD_SELECT,     	   MSGAPI_inpumtdSelect,    		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_AIMPOS,          	 MSGAPI_setAimRefine,    		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_AIMSIZE,          	  MSGAPI_setAimSize,    		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENENHAN,           	       MSGAPI_inpuenhance,       	            0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENBDT,           		   MSGAPI_inputbdt,         		    0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENZOOM,           	   MSGAPI_inputzoom,                     0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_ENFREZZ,           	   MSGAPI_inputfrezz,                      0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_MTD_SELECT,      	   MSGAPI_inputmmtselect,              0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_AXISPOS,     	  	   MSGAPI_inputpositon,                   0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_COAST,             	   MSGAPI_inputcoast,                      0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_FOVSELECT,                MSGAPI_inputfovselect,                 0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_FOVSTAT,                	   MSGAPI_inputfovchange,               0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_SEARCHMOD,              MSGAPI_inputsearchmod,              0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_VIDEOEN,            	   MSGAPI_inputvideotect,                 0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_MMTSHOW,             MSGAPI_mmtshow,                 	     0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_FOVCMD,             MSGAPI_FOVcmd,                 	     0);
    MSGDRIV_attachMsgFun(handle,    MSGID_EXT_INPUT_CFGSAVE,             MSGAPI_SaveCfgcmd,                 	     0);	
    
    return 0;
}


int CProcess021::WriteAxisToFile()
{
	string AxisFile;
	AxisFile = AXIS_FILE;
	char axis_x[10] = "axis_x";
	char axis_y[10] = "axis_y";

	FILE *fp = fopen(AxisFile.c_str(), "wt");
	if(fp !=NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		if(len == 0)
		{
			FileStorage fw(AxisFile,FileStorage::WRITE);
			if(fw.isOpened())
			{
					/*if(extInCtrl.FovStat == 1){
						sprintf(axis_x, "axis_x_%d", 0);
						sprintf(axis_y, "axis_y_%d", 0);
					}
					else if(extInCtrl.FovStat == 2){
						sprintf(axis_x, "axis_x_%d", 1);
						sprintf(axis_y, "axis_y_%d", 1);
					}
					else if(extInCtrl.FovStat == 3){
						sprintf(axis_x, "axis_x_%d", 2);
						sprintf(axis_y, "axis_y_%d", 2);
					}
					else */if(extInCtrl.FovStat == 4){
						sprintf(axis_x, "axis_x_%d", 3);
						sprintf(axis_y, "axis_y_%d", 3);
					}/*
					else if(extInCtrl.FovStat == 5){
						sprintf(axis_x, "axis_x_%d", 4);
						sprintf(axis_y, "axis_y_%d", 4);
					}*/
					
					fw <<axis_x<< (int)extInCtrl.unitAxisXtmp[0];
					fw <<axis_y<< (int)extInCtrl.unitAxisYtmp[0];
				
			}
		}
	}

	FILE *fpp = fopen(AxisFile.c_str(), "rt");
	if(fpp !=NULL)
	{
		fseek(fpp, 0, SEEK_END);
		int len = ftell(fpp);
		fclose(fpp);
		if(len > 10)
			return 0;
	}
	else
		return -1;
}

int CProcess021::ReadAxisFromFile()
{
	string AxisFile;
	AxisFile = AXIS_FILE;

	char axis_x[10] = "axis_x";
	char axis_y[10] = "axis_y";

	FILE *fp = fopen(AxisFile.c_str(), "rt");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);

		if(len < 10)
			return -1;
		else
		{
			FileStorage fr(AxisFile, FileStorage::READ);
			if(fr.isOpened())
			{

				sprintf(axis_x, "axis_x_%d", 3);
				sprintf(axis_y, "axis_y_%d", 3);
				
				extInCtrl.unitAxisXtmp[0] = (int)fr[axis_x];
				extInCtrl.unitAxisYtmp[0] = (int)fr[axis_y];
				//OSA_printf("axisXY=(%d,%d)\n",extInCtrl.unitAxisXtmp[0],extInCtrl.unitAxisYtmp[0]);
				/*
				for(int i=0; i<5; i++){
					sprintf(axis_x, "axis_x_%d", i);
					sprintf(axis_y, "axis_y_%d", i);
					//OSA_printf("axisXY=(%d,%d)\n",(int)fr[axis_x],(int)fr[axis_y]);
					//extInCtrl.unitAxisX[i] = (int)fr[axis_x];
					//extInCtrl.unitAxisY[i] = (int)fr[axis_y];

					if((extInCtrl.FovStat == 1)&&(i==0)){
						extInCtrl.unitAxisX[0] = (int)fr[axis_x];
						extInCtrl.unitAxisY[0] = (int)fr[axis_y];
					}	
					else if((extInCtrl.FovStat == 2)&&(i==1)){
						extInCtrl.unitAxisX[0] = (int)fr[axis_x];
						extInCtrl.unitAxisY[0] = (int)fr[axis_y];
					}	
					else if((extInCtrl.FovStat == 3)&&(i==2)){
						extInCtrl.unitAxisX[0] = (int)fr[axis_x];
						extInCtrl.unitAxisY[0] = (int)fr[axis_y];
					}
					else if((extInCtrl.FovStat == 4)&&(i==3)){
						extInCtrl.unitAxisX[0] = (int)fr[axis_x];
						extInCtrl.unitAxisY[0] = (int)fr[axis_y];
					}
					else if((extInCtrl.FovStat == 5)&&(i==4)){
						extInCtrl.unitAxisX[0] = (int)fr[axis_x];
						extInCtrl.unitAxisY[0] = (int)fr[axis_y];
					}
				}
				*/
				
				return 0;
			}else
				return -1;
		}
	}
	else
		return -1;
}


 #if 1
 void CProcess021::MSGAPI_init_device(long lParam )
{
	
	sThis->msgdriv_event(MSGID_SYS_INIT,NULL);
	OSA_printf("hello world\n");
}

  void CProcess021::MSGAPI_inputsensor(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	//	pIStuts->SensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
	sThis->msgdriv_event(MSGID_EXT_INPUT_SENSOR,NULL);

	/*sThis->Track_fovreacq( pIStuts->unitFovAngle[pIStuts->SensorStat],pIStuts->SensorStat,1);
	if(pIStuts->AvtTrkStat){
		
		sThis->Track_reacq(sThis->trackinfo_obj->reAcqRect,2);
	}
	OSA_printf("hello world\n");*/
}

  void CProcess021::MSGAPI_picp(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		if(pIStuts->PicpSensorStat == 0xFF)
			pIStuts->PicpSensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
		else
			pIStuts->PicpSensorStat = 0xFF;
	
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP,NULL);
	OSA_printf("hello world\n");
}


   void CProcess021::MSGAPI_croppicp(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	//	if(pIStuts->PicpSensorStat == 0xFF)
	//		pIStuts->PicpSensorStat = (pIStuts->SensorStat + 1)%eSen_Max;
	//	else
	//		pIStuts->PicpSensorStat = 0xFF;
	if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x04)
		{
			return ;

		}

	if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x01)
		{
		pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat = (pIStuts->SensorStat+1) % (eSen_Max);
		sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
		//printf("the  PicpSensorStatpri=%d\n ",pIStuts->PicpSensorStatpri);

		}
	else if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x02)
		{
		      pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat =8;
		      sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);

		}
	else if(pIStuts->ImgPicp[pIStuts->SensorStat]==0x03)
		{
			pIStuts->PicpPosStat=(pIStuts->PicpPosStat+1)%4;
			sThis->msgdriv_event(MSGID_EXT_INPUT_PICPCROP,NULL);

		}
	
	
	
	//printf("hello world\n");
}
   void CProcess021::MSGAPI_inputtrack(long lParam )
{
	OSA_printf("%s^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",__func__);
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->AvtTrkStat)
		//	pIStuts->AvtTrkStat = eTrk_mode_acq;
		//else
		//	pIStuts->AvtTrkStat = eTrk_mode_target;
	
	sThis->msgdriv_event(MSGID_EXT_INPUT_TRACK,NULL);
	//printf("%s\n,__func__");
}


     void CProcess021::MSGAPI_inpumtd(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENMTD,NULL);
	OSA_printf("hello world\n");
}
     void CProcess021::MSGAPI_inpumtdSelect(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	int i;
	if(pIStuts->MMTTempStat==3)
		{

			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(sThis->m_mtd[pIStuts->SensorStat]->tg[majormmtid].valid==1)
						{
							//majormmtid++;
							majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;

						}

				}

			
		}
	else if(pIStuts->MMTTempStat==4)
		{

			for(i=0;i<MAX_TARGET_NUMBER;i++)
				{

					if(sThis->m_mtd[pIStuts->SensorStat]->tg[majormmtid].valid==1)
						{
							//majormmtid++;
							if(majormmtid>0)
								majormmtid=(majormmtid-1);
							else
								{
									majormmtid=MAX_TARGET_NUMBER-1;

								}

						}

				}


		}

	
		//majormmtid=majormmtid;
	OSA_printf("MSGAPI_inpumtdSelect\n");
}


	   void CProcess021::MSGAPI_inpuenhance(long lParam )
{
	//	CMD_EXT *pIStuts = &pThis->extInCtrl;
	//	if(pIStuts->ImgEnhStat[pIStuts->SensorStat])
	//		pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Disable;
	//	else
	//		pIStuts->ImgEnhStat[pIStuts->SensorStat] = eImgAlg_Enable;
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENENHAN,NULL);
	OSA_printf("hello world\n");
}

void CProcess021::MSGAPI_setAimRefine(long lParam          /*=NULL*/)
{

	CMD_EXT *pIStuts = &sThis->extInCtrl;
	//OSA_printf("%s msgextInCtrl->TrkBomenCtrl=%d pIStuts->AvtMoveY=%d\n",__func__,pIStuts->AvtMoveX,pIStuts->AvtMoveY);
	if(pIStuts->AvtMoveX==eTrk_ref_left)
		{
			pIStuts->AvtMoveX=-1;
		}
	else if(pIStuts->AvtMoveX==eTrk_ref_right)
		{
			pIStuts->AvtMoveX=1;
		}
		if(pIStuts->AvtMoveY==eTrk_ref_up)
		{
			pIStuts->AvtMoveY=-1;
		}
	else if(pIStuts->AvtMoveY==eTrk_ref_down)
		{
			pIStuts->AvtMoveY=1;
		}
	sThis->msgdriv_event(MSGID_EXT_INPUT_AIMPOS,NULL);

}
void CProcess021::MSGAPI_setAimSize(long lParam          /*=NULL*/)
{

	CMD_EXT *pIStuts = &sThis->extInCtrl;

	//OSA_printf("AimSize = %d\n",pIStuts->AvtTrkAimSize);
	
	sThis->msgdriv_event(MSGID_EXT_INPUT_AIMSIZE,NULL);

}

	     void CProcess021::MSGAPI_inputbdt(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		if(pIStuts->TvCollimation!=1)
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Disable;
		else
			pIStuts->ImgBlobDetect[pIStuts->SensorStat] = eImgAlg_Enable;
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENBDT,NULL);
	
	OSA_printf("fun=%s line=%d \n",__func__,__LINE__);
}


		   void CProcess021::MSGAPI_inputzoom(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Disable;
		//else
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Enable;
	sThis->msgdriv_event(MSGID_EXT_INPUT_ENZOOM,NULL);
	OSA_printf("hello world\n");
}


void CProcess021::MSGAPI_inputfrezz(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Disable;
		//else
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Enable;
	
	
	if( pIStuts->FrCollimation==1)
		{
			//OSA_printf("the*****************************************enable \n");
			
			pIStuts->PicpSensorStat=0;//tv picp sensor
			sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
			//dong jie chuang kou
			pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Enable;
			sThis->msgdriv_event(MSGID_EXT_INPUT_ENFREZZ,NULL);

		}
	else
		{
		
		if((pIStuts->PicpSensorStatpri!=0))//tui picp the sensor is tv
				{
					
					pIStuts->PicpSensorStatpri=pIStuts->PicpSensorStat=2;//tui chu picp
					sThis->msgdriv_event(MSGID_EXT_INPUT_ENPICP, NULL);
					OSA_printf("MSGAPI_inputfrezz*****************************************disable \n");
					//sThis->m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &(sThis->rendpos[pIStuts->PicpPosStat]));
				}
		else
				{
					//sThis->m_display.dynamic_config(CDisplayer::DS_CFG_RenderPosRect, 1, &(sThis->rendpos[pIStuts->PicpPosStat]));
					pIStuts->PicpSensorStat=0;
				}
			//tui chu dong jie chuang kou
			pIStuts->ImgFrezzStat[pIStuts->SensorStat] = eImgAlg_Disable;
			sThis->msgdriv_event(MSGID_EXT_INPUT_ENFREZZ,NULL);
			
			OSA_printf("the*****************************************disable PicpSensorStatpri=%d\n",pIStuts->PicpSensorStatpri);
		}

			
	
	OSA_printf("%s\n",__func__);
}


void CProcess021::MSGAPI_inputmmtselect(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
		//if(pIStuts->ImgZoomStat[pIStuts->SensorStat])
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Disable;
		//else
		//	pIStuts->ImgZoomStat[pIStuts->SensorStat] = eImgAlg_Enable;
	//pThis->msgdriv_event(MSGID_EXT_INPUT_ENFREZZ,NULL);
	if(pIStuts->ImgMtdSelect[pIStuts->SensorStat]  ==eMMT_Next)
		majormmtid=(majormmtid+1)%MAX_TARGET_NUMBER;
	else if(pIStuts->ImgMtdSelect[pIStuts->SensorStat]  ==  eMMT_Prev)
		{
			majormmtid=(majormmtid-1+MAX_TARGET_NUMBER)%MAX_TARGET_NUMBER;
		}
	OSA_printf("%s\n",__func__);
}



void CProcess021::MSGAPI_inputpositon(long lParam )
{
	CMD_EXT *pIStuts = &sThis->extInCtrl;
	if(pIStuts->SensorStat==0)
		{
	if((pIStuts->AvtPosXTv>=50)&&(pIStuts->AvtPosXTv<=vcapWH[pIStuts->SensorStat][0]-50))
		{
			pIStuts->unitAimX = pIStuts->AvtPosXTv;
			pIStuts->unitAxisX[pIStuts->SensorStat ]=pIStuts->AvtPosXTv;
			sThis->m_ImageAxisx=pIStuts->unitAxisX[pIStuts->SensorStat ];
		}
	if((pIStuts->AvtPosYTv>=50)&&(pIStuts->AvtPosYTv<=vcapWH[pIStuts->SensorStat][1]-50))
		{
			pIStuts->unitAimY = pIStuts->AvtPosYTv;
			pIStuts->unitAxisY[pIStuts->SensorStat ]=pIStuts->AvtPosYTv;
			sThis->m_ImageAxisy=pIStuts->unitAxisY[pIStuts->SensorStat ];
		}
		}
	else if(pIStuts->SensorStat==1)
		{
#ifdef VIDEO1280X1024
		if((pIStuts->AvtPosXFir>=50)&&(pIStuts->AvtPosXFir<=1230))
		{
			pIStuts->unitAimX = pIStuts->AvtPosXFir;
			pIStuts->unitAxisX[pIStuts->SensorStat ]=pIStuts->AvtPosXFir;
			sThis->m_ImageAxisx=pIStuts->unitAxisX[pIStuts->SensorStat ];
		}
		if((pIStuts->AvtPosYFir>=50)&&(pIStuts->AvtPosYFir<=974))
		{
			pIStuts->unitAimY = pIStuts->AvtPosYFir;
			pIStuts->unitAxisY[pIStuts->SensorStat ]=pIStuts->AvtPosYFir;
			sThis->m_ImageAxisy=pIStuts->unitAxisY[pIStuts->SensorStat ];
		}


#else
		if((pIStuts->AvtPosXFir>=25)&&(pIStuts->AvtPosXFir<=vcapWH[pIStuts->SensorStat][0]-25))
		{
			pIStuts->unitAimX = pIStuts->AvtPosXFir;
			pIStuts->unitAxisX[pIStuts->SensorStat ]=pIStuts->AvtPosXFir;
			sThis->m_ImageAxisx=pIStuts->unitAxisX[pIStuts->SensorStat ];
		}
		if((pIStuts->AvtPosYFir>=25)&&(pIStuts->AvtPosYFir<=vcapWH[pIStuts->SensorStat][1]-25))
		{
			pIStuts->unitAimY = pIStuts->AvtPosYFir;
			pIStuts->unitAxisY[pIStuts->SensorStat ]=pIStuts->AvtPosYFir;
			sThis->m_ImageAxisy=pIStuts->unitAxisY[pIStuts->SensorStat ];
		}
#endif

		}

	
	
	
	
	
	OSA_printf("%s   THE=unitAimX=%d unitAxisY=%d\n",__func__,pIStuts->unitAxisX[pIStuts->SensorStat ],pIStuts->unitAxisY[pIStuts->SensorStat ]);
}

void CProcess021::MSGAPI_inputcoast(long lParam )
{

	
	sThis->msgdriv_event(MSGID_EXT_INPUT_COAST,NULL);
	
	//printf("%s\n",__func__);
}

void CProcess021::MSGAPI_inputfovselect(long lParam )
{

	CMD_EXT *pIStuts = &sThis->extInCtrl;

	/*
	sThis->m_intervalFrame=1;
	sThis->m_rcAcq.x=pIStuts->unitAxisX[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
	sThis->m_rcAcq.y=pIStuts->unitAxisY[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
	sThis->m_rcAcq.width=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
	sThis->m_rcAcq.height=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
	OSA_printf("XY(%f,%f)  WH(%f,%f)\n",sThis->m_rcAcq.x,sThis->m_rcAcq.y,sThis->m_rcAcq.width,sThis->m_rcAcq.height);
	*/

	if(pIStuts->changeSensorFlag == 0){
		//OSA_printf("FovStat = %d SensorStat=%d\n",pIStuts->FovStat,pIStuts->SensorStat);
		if(pIStuts->SensorStat == 0){
			if(pIStuts->FovStat == 1)
				sThis->Track_fovreacq( 2400,pIStuts->SensorStat,0);
			else if(pIStuts->FovStat == 3)
				sThis->Track_fovreacq( 330,pIStuts->SensorStat,0);
			else if(pIStuts->FovStat == 4)	
				sThis->Track_fovreacq( 110,pIStuts->SensorStat,0);					
			else if(pIStuts->FovStat == 5){
				sThis->Track_fovreacq( 55,pIStuts->SensorStat,0);
			}
		}
		else if(pIStuts->SensorStat == 1){
			if(pIStuts->FovStat == 1)
				sThis->Track_fovreacq( 4000,pIStuts->SensorStat,0);
			else if(pIStuts->FovStat == 4)
				sThis->Track_fovreacq( 120,pIStuts->SensorStat,0);
			else if(pIStuts->FovStat == 5)
				sThis->Track_fovreacq( 60,pIStuts->SensorStat,0);

		}

		//OSA_printf("fovselectXY(%f,%f),WH(%f,%f)\n",sThis->trackinfo_obj->reAcqRect.x,sThis->trackinfo_obj->reAcqRect.y,sThis->trackinfo_obj->reAcqRect.width,sThis->trackinfo_obj->reAcqRect.height);
		if(pIStuts->AvtTrkStat){
			
			sThis->Track_reacq(sThis->trackinfo_obj->reAcqRect,2);
		}
	}
}

void CProcess021::MSGAPI_inputfovchange(long lParam )
{

	CMD_EXT *pIStuts = &sThis->extInCtrl;
	#if 0
	sThis->m_intervalFrame=1;
	
	sThis->m_rcAcq.x=pIStuts->unitAxisX[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0]/2;
	sThis->m_rcAcq.y=pIStuts->unitAxisY[pIStuts->SensorStat]-trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1]/2;
	sThis->m_rcAcq.width=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][0];
	sThis->m_rcAcq.height=trkWinWH[pIStuts->SensorStat][pIStuts->AvtTrkAimSize][1];
	#endif

	//OSA_printf("%s:unitFovAngle = %f\n",__func__,pIStuts->unitFovAngle[pIStuts->SensorStat]);
	sThis->Track_fovreacq( pIStuts->unitFovAngle[pIStuts->SensorStat],pIStuts->SensorStat,0);

}


void CProcess021::MSGAPI_inputsearchmod(long lParam )
{

	CMD_EXT *pIStuts = &sThis->extInCtrl;
	float panenv=pIStuts->TrkPanev/100.0;
	float TrkTitlev=pIStuts->TrkTitlev/100.0;
	if(panenv>5)
		{
			//sThis->m_searchmod=1;
		}
	else if(panenv<-5)
		{
			//sThis->m_searchmod=2;
		}
	else 
		{
			//sThis->m_searchmod=0;
		}
	
	// printf("the TrkPanev=%f TrkTitlev=%f\n",pIStuts->TrkPanev/100.0,pIStuts->TrkTitlev/100.0);
	
}


 void CProcess021::MSGAPI_inputvideotect(long lParam )
 	{

		OSA_printf("MSGAPI_inputvideotect*******************\n");
		sThis->msgdriv_event(MSGID_EXT_INPUT_VIDEOEN,NULL);
 	}

  void CProcess021::MSGAPI_mmtshow(long lParam )
 	{

		
		OSA_printf("MSGAPI_mmtshow\n");
		//sThis->msgdriv_event(MSGID_EXT_INPUT_MMTSHOW,NULL);
		//sThis->msgdriv_event(MSGID_EXT_INPUT_VIDEOEN,NULL);
 	}
    void CProcess021::MSGAPI_FOVcmd(long lParam )
 	{
		CMD_EXT *pIStuts = &sThis->extInCtrl;
		if((pIStuts->FovCtrl==5)&&(pIStuts->SensorStat==0))
			sThis->tvzoomStat=1;
		else
			sThis->tvzoomStat=0;
		
 	}
void CProcess021::MSGAPI_SaveCfgcmd(long lParam )
{
	
	sThis->msgdriv_event(MSGID_EXT_INPUT_CFGSAVE,NULL);
	

}	
 #endif


