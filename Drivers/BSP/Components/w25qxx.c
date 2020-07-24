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

uint16_t W25QXX_TYPE = W25Q256;	//默认是W25Q256
uint8_t W25QXX_QPI_MODE = 0;		//QSPI模式标志:0,SPI模式;1,QPI模式.

void W25QXX_Init( void );					//初始化W25QXX
void W25QXX_Qspi_Enable( void );			//使能QSPI模式
void W25QXX_Qspi_Disable( void );			//关闭QSPI模式
uint16_t  W25QXX_ReadID( void );  	    		//读取FLASH ID
uint8_t W25QXX_ReadSR( uint8_t regno );           //读取状态寄存器
void W25QXX_4ByteAddr_Enable( void );   //使能4字节地址模式
void W25QXX_Write_SR( uint8_t regno, uint8_t sr ); //写状态寄存器
void W25QXX_Write_Enable( void );  		//写使能
void W25QXX_Write_Disable( void );		//写保护
void W25QXX_Write_NoCheck( uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite ); //写flash,不校验
void W25QXX_Read( uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead ); //读取flash
void W25QXX_Write( uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite ); //写入flash
void W25QXX_Erase_Chip( void );    	  	//整片擦除
void W25QXX_Erase_Sector( uint32_t Dst_Addr );	//扇区擦除
void W25QXX_Wait_Busy( void );           	//等待空闲



/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Init(void)
{ 
    uint8_t temp;    
	BSP_QSPI_Init();					//初始化QSPI
 	W25QXX_Qspi_Enable();			//使能QSPI模式
	W25QXX_TYPE=W25QXX_ReadID();	//读取FLASH ID.
	//App_Printf("ID:%x\r\n",W25QXX_TYPE);
	if(W25QXX_TYPE==W25Q256)        //SPI FLASH为W25Q256
    {
        temp=W25QXX_ReadSR(3);      //读取状态寄存器3，判断地址模式
        if((temp&0X01)==0)			//如果不是4字节地址模式,则进入4字节地址模式
		{ 
			W25QXX_Write_Enable();	//写使能
			QSPI_Send_CMD(W25X_Enable4ByteAddr,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//QPI,使能4字节地址指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据 
		}
		W25QXX_Write_Enable();		//写使能
		QSPI_Send_CMD(W25X_SetReadParam,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES); 		//QPI,设置读参数指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
		temp=3<<4;					//设置P4&P5=11,8个dummy clocks,104M
		QSPI_Transmit(&temp,1);		//发送1个字节	   
    }
}  
//W25QXX进入QSPI模式
void W25QXX_Qspi_Enable(void)
{
	uint8_t stareg2;
	stareg2=W25QXX_ReadSR(2);		//先读出状态寄存器2的原始值
	if((stareg2&0X02)==0)			//QE位未使能
	{
		W25QXX_Write_Enable();		//写使能 
		stareg2|=1<<1;				//使能QE位		
		W25QXX_Write_SR(2,stareg2);	//写状态寄存器2
	}
	QSPI_Send_CMD(W25X_EnterQPIMode,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
	W25QXX_QPI_MODE=1;				//标记QSPI模式
}

//W25QXX退出QSPI模式
void W25QXX_Qspi_Disable(void)
{ 
	QSPI_Send_CMD(W25X_ExitQPIMode,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);//写command指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
	W25QXX_QPI_MODE=0;				//标记SPI模式
}

/*******************************************************
    @brief  :读取W25QXX的状态寄存器
    @note   :W25QXX一共有3个状态寄存器
            状态寄存器1：
            BIT7  6   5   4   3   2   1   0
            SPR   RV  TB BP2 BP1 BP0 WEL BUSY
            SPR:默认0,状态寄存器保护位,配合WP使用
            TB,BP2,BP1,BP0:FLASH区域写保护设置
            WEL:写使能锁定
            BUSY:忙标记位(1,忙;0,空闲)
            默认:0x00
            状态寄存器2：
            BIT7  6   5   4   3   2   1   0
            SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
            状态寄存器3：
            BIT7      6    5    4   3   2   1   0
            HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
    @param  :regno:状态寄存器号:1~3
    @param  :
    @param  :
    @retval :状态寄存器值
*******************************************************/
uint8_t W25QXX_ReadSR( uint8_t regno )
{
    uint8_t byte = 0, command = 0;

    switch ( regno )
    {
        case 1:
            command = W25X_ReadStatusReg1;  //读状态寄存器1指令
            break;

        case 2:
            command = W25X_ReadStatusReg2;  //读状态寄存器2指令
            break;

        case 3:
            command = W25X_ReadStatusReg3;  //读状态寄存器3指令
            break;

        default:
            command = W25X_ReadStatusReg1;
            break;
    }

    if ( W25QXX_QPI_MODE )
    {
        QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);   //QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
    }

    else
    {
        QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);    //SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
    }

    QSPI_Receive( &byte, 1 );
    return byte;
}

