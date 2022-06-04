#include <LinkedList.h>

enum class Card_Status
{
    Empty, Exists, Inserted
};

class Card_Whitelist 
{
  
  private:
      
    LinkedList<String> container;
    
    String administrator_card = "17916638149";
    
  public:

    //
    //  Adds one card to the whitelist
    //
    Card_Status add_card( Card_Reader _reader, Database _db )
    {
        auto card_b = _reader.get_card();

        if ( card_b == NULL )
        {
            return Card_Status::Empty;
        }

        auto card = _reader.convert(card_b);
        
        auto hasCard = contains_card( card );
          
        if ( !hasCard ) 
        {
            container.add( card );
            _db.add_card(card_b);
            return Card_Status::Inserted;
        }

        return Card_Status::Exists;
    }

    //
    //  changes admin card to given value
    //
    void set_admin( String admin )
    {
      administrator_card = admin;
    }

    //
    //  adds given card to the whitelist, used when getting card whitelist from eeprom
    //
    void add_c(String card)
    {
      container.add( card );
    }

    //
    //  removes card from the whitelist
    //
    void remove_card( Database _db ) 
    {
        if ( container.size() == 0 ) 
        {
            Serial.println("No cards to remove");
            return;  
        }

        auto time_to_abort = millis() + 10000;

        Serial.println( "Choose index of the card you wish to delete: " );
        
        while ( millis() <= time_to_abort ) 
        {
            if ( Serial.available() > 0 ) 
            {
                auto index = Serial.readString().toInt() - 1;
                auto card = container.remove(index);
                _db.remove_card(index);
                
                Serial.println( "Removed card: " + card );
                return;
            }
        }
    }

    //
    //  prints all cards on whitelist using Serial communication
    //
    void print_cards() 
    {
        if ( container.size() == 0 ) 
        {
            Serial.println("Whitelist empty");
            return;  
        }
        
        for ( auto i = 0; i < container.size(); ++i )
        {
            Serial.println( String( i + 1 ) + ") " + container.get(i) );
        }
    }

    //
    //  checks if given card is on the whiteelist
    //
    bool contains_card( String target ) 
    {
        for ( auto i = 0; i < container.size(); ++i )
        {
            if ( container.get(i) == target )
            {
                return true;
            }
        }
            
        return false;
    }

    //
    //  checks if given card is the admin card
    //
    bool is_administrator_card( String target ) 
    {
        return target == administrator_card;
    }

    //
    //  changes admin card
    //
    Card_Status change_administrator_card( Card_Reader _reader, Database _db ) 
    {
        auto card_b = _reader.get_card();

        if ( card_b == NULL )
        {
            return Card_Status::Empty;
        }

        auto card = _reader.convert(card_b);
        administrator_card = card;
        _db.change_admin(card_b);

        return Card_Status::Exists;
    }
};
