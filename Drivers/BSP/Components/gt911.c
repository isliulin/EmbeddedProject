/**
  ******************************************************************************
    @file		gt911.c
    @author
    @version
    @date
    @brief		7寸电容触摸屏gt911 驱动代码
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"


/* Touch screen driver structure initialization */
TS_DrvTypeDef gt911_ts_drv =
{
    GT911_Init,		//初始化触摸屏控制器
    GT911_Scan,		//扫描触摸屏
    0,				//当前坐标x
    0,				//当前坐标y
    0,				//笔的状态 
    1,				//横竖屏及电容屏电阻屏状态
};

// emWin要用到的触摸参数，在函GT911_Init初始为给图层0发送触摸数据
//GUI_PID_STATE State = {0};

// GT911单个触点配置参数，一次性写入, GT911配置信息  186
/*
uint8_t ucGT911_CFG_TBL[] =
{
    0x00,		//0x8047		版本号
    0xE0, 0x01,	//0x8048/8049	X坐标输出最大值480
    0x20, 0x03,	//0x804a/804b	Y坐标输出最大值800
    0x05,	//0x804c	输出触点个数上限
    0x35,	//0x804d	自定义拉伸,坐标不交换,软件降噪,INT 触发方式:下降沿触发
    0x02,	//0x804e
    0x02,	//0x804f	手指按下去抖动次数
    0x3F,	//0x8050	原始坐标窗口滤波值
    0x19,	//0x8051	大面积触点个数
    0x0F,	//0x8052	噪声消除值
    0x78,	//0x8053	屏上触摸点从无到有的阈值
    0x6E,	//0x8054	屏上触摸点从有到无的阈值
    0x05,	//0x8055	进低功耗时间	s
    0x05,	//0x8056	坐标上报率
    0x00,	//0x8057	Reserved
    0x00,	//0x8058	Reserved
    0x00, 0X00,	//0x8059-0x805a	reserved
    0x00,	//0x805b	上下边框的空白区（以 32 为系数）
    0x00,	//0x805c	左右边框的空白区（以 32 为系数）
    0x00,	//0x805d	划线过程中小filter设置
    0x18,	//0x805e	拉伸区间	1	系数
    0x1A,	//0x805f	拉伸区间	2	系数
    0x1E,	//0x8060	拉伸区间	3	系数
    0x14,	//0x8061	各拉伸区间基数
    0x8A,	//0x8062	Drv_GroupA_Num
    0x2A,	//0x8063	Drv_GroupB_Num
    0xCC,	//0x8064	Sensor_Num*
    0x3C,	//0x8065	驱动组A的驱动频率倍频系数
    0x3A,	//0x8066	驱动组B的驱动频率倍频系数
    0xB3,	//0x8067	驱动组A、B的基频
    0x15,	//0x8068
    0x00,	//0x8069	相邻两次驱动信号输出时间间隔
    0x00,	//0x806a
    0x00,	//0x806b	、、
    0xDB,	//0x806c	、、
    0x02,	//0x806d	原始值放大系数
    0x3D,	//0x806e	、、
    0x00,	//0x806f	reserved
    0x00,	//0x8070	、、
    0x00, 	//0x8071
    0x00,	//0x8072    reserved
    0x00,	//0x8073
    0x00, 	//0x8074	笔原始值放大系数（2 的 N 次方）
    0x00, 	//0x8075	笔触摸有效阈值（驱动方向）
    0x00, 	//0x8076	笔触摸有效阈值（感应方向）
    0x00, 	//0x8077	触摸笔超时退出时间（以秒为单位）
    0x02, 	//0x8078	削底系数 N，削底量=Base+Base/2 的 N次方（通常 2-4）
    0x00,	//0x8079 	reserved
    0x28,	//0x807a	跳频范围的起点频率( 以 2KHz 为单位，例如 50 表示 100KHz )
    0x96,	//0x807b	跳频范围的终点频率( 以 2KHz 为单位，例如 150 表示 300KHz )
    0x82,	//0x807c	多次噪声检测后确定噪声量,1-63 有效， 建议 20
    0x00,	//0x807d	噪声检测超时时间，以秒为单位
    0x05,	//0x807e	最优频率选定条件，当前工作频率干扰量－最小干扰量>设定值 x4，则选定最优频率和跳频
    0x64,	//0x807f	判别有干扰的门限
    0x00, 0x00,	//0x8080-0x8081	reserved
    0xF6,	//0x8082	跳频检测区间频段1中心点基频（适用于驱动A、B）
    0x16,	//0x8083
    0x0E,	//0x8084	跳频检测区间频段1中心点倍频系数
    0xE4,	//0x8085	跳频检测区间频段2中心点基频(驱动A、B在此基础上换算)
    0x1B,	//0x8086
    0x11,	//0x8087	跳频检测区间频段2中心点倍频系数
    0x3A,	//0x8088	跳频检测区间频段3中心点基频（适用于驱动A、B）
    0x1F,	//0x8089
    0x14,	//0x808a	跳频检测区间频段3中心点倍频系数
    0x31,	//0x808b	跳频检测区间频段4中心点基频（适用于驱动A、B）
    0x19,	//0x808c
    0x20,	//0x808d	跳频检测区间频段4中心点倍频系数
    0x08,	//0x808e	跳频检测区间频段5中心点基频（适用于驱动A、B）
    0x1E,	//0x808f
    0x22,	//0x8090	跳频检测区间频段5中心点倍频系数
    0x00, 0x00,		//0x8091,0x8092  reserved
    0x00,	//0x8093	key1位置	0：无按键
    0x00,	//0x8094	key2位置	0：无按键
    0x00,	//0x8095	key3位置	0：无按键
    0x00,	//0x8096	key4位置	0：无按键
    0x22,	//0x8097	长按更新时间(1~16s) 按键有效区间设置(单侧):0-15 有效
    0x10,	//0x8098	触摸按键按键阈值
    0x10,	//0x8099	触摸按键松键阈值
    0x55,	//0x809a	KeySens_1(按键 1 灵敏度系数） KeySens_2（按键 2 灵敏度系数）
    0x55,	//0x809b	KeySens_3(按键 3 灵敏度系数） KeySens_4（按键 4 灵敏度系数）
    0x07,	//0x809c	手指从屏上离开后抑制按键的时间（以100ms 为单位）, 0 表示 600ms 抑制
    0x00, 0x00, 0x00, 0x00, 0x00, //0x809d-0x80a1	reserved
    0x00, 	//0x80a2	Drv_Start_Ch(驱动方向起始通道） Drv_End_Ch（结束通道，为起始通道加此值）
    0x00, 	//0x80a3	Sens_Start_Ch(感应方向起始通道） Sens_End_Ch（结束通道，为起始通道加此值）
    0x00, 	//0x80a4	设定值× 10=接近感应生效阈值
    0x00, 	//0x80a5	设定值× 10=接近感应无效阈值
    0x00, 	//0x80a6	采样值累加次数
    0x00, 	//0x80a7
    0x00, 	//0x80a8
    0x00, 	//0x80a9
    0x00,	//0x80aa
    0x00, 	//0x80ab
    0x00, 	//0x80ac
    0x00, 	//0x80ad
    0x00, 	//0x80ae	自电容 Touch Level(驱动方向)
    0x00, 	//0x80af
    0x00, 	//0x80b0
    0x00, 	//0x80b1
    0x00,	//0x80b2
    0x00, 0x00, 0x00, 	//0x80b3~0x80b5		reserved
    0x00,	//0x80b6
    0x02,	//0x80b7	//ITO Sensor0 对应的芯片通道号0,总共30个通道号到0x80d4,用到的14个
    0x04,	//0x80b8	//ITO Sensor0 对应的芯片通道号1
    0x06,	//0x80b9
    0x08,	//0x80ba
    0x0A,	//0x80bb
    0x0C,	//0x80bc
    0x0E,	//0x80bd
    0x10,	//0x80be
    0x12,	//0x80bf
    0x14,	//0x80c0
    0x16,	//0x80c1
    0x18,	//0x80c2
    0x1A,	//0x80c3
    0x1C,	//0x80c4  
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,	//0x80c5-0x80d4	reserved
    0x00,	//0x80d5	//ITO Driver0 对应的芯片通道号0,总共42到0x80fe，用到26个
    0x02,	//0x80d6	//ITO Driver0 对应的芯片通道号1
    0x04,	//0x80d7
    0x06,	//0x80d8
    0x08,	//0x80d9
    0x0A,	//0x80da
    0x0C,	//0x80db
    0x0E,	//0x80dc
    0x10,	//0x80dd
    0x12,	//0x80de
    0x14,	//0x80df
    0x16,	//0x80e0
    0x18,	//0x80e1
    0x1A,	//0x80e2
    0x1C,	//0x80e3
    0x1E,	//0x80e4
    0x20,	//0x80e5
    0x22,	//0x80e6  
    0x24,	//0x80e7
    0x26,	//0x80e8
    0x28,	//0x80e9
    0x2A,	//0x80ea
    0x2C,	//0x80eb
    0x2E,	//0x80ec
    0x30,	//0x80ed
    0x32,	//0x80ee
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,	//0x80ef-0x80fe	reserved
    0xD1,	//0x80ff	配置信息校验(0x8047 到 0x80FE 之字节和的补码)
    0x01	//0x8100	配置已更新标记(由主控写入标记)
};
*/
  uint8_t ucGT911_CFG_TBL[186]={
	0x00,0x00,0x04,0x58,0x02,0x0A,0x0D,0x00,
	0x01,0x08,0x28,0x05,0x50,0x32,0x03,0x05,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x8C,0x2E,0x0E,0x27,0x24,
	0xD0,0x07,0x00,0x00,0x01,0x99,0x04,0x1D,
	0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,
	0x00,0x00,0x00,0x0F,0x23,0x94,0xC5,0x02,
	0x07,0x00,0x00,0x04,0xA2,0x10,0x00,0x8C,
	0x13,0x00,0x7C,0x16,0x00,0x68,0x1B,0x00,
	0x5C,0x20,0x00,0x5C,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x25,0x14,0x04,0x14,
	0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
	0x12,0x14,0x16,0x18,0x1A,0x1C,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
	0x04,0x06,0x08,0x0A,0x0C,0x0F,0x10,0x12,
	0x13,0x14,0x16,0x18,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x24,0x26,0x28,0x29,0x2A,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x43,0x01
    };




