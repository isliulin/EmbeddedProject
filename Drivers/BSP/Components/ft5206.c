
//7寸电容触摸屏-FT5206 驱动代码

#include "include.h"


/* Touch screen driver structure initialization */
TS_DrvTypeDef ft5206_ts_drv =
{
    FT5206_Init,	//初始化触摸屏控制器
    FT5206_Scan,	//扫描触摸屏
    0,				//当前坐标x
    0,				//当前坐标y
    0,				//笔的状态 
    1,				//横竖屏及电容屏电阻屏状态
};



//向FT5206写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
uint8_t FT5206_WR_Reg( uint16_t reg, uint8_t *buf, uint8_t len )
{
    uint8_t i;
    uint8_t ret = 0;
    TS_IIC_Start();
    TS_IIC_Send_Byte( FT_CMD_WR );	//发送写命令
    TS_IIC_Wait_Ack();
    TS_IIC_Send_Byte( reg & 0XFF );   	//发送低8位地址
    TS_IIC_Wait_Ack();

    for ( i = 0; i < len; i++ )
    {
        TS_IIC_Send_Byte( buf[i] );  	//发数据
        ret = TS_IIC_Wait_Ack();

        if ( ret )
        {
            break;
        }
    }

    TS_IIC_Stop();					//产生一个停止条件
    return ret;
}
//从FT5206读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度
void FT5206_RD_Reg( uint16_t reg, uint8_t *buf, uint8_t len )
{
    uint8_t i;
    TS_IIC_Start();
    TS_IIC_Send_Byte( FT_CMD_WR );   	//发送写命令
    TS_IIC_Wait_Ack();
    TS_IIC_Send_Byte( reg & 0XFF );   	//发送低8位地址
    TS_IIC_Wait_Ack();
    TS_IIC_Start();
    TS_IIC_Send_Byte( FT_CMD_RD );   	//发送读命令
    TS_IIC_Wait_Ack();

    for ( i = 0; i < len; i++ )
    {
        buf[i] = TS_IIC_Read_Byte( i == ( len - 1 ) ? 0 : 1 ); //发数据
    }

    TS_IIC_Stop();//产生一个停止条件
}
//初始化FT5206触摸屏
//返回值:0,初始化成功;1,初始化失败
void FT5206_Init( void )
{
    uint8_t temp[2];
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOH时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();			//开启GPIOI时钟

    //TS_INT
    GPIO_Initure.Pin = TS_INT_PIN;          //PH7
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //输入
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init( TS_INT_PORT, &GPIO_Initure );  //初始化

   //TS_RST
    GPIO_Initure.Pin = TS_RST_PIN;          //PI8
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
    HAL_GPIO_Init( TS_RST_PORT, &GPIO_Initure );  //初始化

    TS_IIC_Init();      	//初始化电容屏的I2C总线
    FT_RST( 0 );				//复位
    Delay_ms( 20 );
    FT_RST( 1 );				//释放复位
    Delay_ms( 50 );
    temp[0] = 0;
    FT5206_WR_Reg( FT_DEVIDE_MODE, temp, 1 );	//进入正常操作模式
    FT5206_WR_Reg( FT_ID_G_MODE, temp, 1 );		//查询模式
    temp[0] = 22;								//触摸有效值，22，越小越灵敏
    FT5206_WR_Reg( FT_ID_G_THGROUP, temp, 1 );	//设置触摸有效值
    temp[0] = 12;								//激活周期，不能小于12，最大14
    FT5206_WR_Reg( FT_ID_G_PERIODACTIVE, temp, 1 );
    //读取版本号，参考值：0x3003
    FT5206_RD_Reg( FT_ID_G_LIB_VERSION, &temp[0], 2 );

    if ( ( temp[0] == 0X30 && temp[1] == 0X03 ) || temp[1] == 0X01 || temp[1] == 0X02 ) //版本:0X3003/0X0001/0X0002
    {
       
    }   
}

const uint16_t FT5206_TPX_TBL[5] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};
//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t FT5206_Scan( uint8_t mode )
{
    uint8_t buf[4];
    uint8_t i = 0;
    uint8_t res = 0;
    uint8_t temp;
    static uint8_t t = 0; //控制查询间隔,从而降低CPU占用率
    t++;

    if ( ( t % 10 ) == 0 || t < 10 ) //空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
    {
        FT5206_RD_Reg( FT_REG_NUM_FINGER, &mode, 1 ); //读取触摸点的状态

        if ( ( mode & 0XF ) && ( ( mode & 0XF ) < 6 ) )
        {
            temp = 0XFF << ( mode & 0XF ); //将点的个数转换为1的位数,匹配ts_driver->ucStatus定义
            ts_driver->ucStatus = ( ~temp ) | TS_PRES_DOWN | TS_CATH_PRES;

            for ( i = 0; i < 5; i++ )
            {
                if ( ts_driver->ucStatus & ( 1 << i ) )		//触摸有效?
                {
                    FT5206_RD_Reg( FT5206_TPX_TBL[i], buf, 4 );	//读取XY坐标值

                    if ( ts_driver->ucTouchType & 0X01 ) //横屏
                    {
                        ts_driver->y[i] = ( ( uint16_t )( buf[0] & 0X0F ) << 8 ) + buf[1];
                        ts_driver->x[i] = ( ( uint16_t )( buf[2] & 0X0F ) << 8 ) + buf[3];
                    }

                    else
                    {
                        ts_driver->x[i] = lcddev.width - ( ( ( uint16_t )( buf[0] & 0X0F ) << 8 ) + buf[1] );
                        ts_driver->y[i] = ( ( uint16_t )( buf[2] & 0X0F ) << 8 ) + buf[3];
                    }

                    if ( ( buf[0] & 0XF0 ) != 0X80 )
                    {
                        ts_driver->x[i] = ts_driver->y[i] = 0;    //必须是contact事件，才认为有效
                    }

                    //App_Printf("x[%d]:%d,y[%d]:%d\r\n",i,ts_driver->x[i],i,ts_driver->y[i]);
                }
            }

            res = 1;

            if ( ts_driver->x[0] == 0 && ts_driver->y[0] == 0 )
            {
                mode = 0;    //读到的数据都是0,则忽略此次数据
            }

            t = 0;		//触发一次,则会最少连续监测10次,从而提高命中率
        }
    }

    if ( ( mode & 0X1F ) == 0 ) //无触摸点按下
    {
        if ( ts_driver->ucStatus & TS_PRES_DOWN )	//之前是被按下的
        {
            ts_driver->ucStatus &= ~( 1 << 7 );	//标记按键松开
        }

        else						//之前就没有被按下
        {
            ts_driver->x[0] = 0xffff;
            ts_driver->y[0] = 0xffff;
            ts_driver->ucStatus &= 0XE0;	//清除点有效标记
        }
    }

    if ( t > 240 )
    {
        t = 10;    //重新从10开始计数
    }

    return res;
}




























