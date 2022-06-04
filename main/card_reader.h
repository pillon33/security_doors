#include <SPI.h>
#include <MFRC522.h>

class Card_Reader 
{
  private:
  
    int SS_PIN = 10;
    
    int RST_PIN = 9;
    
  public:
  
    MFRC522 rfid = MFRC522( SS_PIN, RST_PIN );

    void setup() 
    {
        SPI.begin();
        rfid.PCD_Init();
    }

    //
    //  Converts card from byte array to string
    //
    String convert( byte* arr ) 
    {
        String output;
        
        for ( auto i = 0; i < 4; ++i ) 
        {
            output += String( arr[i] );
        }
        
        return output;
    }

    String get_card_wait_free()
    {
        if ( !rfid.PICC_IsNewCardPresent() )
        {
            return "";
        }
      
        if ( !rfid.PICC_ReadCardSerial() )
        {
            return "";
        }

        auto card = convert( rfid.uid.uidByte );
        
        return card;
    }

    //
    //  Returns card as byte array or NULL if couldn't read the card
    //
    byte* get_card() 
    {
        auto time_to_wait = millis() + 10000;
        
        while ( millis() <= time_to_wait ) 
        {
            if ( !rfid.PICC_IsNewCardPresent() )
            {
                continue;
            }
          
            if ( !rfid.PICC_ReadCardSerial() )
            {
                continue;
            }
            
            return rfid.uid.uidByte;
        }

        return NULL;
    }
};
