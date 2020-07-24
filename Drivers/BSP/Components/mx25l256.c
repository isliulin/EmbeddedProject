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


uint8_t MX25Lxx_QSPI_MODE = 1;		//QSPIģʽ��־:0,SPIģʽ;1,QPIģʽ.

void MX25Lxx_Init( void );					//��ʼ��MX25Lxx
static void MX25Lxx_QspiEnable( void );			//ʹ��QSPIģʽ
static void MX25Lxx_QspiDisable( void );			//�ر�QSPIģʽ
static uint32_t  MX25Lxx_ReadID( void );  	    		//��ȡFLASH ID
static uint8_t MX25Lxx_ReadSR( void );          //��ȡ״̬�Ĵ���
static uint8_t MX25Lxx_ReadCR( void );			//��ȡMX25Lxx�����üĴ���
static void MX25Lxx_WriteSR( uint8_t ucSR, uint8_t ucCR); //д״̬�Ĵ���
static void MX25Lxx_WriteEnable( void );  		//дʹ��
static void MX25Lxx_WriteDisable( void );		//д����
static void MX25Lxx_WriteNoCheck( uint8_t *pucBuffer, uint32_t ulWriteAddr, uint16_t usNumByteToRead ); //дflash,��У��
void MX25Lxx_Read( uint8_t *pucBuffer, uint32_t ReadAddr, uint16_t NumByteToRead ); //��ȡflash
void MX25Lxx_Write( uint8_t *pucBuffer, uint32_t ulWriteAddr, uint16_t usNumByteToWrite ); //д��flash
static void MX25Lxx_EraseChip( void );    	  	//��Ƭ����
static void MX25Lxx_EraseSector( uint32_t ulDstAddr );	//��������
static void MX25Lxx_SoftwareReset( void );
static void MX25Lxx_EnterFourBytesAddress( void );
	
/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void MX25Lxx_Init( void )
{
    uint8_t ucTemp1, ucTemp2;
    uint32_t ulDevId;

    BSP_QSPI_Init();				//��ʼ��QSPI
	
	MX25Lxx_WriteEnable();	
	MX25Lxx_QspiEnable();			//ʹ��QSPIģʽ
	MX25Lxx_WriteEnable();			//дʹ��			
	MX25Lxx_EnterFourBytesAddress();//����4�ֽ�ģʽ
	
	MX25Lxx_WriteSR(0x42, 0x28);	
	MX25Lxx_SoftwareReset();
//    ucTemp1 = MX25Lxx_ReadSR();		//��״̬�Ĵ���	
//	if((ucTemp1 & 0x42) != 0x42)
//	{
//		ucTemp1 = ucTemp1 | 0x42;	//Quad enable,  Erite enable
//	}			 
//	    
//    ucTemp2 = MX25Lxx_ReadCR();		//�����üĴ���	
//	if((ucTemp2 & 0x28) != 0x28)		//����4�ֽڵ�ַģʽ
//	{
//		ucTemp2 = ucTemp2 | 0x28;	//
//	}	
//	
//	MX25Lxx_WriteSR(ucTemp1, ucTemp2);	//Quad enable,  Erite enable,4add
						
	ulDevId = MX25Lxx_ReadID();		//��ȡFLASH ID.
	ucTemp1 = MX25Lxx_ReadSR();		//��״̬�Ĵ���	
	ucTemp2 = MX25Lxx_ReadCR();		//�����üĴ���,��ѯ������3�ֽڵ�ַģʽ��������
}

