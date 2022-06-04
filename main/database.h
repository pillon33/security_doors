#include <EEPROM.h>

class Database
{
  private:
    int last_id = 156;
    byte _default_data[8] = {0x01, 0x02, 0x03, 0x04, 0xb3, 0xa6, 0x26, 0x95};

  public:
    short id;
    short idx;
    
    void setup()
    {
      //  it's 255 if this place in eeprom was never used, max id value is 156
      if ( EEPROM.read(0) == 255)
      {
        EEPROM.write(0, 0);
        id = 0;
      }
      else
      {
        id = EEPROM.read(0);
      }
      //
      //  1 byte for id
      //  4 bytes for card uid
      //  1 byte for status
      //  id multiplied by 6 beacuse one dataset takes 6 bytes
      //
      short idx = 6*id + 1;
    }

    //
    //  writes given data to eeprom
    //
    void write_data(byte* card, byte operation_status)
    {
      if ( id == last_id )
      {
        return;
      }
      idx = 6*id + 1;
      
      EEPROM.write(idx++, id++);
      EEPROM.write(0, id);
      for ( short i=0; i<4; i++ )
      {
        EEPROM.write(idx++, card[i]);
      }
      EEPROM.write(idx++, operation_status);
    }

    //
    //  prints all data from eeprom
    //
    void read_data()
    {
      // min idx is 1, 0 is reserved for current id
      if (idx == 0)
      {
        Serial.println( "Database Empty" );
        return;
      }

      for ( short i=0; i<id; i++ )
      {
        String print_data = "";
        print_data = String( EEPROM.read( 6*i + 1 ) ) + ", ";
        for ( short j=1; j<5; j++ )
        {
          print_data += String( EEPROM.read( 6*i + j + 1 ) ) + " ";
        }


        // makes database entry more legible
        auto op_status = String( EEPROM.read( 6*i + 6 ) );
        if (op_status == "1")
        {
          op_status = "authorized successfully";
        }
        else if (op_status == "2")
        {
          op_status = "wrong code";
        }
        else if (op_status == "3")
        {
          op_status = "wrong card";
        }
        else if (op_status == "4")
        {
          op_status = "admin in configuration mode";
        }
        else if (op_status == "5")
        {
          op_status = "wrong admin in configuration mode";
        }
        else
        {
          op_status = "unknown value";
        }

        
        print_data += ", " + op_status + ";";
        Serial.println( print_data );
      }
    }

    //
    //  clears data from eeprom
    //
    void clear_data()
    {
      if ( idx == 0 )
      {
        return;
      }

      for ( short i=0; i<idx; i++ )
      {
        EEPROM.write(i, 0);
      }
      idx = 0;
      id = 0;
    }

    //
    //  clears all data from eeprom
    //
    void clear_all()
    {
      for ( int i=0; i<=1024; i++ )
      {
        EEPROM.write(i, 0);
      }
    }
    
    //
    //  saves default data to eeprom
    //
    void save_default()
    {
      int idx = 936;
      for (int i=0; i<8; i++)
      {
        EEPROM.write(idx++, _default_data[i]);
      }
      EEPROM.write(idx++, 0);   // to make sure there's no garbage in the number of cards place.
    }

    //
    //  resets configuration to default
    //
    void reset_to_default()
    {
      clear_all();
      save_default();
    }

    //
    //  gets the values from the memory
    //
    void get_values(String &code, String &admin, int &n)
    {
      code = "";
      for (int i=936; i<940; i++)
      {
        code += String(EEPROM.read(i));
      }
    
      admin = "";
      for (int i=940; i<944; i++)
      {
        admin += String(EEPROM.read(i));
      }
    
      n = int(EEPROM.read(944));
    }

    //
    //  saves card in eeprom
    //
    void add_card(byte card[4])
    {
      int n = int(EEPROM.read(944));
      int idx = 945 + n*4;
    
      if (idx + 4 >= 1023)
        return;
      
      for (int i=0; i<4; i++)
      {
        EEPROM.write(idx++, card[i]);
      }
      EEPROM.write(944, byte(n+1));
    }

    //
    //  returns ith card from eeprom
    //
    String get_card(int i)
    {
      int n = int(EEPROM.read(944));
      if (i > n)
        return "";
      int idx = 945 + i*4;
      if (idx + 3 >= 1023)
        return "";
    
      String card = "";
      for (int j=0; j<4; j++)
      {
        card += String(EEPROM.read(idx++));
      }
      return card;
    }

    //
    //  removes ith card from eeprom
    //
    void remove_card(int i)
    {
      int n = int(EEPROM.read(944));
      if (i > n)
        return;
      int idx = 945 + i*4;
      if (idx + (n - i)*4 >= 1023)
        return;
    
      if (i == n)
      {
        for (int j=0; j<4; j++)
        {
          EEPROM.write(idx++, 0);
        }
        EEPROM.write(944, byte(n-1));
        return;
      }
    
      for ( int j=i; j<n; j++)
      {
        for ( int k=0; k<4; k++ )
        {
          EEPROM.write(idx++, EEPROM.read(idx + 4));
        }
      }
    
      for (int j=0; j<4; j++)
      {
        EEPROM.write(idx++, 0);
      }
      EEPROM.write(944, byte(n-1));
    }

    //
    //  changes code in eeprom
    //
    void change_code(String code)
    {
      int idx = 936;
      for (int i=0; i<4; i++)
      {
        EEPROM.write(idx++, byte(code[i]) - 48);
      }
    }

    //
    // changes admin card in eeprom
    //
    void change_admin(byte card[4])
    {
      int idx = 940;
    
      for (int i=0; i<4; i++)
      {
        EEPROM.write(idx++, card[i]);
      }
    }
};