//extern GUI_PID_STATE State;

static uint8_t GT911_WriteReg( uint16_t usRegAddr, uint8_t *pucRegBuf, uint8_t ucLen );
static void GT911_ReadReg( uint16_t usRegAddr, uint8_t *pucRegBuf, uint8_t ucLen );
static void GT911_RST_INT_PIN_Init( void );
static void GT911_INT_PIN_InputSuspend_Init( void );
static void GT911_Reset_Sequence( uint8_t ucAddr );
static void GT911_Soft_Reset( void );
static uint8_t GT911_ReadStatue( void );
static void GT911_Exti_Int( void );
static uint16_t GT911_ReadFirmwareVersion( void );
static uint32_t GT911_ReadProductID( void );
void GT911_Init( void );
uint8_t GT911_OnePiontScan( uint8_t mode );
uint8_t GT911_Scan( uint8_t ucMode );

/*******************************************************
    @brief  : 初始化GT911触摸芯片
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void GT911_Init( void )
{
    uint8_t ucConfigChecksum = 0, i;
    uint8_t ucIDBuf[5] = {0};
    //	uint8_t ucCFG[186] = {0};

    TS_IIC_Init();	//IIC接口初始化

    //emWin默认是发给图层1，如果是发给图层2，修改Layer参数为1
    //    State.Layer = 0;
  
    GT911_Reset_Sequence( GT911_IIC_ADDR ); //复位GT911，设定设备地址为0xBA/0xBB

    GT911_Soft_Reset();  //软复位
    Delay_ms( 100 );

    GT911_ReadReg( GT911_PRODUCT_ID_REG, ucIDBuf, 4 );  //0x8140读取产品ID
    GT911_ReadReg( GT911_FIRMWARE_VERSION_REG, ucIDBuf, 2 ); // 固件版本号
    GT911_ReadReg( GT911_VENDOR_ID_REG, ucIDBuf, 1 ); // 当前模组选项信息
    //	GT911_ReadReg( GT911_CONFIG_VERSION_REG, ucCFG, 186 );//0x8047读取配置信息

    //0x8047读取配置文件版本
    ucGT911_CFG_TBL[0] = GT911_ReadStatue();

    if ( ucGT911_CFG_TBL[0] < 0x40 )	//配置参数版本号低于此值时才重新配置
    {
        //计算校验和,0x8047 到 0x80FE 之字节和的补码
        for ( i = 0; i < ( sizeof( ucGT911_CFG_TBL ) - 2 ); i++ )	//前184个
        {
            ucConfigChecksum += ucGT911_CFG_TBL[i];
        }

        ucGT911_CFG_TBL[184] = ( ~ucConfigChecksum ) + 1;

        //发送配置信息参数
        GT911_WriteReg( GT911_CONFIG_REG, ( uint8_t * )ucGT911_CFG_TBL, sizeof( ucGT911_CFG_TBL ) );
    }

    GT911_Soft_Reset();  //软复位

    //初始化校准，等待200ms
    Delay_ms( 200 );
}

/*******************************************************
    @brief  : 写1个或连续的多个寄存器
    @note   :
    @param  : usRegAddr : 寄存器地址
    @param  : pucRegBuf : 寄存器数据缓冲区
    @param  : ucLen : 数据长度
    @retval :
*******************************************************/
static uint8_t GT911_WriteReg( uint16_t usRegAddr, uint8_t *pucRegBuf, uint8_t ucLen )
{
    uint8_t i;
    uint8_t ucStatus = 0;

    TS_IIC_Start();					//总线开始信号

    TS_IIC_Send_Byte( GT911_IIC_ADDR );	//发送设备地址(写命令)
    TS_IIC_Wait_Ack();

    TS_IIC_Send_Byte( usRegAddr >> 8 );	//地址高8位
    TS_IIC_Wait_Ack();

    TS_IIC_Send_Byte( usRegAddr & 0xff );		//地址低8位
    TS_IIC_Wait_Ack();

    for ( i = 0; i < ucLen; i++ )
    {
        TS_IIC_Send_Byte( pucRegBuf[i] );		//寄存器数据
        ucStatus = TS_IIC_Wait_Ack();

        if ( ucStatus )
        {
            break;
        }
    }

    TS_IIC_Stop();                   			//总线停止信号
    return ucStatus;
}