/*******************************************************
    @brief  : MX25Lxx����QSPIģʽ
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_QspiEnable( void )
{
	uint32_t ulInstructionMode, ulDataMode;

	ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
	ulDataMode        = QSPI_DATA_NONE;  
	     	
	QSPI_Send_CMD( 
					ENTER_QUAD_CMD, 			//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 
				);
    MX25Lxx_QSPI_MODE = 1;				//���QSPIģʽ
}

/*******************************************************
    @brief  : MX25Lxx�˳�QSPIģʽ
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_QspiDisable( void )
{
	uint32_t ulInstructionMode, ulDataMode;

	ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
	ulDataMode        = QSPI_DATA_NONE;  
	     	
	QSPI_Send_CMD( 
					EXIT_QUAD_CMD, 				//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 	
				);
    MX25Lxx_QSPI_MODE = 0;		//���SPIģʽ
}



/*******************************************************
    @brief  :MX25Lxxдʹ��
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_WriteEnable( void )
{
	uint32_t ulInstructionMode, ulDataMode;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_NONE;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_NONE;  
	} 
    	
	QSPI_Send_CMD( 
					WRITE_ENABLE_CMD, 			//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 
				);	
}

/*******************************************************
    @brief  :MX25Lxxд��ֹ
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_WriteDisable( void )
{
	uint32_t ulInstructionMode, ulDataMode;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_NONE;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_NONE;  
	} 
    	
	QSPI_Send_CMD( 
					WRITE_DISABLE_CMD, 			//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 
				);
}

/*******************************************************
  * @brief  : This function set the QSPI memory in 4-byte address mode
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
static void MX25Lxx_EnterFourBytesAddress( void )
{
	uint32_t ulInstructionMode, ulDataMode;

	ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
	ulDataMode        = QSPI_DATA_NONE;  
	     	
	QSPI_Send_CMD( 
					ENTER_4_BYTE_ADDR_MODE_CMD,	//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 
				);	
}

/*******************************************************
    @brief  :��ȡоƬID
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static uint32_t MX25Lxx_ReadID( void )
{
    uint8_t ucBuf[3] = {0};
    uint32_t usDeviceId;
	uint32_t ulInstructionMode, ulDataMode, ulCommond;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
		ulCommond		  =	MULTIPLE_IO_READ_ID_CMD;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE;  
		ulCommond		  =	READ_ID_CMD;
	} 
	    
	QSPI_Send_CMD( 
					ulCommond, 					//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ
				);
	
    QSPI_Receive( ucBuf, 3 );
    usDeviceId = ( ucBuf[0] << 16 ) | ( ucBuf[1] << 8 ) | ucBuf[2];
    return usDeviceId;
}

/*******************************************************
    @brief  :��ȡMX25Lxx��״̬�Ĵ���
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :״̬�Ĵ���ֵ
*******************************************************/
static uint8_t MX25Lxx_ReadSR( void )
{
    uint8_t ucStatus = 0;
	uint32_t ulInstructionMode, ulDataMode;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE;  
	} 
    
	QSPI_Send_CMD( 
					READ_STATUS_REG_CMD, 		//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ
				);	

    QSPI_Receive( &ucStatus, 1 );
    return ucStatus;
}

/*******************************************************
    @brief  :��ȡMX25Lxx�����üĴ���
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :���üĴ���ֵ
*******************************************************/
static uint8_t MX25Lxx_ReadCR( void )
{
    uint8_t ucStatus = 0;
	uint32_t ulInstructionMode, ulDataMode;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE;  
	} 
    
	QSPI_Send_CMD( 
					READ_CFG_REG_CMD, 			//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ
				);	

    QSPI_Receive( &ucStatus, 1 );
    return ucStatus;
}

/*******************************************************
    @brief  : write enable
    @note   :
    @param  :
    @param  :
    @param  :
    @retval : 1=write enable,0=not write enable
*******************************************************/
static uint8_t MX25Lxx_ReadyToWrite( void )
{
    return ( ( (MX25Lxx_ReadSR() & MX25L256_SR_WREN) == MX25L256_SR_WREN) ? 1 : 0 );
}

/*******************************************************
    @brief  : Write in Progress
    @note   :
    @param  :
    @param  :
    @param  :
    @retval : 1=write operation, 0=not in write operation
*******************************************************/
static uint8_t MX25Lxx_WriteComplete( void )
{
    return ( ( ( MX25Lxx_ReadSR() & MX25L256_SR_WIP ) == MX25L256_SR_WIP ) ? 1 : 0 );
}