/*******************************************************
    @brief  :写W25QXX状态寄存器
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Write_SR( uint8_t regno, uint8_t sr )
{
    uint8_t command = 0;

    switch ( regno )
    {
        case 1:
            command = W25X_WriteStatusReg1;  //写状态寄存器1指令
            break;

        case 2:
            command = W25X_WriteStatusReg2;  //写状态寄存器2指令
            break;

        case 3:
            command = W25X_WriteStatusReg3;  //写状态寄存器3指令
            break;

        default:
            command = W25X_WriteStatusReg1;
            break;
    }

    if ( W25QXX_QPI_MODE )
    {
        QSPI_Send_CMD(command,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_4_LINES);    //QPI,写command指令,地址为0,4线传数据_8位地址_无地址_4线传输指令,无空周期,1个字节数据
    }

    else
    {
        QSPI_Send_CMD(command,0,0, QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_1_LINE);  //SPI,写command指令,地址为0,单线传数据_8位地址_无地址_单线传输指令,无空周期,1个字节数据
    }

    QSPI_Transmit( &sr, 1 );
}

/*******************************************************
    @brief  :W25QXX写使能
    @note   :将S1寄存器的WEL置位
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Write_Enable( void )
{
    if ( W25QXX_QPI_MODE )
    {
        QSPI_Send_CMD(W25X_WriteEnable,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);  //QPI,写使能指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
    }

    else
    {
        QSPI_Send_CMD(W25X_WriteEnable,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);   //SPI,写使能指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
    }
}

/*******************************************************
    @brief  :W25QXX写禁止
    @note   :将WEL清零
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Write_Disable( void )
{
    if ( W25QXX_QPI_MODE )
    {
        QSPI_Send_CMD(W25X_WriteDisable,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE); //QPI,写禁止指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
    }

    else
    {
        QSPI_Send_CMD(W25X_WriteDisable,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE);	  //SPI,写禁止指令,地址为0,无数据_8位地址_无地址_单线传输指令,无空周期,0个字节数据
    }
}


/*******************************************************
    @brief  :读取芯片ID
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :0XEF13,表示芯片型号为W25Q80
            0XEF14,表示芯片型号为W25Q16
            0XEF15,表示芯片型号为W25Q32
            0XEF16,表示芯片型号为W25Q64
            0XEF17,表示芯片型号为W25Q128
            0XEF18,表示芯片型号为W25Q256
*******************************************************/
uint16_t W25QXX_ReadID( void )
{
    uint8_t temp[2];
    uint16_t deviceid;

    if ( W25QXX_QPI_MODE )
    {
        QSPI_Send_CMD(W25X_ManufactDeviceID,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_24_BITS,QSPI_DATA_4_LINES);  //QPI,读id,地址为0,4线传输数据_24位地址_4线传输地址_4线传输指令,无空周期,2个字节数据
    }

    else
    {
        QSPI_Send_CMD(W25X_ManufactDeviceID,0,0,QSPI_INSTRUCTION_1_LINE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_24_BITS,QSPI_DATA_1_LINE); //SPI,读id,地址为0,单线传输数据_24位地址_单线传输地址_单线传输指令,无空周期,2个字节数据
    }

    QSPI_Receive( temp, 2 );
    deviceid = ( temp[0] << 8 ) | temp[1];
    return deviceid;
}

/*******************************************************
    @brief  :读取SPI FLASH 仅支持QPI模式
    @note   : 在指定地址开始读取指定长度的数据
    @param  :pBuffer:数据存储区
    @param  :ReadAddr:开始读取的地址(最大32bit)
    @param  :NumByteToRead:要读取的字节数(最大65535)
    @retval :
*******************************************************/
void W25QXX_Read( uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead )
{
    QSPI_Send_CMD(W25X_FastReadData,ReadAddr,8,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_32_BITS,QSPI_DATA_4_LINES);	//QPI,快速读数据,地址为ReadAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,8空周期,NumByteToRead个数据
    QSPI_Receive( pBuffer, NumByteToRead );
}


/*******************************************************
    @brief  :在指定地址开始写入最大256字节的数据
    @note   :SPI在一页(0~65535)内写入少于256个字节的数据
    @param  :pBuffer:数据存储区
    @param  :WriteAddr:开始写入的地址(24bit)
    @param  :NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
    @retval :
*******************************************************/
void W25QXX_Write_Page( uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite )
{
    W25QXX_Write_Enable();					//写使能
    QSPI_Send_CMD(W25X_PageProgram,WriteAddr,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_32_BITS,QSPI_DATA_4_LINES);//QPI,页写指令,地址为WriteAddr,4线传输数据_32位地址_4线传输地址_4线传输指令,无空周期,NumByteToWrite个数据
    QSPI_Transmit( pBuffer, NumByteToWrite );
    W25QXX_Wait_Busy();					   //等待写入结束
}

