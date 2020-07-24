/**
  ******************************************************************************
  * @file		bsp_ltdc.c    
  * @author  
  * @version 
  * @date    
  * @brief		LTDC驱动代码
  ******************************************************************************
  * @history

  ******************************************************************************
  */


#include "include.h"

//用户修改配置部分:

//定义颜色像素格式,一般用RGB565
#define LCD_PIXFORMAT				LCD_PIXEL_FORMAT_RGB565	

//定义默认背景层颜色
#define LTDC_BACKLAYERCOLOR			0X00000000	

//LCD帧缓冲区首地址,这里定义在SDRAM里面.
#define LCD_FRAME_BUF_ADDR			SDRAM_LCD_BUF1  

/* 偏移地址计算公式:
   Maximum width x Maximum Length x Maximum Pixel size (ARGB8888) in bytes
   => 640 x 480 x 4 =  1228800 or 0x12C000 */
#define BUFFER_OFFSET          (uint32_t)(lcddev.width * lcddev.height * 2)
//----------------------------------------------------------------------------

LTDC_HandleTypeDef  LTDC_Handler;	    //LTDC句柄
DMA2D_HandleTypeDef DMA2D_Handler; 	    //DMA2D句柄

//根据不同的颜色格式,定义帧缓存数组
//#if (LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888) || (LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888)
////    uint32_t ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
//uint32_t ltdc_lcd_framebuf[1280][800] __attribute__((section(".ARM.__at_0xC0000000")));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小

//#else
////    uint16_t ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
//uint16_t ltdc_lcd_framebuf[1280][800] __attribute__((section(".ARM.__at_0xC0000000")));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
//                                   												  	
//#endif

uint32_t ulCurrentFrameBuffer;
uint32_t *ltdc_framebuf[2] = {0};		//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域
_ltdc_dev lcdltdc;						//管理LCD LTDC的重要参数


/*******************************************************
  * @brief  : LTDC开关
  * @note   : 
  * @param  : lcd_switch:1 打开,0，关闭
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Switch(uint8_t sw)
{
    if (sw == 1)
    {
        __HAL_LTDC_ENABLE(&LTDC_Handler);
    }
    else if (sw == 0)
    {
        __HAL_LTDC_DISABLE(&LTDC_Handler);
    }
}

/*******************************************************
  * @brief  : 开关指定层
  * @note   : 
  * @param  : layerx:层号,0,第一层; 1,第二层
  * @param  : sw:1 打开;0关闭
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Layer_Switch(uint8_t layerx, uint8_t sw)
{
    if (sw == 1)
    {
        __HAL_LTDC_LAYER_ENABLE(&LTDC_Handler, layerx);
    }
    else if (sw == 0)
    {
        __HAL_LTDC_LAYER_DISABLE(&LTDC_Handler, layerx);
    }

    __HAL_LTDC_RELOAD_CONFIG(&LTDC_Handler);
}

/*******************************************************
  * @brief  :选择层 
  * @note   : 
  * @param  : layerx:层号;0,第一层;1,第二层;
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Select_Layer(uint8_t layerx)
{
    lcdltdc.activelayer = layerx;
}

/*******************************************************
  * @brief  : 设置LCD显示方向
  * @note   : 
  * @param  : dir:0,竖屏；1,横屏
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Display_Dir(uint8_t dir)
{
    lcdltdc.dir = dir; 	//显示方向

    if (dir == 0)			//竖屏
    {
        lcdltdc.width = lcdltdc.pheight;
        lcdltdc.height = lcdltdc.pwidth;
    }
    else if (dir == 1)	//横屏
    {
        lcdltdc.width = lcdltdc.pwidth;
        lcdltdc.height = lcdltdc.pheight;
    }
}

/*******************************************************
  * @brief  : 画点函数
  * @note   : 
  * @param  : x,y:坐标
  * @param  : color:颜色值
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Draw_Point(uint16_t x, uint16_t y, uint32_t color)
{
    #if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888

    if (lcdltdc.dir)	//横屏
    {
        *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
    else 			//竖屏
    {
        *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y)) = color;
    }

    #else

    if (lcdltdc.dir)	//横屏
    {
        *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
    else 			//竖屏
    {
        *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color;
    }
    #endif
}

/*******************************************************
  * @brief  : 读点函数
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 颜色值
*******************************************************/
uint32_t LTDC_Read_Point(uint16_t x, uint16_t y)
{
    #if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888

    if (lcdltdc.dir)	//横屏
    {
        return *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else 			//竖屏
    {
        return *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y));
    }

    #else

    if (lcdltdc.dir)	//横屏
    {
        return *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else 			//竖屏
    {
        return *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y));
    }

    #endif
}