/*******************************************************
    @brief  :дMX25Lxx״̬�Ĵ��������üĴ���
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_WriteSR( uint8_t ucSR, uint8_t ucCR )
{
    uint8_t ucStatus;
	uint32_t ulInstructionMode, ulDataMode;
	uint8_t ucSendBuf[2];
	
	ucSendBuf[0] = ucSR;
	ucSendBuf[1] = ucCR;
	
	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE;  
	} 
    
    MX25Lxx_WriteEnable();
    ucStatus = MX25Lxx_ReadyToWrite();	
    if ( ucStatus )
    {
		QSPI_Send_CMD( 
						WRITE_STATUS_CFG_REG_CMD, 	//Ҫ���͵�ָ��
						0, 							//���͵���Ŀ�ĵ�ַ
						0, 							//��ָ��������
						ulInstructionMode, 			//ָ��ģʽ
						QSPI_ADDRESS_NONE, 			//��ַģʽ
						QSPI_ADDRESS_8_BITS, 		//��ַ����
						ulDataMode 					//����ģʽ	
					);		

        QSPI_Transmit( ucSendBuf, 2 );
				
        while ( MX25Lxx_WriteComplete() )		
		{
			;//�ȴ�д�����
		}			
    }
	
	
	ucStatus = MX25Lxx_ReadyToWrite();	
    if ( ucStatus )
    {
		QSPI_Send_CMD( 
						WRITE_STATUS_CFG_REG_CMD, 	//Ҫ���͵�ָ��
						0, 							//���͵���Ŀ�ĵ�ַ
						0, 							//��ָ��������
						ulInstructionMode, 			//ָ��ģʽ
						QSPI_ADDRESS_NONE, 			//��ַģʽ
						QSPI_ADDRESS_8_BITS, 		//��ַ����
						ulDataMode 					//����ģʽ	
					);		

        QSPI_Transmit( &ucSR, 1 );
		QSPI_Transmit( &ucCR, 1 );
    }	
}

/*******************************************************
    @brief  :��ȡSPI FLASH ��֧��QPIģʽ
    @note   : ��ָ����ַ��ʼ��ȡָ�����ȵ�����
    @param  :pucBuffer:���ݴ洢��
    @param  :ReadAddr:��ʼ��ȡ�ĵ�ַ(���32bit)
    @param  :NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
    @retval :
*******************************************************/
void MX25Lxx_Read( uint8_t *pucBuffer, uint32_t ulReadAddr, uint16_t usNumByteToRead )
{
	uint32_t ulInstructionMode, ulAddressMode, ulDataMode, ulCommond, ucDummyCycle;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulAddressMode     = QSPI_ADDRESS_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
		ulCommond		  =	QUAD_READ_4_BYTE_ADDR_CMD;
		ucDummyCycle	  = 6;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulAddressMode     = QSPI_ADDRESS_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE;  
		ulCommond		  =	FAST_READ_CMD;
		ucDummyCycle	  = 8;
	} 
    
	QSPI_Send_CMD( 
					ulCommond,					//Ҫ���͵�ָ��
					ulReadAddr, 				//���͵���Ŀ�ĵ�ַ
					ucDummyCycle, 				//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					ulAddressMode, 				//��ַģʽ
					QSPI_ADDRESS_32_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ
				);	

	QSPI_Receive( pucBuffer, usNumByteToRead );
}

/*******************************************************
    @brief  :��ָ����ַ��ʼд�����256�ֽڵ�����
    @note   :SPI��һҳ(0~65535)��д������256���ֽڵ�����
    @param  :pucBuffer:���ݴ洢��
    @param  :ulWriteAddr:��ʼд��ĵ�ַ(24bit)
    @param  :usNumByteToRead:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
    @retval :
*******************************************************/
static void MX25Lxx_WritePage( uint8_t *pucBuffer, uint32_t ulWriteAddr, uint16_t usNumByteToWrite )
{
	uint32_t ulInstructionMode, ulAddressMode, ulDataMode, ulCommond;

	MX25Lxx_WriteEnable();					//дʹ��
	
	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulAddressMode     = QSPI_ADDRESS_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
		ulCommond		  =	QSPI_PAGE_PROG_4_BYTE_ADDR_CMD;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulAddressMode     = QSPI_ADDRESS_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE; 
		ulCommond		  =	PAGE_PROG_CMD;		
	} 
   
    if ( MX25Lxx_ReadyToWrite() )
    {
		QSPI_Send_CMD( 
						ulCommond, 					//Ҫ���͵�ָ��
						ulWriteAddr, 				//���͵���Ŀ�ĵ�ַ
						0, 							//��ָ��������
						ulInstructionMode, 			//ָ��ģʽ
						ulAddressMode, 				//��ַģʽ
						QSPI_ADDRESS_32_BITS, 		//��ַ����
						ulDataMode 					//����ģʽ	
					);
	 
        QSPI_Transmit( pucBuffer, usNumByteToWrite );

        while ( MX25Lxx_WriteComplete() )		
		{
			;//�ȴ�д�����
		}
    }
}

