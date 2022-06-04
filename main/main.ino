#include "database.h"
#include "lock.h"
#include "screen.h"
#include "keypad.h"
#include "door_code.h"
#include "card_reader.h"
#include "card_whitelist.h"
//
//  Allows to control the door state
//
Lock lock;

//
//  Allows to enter desired text on the screen
//
Screen screen;

//
//  Allows to communicate with the user
//
Keyboard keyboard;

//
//  Allows to control the current door code
//
Door_Code code;

//
//  Allows to read and convert RFID card tokens
//
Card_Reader reader;

//
//  Allows to control and monitor RFID cards
//
Card_Whitelist whitelist;

//
//  Locks the door after three wrong authorizations in a row
//
int counter;

//
//  Allows to save data about operations and configuration
//
Database database;

String code_str;
String admin;
int n;

//
//  Swaps program behavior based on user's choice
//
void menu() 
{
    screen.message( "AWAITING", 0, 4, 0, true );
    screen.message( "CARD", 1, 6 );
    
    auto card_b = reader.get_card();

    if (card_b == NULL)
    {
      return;
    }

    auto card = reader.convert(card_b);

    if ( !whitelist.is_administrator_card( card ) )
    {
        database.write_data( card_b, 5 );
        return;
    }

    database.write_data( card_b, 4 );

    screen.message( "DOOR", 0, 6, 0, true );
    screen.message( "SETTINGS", 1, 4 );
    
    //auto time_to_wait = millis() + 10000;

    auto key = keyboard.get_key();
    
    while ( key != '*' )
    {
        key = keyboard.get_key();
        switch ( key ) 
        {
            case '1' :
                {
                    screen.message( "AWAITING", 0, 4, 0, true );
                    screen.message( "NEW CARD", 1, 4 );
                                        
                    auto status = whitelist.add_card( reader, database );

                    if ( status == Card_Status::Exists )
                    {
                        screen.message( "ALREADY", 0, 5, 0, true );
                        screen.message( "INSERTED", 1, 4, 3000);
                    }
                    else if ( status == Card_Status::Inserted )
                    {
                        screen.message( "SUCCESSFULLY", 0, 2, 0, true );
                        screen.message( "INSERTED", 1, 4, 3000);
                    }
                }
                screen.message( "DOOR", 0, 6, 0, true );
                screen.message( "SETTINGS", 1, 4 );
                break;
            case '2' :
                {
                    screen.message( "AWAITING", 0, 4, 0, true );
                    screen.message( "NEW CARD", 1, 4 );
                    
                    auto status = whitelist.change_administrator_card( reader, database );

                    if ( status != Card_Status::Empty )
                    {
                        screen.message( "SUCCESSFULLY", 0, 2, 0, true );
                        screen.message( "CHANGED CARD", 1, 2, 3000);
                    }
                }
                screen.message( "DOOR", 0, 6, 0, true );
                screen.message( "SETTINGS", 1, 4 );
                break;
            case '3' :
                {
                    screen.message( "INSERT CODE", 0, 2, 0, true );

                    auto status = code.swap( screen, keyboard, database );

                    if ( status != Door_Code_Status::Empty )
                    {
                        screen.message( "SUCCESSFULLY", 0, 2, 0, true );
                        screen.message( "CHANGED CODE", 1, 2, 3000);
                    }
                }
                screen.message( "DOOR", 0, 6, 0, true );
                screen.message( "SETTINGS", 1, 4 );
                break;
            case '4' :
                Serial.println('2');
                whitelist.print_cards();
                Serial.println('0');
                break;
            case '5' :
                Serial.println('3');
                whitelist.remove_card(database);
                break;
            case '6' :
                Serial.println('1');
                database.read_data();
                Serial.println('0');
                break;
            case '7' :
                database.clear_data();
                break;
            case '8' :
                database.reset_to_default();
                assign_values();
                break;
            default:
                break;
        }
    }

    screen.flush( 2000 );
}

//
//
//
bool has_passed_two_factor_authentication() 
{
    screen.message( "AWAITING", 0, 4, 0, true );
    screen.message( "CARD", 1, 6, 2500, false, false );
    
    auto card_b = reader.get_card();
    auto card = reader.convert(card_b);

    if ( !whitelist.is_administrator_card( card ) )
    {
        if ( !whitelist.contains_card( card ) )
        {
            database.write_data( card_b, 3 );
            return false;
        }
    }

    screen.message( "INSERT CODE", 0, 2, 0, true, false );
    
    auto access_code = keyboard.enter( 4, screen );
    auto is_valid = code.is_valid( access_code );

    if ( is_valid == Door_Code_Status::Valid )
    {
       database.write_data( card_b, 1 );
       screen.message( "SUCCESSFULLY", 0, 2, 0, true );
       screen.message( "AUTHORIZED", 1, 3 );
    }
    else
    {
        database.write_data( card_b, 2 );
    }
    
    return is_valid == Door_Code_Status::Valid;
}

//
//  
//
void handle_lock() 
{
    auto should_open = has_passed_two_factor_authentication();
    
    if ( should_open )
    {
        lock.open();
    }
    
    
    screen.flush( 1000 );
}

//
//
//
void force_door_unlock()
{
    screen.message( "AWAITING", 0, 4, 0 );
    screen.message( "ADMINISTRATOR", 1, 2 );

    auto card = reader.get_card_wait_free();

    if ( whitelist.is_administrator_card( card ) )
    {
        code.unlock_doors();
        screen.flush( 0 );
    }
}

//
//  assigns values saved in eeprom
//
void assign_values()
{
  database.get_values(code_str, admin, n);
  if (n>0)
  {
        for (int i=0; i<n; i++)
        {
          auto card = database.get_card(i);
          whitelist.add_c(card);
        }
  }
  code.set_code( code_str );
  whitelist.set_admin( admin );
}

void setup()
{
    database.setup();
    assign_values();
    lock.setup();
    reader.setup();
    screen.setup();
    Serial.begin(9600);
}

void loop() 
{
    if ( code.should_lock_doors() )
    {
        force_door_unlock();
        return;
    }
    
    screen.message( "PRESS # TO", 0, 3 );
    screen.message( "UNLOCK DOORS", 1, 2 );
    
    switch ( keyboard.get_key() )
    {
        case '*':
            menu();
            break;
        case '#':
            handle_lock();
            break;
        default:
            break;
    }
}