/*******************************************************
  * @brief  : LTDC填充矩形,DMA2D填充
  * @note   : 有时候需要频繁的调用填充函数，所以为了速度，填充函数采用寄存器版本
  * @param  : (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
  * @param  : color:要填充的颜色
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{
    uint32_t psx, psy, pex, pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;

    //坐标系转换
    if (lcdltdc.dir)	//横屏
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else			//竖屏
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }

    offline = lcdltdc.pwidth - (pex - psx + 1);
    addr = ((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
    RCC->AHB1ENR |= 1 << 23;			//使能DM2D时钟
    DMA2D->CR = 3 << 16;				//寄存器到存储器模式
    DMA2D->OPFCCR = LCD_PIXFORMAT;	//设置颜色格式
    DMA2D->OOR = offline;				//设置行偏移
    DMA2D->CR &= ~(1 << 0);				//先停止DMA2D
    DMA2D->OMAR = addr;				//输出存储器地址
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);	//设定行数寄存器
    DMA2D->OCOLR = color;				//设定输出颜色寄存器
    DMA2D->CR |= 1 << 0;				//启动DMA2D

    while ((DMA2D->ISR & (1 << 1)) == 0)	//等待传输完成
    {
        timeout++;

        if (timeout > 0X1FFFFF)
        {
            break;    //超时退出
        }
    }

    DMA2D->IFCR |= 1 << 1;				//清除传输完成标志
}

/*******************************************************
  * @brief  : 在指定区域内填充指定颜色块,DMA2D填充
  * @note   : 此函数仅支持uint16_t,RGB565格式的颜色数组填充.
  * @param  :(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1) 
  * @param  : sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
  * @param  :color:要填充的颜色数组首地址 
  * @retval : 
*******************************************************/
void LTDC_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t psx, psy, pex, pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
    uint32_t timeout = 0;
    uint16_t offline;
    uint32_t addr;

    //坐标系转换
    if (lcdltdc.dir)	//横屏
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else			//竖屏
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }

    offline = lcdltdc.pwidth - (pex - psx + 1);
    addr = ((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
    RCC->AHB1ENR |= 1 << 23;			//使能DM2D时钟
    DMA2D->CR = 0 << 16;				//存储器到存储器模式
    DMA2D->FGPFCCR = LCD_PIXFORMAT;	//设置颜色格式
    DMA2D->FGOR = 0;					//前景层行偏移为0
    DMA2D->OOR = offline;				//设置行偏移
    DMA2D->CR &= ~(1 << 0);				//先停止DMA2D
    DMA2D->FGMAR = (uint32_t)color;		//源地址
    DMA2D->OMAR = addr;				//输出存储器地址
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);	//设定行数寄存器
    DMA2D->CR |= 1 << 0;				//启动DMA2D

    while ((DMA2D->ISR & (1 << 1)) == 0)	//等待传输完成
    {
        timeout++;

        if (timeout > 0X1FFFFF)
        {
            break;    //超时退出
        }
    }

    DMA2D->IFCR |= 1 << 1;				//清除传输完成标志
}

