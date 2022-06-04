####################################################     SECURITY DOORS    ####################################################

                                                Project made for arduino classes
                            
Arduino modules used:
    - 4x3 Keypad
    - I2C LCD screen
    - 180 degrees Servo
    - RFID
    

Security door lock with two factor authentication via rfid card and 4 digit code. Fully configurable via admin interface. 
It saves all data about authorization in eeprom, python script saves the data on computer in json file.
All changes in configuration are saved in eeprom as well. After entering 3 wrong codes in a row system locks 
until admin card is inserted to prevent breaking attempts.
