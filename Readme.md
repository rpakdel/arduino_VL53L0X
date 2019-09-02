#  Parking sensor using VL53L0X LIDAR


* [Sensor unit](https://www.st.com/en/imaging-and-photonics-solutions/vl53l0x.html) or from [Adafruit](https://www.adafruit.com/product/3317)
* Distance is measured on separate Arduino and sent to display unit via [NRF24L01](https://www.nordicsemi.com/?sc_itemid=%7B36BDC7E1-5E35-4967-8B4D-E41DCD96512F%7D)
* Display unit shows forward, stop or reverse depending on user defined stop distance
* Stop distance is stored on EEPROM and restored after power loss