/*******************************************************
  * @brief  : LCD清屏
  * @note   : 
  * @param  : color:颜色值
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Clear(uint32_t color)
{
    LTDC_Fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}

/*******************************************************
  * @brief  : LTDC时钟(Fdclk)设置函数
  * @note   : Fvco=Fin*pllsain;//Fvco:VCO频率,Fin:输入时钟频率一般为1Mhz(来自系统时钟PLLM分频后的时钟,见时钟树图)
Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;
  * @param  : pllsain:SAI时钟倍频系数N,取值范围:50~432.
  * @param  : pllsair:SAI时钟的分频系数R,取值范围:2~7
  * @param  : 
假设:外部晶振为25M,pllm=25的时候,Fin=1Mhz.
例如:要得到20M的LTDC时钟,则可以设置:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
Fdclk=1*400/5/4=400/20=20Mhz  
  * @retval : 返回值:0,成功;1,失败
*******************************************************/
uint8_t LTDC_Clk_Set(uint32_t pllsain, uint32_t pllsair, uint32_t pllsaidivr)
{
    RCC_PeriphCLKInitTypeDef PeriphClkIniture;
    //LTDC输出像素时钟，需要根据自己所使用的LCD数据手册来配置！
    PeriphClkIniture.PeriphClockSelection = RCC_PERIPHCLK_LTDC;	//LTDC时钟
    PeriphClkIniture.PLLSAI.PLLSAIN = pllsain;
    PeriphClkIniture.PLLSAI.PLLSAIR = pllsair;
    PeriphClkIniture.PLLSAIDivR = pllsaidivr;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture) == HAL_OK) //配置像素时钟
    {
        return 0;   //成功
    }
    else
    {
        return 1;    //失败
    }
}

/*******************************************************
  * @brief  : LTDC层颜窗口设置,窗口以LCD面板坐标系为基准
  * @note   : 此函数必须在LTDC_Layer_Parameter_Config之后再设置.
  * @param  : layerx:层值,0/1. 
  * @param  : sx,sy:起始坐标
  * @param  : width,height:宽度和高度
  * @retval : 
*******************************************************/
void LTDC_Layer_Window_Config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    HAL_LTDC_SetWindowPosition(&LTDC_Handler, sx, sy, layerx); //设置窗口的位置
    HAL_LTDC_SetWindowSize(&LTDC_Handler, width, height, layerx); //设置窗口大小
}


/*******************************************************
  * @brief  : LTDC基本参数设置.
  * @note   : 此函数,必须在LTDC_Layer_Window_Config之前设置
  * @param  : layerx:层值,0/1. 
  * @param  : bufaddr:层颜色帧缓存起始地址
  * @param  : pixformat:颜色格式.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
  * @param  : alpha:层颜色Alpha值,0,全透明;255,不透明
  * @param  : alpha0:默认颜色Alpha值,0,全透明;255,不透明
  * @param  : bfac1:混合系数1,4(100),恒定的Alpha;6(101),像素Alpha*恒定Alpha
  * @param  : bfac2:混合系数2,5(101),恒定的Alpha;7(111),像素Alpha*恒定Alpha
  * @param  : bkcolor:层默认颜色,32位,低24位有效,RGB888格式	
  * @retval : 
*******************************************************/
void LTDC_Layer_Parameter_Config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor)
{
    LTDC_LayerCfgTypeDef pLayerCfg;
    pLayerCfg.WindowX0 = 0;                     //窗口起始X坐标
    pLayerCfg.WindowY0 = 0;                     //窗口起始Y坐标
    pLayerCfg.WindowX1 = lcdltdc.pwidth;        //窗口终止X坐标
    pLayerCfg.WindowY1 = lcdltdc.pheight;       //窗口终止Y坐标
    pLayerCfg.PixelFormat = pixformat;		  //像素格式
    pLayerCfg.Alpha = alpha;				      //Alpha值设置，0~255,255为完全不透明
    pLayerCfg.Alpha0 = alpha0;			      //默认Alpha值
    pLayerCfg.BlendingFactor1 = (uint32_t)bfac1 << 8; //设置层混合系数
    pLayerCfg.BlendingFactor2 = (uint32_t)bfac2 << 8;	//设置层混合系数
    pLayerCfg.FBStartAdress = bufaddr;	      //设置层颜色帧缓存起始地址
    pLayerCfg.ImageWidth = lcdltdc.pwidth;      //设置颜色帧缓冲区的宽度
    pLayerCfg.ImageHeight = lcdltdc.pheight;    //设置颜色帧缓冲区的高度
    pLayerCfg.Backcolor.Red = (uint8_t)(bkcolor & 0X00FF0000) >> 16; //背景颜色红色部分
    pLayerCfg.Backcolor.Green = (uint8_t)(bkcolor & 0X0000FF00) >> 8; //背景颜色绿色部分
    pLayerCfg.Backcolor.Blue = (uint8_t)bkcolor & 0X000000FF;    //背景颜色蓝色部分
    HAL_LTDC_ConfigLayer(&LTDC_Handler, &pLayerCfg, layerx); //设置所选中的层
}

