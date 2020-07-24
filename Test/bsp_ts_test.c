/**
  ******************************************************************************
    @file    	bsp_ts_test.c
    @author
    @version
    @date
    @brief		触摸屏测试
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"


void ctp_test( void );
	



/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void gui_draw_hline( uint16_t x0, uint16_t y0, uint16_t len, uint16_t color )
{
    if ( len == 0 )
    {
        return;
    }

    LCD_Fill( x0, y0, x0 + len - 1, y0, color );
}

/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void gui_fill_circle( uint16_t x0, uint16_t y0, uint16_t r, uint16_t color )
{
    uint32_t i;
    uint32_t imax = ( ( uint32_t )r * 707 ) / 1000 + 1;
    uint32_t sqmax = ( uint32_t )r * ( uint32_t )r + ( uint32_t )r / 2;
    uint32_t x = r;
    gui_draw_hline( x0 - r, y0, 2 * r, color );

    for ( i = 1; i <= imax; i++ )
    {
        if ( ( i * i + x * x ) > sqmax ) // draw lines from outside
        {
            if ( x > imax )
            {
                gui_draw_hline( x0 - i + 1, y0 + x, 2 * ( i - 1 ), color );
                gui_draw_hline( x0 - i + 1, y0 - x, 2 * ( i - 1 ), color );
            }

            x--;
        }

        // draw lines from inside (center)
        gui_draw_hline( x0 - x, y0 + i, 2 * x, color );
        gui_draw_hline( x0 - x, y0 - i, 2 * x, color );
    }
}

/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void lcd_draw_bline( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color )
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    if ( x1 < size || x2 < size || y1 < size || y2 < size )
    {
        return;
    }

    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if ( delta_x > 0 )
    {
        incx = 1;    //设置单步方向
    }

    else if ( delta_x == 0 )
    {
        incx = 0;    //垂直线
    }

    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if ( delta_y > 0 )
    {
        incy = 1;
    }

    else if ( delta_y == 0 )
    {
        incy = 0;    //水平线
    }

    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y )
    {
        distance = delta_x;    //选取基本增量坐标轴
    }

    else
    {
        distance = delta_y;
    }

    for ( t = 0; t <= distance + 1; t++ ) //画线输出
    {
        gui_fill_circle( uRow, uCol, size, color ); //画点
        xerr += delta_x ;
        yerr += delta_y ;

        if ( xerr > distance )
        {
            xerr -= distance;
            uRow += incx;
        }

        if ( yerr > distance )
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void ctp_test( void )
{
    uint16_t t = 0;
    uint16_t i = 0;
    uint16_t lastpos[5][2];		//最后一次的数据
    uint16_t POINT_COLOR_TBL[5] = {RED, GREEN, BLACK, BROWN, GRED};

	LCD_Clear(BLUE);//清屏	
	
    for ( ;; )
    {
        ts_driver->Scan( 0 );

        for ( t = 0; t < 5; t++ )
        {
            if ( ( ts_driver->ucStatus ) & ( 1 << t ) )
            {
//                SEGGER_RTT_printf(0, "XY:(%d, %d)\r\n", ts_driver->x[t], ts_driver->y[t]);
                if ( ts_driver->x[t] < lcddev.width && ts_driver->y[t] < lcddev.height )
                {
                    if ( lastpos[t][0] == 0XFFFF )
                    {
                        lastpos[t][0] = ts_driver->x[t];
                        lastpos[t][1] = ts_driver->y[t];
                    }

                    lcd_draw_bline( lastpos[t][0], lastpos[t][1], ts_driver->x[t], ts_driver->y[t], 2, POINT_COLOR_TBL[t] ); //画线
                    lastpos[t][0] = ts_driver->x[t];
                    lastpos[t][1] = ts_driver->y[t];

                    if ( ts_driver->x[t] > ( lcddev.width - 24 ) && ts_driver->y[t] < 20 )
                    {
                        LCD_Clear(BLUE);//清屏
                    }
                }
            }
            else
            {
                lastpos[t][0] = 0XFFFF;
            }
        }
		vTaskDelay(pdMS_TO_TICKS( 5 ));
    }
}
//---------------------------------------------------
