# Introduction 

So first I nicely asked the company who produces the Coils for a schematic to see if they would help me out and save me from this headache but they said no. 

# The Process

So I then began to break down the circuit into bite sized pieces so I could find the overall functionality so I would be able to modify it. The first step taken was to find each individual chip's datasheets and they were as follows:

* [HY1808A N channel MOSFET](https://alltransistors.com/adv/pdfview.php?doc=hy1808ap_hy1808m_hy1808b_hy1808ps_hy1808pm.pdf&dire=_1) (2x)
* [IRFP264 N channle Power MOSFET](https://www.vishay.com/docs/91217/irfp264.pdf) (1x)
* [NE555 timer](https://www.ti.com/lit/ds/symlink/ne555.pdf?ts=1736063138480&ref_url=https%253A%252F%252Fwww.mouser.ch%252F) (2x)
* [XLSEMI XL7005A Buck Converter](https://www.lcsc.com/datasheet/lcsc_datasheet_1811081614_XLSEMI-XL7005A_C50848.pdf) (1x)
* [78L05 Voltage Regulator](https://www.utmel.com/components/78l05-voltage-regulator-circuit-equivalent-and-78l05-vs-7805?id=431#cat0) (1x)
* [LM393 Dual Comparator](https://www.ti.com/lit/ds/symlink/lm393.pdf?ts=1736064881127&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FLM393) (1x)
* Unknown Bluetooth Chip (1x)


There is a switch which seperates the bluetooth and the astable mode.

# What to change

As the tesla coil is controlled by a PWM and switching for charging and discharging of the second coil it was quickly determined that the 555 Chips was the driver for the circuit so I attempted my first adjustment by removing the 555 and attaching the timer peripheral to the output of this chip as well as tying the grounds together from the STM32 to the Coil. This did not work. So I took a step backwards and looked more into how the 555 Works and the output depends greatly on the magnitude of the Vcc. So I would havd to trigger the Chip myself instead.

## Approximate Schematic / Block Diagram 