/*******************************************************
  * @brief  :读取面板参数 
  * @note   : 
				PG6=R7(M0);PI2=G7(M1);PI7=B7(M2);
				M2:M1:M0
				0 :0 :0	//4.3寸480*272 RGB屏,ID = LCD_43_480x272
				0 :0 :1	//7寸800*480 RGB屏,ID = LCD_70_800x480
				0 :1 :0	//7寸1024*600 RGB屏,ID = LCD_70_1024x600
				0 :1 :1	//7寸1280*800 RGB屏,ID = LCD_70_1280x800
				1 :0 :0	//8寸1024*768 RGB屏,ID = LCD_80_1024x768
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval :返回值:LCD ID:0,非法;其他值,ID; 
*******************************************************/
uint16_t LTDC_PanelID_Read(void)
{
    uint8_t idx = 0;
    GPIO_InitTypeDef GPIO_InitStruct;
	
    __HAL_RCC_GPIOG_CLK_ENABLE();       //使能GPIOG时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();       //使能GPIOI时钟
	
    GPIO_InitStruct.Pin = GPIO_PIN_6;      //PG6
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //输入
    GPIO_InitStruct.Pull = GPIO_PULLUP;    //上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; //高速
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); //初始化
	
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_7; //PI2,7
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);    //初始化
	
    idx = (uint8_t)HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6); //读取M0
    idx |= (uint8_t)HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_2) << 1; //读取M1
    idx |= (uint8_t)HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) << 2; //读取M2

    if (idx == 0)
    {
        return LCD_43_480x272;    //4.3寸屏,480*272分辨率
    }
    else if (idx == 1)
    {
        return LCD_70_800x480;    //7寸屏,800*480分辨率
    }
    else if (idx == 2)
    {
        return LCD_70_1024x600;    //7寸屏,1024*600分辨率
    }
    else if (idx == 3)
    {
        return LCD_70_1280x800;    //7寸屏,1280*800分辨率
    }
    else if (idx == 4)
    {
        return LCD_80_1024x768;    //8寸屏,1024*768分辨率
    }
    else
    {
        return 0;
    }
}

