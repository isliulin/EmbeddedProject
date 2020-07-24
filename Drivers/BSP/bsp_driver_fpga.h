/**
  ******************************************************************************
  * @file
  * @author
  * @version
  * @date
  * @brief
  *
  ******************************************************************************
  * @attention
  *
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_DRIVER_FPGA_H
#define __BSP_DRIVER_FPGA_H

#ifdef __cplusplus
extern "C" {
    #endif

    #define FPGA_BASE_ADDR  SRAM_BANK_ADDR       

    #define FPGA_SYS_RST_REG               (0x00)    //W,系统复位寄存器
    #define SYS_RST                        (0<<0)    //系统复位
    #define SYS_SET                        (1<<0)    //系统置位
    #define PROTECT_RST                    (0<<1)    //保护复位
    #define PROTECT_SET                    (1<<1)    //保护置位

    #define FPGA_SYS_STATUS_REG            (0x01)    //R,系统状态,保留

    //中断
    #define FPGA_INT_CTRL_REG              (0x02)    //W,中断控制寄存器
    #define AD_INT_ENABLE                  (1<<0)    //AD中断使能
    #define AD_INT_DISABLE                 (0<<0)    //AD中断禁止
    #define GPS_INT_ENABLE                 (1<<1)    //GPS中断使能
    #define GPS_INT_DISABLE                (0<<1)    //GPS中断禁止
    #define OVERCURRENT_INT_ENABLE         (1<<2)    //过流中断使能
    #define OVERCURRENT_INT_DISABLE        (0<<2)    //过流中断禁止
    #define OVERVOLTAGE_INT_ENABLE         (1<<3)    //过压中断使能
    #define OVERVOLTAGE_INT_DISABLE        (0<<3)    //过压中断禁止
    #define VOLTAGE_VALUE_INT_ENABLE       (1<<4)    //电压数据更新中断使能
    #define VOLTAGE_VALUE_INT_DISABLE      (0<<4)    //电压数据更新中断禁止
    #define SEND_STOP_INT_ENABLE           (1<<5)    //发射停止中断使能
    #define SEND_STOP_INT_DISABLE          (0<<5)    //发射停止中断禁止
    #define TXCO_64S_LOCK_INT_ENABLE       (1<<6)    //GPS_PPS连续64秒锁定温补晶振计数有效中断使能
    #define TXCO_64S_LOCK_INT_DISABLE      (0<<6)    //GPS_PPS连续64秒锁定温补晶振计数有效中断禁止
    #define OTP_INT_ENABLE                 (1<<7)    //过温中断使能
    #define OTP_INT_DISABLE                (0<<7)    //过温中断禁止

    #define FPGA_INT_STATUS_REG            (0x03)    //R,中断状态寄存器  
    #define AD_DATA_READABLE               (1<<0)    //AD数据准备就绪
    #define GPS_MSG_READABLE               (1<<1)    //GPS信息准备就绪
    #define OVERCURRENT_INT_STATUS         (1<<2)    //过流中断状态
    #define OVERVOLTAGE_INT_STATUS         (1<<3)    //过压中断状态
    #define VOLTAGE_VALUE_READABLE         (1<<4)    //电压数据准备就绪
    #define SEND_STOP_INT_STATUS           (1<<5)    //发射停止中断状态
    #define TXCO_64S_LOCK_INT_STATUS       (1<<6)    //GPS_PPS连续64秒锁定温补晶振计数有效中断状态
    #define OTP_INT_STATUS                 (1<<7)    //过温中断状态

    #define CLEAR_FPGA_INT_STATUS_REG      (0x04)    //W,清中断状态寄存器
    #define CLEAR_OVERCURRENT_STATUS       (1<<2)    //清过流中断
    #define CLEAR_OVERVOLTAGE_STATUS       (1<<3)    //清过压中断
    #define CLEAR_SEND_STOP_INT_STATUS     (1<<5)    //清发射停止中断
    #define CLEAR_TXCO_64S_LOCK_INT_STATUS (1<<6)    //清GPS_PPS连续64秒锁定温补晶振计数有效中断状态

    #define PROTECT_STATUS_REG             (0x05)    //R,保护状态寄存器
    #define PROTECT_STATUS                 (1<<0)    //保护
    #define OVERVOLTAGE_STATUS             (1<<1)    //过压
    #define OVERCURRENT_STATUS             (1<<2)    //过流
    #define IGBT_STATUS                    (1<<3)    //IGBT

    //#define ERROR_STATUS_REG               (0x0B)    //R,错误状态寄存器
    //#define OVERCURRENT_STATUS             (1<<0)    //过流
    //#define OVERVOLTAGE_STATUS             (1<<1)    //过压
    //#define OTP_STATUS                     (1<<2)    //过温

    #define START_STOP_STATUS_REG          (0x0C)    //R,启动停止状态寄存器(Key_Value)
    #define SEND_STOP_STATUS               (1<<0)    //发射停止,“停止”键，按下为“1”，弹起为“0”
    #define SEND_START_READY               (1<<1)    //发射就绪,“发射”键，按下为“1”，弹起为“0”

    #define VOLTAGE_VALUE_1ST_BYTE_REG     (0x0D)    //R,电压值(AD原始码)第一字节寄存器
    #define VOLTAGE_VALUE_2ND_BYTE_REG     (0x0E)    //R,电压值(AD原始码)第二字节寄存器
    #define VOLTAGE_VALUE_3RD_BYTE_REG     (0x0F)    //R,电压值(AD原始码)第三字节寄存器

    #define TXCO_64S_COUNT_1ST_BYTE_REG    (0x10)    //W,温补晶振64秒默认标准计数值第一字节寄存器
    #define TXCO_64S_COUNT_2ND_BYTE_REG    (0x11)    //W,温补晶振64秒默认标准计数值第二字节寄存器
    #define TXCO_64S_COUNT_3RD_BYTE_REG    (0x12)    //W,温补晶振64秒默认标准计数值第三字节寄存器
    #define TXCO_64S_COUNT_4TH_BYTE_REG    (0x13)    //W,温补晶振64秒默认标准计数值第四字节寄存器

    #define TXCO_SET_REG                   (0x10)    //W,TXCO设置寄存器
    #define TXCO_SET_ENABLE                (1<<0)    //默认标准计数值设置标识，“1”有效

    #define FPGA_GPS_FIFO_REG              (0x15)    //R,GPS的FIFO寄存器，GPS数据帧/秒

    #define FPGA_SEND_CTRL_REG             (0x20)    //W,发射控制寄存器
    #define SEND_START_CTRL                (1<<0)    //发射开始
    #define SEND_STOP_CTRL                 (0<<0)    //发射停止

    #define FPGA_WORK_MODE_REG             (0x21)    //W,工作模式寄存器,发射模式：“0x00”时间域,“0x01”单频,“0x02”双频,“0x03”三频

    #define FPGA_POWER_TIME_1ST_BYTE_REG   (0x22)    //W,时间域的供电时间第一字节寄存器
    #define FPGA_POWER_TIME_2ND_BYTE_REG   (0x23)    //W,时间域的供电时间第二字节寄存器
    #define FPGA_POWER_TIME_3RD_BYTE_REG   (0x24)    //W,时间域的供电时间第三字节寄存器
    #define FPGA_POWER_TIME_4TH_BYTE_REG   (0x25)    //W,时间域的供电时间第四字节寄存器

    #define FPGA_STOP_TIME_1ST_BYTE_REG    (0x26)    //W,时间域的停止供电时间第一字节寄存器
    #define FPGA_STOP_TIME_2ND_BYTE_REG    (0x27)    //W,时间域的停止供电时间第二字节寄存器
    #define FPGA_STOP_TIME_3RD_BYTE_REG    (0x28)    //W,时间域的停止供电时间第三字节寄存器
    #define FPGA_STOP_TIME_4TH_BYTE_REG    (0x29)    //W,时间域的停止供电时间第四字节寄存器

    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG      (0x2A)    //W,双频的高频半周期计数值第一字节寄存器
    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_2ND_BYTE_REG      (0x2B)    //W,双频的高频半周期计数值第二字节寄存器
    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_3RD_BYTE_REG      (0x2C)    //W,双频的高频半周期计数值第三字节寄存器 
    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_4TH_BYTE_REG      (0x2D)    //W,双频的高频半周期计数值第四字节寄存器 

    #define HIGH_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG	       (0x2F)    //W,双频的高频校正半周期高字节寄存器 
    #define HIGH_FREQUENCY_ADJUST_HALF_PERIOD_LSB_REG          (0x30)    //W,双频的高频校正半周期低字节寄存器

    #define HIGH_FREQUENCY_SEND_HALF_PERIOD_MSB_REG            (0x31)    //W,双频的高频发射半周期高字节寄存器
    #define HIGH_FREQUENCY_SEND_HALF_PERIOD_LSB_REG            (0x32)    //W,双频的高频发射半周期低字节寄存器

    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG       (0x33)    //W,双频的低频半周期计数值第一字节寄存器
    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_2ND_BYTE_REG       (0x34)    //W,双频的低频半周期计数值第二字节寄存器
    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_3RD_BYTE_REG       (0x35)    //W,双频的低频半周期计数值第三字节寄存器 
    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_4TH_BYTE_REG       (0x36)    //W,双频的低频半周期计数值第四字节寄存器 

    #define LOW_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG	       (0x37)    //W,双频的低频校正半周期第一字节寄存器 
    #define LOW_FREQUENCY_ADJUST_HALF_PERIOD_LSB_REG           (0x38)    //W,双频的低频校正半周期第二字节寄存器

    #define LOW_FREQUENCY_SEND_HALF_PERIOD_MSB_REG             (0x39)    //W,双频的低频发射半周期第一字节寄存器
    #define LOW_FREQUENCY_SEND_HALF_PERIOD_LSB_REG             (0x3A)    //W,双频的低频发射半周期第二字节寄存器

    #define FREQUENCY_RATIO                                    (0x3B)    //W,双频的频比

    #define FPGA_VOLTAGE_GAIN_REG          (0x50)    //W,电压增益寄存器
    #define FPGA_CURRENT_GAIN_REG          (0x51)    //W,电流增益寄存器

    #define FPGA_AD_INPUT_SELECT_REG       (0x52)    //W,ADS1255输入选择寄存器,电压/电流通道选择
    #define VOLTAGE_CHANNEL                (0x00)    //电压通道
    #define CURRENT_CHANNEL                (0x01)    //电流通道

    #define FPGA_AD_RST_REG                (0x53)    //W,AD复位寄存器
    #define SAMPLE_RST                     (0<<0)    //AD复位
    #define SAMPLE_SET                     (1<<0)    //AD置位

    #define FPGA_AD_CMD_REG                (0x54)    //W,AD命令寄存器
    #define AD_CMD_DISABLE                 (0<<0)    //AD命令禁止
    #define AD_CMD_ENABLE                  (1<<0)    //AD命令使能

    #define FPGA_AD_FIFO_RST_REG           (0x55)    //W,AD的FIFO复位寄存器
    #define AD_FIFO_RST                    (1<<0)    //AD的FIFO复位
    #define AD_FIFO_SET                    (0<<0)    //AD的FIFO置位

    #define FPGA_SAMPLE_CMD_REG            (0x56)    //R,采样命令寄存器
    #define SAMPLE_DISABLE                 (0<<0)    //采样禁止
    #define SAMPLE_ENABLE                  (1<<0)    //采样使能

    #define FPGA_TEMP_MSB_REG              (0x80)    //R,温度高字节寄存器,二进制补码，系数0.0625
    #define FPGA_TEMP_LSB_REG              (0x81)    //R,温度低字节寄存器

    #define WORK_STATUS_REG                (0x82)    //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供

    #define SEND_STOP_FLAG                 (0x83)    //R,发射停止标志
    #define END_OF_SEND_PERIOD             (1<<0)    //发射周期结束
    #define END_OF_STOP_KEY                (1<<1)    //停止键停止
    #define END_OF_PROTECT                 (1<<2)    //保护停止 
    #define END_OF_CMD                     (1<<3)    //命令停止

   /*#define CLEAR_SEND_STOP_FLAG           (0x84)    //W,清发射停止标志
    #define CLEAR_END_OF_SEND_PERIOD       (1<<0)    //清发射周期结束
    #define CLEAR_END_OF_STOP_KEY          (1<<1)    //清停止键停止
    #define CLEAR_END_OF_PROTECT           (1<<2)    //清保护停止 
    #define CLEAR_END_OF_CMD               (1<<3)    //清命令停止*/

    #define FPGA_AD_FIFO_REG               (0xF1)    //R,AD数据的FIFO寄存器

    #define FPGA_AD_CONFIG_CMD_REG         (0xF3)    //W,ADS1255配置命令寄存器
    #define FPGA_AD_CONFIG_DATA_REG        (0xF4)    //W,ADS1255配置数据寄存器



    extern uint32_t ulFpgaCountFrequency;    //FPGA计数时钟标准频率


    void BSP_FPGA_Init(void);
    HAL_StatusTypeDef fpgaWriteBuffer(uint32_t ulWriteAddr, uint8_t *pucBuffer, uint32_t ulLen);
    HAL_StatusTypeDef fpgaReadBuffer(uint32_t ulReadAddr, uint8_t *pucBuffer, uint32_t ulLen);
    void fpgaWriteByte(uint32_t ulWriteAddr, uint8_t ucVal);
    uint8_t fpgaReadByte(uint32_t ulReadAddr);

    #ifdef __cplusplus
}
#endif

#endif /* __BSP_DRIVER_FPGA_H */