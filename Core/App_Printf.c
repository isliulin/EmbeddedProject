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

/*******************************************************
  * @brief  :线程安全的printf
  * @note   :
  * @param  :同printf
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void  App_Printf(char *format, ...)
{
	char  buf_str[512 + 1];
	va_list   v_args;


	va_start(v_args, format);
	(void)vsnprintf((char *)& buf_str[0],
				    (size_t) sizeof(buf_str),
					(char const *)format,
					v_args);
	va_end(v_args);

	xSemaphoreTake(xPrintfMutexSemaphore_Handle, portMAX_DELAY);

	printf("%s", buf_str);

	xSemaphoreGive(xPrintfMutexSemaphore_Handle);
}

//-----------------------------------------------