/*******************************************************
    @brief  :无检验写SPI FLASH
    @note   :必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
            具有自动换页功能,在指定地址开始写入指定长度的数据,但是要确保地址不越界!
    @param  :pBuffer:数据存储区
    @param  :WriteAddr:开始写入的地址(最大32bit)
    @param  :NumByteToWrite:要写入的字节数(最大65535)
    @retval :CHECK OK
*******************************************************/
void W25QXX_Write_NoCheck( uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite )
{
    uint16_t pageremain;
    pageremain = 256 - WriteAddr % 256; //单页剩余的字节数

    if ( NumByteToWrite <= pageremain )
    {
        pageremain = NumByteToWrite;    //不大于256个字节
    }

    for ( ;; )
    {
        W25QXX_Write_Page( pBuffer, WriteAddr, pageremain );

        if ( NumByteToWrite == pageremain )
        {
            break;    //写入结束了
        }

        else //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;			 //减去已经写入了的字节数

            if ( NumByteToWrite > 256 )
            {
                pageremain = 256;    //一次可以写入256个字节
            }

            else
            {
                pageremain = NumByteToWrite;    //不够256个字节了
            }
        }
    }
}


/*******************************************************
    @brief  :写SPI FLASH  在指定地址开始写入指定长度的数据
    @note   :该函数带擦除操作!
    @param  :pBuffer:数据存储区
    @param  :WriteAddr:开始写入的地址(最大32bit)
    @param  :NumByteToWrite:要写入的字节数(最大65535)
    @retval :
*******************************************************/
void W25QXX_Write( uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite )
{
    uint8_t W25QXX_BUFFER[4096];
    uint32_t secPos;
    uint16_t secOff;
    uint16_t secRemain;
    uint16_t i;
    uint8_t *W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secPos = WriteAddr / 4096; //扇区地址
    secOff = WriteAddr % 4096; //在扇区内的偏移
    secRemain = 4096 - secOff; //扇区剩余空间大小

    if ( NumByteToWrite <= secRemain )
    {
        secRemain = NumByteToWrite;    //不大于4096个字节
    }

    while ( 1 )
    {
        W25QXX_Read( W25QXX_BUF, secPos * 4096, 4096 ); //读出整个扇区的内容

        for ( i = 0; i < secRemain; i++ ) //校验数据
        {
            if ( W25QXX_BUF[secOff + i] != 0XFF )
            {
                break;    //需要擦除
            }
        }

        if ( i < secRemain ) //需要擦除
        {
            W25QXX_Erase_Sector( secPos ); //擦除这个扇区

            for ( i = 0; i < secRemain; i++ )	 //复制
            {
                W25QXX_BUF[i + secOff] = pBuffer[i];
            }

            W25QXX_Write_NoCheck( W25QXX_BUF, secPos * 4096, 4096 ); //写入整个扇区
        }

        else
        {
            W25QXX_Write_NoCheck( pBuffer, WriteAddr, secRemain );  //写已经擦除了的,直接写入扇区剩余区间.
        }

        if ( NumByteToWrite == secRemain )
        {
            break;    //写入结束了
        }

        else//写入未结束
        {
            secPos++;//扇区地址增1
            secOff = 0; //偏移位置为0
            pBuffer += secRemain; //指针偏移
            WriteAddr += secRemain; //写地址偏移
            NumByteToWrite -= secRemain;				//字节数递减

            if ( NumByteToWrite > 4096 )
            {
                secRemain = 4096;    //下一个扇区还是写不完
            }

            else
            {
                secRemain = NumByteToWrite;    //下一个扇区可以写完了
            }
        }
    }
}


/*******************************************************
    @brief  :擦除整个芯片
    @note   :等待时间长...
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Erase_Chip( void )
{
    W25QXX_Write_Enable();					//SET WEL
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_ChipErase,0,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_NONE,QSPI_ADDRESS_8_BITS,QSPI_DATA_NONE); //QPI,写全片擦除指令,地址为0,无数据_8位地址_无地址_4线传输指令,无空周期,0个字节数据
    W25QXX_Wait_Busy();						//等待芯片擦除结束
}

/*******************************************************
    @brief  :擦除一个扇区
    @note   :擦除一个扇区的最少时间:150ms
    @param  :Dst_Addr:扇区地址 根据实际容量设置
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Erase_Sector( uint32_t Dst_Addr )
{
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();                  //SET WEL
    W25QXX_Wait_Busy();
    QSPI_Send_CMD(W25X_SectorErase,Dst_Addr,0,QSPI_INSTRUCTION_4_LINES,QSPI_ADDRESS_4_LINES,QSPI_ADDRESS_32_BITS,QSPI_DATA_NONE); //QPI,写扇区擦除指令,地址为0,无数据_32位地址_4线传输地址_4线传输指令,无空周期,0个字节数据
    W25QXX_Wait_Busy();   				    //等待擦除完成
}


/*******************************************************
    @brief  :等待空闲
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void W25QXX_Wait_Busy( void )
{
    while ( ( W25QXX_ReadSR( 1 ) & 0x01 ) == 0x01 ); // 等待BUSY位清空
}

//------------------------------------------------------------------------
