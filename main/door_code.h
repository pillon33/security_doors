enum class Door_Code_Status
{
    Empty, Invalid, Valid
};

class Door_Code 
{
  
  private:

    int attempts = 0;
    
    String door_code = "1234";
    
  public:

    void unlock_doors()
    {
        attempts = 0; 
    }

    void set_code( String code )
    {
      door_code = code;
    }
    
    bool should_lock_doors()
    {
        return attempts == 3;
    }
    
    Door_Code_Status is_valid( String code ) 
    {
        if ( code == "" )
        {
            return Door_Code_Status::Empty;
        }

        if ( code != door_code )
        {
            attempts += 1;
            return Door_Code_Status::Invalid;
        }

        attempts = 0;
        
        return Door_Code_Status::Valid;
    }

    Door_Code_Status swap( Screen _screen, Keyboard _keyboard, Database _db ) 
    {   
        auto code = _keyboard.enter( 4, _screen );
        auto status = is_valid( code );

        if ( status == Door_Code_Status::Empty )
        {
            return Door_Code_Status::Empty;
        }

        door_code = code;
        _db.change_code(code);

        return Door_Code_Status::Valid;
    }
};