/*******************************************************
    @brief  :�޼���дSPI FLASH
    @note   :����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
            �����Զ���ҳ����,��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
    @param  :pucBuffer:���ݴ洢��
    @param  :ulWriteAddr:��ʼд��ĵ�ַ(���32bit)
    @param  :usNumByteToRead:Ҫд����ֽ���(���65535)
    @retval :CHECK OK
*******************************************************/
static void MX25Lxx_WriteNoCheck( uint8_t *pucBuffer, uint32_t ulWriteAddr, uint16_t usNumByteToWrite )
{
    uint16_t usPageRemain;
    usPageRemain = 256 - ulWriteAddr % 256; //��ҳʣ����ֽ���

    if ( usNumByteToWrite <= usPageRemain )
    {
        usPageRemain = usNumByteToWrite;    //������256���ֽ�
    }

    for ( ;; )
    {
        MX25Lxx_WritePage( pucBuffer, ulWriteAddr, usPageRemain );

        if ( usNumByteToWrite == usPageRemain )
        {
            break;    //д�������
        }
        else //usNumByteToRead>usPageRemain
        {
            pucBuffer += usPageRemain;
            ulWriteAddr += usPageRemain;

            usNumByteToWrite -= usPageRemain;			 //��ȥ�Ѿ�д���˵��ֽ���

            if ( usNumByteToWrite > 256 )
            {
                usPageRemain = 256;    //һ�ο���д��256���ֽ�
            }
            else
            {
                usPageRemain = usNumByteToWrite;    //����256���ֽ���
            }
        }
    }
}


/*******************************************************
    @brief  :дSPI FLASH  ��ָ����ַ��ʼд��ָ�����ȵ�����
    @note   :�ú�������������!
    @param  :pucBuffer:���ݴ洢��
    @param  :ulWriteAddr:��ʼд��ĵ�ַ(���32bit)
    @param  :usNumByteToRead:Ҫд����ֽ���(���65535)
    @retval :
*******************************************************/
void MX25Lxx_Write( uint8_t *pucBuffer, uint32_t ulWriteAddr, uint16_t usNumByteToWrite )
{
    uint8_t *pucBuf = (uint8_t *)malloc(4097);
    uint32_t ulSectionLocation;
    uint16_t usSectionOffset;
    uint16_t usSectionRemain;
    uint16_t i;
//    uint8_t *pucMX25Lxx_BUF;

//    pucMX25Lxx_BUF = pucBuf;
    ulSectionLocation = ulWriteAddr / 4096; //������ַ
    usSectionOffset = ulWriteAddr % 4096; //�������ڵ�ƫ��
    usSectionRemain = 4096 - usSectionOffset; //����ʣ��ռ��С

    if ( usNumByteToWrite <= usSectionRemain )
    {
        usSectionRemain = usNumByteToWrite;    //������4096���ֽ�
    }

    for ( ;; )
    {
        MX25Lxx_Read( pucBuf, ulSectionLocation * 4096, 4096 ); //������������������

        for ( i = 0; i < usSectionRemain; i++ ) //У������
        {
            if ( pucBuf[usSectionOffset + i] != 0XFF )
            {
                break;    //��Ҫ����
            }
        }

        if ( i < usSectionRemain ) //��Ҫ����
        {
            MX25Lxx_EraseSector( ulSectionLocation ); //�����������

			/*MX25Lxx_Read( pucBuf, ulSectionLocation * 4096, 4096 ); //����(�����ã����ɾ��)
			for(i = 0; i < 4096; i++)
			{
				pucBuf[i] = 0xFF;
			}*/
			
            for ( i = 0; i < usSectionRemain; i++ )	 //����
            {
                pucBuf[i + usSectionOffset] = pucBuffer[i];
            }

            MX25Lxx_WriteNoCheck( pucBuf, ulSectionLocation * 4096, 4096 ); //д����������
        }
        else
        {
            MX25Lxx_WriteNoCheck( pucBuffer, ulWriteAddr, usSectionRemain );  //д�Ѿ������˵�,ֱ��д������ʣ������.
        }

        if ( usNumByteToWrite == usSectionRemain )
        {
            break;    //д�������
        }
        else//д��δ����
        {
            ulSectionLocation++;//������ַ��1
            usSectionOffset = 0; //ƫ��λ��Ϊ0
            pucBuffer += usSectionRemain; //ָ��ƫ��
            ulWriteAddr += usSectionRemain; //д��ַƫ��
            usNumByteToWrite -= usSectionRemain;				//�ֽ����ݼ�

            if ( usNumByteToWrite > 4096 )
            {
                usSectionRemain = 4096;    //��һ����������д����
            }
            else
            {
                usSectionRemain = usNumByteToWrite;    //��һ����������д����
            }
        }
    }

    free(pucBuf);
	pucBuf = NULL;
}


