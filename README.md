# HSI-bar_Prius2G-Arduino
HSI bar for the Prius 2G with Arduino and CanBus Shield

You'll need an Arduino, CanBus Shield and a 2x16 serial LCD
The code is for Arduino IDE 1.00

Tested with a European 2005 Prius
On some versions you need to check the P,R,N,D codes and change the lines where the mud2 variable is used

HSI bar allways present on top LCD row

Using the UP button on the CanBus Shield you can change screens (short press) or reset values (long press)

Download this file to read the rest of the explanations.

Description of bottom row on LCD for the 4 screens:

1.
Left: Battery current (negative values mean battery is charging, positive values mean battery is discharging)
Middle: ICE cooling liquid temperature (in ºC) and SOC (Battery state), alternate every 2 seconds
Right: ICE RPM, if there is a E after the value it means ICE is just rotating but not producing power

2. Trip 1
Left: Current ICE consumption in liters/100km
Middle: Trip 1 travelled distance
Right: Trip 1 average consuption in liters/100km

3. Trip 2 - Same as trip 1

4. 
Left: Predicted range
Middle: Current state of the fuel tank
Right: EV driving ratio (50% means ICE is turned on half the time)



