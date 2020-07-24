/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @history

  ******************************************************************************
  */
  
#ifndef __BSP_DRIVER_GPS_H
#define __BSP_DRIVER_GPS_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "include.h" 
 

//GPS NMEA-0183协议重要参数结构体定义,卫星信息
typedef struct
{
    uint8_t		ucNumber;		//卫星编号
    uint8_t 	ucElevation;	//卫星仰角
    uint16_t 	usAzimuth;		//卫星方位角
    uint8_t 	ucSNR;			//信噪比
} GPSNmeaMsg_T;

//北斗 NMEA-0183协议重要参数结构体定义,卫星信息
typedef struct
{
    uint8_t 	ucBEIDOUNumber;		    //卫星编号
    uint8_t 	ucBEIDOUElevation;		//卫星仰角
    uint16_t 	usBEIDOUAzimuth;		//卫星方位角
    uint8_t 	ucBEIDOUSNR;			//信噪比
} BEIDOUNmeaMsg_T;

//UTC时间信息
typedef struct
{
    uint8_t	    ucYear;	    //年份
    uint8_t 	ucMonth;	//月份
    uint8_t 	ucDate;		//日期
    uint8_t 	ucHour; 	//小时
    uint8_t 	ucMinute; 	//分钟
    uint8_t 	ucSecond; 	//秒钟
} NmeaUtc_T;

//NMEA 0183 协议解析后数据存放结构体
typedef struct
{
    uint8_t				ucVisibleGPSNumber;			//可见GPS卫星数
    uint8_t				ucVisibleBEIDOUNumber;		//可见北斗卫星数
    GPSNmeaMsg_T		tGPSMsg[12];				//暂且算最多12颗GPS卫星
    BEIDOUNmeaMsg_T 	tBeidouMsg[12];			    //暂且算最多12颗北斗卫星
    NmeaUtc_T 			tUtc;						//UTC时间
    uint32_t 			ulLatitude;					//纬度 分扩大100000倍,实际要除以100000
    uint8_t 			ucSorN_Latitude;			//北纬/南纬,N:北纬;S:南纬
    uint32_t 			ulLongitude;			    //经度 分扩大100000倍,实际要除以100000
    uint8_t 			ucEorW_Longitude;			//东经/西经,E:东经;W:西经
    uint8_t             ucGPSStatus;                //GPS状态:A有效，V无效
    uint8_t 			ucLocationSatelliteNum;		//用于定位的GPS卫星数,0~12.
    uint8_t 			ucLocationSatellite[12];	//用于定位的卫星编号
    uint8_t 			ucFixMode;					//定位类型:1,没有定位;2,2D定位;3,3D定位
    uint16_t 			usPDOP;						//位置精度因子 0~500,对应实际值0~50.0
    uint16_t 			usHDOP;						//水平精度因子 0~500,对应实际值0~50.0
    uint16_t 			usVDOP;						//垂直精度因子 0~500,对应实际值0~50.0
    int32_t				lAltitude;			 		//海拔高度,放大了10倍,实际除以10.单位:0.1m
    uint16_t 			ulSpeed;					//地面速率,放大了1000倍,实际除以10.单位:0.001公里/小时
} NMEA_MSG_T;

extern NMEA_MSG_T *ptGPSMsg;

void gpsNmeaAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf);
void gpsNmeaGNRMCAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
void gpsNmeaGPRMCAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
void gpsNmeaGPGSVAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
void gpsNmeaBDGSVAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
void gpsNmeaGNGGAAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
void gpsNmeaGNGSAAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
void gpsNmeaGNVTGAnalysis(NMEA_MSG_T *ptGPSMsg, uint8_t *pucBuf);
uint32_t gpsNmeaPow(uint8_t ucBottom, uint8_t ucPower);

#ifdef __cplusplus
}
#endif

#endif	//__BSP_DRIVER_GPS_H 