/*******************************************************
    @brief  : 读1个或连续的多个寄存器
    @note   :
    @param  : usRegAddr : 寄存器地址
    @param  : pucRegBuf : 寄存器数据缓冲区
    @param  : ucLen : 数据长度
    @retval :
*******************************************************/
static void GT911_ReadReg( uint16_t usRegAddr, uint8_t *pucRegBuf, uint8_t ucLen )
{
    uint8_t i;
    uint8_t ack1 = 0, ack2 = 0, ack3 = 0, ack4 = 0;

    TS_IIC_Start();					//总线开始信号

    TS_IIC_Send_Byte( GT911_IIC_ADDR );	//发送设备地址+写信号
    ack1 = TS_IIC_Wait_Ack();

    TS_IIC_Send_Byte( usRegAddr >> 8 );	//地址高8位
    ack2 = TS_IIC_Wait_Ack();

    TS_IIC_Send_Byte( usRegAddr & 0xff );		//地址低8位
    ack3 = TS_IIC_Wait_Ack();
    TS_IIC_Stop();

    TS_IIC_Start();
    TS_IIC_Send_Byte( GT911_IIC_ADDR + 0x01 );	//发送设备地址+读信号
    ack4 = TS_IIC_Wait_Ack();

    for ( i = 0; i < ucLen; i++ )
    {
        //后续还有数据,要发送应答信号,读取到最后一个寄存器则不发送应答信号
        pucRegBuf[i] = TS_IIC_Read_Byte( i == ( ucLen - 1 ) ? 0 : 1 ); //发数据
    }

    TS_IIC_Stop();							//总线停止信号
}

