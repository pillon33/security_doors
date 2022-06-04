#include <Keypad.h>

class Keyboard 
{
  
  private:
  
    byte KEYBOARD_ROW_PINS[4] = { 7, 2, 3, 5 };
    
    byte KEYBOARD_COL_PINS[3] = { 6, 8, 4 };
    
    char KEYS[4][3] = {
      {'1', '2', '3'},
      {'4', '5', '6'},
      {'7', '8', '9'},
      {'*', '0', '#'}
    };

    Keypad keypad = Keypad( makeKeymap( KEYS ), KEYBOARD_ROW_PINS, KEYBOARD_COL_PINS, 4, 3 );
    
  public:

    char get_key()
    {
        return keypad.getKey();
    }

    String enter( int target_length, Screen _screen ) 
    {      
        auto target = String();
        auto time_to_abort = millis() + 10000;
        
        while ( target.length() != target_length ) 
        {           
            if ( millis() >= time_to_abort )
            {
                return ""; 
            }
            
            auto key = get_key();
          
            if ( isDigit( key ) ) 
            {
                target += key;
            }

            _screen.message( target, 1, 6 );
        }
        
        return target;
    }
};
