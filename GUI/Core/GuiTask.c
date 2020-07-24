/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"

//TF卡根目录文件名，字体文件
char *g_pcFontFileName = "MSYH_Font.bin";



extern WM_HWIN CreateWindow(void); //在文件MainWindos.c里

/*
*********************************************************************************************************
*	                                  定义XBF格式字体
*********************************************************************************************************
*/
// 16点阵定义
GUI_XBF_DATA XBF_Data16;
GUI_FONT XBF_Font16;
void *Fontfile16;      //使用文件系统时需要

// 24点阵定义
GUI_XBF_DATA XBF_Data24;
GUI_FONT XBF_Font24;
void *Fontfile24;       //使用文件系统时需要

// 32点阵定义
GUI_XBF_DATA XBF_Data32;
GUI_FONT XBF_Font32;
void *Fontfile32; //使用文件系统时需要


FontInfo_T tFontInfo;



extern void emwin_texttest(void);
extern void batTest(void);

extern WM_HWIN CreateWindow(void); //

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void CreatDeskTop(void)
{
    WM_HTIMER hTimer;
    WM_HWIN hWinMain;

    hWinMain = CreateWindow();

    if (g_ucPulseWidth < 1)
    {
        g_ucPulseWidth = 1;
    }
    hTimer = WM_CreateTimer(WM_GetClientWindow(hWinMain), 0, g_ucPulseWidth * 1000, 0); //创建一个窗口定时器
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void vGUITask(void *pvParameters)
{
    uint8_t ucSta = 0;

    //(void)pvParameters;

    __HAL_RCC_CRC_CLK_ENABLE();      //使能CRC时钟
    WM_SetCreateFlags(WM_CF_MEMDEV); //开启emWin存储设备
    GUI_Init();
    WM_MULTIBUF_Enable(1); //开启emWin多缓冲，RGB屏会用到

    GUI_UC_SetEncodeUTF8();                               //使能 UTF-8 编码
    GUI_SetXBF();                                         // 创建XBF字体（文件编码要为UTF8）
    	

//    xSemaphoreTake(xFontSemaphore_Handle, portMAX_DELAY); //获取信号量，在guiCheckFont释放
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待任务通知，在guiCheckFont函数释放，代替上面注释的二进制信号量
    

    //GUI_CURSOR_Show();//显示光标

    //WM_SetDesktopColor(GUI_WHITE); //调用此函数会自动的刷新桌面窗口

    GUI_SetBkColor(GUI_DARKBLUE); //设置背景颜色
    GUI_Clear();                  //清屏
    GUI_SetFont(&XBF_Font24);     //设置字体
    GUI_SetColor(GUI_WHITE);      //设置前景色(如文本，画线等颜色)    
    
    
    //    emwin_texttest();   

   //CreatDeskTop();
    //
    for (;;)
    {
        batTest();
        GUI_Delay(10000);
    }
}


/*******************************************************
  * @brief  : XBF字体的回调函数, 16点阵
  * @note   : 
  * @param  : Off      - 地址偏移
  * @param  : NumBytes - 需要读出的字节数
  * @param  : pVoid    - 指针变量，一般用于带文件系统时的FIL类型变量
  * @param  :  pBuffer  - 获取字体的点阵数据
  * @retval : 0 表示成功 1 表示失败
*******************************************************/
static int _cbGetData16(U32 Off, U16 NumBytes, void *pVoid, void *pBuffer)
{
    // 读取点阵数据
    MX25Lxx_Read(pBuffer, XBF_FONT16_BASE_ADDRESS + sizeof(tFontInfo) + Off, NumBytes);
    return 0;
}

/*******************************************************
  * @brief  : XBF字体的回调函数, 24点阵
  * @note   : 
  * @param  : Off      - 地址偏移
  * @param  : NumBytes - 需要读出的字节数
  * @param  : pVoid    - 指针变量，一般用于带文件系统时的FIL类型变量
  * @param  :  pBuffer  - 获取字体的点阵数据
  * @retval : 0 表示成功 1 表示失败
*******************************************************/
static int _cbGetData24(U32 Off, U16 NumBytes, void *pVoid, void *pBuffer)
{
    // 读取点阵数据
    MX25Lxx_Read(pBuffer, XBF_FONT24_BASE_ADDRESS + sizeof(tFontInfo) + Off, NumBytes);
    return 0;
}

/*******************************************************
  * @brief  : XBF字体的回调函数, 32点阵
  * @note   : 
  * @param  : Off      - 地址偏移
  * @param  : NumBytes - 需要读出的字节数
  * @param  : pVoid    - 指针变量，一般用于带文件系统时的FIL类型变量
  * @param  :  pBuffer  - 获取字体的点阵数据
  * @retval : 0 表示成功 1 表示失败
*******************************************************/
static int _cbGetData32(U32 Off, U16 NumBytes, void *pVoid, void *pBuffer)
{
    // 读取点阵数据
    MX25Lxx_Read(pBuffer, XBF_FONT32_BASE_ADDRESS + sizeof(tFontInfo) + Off, NumBytes);
    return 0;
}

/*******************************************************
  * @brief  : 创建XBF字体
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void GUI_SetXBF(void)
{
    //创建16点阵字体
    GUI_XBF_CreateFont(
        &XBF_Font16,       // GUI_FONT类型变量
        &XBF_Data16,       // GUI_XBF_DATA类型变量
        GUI_XBF_TYPE_PROP, // 字体类型
        _cbGetData16,      // 回调函数
        &Fontfile16);      // 回调函数参数

    // 创建24点阵字体
    GUI_XBF_CreateFont(
        &XBF_Font24,       // GUI_FONT类型变量
        &XBF_Data24,       // GUI_XBF_DATA类型变量
        GUI_XBF_TYPE_PROP, // 字体类型
        _cbGetData24,      // 回调函数
        &Fontfile24);      // 回调函数参数

    // 创建32点阵字体
    GUI_XBF_CreateFont(
        &XBF_Font32,       // GUI_FONT类型变量
        &XBF_Data32,       // GUI_XBF_DATA类型变量
        GUI_XBF_TYPE_PROP, // 字体类型
        _cbGetData32,      // 回调函数
        &Fontfile32);      // 回调函数参数
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void guiCheckFont(void)
{
    char pcFontPath[30] = {0};
    uint32_t ulByteRead;
    uint32_t ulWriteAdd, ulIncrement = 0;
    uint8_t ucaTempBuf[4096];
    FRESULT eResult;
    FIL Font_File;
    
    const TickType_t xWaitTicks = pdMS_TO_TICKS(300); // 设置最大等待时间为 300ms

    MX25Lxx_Read((uint8_t *)&tFontInfo, FONT_INFO_ADDRESS, sizeof(tFontInfo));
    if (tFontInfo.ucFontInfo != 0xAA) //字体不存在
    {
        //读出SD卡里的字体文件,写入外部Flash
        strcpy((char *)pcFontPath, (char const *)g_caSDPath);
        strcat((char *)pcFontPath, (char *)g_pcFontFileName);

        ulWriteAdd = (uint32_t)FONT_INFO_ADDRESS + sizeof(tFontInfo) + XBF_FONT16_BASE_ADDRESS; //字库写入FLASH首地址
        tFontInfo.ulFont16Add = XBF_FONT16_BASE_ADDRESS + sizeof(tFontInfo);
        tFontInfo.ulFont24Add = XBF_FONT24_BASE_ADDRESS + sizeof(tFontInfo);
        tFontInfo.ulFont32Add = XBF_FONT32_BASE_ADDRESS + sizeof(tFontInfo);

		xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
        HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
        
		eResult = f_open(&Font_File, pcFontPath, FA_OPEN_EXISTING | FA_READ); //打开文件,如果文件不存在,则打开失败。
        if (eResult == FR_OK)
        {
            for (;;)
            {
                eResult = f_read(&Font_File, ucaTempBuf, 4096, (UINT *)&ulByteRead); //读取数据
                if ((eResult != FR_OK) || (ulByteRead == 0))
                {
                    break;
                }
                MX25Lxx_Write(ucaTempBuf, ulWriteAdd + ulIncrement, 4096); //写入4096个数据，第一次从tFontInfo后面开始
                ulIncrement += ulByteRead;

                if (ulByteRead != 4096)
                {
                    break; //读完了.
                }
            }
            f_close(&Font_File);
        }

		xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取

        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
        
        //全部更新完成
        tFontInfo.ucFontInfo = 0xAA;
        MX25Lxx_Write((uint8_t *)&tFontInfo, FONT_INFO_ADDRESS, sizeof(tFontInfo)); //保存字库信息

//        xSemaphoreGive(xFontSemaphore_Handle); //释放信号量，在vGUITask、AutoReloadCallback获取
        xTaskNotifyGive(xGUITask_Handle); //发送任务通知给vGUITask，用来代替上面注释的二值信号量
    }
    else
    {
//        xSemaphoreGive(xFontSemaphore_Handle); //释放信号量，在vGUITask、AutoReloadCallbackk获取
        xTaskNotifyGive(xGUITask_Handle); //发送任务通知给vGUITask，用来代替上面注释的二值信号量
    }	
}



//---------------------------------------------------------------------------------