/*******************************************************
    @brief  : 初始化RST引脚为推挽输出，INT引脚为开漏输出
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void GT911_RST_INT_PIN_Init( void )
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();			//开启GPIOE时钟

    //TS_RST
    GPIO_Initure.Pin = TS_RST_PIN;          //
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
    GPIO_Initure.Pull = GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init( TS_RST_PORT, &GPIO_Initure );  //初始化

    //TS_INT
    GPIO_Initure.Pin = TS_INT_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;  //开漏模式输出
    GPIO_Initure.Pull = GPIO_PULLDOWN;          //下拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init( TS_INT_PORT, &GPIO_Initure );   //初始化
}

/*******************************************************
    @brief  : 把INT引脚设置为悬空输入模式
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void GT911_INT_PIN_InputSuspend_Init( void )
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();			//开启GPIOE时钟

    //TS_INT
    GPIO_Initure.Pin = TS_INT_PIN;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;				//开漏模式输出
    GPIO_Initure.Pull = GPIO_NOPULL;          			//上下拉电阻不使能
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     			//高速
    HAL_GPIO_Init( TS_INT_PORT, &GPIO_Initure );   		//初始化

}

/*******************************************************
    @brief  : G911硬复位操作,RST为低电平时,INT持续为低电平,1ms后RST置为高电平,10ms后INT设置为输入,
			  使GT911地址设定为0xBA/0xBB。
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void GT911_Reset_Sequence( uint8_t ucAddr )
{
    GT911_RST_INT_PIN_Init();

    switch ( ucAddr )
    {
        case 0xBA:
            GT911_RST_L();			//RST引脚低电平
            GT911_INT_L();			//INT引脚低电平
            Delay_ms( 30 ); 		//延时30ms，最短1
            GT911_RST_H();			//RST引脚高电平
            Delay_ms( 30 );			//延时30ms，最短5
            break;

        case 0x28:
            GT911_RST_L();          //RST引脚低电平
            GT911_INT_H();          //INT引脚高电平
            Delay_ms( 30 );         //延时30ms，最短1
            GT911_RST_H();          //RST引脚高电平
            GT911_INT_H();          //INT引脚高电平
            Delay_ms( 30 );         //延时30ms，最短20
            GT911_INT_L();
            Delay_ms( 30 );          //延时30ms，最短20
            GT911_INT_H();
            break;

        default:    //缺省为0xBA
            GT911_RST_L();			//RST引脚低电平
            GT911_INT_L();			//INT引脚低电平
            Delay_ms( 30 ); 		//延时30ms，最短1
            GT911_RST_H();			//RST引脚高电平
            Delay_ms( 30 );			//延时30ms，最短5
            break;
    }

    GT911_INT_PIN_InputSuspend_Init();  //设定INT引脚为输入悬空
    Delay_ms( 100 );
}

/*******************************************************
    @brief  : G911软复位操作。
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void GT911_Soft_Reset( void )
{
    uint8_t ucBuf[2];
    ucBuf[0] = 0x02;
    GT911_WriteReg( GT911_COMMAND_REG, ( uint8_t * )ucBuf, 1 );
}

/*******************************************************
    @brief  :
    @note   : G911读出产品ID与配置参数的版本号
    @param  :
    @param  :
    @param  :
    @retval : 配置参数的版本号
*******************************************************/
static uint8_t GT911_ReadStatue( void )
{
    uint8_t ucBuf[5] = {0};

    GT911_ReadReg( GT911_PRODUCT_ID_REG, ucBuf, 3 );
    GT911_ReadReg( GT911_CONFIG_VERSION_REG, ( uint8_t * )&ucBuf[3], 1 );
    return ucBuf[3];
}