/*******************************************************
  * @brief  :LCD初始化函数 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void LTDC_Init(void)
{
    uint16_t lcdid = 0;
	
    lcdid = LTDC_PanelID_Read();			//读取LCD面板ID

    if (lcdid == LCD_43_480x272)
    {
        lcdltdc.pwidth = 480;			//面板宽度,单位:像素
        lcdltdc.pheight = 272;			//面板高度,单位:像素
        lcdltdc.hsw = 1;				//水平同步宽度
        lcdltdc.vsw = 1;				//垂直同步宽度
        lcdltdc.hbp = 40;				//水平后廊
        lcdltdc.vbp = 8;				//垂直后廊
        lcdltdc.hfp = 5;				//水平前廊
        lcdltdc.vfp = 8;				//垂直前廊
        LTDC_Clk_Set(288, 4, RCC_PLLSAIDIVR_8); //设置像素时钟 9Mhz
        //其他参数待定.
    }
    else if (lcdid == LCD_70_800x480)
    {
        lcdltdc.pwidth = 800;			//面板宽度,单位:像素
        lcdltdc.pheight = 480;			//面板高度,单位:像素
        lcdltdc.hsw = 1;				//水平同步宽度
        lcdltdc.vsw = 1;				//垂直同步宽度
        lcdltdc.hbp = 46;				//水平后廊
        lcdltdc.vbp = 23;				//垂直后廊
        lcdltdc.hfp = 210;				//水平前廊
        lcdltdc.vfp = 22;				//垂直前廊
        LTDC_Clk_Set(360, 2, RCC_PLLSAIDIVR_4); //设置像素时钟 45M
    }
    else if (lcdid == LCD_70_1024x600)
    {
        lcdltdc.pwidth = 1024;			//面板宽度,单位:像素
        lcdltdc.pheight = 600;			//面板高度,单位:像素
        lcdltdc.hsw = 20;				//水平同步宽度
        lcdltdc.vsw = 3;				//垂直同步宽度
        lcdltdc.hbp = 140;			  	//水平后廊
        lcdltdc.vbp = 20;				//垂直后廊
        lcdltdc.hfp = 160;			  	//水平前廊
        lcdltdc.vfp = 12;				//垂直前廊
        LTDC_Clk_Set(360, 2, RCC_PLLSAIDIVR_4); //设置像素时钟  45Mhz
        //其他参数待定.
    }
    else if (lcdid == LCD_70_1280x800)
    {
        lcdltdc.pwidth = 1280;			//面板宽度,单位:像素
        lcdltdc.pheight = 800;			//面板高度,单位:像素
        //其他参数待定.
    }
    else if (lcdid == LCD_80_1024x768)
    {
        lcdltdc.pwidth = 1024;			//面板宽度,单位:像素
        lcdltdc.pheight = 768;			//面板高度,单位:像素
        //其他参数待定.
    }

    lcddev.width = lcdltdc.pwidth;
    lcddev.height = lcdltdc.pheight;

	#if (LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888) || (LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888)
//    ltdc_framebuf[0] = (uint32_t *)&ltdc_lcd_framebuf;
	ltdc_framebuf[0] = (uint32_t *)LCD_FRAME_BUF_ADDR;
    lcdltdc.pixsize = 4;				//每个像素占4个字节
    #else
    lcdltdc.pixsize = 2;				//每个像素占2个字节
//    ltdc_framebuf[0] = (uint32_t *)&ltdc_lcd_framebuf;
	ltdc_framebuf[0] = (uint32_t *)LCD_FRAME_BUF_ADDR;
    #endif

    //LTDC配置
    LTDC_Handler.Instance = LTDC;
    LTDC_Handler.Init.HSPolarity = LTDC_HSPOLARITY_AL;       //水平同步极性
    LTDC_Handler.Init.VSPolarity = LTDC_VSPOLARITY_AL;       //垂直同步极性
    LTDC_Handler.Init.DEPolarity = LTDC_DEPOLARITY_AL;       //数据使能极性
    LTDC_Handler.Init.PCPolarity = LTDC_PCPOLARITY_IPC;      //像素时钟极性
    LTDC_Handler.Init.HorizontalSync = lcdltdc.hsw - 1;      //水平同步宽度
    LTDC_Handler.Init.VerticalSync = lcdltdc.vsw - 1;        //垂直同步宽度
    LTDC_Handler.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1; //水平同步后沿宽度
    LTDC_Handler.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1; //垂直同步后沿高度
    LTDC_Handler.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1; //有效宽度
    LTDC_Handler.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1; //有效高度
    LTDC_Handler.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1; //总宽度
    LTDC_Handler.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1; //总高度
    LTDC_Handler.Init.Backcolor.Red = 0;         //屏幕背景层红色部分
    LTDC_Handler.Init.Backcolor.Green = 0;       //屏幕背景层绿色部分
    LTDC_Handler.Init.Backcolor.Blue = 0xFF;        //屏幕背景色蓝色部分
    HAL_LTDC_Init(&LTDC_Handler);
	
    //层配置
    LTDC_Layer_Parameter_Config(0, (uint32_t)ltdc_framebuf[0], LCD_PIXFORMAT, 255, 0, 6, 7, 0X000000); //层参数配置
    LTDC_Layer_Window_Config(0, 0, 0, lcdltdc.pwidth, lcdltdc.pheight);	//层窗口配置,以LCD面板坐标系为基准,不要随便修改!
    //	LTDC_Layer_Parameter_Config(1,(uint32_t)ltdc_framebuf[1],LCD_PIXFORMAT,127,0,6,7,0X000000);//层参数配置
    //	LTDC_Layer_Window_Config(1,0,0,lcdltdc.pwidth,lcdltdc.pheight);	//层窗口配置,以LCD面板坐标系为基准,不要随便修改!
    LCD_Display_Dir( 1 );		//默认为横屏
    LTDC_Select_Layer(0); 			//选择第1层
	LCD_BACK_LED(1);				  		//点亮背光
    LTDC_Clear(0XFFFFFFFF);			//清屏
}

/*******************************************************
  * @brief  : LTDC底层IO初始化和时钟使能,此函数会被HAL_LTDC_Init()调用
  * @note   : 
  * @param  : hltdc:LTDC句柄
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	
    __HAL_RCC_LTDC_CLK_ENABLE();                //使能LTDC时钟
    __HAL_RCC_DMA2D_CLK_ENABLE();               //使能DMA2D时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();               //使能GPIOB时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();               //使能GPIOF时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();               //使能GPIOG时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();               //使能GPIOH时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();               //使能GPIOI时钟
	
    //背光引脚
    GPIO_InitStruct.Pin = LCD_BL_PIN;              //PB5推挽输出，控制背光
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    //推挽输出
    GPIO_InitStruct.Pull = GPIO_PULLUP;            //上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;       //高速
    HAL_GPIO_Init(LCD_BL_PORT, &GPIO_InitStruct);
    /*LTDC GPIO Configuration    
    PE5     ------> LTDC_G0
    PE6     ------> LTDC_G1
    PI9     ------> LTDC_VSYNC
    PI10     ------> LTDC_HSYNC
    PF10     ------> LTDC_DE
    PH2     ------> LTDC_R0
    PH3     ------> LTDC_R1
    PH8     ------> LTDC_R2
    PH9     ------> LTDC_R3
    PH10     ------> LTDC_R4
    PH11     ------> LTDC_R5
    PH12     ------> LTDC_R6
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PH13     ------> LTDC_G2
    PH14     ------> LTDC_G3
    PH15     ------> LTDC_G4
    PI0     ------> LTDC_G5
    PI1     ------> LTDC_G6
    PI2     ------> LTDC_G7
    PG10     ------> LTDC_B2
    PG11     ------> LTDC_B3
    PG12     ------> LTDC_B1
    PG14     ------> LTDC_B0
    PI4     ------> LTDC_B4
    PI5     ------> LTDC_B5
    PI6     ------> LTDC_B6
    PI7     ------> LTDC_B7 
    */	
	
    //初始化PF10 LCD_DE
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        //复用
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;    //复用为LTDC
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		
	//PG7 LCD_CLK	
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        //复用
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;    //复用为LTDC
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
	
    //PI9,10  LCD_VSYNC LCD_HSYNC
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        //复用
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;    //复用为LTDC
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
		
    //初始化LCD_R0~7
	//PH2,3,8,9,10,11,12
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;                       
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
	HAL_GPIO_Init( GPIOH, &GPIO_InitStruct );

	//PG6
    GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;	
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);	
	//-------------------------------------
		
	//初始化LCD_G0~7
	//PE5,6
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	
	
    //PH13,14,15
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;	
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);	
	
    //PI0,1,2
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;	
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);	
	//-------------------------------------
	
	//初始化LCD_B0~7
	//PG14,12,10,11
    GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;	
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
		
    //PI4,5,6,7
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;	
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	//------------------------------------
}

//----------------------------------------------------
