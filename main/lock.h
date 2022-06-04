#include <Servo.h>

class Lock 
{
  
  private:
  
    int pin = 17;
    
    int locked_angle = 0;
    
    int opened_angle = 90;
    
    Servo servo;
    
  public:
  
    void setup() 
    {
        servo.attach( pin );
        servo.write( locked_angle );
    }
    
    void open() 
    {
        servo.write( opened_angle );
        delay( 10000 );
        servo.write( locked_angle );
    }
};