/*******************************************************
    @brief  : G911中断引脚初始化
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void GT911_Exti_Int( void )
{

}

/*******************************************************
    @brief  : 获得GT911的芯片固件版本
    @note   :
    @param  :
    @param  :
    @param  :
    @retval : 16位版本号
*******************************************************/
static uint16_t GT911_ReadFirmwareVersion( void )
{
    uint8_t ucBuf[2];

    GT911_ReadReg( GT911_FIRMWARE_VERSION_REG, ucBuf, 2 );

    return ( ( uint16_t )ucBuf[1] << 8 ) + ucBuf[0];
}

/*******************************************************
    @brief  : 读取GT911触摸数据，这里仅读取一个触摸点
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
uint8_t GT911_OnePiontScan( uint8_t mode )
{
    return 1;
}

/*******************************************************
    @brief  : 读取GT911触摸数据。读取全部的数据。
    @note   :
    @param  :
    @param  :
    @param  :
    @retval ://返回值:当前触屏状态.0-触屏无触摸;1-触屏有触摸
*******************************************************/
uint8_t GT911_Scan( uint8_t ucMode )
{
    uint8_t ucBuf[4] = {0}, ucStaBuf[1] = {0};
    uint8_t ucClearBuf[1] = {0};
    uint8_t i;
    uint8_t ucStatus, ucTempStatus;
    uint8_t ucTpDown = 0;
	static uint8_t ucCount = 0;	//控制查询间隔,从而降低CPU占用率  
	uint16_t GT911_TPX_TBL[5]={GT911_TP1_REG, GT911_TP2_REG, GT911_TP3_REG, GT911_TP4_REG, GT911_TP5_REG};
	
	ucCount++;
	
	if( ((ucCount % 10) == 0) || (ucCount < 10))	//空闲时,每进入10次函数才检测1次,从而节省CPU使用率
	{
		GT911_ReadReg( GT911_READ_XY_REG, ucStaBuf, 1 );

		if ( ( (ucStaBuf[0] & 0x80 ) != 0x80) && ( ( ucStaBuf[0] & 0x0F ) <= TS_MAX_TOUCH_POINT ) )	//Buffer status未就绪,但有触摸点
		{			
			GT911_WriteReg( GT911_CLEARBUF_REG, ucClearBuf, 1 );	//清标志
			return ucTpDown;
		}
		if ( ( ( ucStaBuf[0] & 0x0F )  ) && ( ( ucStaBuf[0] & 0x0F ) <= TS_MAX_TOUCH_POINT )) //有触摸且少于6个点
		{
			ucStatus = 0xFF << ( ucStaBuf[0] & 0x0F );	//将点的个数转换为1的位数,匹配ts_driver->ucStatus定义
			ucTempStatus = ts_driver->ucStatus;			//保存当前的ts_driver->ucStatus值
			ts_driver->ucStatus = ( ~ucStatus ) | TS_PRES_DOWN | TS_CATH_PRES; 
		   
			ts_driver->x[4] = ts_driver->x[0];	//保存触点0的数据
			ts_driver->y[4] = ts_driver->y[0];

			for ( i = 0; i < 5; i++ )
			{
				if ( ts_driver->ucStatus & ( 1 << i ) )	//触摸有效?
				{
					GT911_ReadReg( GT911_TPX_TBL[i], ucBuf, 4 );	//读取XY坐标值					
					GT911_WriteReg( GT911_CLEARBUF_REG, ucClearBuf, 1 );	//清标志
					if(ts_driver->ucTouchType & 0x01)//横屏
					{	
						ts_driver->y[i] = ((uint16_t)ucBuf[1] << 8) + ucBuf[0];
						ts_driver->x[i] = ((uint16_t)ucBuf[3] << 8) + ucBuf[2];											
					}
					else
					{
						ts_driver->x[i] = ((uint16_t)ucBuf[1] << 8) + ucBuf[0];
						ts_driver->y[i] = ((uint16_t)ucBuf[3] << 8) + ucBuf[2];
					} 						
				}				
			}
			
			ucTpDown = 1;

			if ( ts_driver->x[0] > lcddev.width || ts_driver->y[0] > lcddev.height ) //非法数据(坐标超出了)
			{
				if ( ( ucMode & 0x0F ) > 1 )		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					ts_driver->x[0] = ts_driver->x[1];
					ts_driver->y[0] = ts_driver->y[1];
					ucCount = 0;				//触发一次,则会最少连续监测10次,从而提高命中率
				}
				else					//非法数据,则忽略此次数据(还原原来的)
				{
					ts_driver->x[0] = ts_driver->x[4];
					ts_driver->y[0] = ts_driver->y[4];
					ucMode = 0x80;
					ts_driver->ucStatus = ucTempStatus;	//恢复ts_driver->ucStatus
				}
			}
			else
			{
				ucCount = 0;    //触发一次,则会最少连续监测10次,从而提高命中率
			}
		}	
		if((ucStaBuf[0] & 0x8F) == 0x80)	//无触摸点按下
		{ 
			if(ts_driver->ucStatus & TS_PRES_DOWN)	//之前是被按下的
			{
				ts_driver->ucStatus &= ~(1 << 7);	//标记按键松开
			}
			else						//之前就没有被按下
			{ 
				ts_driver->x[0] = 0xffff;
				ts_driver->y[0] = 0xffff;
				ts_driver->ucStatus &= 0xE0;	//清除点有效标记	
			}	 
		} 	
		if(ucCount > 240)
		{
			ucCount = 10;//重新从10开始计数
		}		
	}
	GT911_WriteReg( GT911_CLEARBUF_REG, ucClearBuf, 1 );	//清标志
	return ucTpDown;
}

/*******************************************************
    @brief  : 识别显示模块类别。读取GT911 ProductID。
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static uint32_t GT911_ReadProductID( void )
{
    uint8_t ucBuf[4];
    uint32_t ulIDValue = 0;

    //Product_ID
    GT911_ReadReg( GT911_PRODUCT_ID_REG, ucBuf, 4 );
    ulIDValue = ( ( uint32_t )ucBuf[3] << 24 ) + ( ( uint32_t )ucBuf[2] << 16 ) + ( ( uint32_t )ucBuf[1] << 8 ) + ucBuf[0];
    return ulIDValue;
}



//----------------------------------------------
