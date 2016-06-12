HDC1000 Arduino Library
=======================

### Setup your sensor:
First, declare it:

`HDC1000 mySensor;`

Default address is 0x40 and the DRDYn pin is disabled. You can change your address by declaring your sensor like this:

`HDC1000 mySensor(addr, DRDYn_pin);`

Then, initialize it:

`mySensor.begin();`

Default options are set to enable both temperature and humidity readings, at 14-bit resolution, with the integrated heater on. You can change them by initializing your sensor like this:

`mySensor.begin(mode, resolution, heater);`

"mode" can be _HDC1000_SINGLE_MEASUR_, _HDC1000_BOTH_TEMP_HUMI_ or _HDC1000_RST_.

"resolution" can be _HDC1000_TEMP_HUMI_14BIT_, _HDC1000_TEMP_11BIT_, _HDC1000_HUMI_11BIT_, _HDC1000_HUMI_8BIT_.

"heater" can be _HDC1000_HEAT_ON_ or _HDC1000_HEAT_OFF_.

### Read some values
You can read temperature and humidity by using the following commands:

`double temperature = mySensor.getTemp();` in Celsius degrees.

`double humidity = mySensor.getHumi();` in %

You can also access raw values from the sensor:

`uint16_t temp = mySensor.getRawTemp();`

`uint16_t humi = mySensor.getRawHumi();`

### Check your battery

The following code returns '1' if the battery voltage is <2.8V, '0' otherwise.

`boolean isDead = mySensor.battery();`

### Read the internal registers' configuration:

This returns the 16-bit value of the internal configuration register (0x02): the last 8 bits are always zero and leading zeros are not displayed.

`uint16_t config = mySensor.readConfig();`

Happy making!

This software is written by Pillar Zuo for seeed studio
and is licensed under The MIT License. Check License.txt for more information.

Contributing to this software is warmly welcomed. You can do this basically by
forking, committing modifications and then pulling requests (follow the links above
for operating guide). Adding change log and your contact into file header is encouraged.
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. 
Benefiting from local manufacture power and convenient global logistic system, 
we integrate resources to serve new era of innovation. Seeed also works with 
global distributors and partners to push open hardware movement.
