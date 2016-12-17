
#include "mbed.h"
#include "SerialBuffered.h"
 
//extern Serial loggerSerial;
 
SerialBuffered::SerialBuffered( size_t bufferSize, PinName tx, PinName rx, int baud ) : RawSerial(  tx,  rx, baud ) 
{
    m_buffSize = 0;
    m_contentStart = 0;
    m_contentEnd = 0;
    m_timeout = 1.0;
   
    
    attach( this, &SerialBuffered::handleInterrupt );
    
    m_buff = (uint8_t *) malloc( bufferSize );
    if( m_buff == NULL )
    {
        //loggerSerial.printf("SerialBuffered - failed to alloc buffer size %d\r\n", (int) bufferSize );
    }
    else
    {
        m_buffSize = bufferSize;
    }
}


SerialBuffered::~SerialBuffered()
{
    if( m_buff )
        free( m_buff );
}

void SerialBuffered::setTimeout( float seconds )
{
    m_timeout = seconds;
}
    
size_t SerialBuffered::readBytes( uint8_t *bytes, size_t requested )
{
    int i = 0;

    for( ; i < requested; )
    {
        int c = getc();
        if( c < 0 )
            break;
        bytes[i] = c;
        i++;
    }
    
    return i;
        
}


int SerialBuffered::getc()
{
    m_timer.reset();
    m_timer.start();
    while( m_contentStart == m_contentEnd )
    {
      

        wait_ms( 1 );
        if( m_timeout > 0 &&  m_timer.read() > m_timeout )
            return EOF;
    }

    m_timer.stop();
   
    uint8_t result = m_buff[m_contentStart++];
    m_contentStart =  m_contentStart % m_buffSize;

   
    return result;    
}


int SerialBuffered::readable()
{
    return m_contentStart != m_contentEnd ;
}

#include "stm32l4xx_hal_def.h"
#include "stm32l4xx_hal_uart.h"

void SerialBuffered::handleInterrupt()
{
    
    while( RawSerial::readable())
    {
        if( m_contentStart == (m_contentEnd +1) % m_buffSize)
        {
           //loggerSerial.printf("SerialBuffered - buffer overrun, data lost!\r\n" );
          RawSerial::getc();
          //UART_HandleTypeDef huart;
          //huart.Instance = (USART_TypeDef *)USART1_BASE;
          //__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_RXNE);
          //volatile uint32_t tmpval = huart.Instance->RDR;
          
          //break;

        }
        else
        {
          
            m_buff[ m_contentEnd ++ ] = RawSerial::getc();
            m_contentEnd = m_contentEnd % m_buffSize;
            
           
           
        }
    }
}