/*******************************************************
    @brief  :��������оƬ
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_EraseChip( void )
{
	uint32_t ulInstructionMode;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
	} 
    
	MX25Lxx_WriteEnable();
	
	if ( MX25Lxx_ReadyToWrite() )
	{
		QSPI_Send_CMD( 
						CHIP_ERASE_CMD,				//Ҫ���͵�ָ��
						0, 							//���͵���Ŀ�ĵ�ַ
						0, 							//��ָ��������
						ulInstructionMode, 			//ָ��ģʽ
						QSPI_ADDRESS_NONE, 			//��ַģʽ
						QSPI_ADDRESS_8_BITS, 		//��ַ����
						QSPI_DATA_NONE 				//����ģʽ	
					);
		while ( MX25Lxx_WriteComplete() );		//�ȴ�д�����	
	}  
}

/*******************************************************
    @brief  :����һ������
    @note   :
    @param  :ulDst_Addr:������ַ ����ʵ����������
    @param  :
    @param  :
    @retval :
*******************************************************/
static void MX25Lxx_EraseSector( uint32_t ulDstAddr )
{    
	uint32_t ulInstructionMode, ulAddressMode, ulDataMode, ulCommond;
	ulDstAddr *= 4096;	//����Ϊ������ַ,��˻ָ�Ϊ��ͨ��ַ
	
	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulAddressMode     = QSPI_ADDRESS_4_LINES;
		ulCommond		  = SECTOR_ERASE_4K_ADDR_CMD;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulAddressMode     = QSPI_ADDRESS_1_LINE;
		ulCommond		  = SECTOR_ERASE_CMD;		
	}
    
	MX25Lxx_WriteEnable();
	
	if ( MX25Lxx_ReadyToWrite() )
	{
		QSPI_Send_CMD( 
						ulCommond,					//Ҫ���͵�ָ��
						ulDstAddr, 					//���͵���Ŀ�ĵ�ַ
						0, 							//��ָ��������
						ulInstructionMode, 			//ָ��ģʽ
						ulAddressMode, 				//��ַģʽ
						QSPI_ADDRESS_32_BITS, 		//��ַ����
						QSPI_DATA_NONE 				//����ģʽ	
					);
		while ( MX25Lxx_WriteComplete() );		//�ȴ�д�����
	}
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
static void MX25Lxx_SoftwareReset( void )
{ 
	uint32_t ulInstructionMode, ulDataMode;

	if(MX25Lxx_QSPI_MODE)   // Work In QUAD Model
	{
		ulInstructionMode = QSPI_INSTRUCTION_4_LINES;
		ulDataMode        = QSPI_DATA_4_LINES;
	}
	else
	{
		ulInstructionMode = QSPI_INSTRUCTION_1_LINE;
		ulDataMode        = QSPI_DATA_1_LINE;  
	} 
    	
	QSPI_Send_CMD( 
					RESET_ENABLE_CMD, 			//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 
				);
	QSPI_Send_CMD( 
					RESET_MEMORY_CMD, 			//Ҫ���͵�ָ��
					0, 							//���͵���Ŀ�ĵ�ַ
					0, 							//��ָ��������
					ulInstructionMode, 			//ָ��ģʽ
					QSPI_ADDRESS_NONE, 			//��ַģʽ
					QSPI_ADDRESS_8_BITS, 		//��ַ����
					ulDataMode 					//����ģʽ 
				);
}
//------------------------------------------------------------------------