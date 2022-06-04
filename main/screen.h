#include <LiquidCrystal_I2C.h>

class Screen 
{
  
  private:
  
    LiquidCrystal_I2C LCD = LiquidCrystal_I2C( 0x27, 16, 2 );
      
  public:
  
    void setup() 
    {
        LCD.init();
        LCD.backlight();
    }

    void flush( int delay_time )
    {
        delay( delay_time );
        LCD.clear();
    }
    
    void message( 
        String text, 
        int row = 0, 
        int column = 0, 
        int delay_time = 0, 
        bool flush_at_start = false, 
        bool flush_at_end = false ) 
    {
        if ( flush_at_start )
        {
            LCD.clear();
        }
        
        LCD.setCursor( column, row  );
        LCD.print( text );
      
        if ( delay_time != 0 ) 
        {
            delay( delay_time );
        }
      
        if ( flush_at_end ) 
        {
            LCD.clear();
        }
    }
};
