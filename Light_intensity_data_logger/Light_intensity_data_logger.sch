EESchema Schematic File Version 4
LIBS:Light_intensity_data_logger-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 10600 4400 0    50   ~ 0
Vin
$Comp
L power:GND #PWR0102
U 1 1 5E1FDE5A
P 8300 4700
F 0 "#PWR0102" H 8300 4450 50  0001 C CNN
F 1 "GND" V 8305 4572 50  0000 R CNN
F 2 "" H 8300 4700 50  0001 C CNN
F 3 "" H 8300 4700 50  0001 C CNN
	1    8300 4700
	0    1    1    0   
$EndComp
$Comp
L bosch:BME280 U1
U 1 1 5E2027F6
P 2850 3050
F 0 "U1" H 2421 3096 50  0000 R CNN
F 1 "BME280" H 2421 3005 50  0000 R CNN
F 2 "digikey-footprints:Pressure_Sensor_LGA-8_2.5x2.5mm_BME280" H 2850 2850 50  0001 C CNN
F 3 "https://www.bosch-sensortec.com/media/boschsensortec/downloads/environmental_sensors_2/humidity_sensors_1/bme280/bst-bme280-ds002.pdf" H 2850 2850 50  0001 C CNN
F 4 "828-1063-1-ND" H 2850 3050 50  0001 C CNN "Part#"
	1    2850 3050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5E213968
P 2750 3650
F 0 "#PWR0103" H 2750 3400 50  0001 C CNN
F 1 "GND" H 2755 3477 50  0000 C CNN
F 2 "" H 2750 3650 50  0001 C CNN
F 3 "" H 2750 3650 50  0001 C CNN
	1    2750 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 5E21418B
P 2950 3650
F 0 "#PWR0104" H 2950 3400 50  0001 C CNN
F 1 "GND" H 2955 3477 50  0000 C CNN
F 2 "" H 2950 3650 50  0001 C CNN
F 3 "" H 2950 3650 50  0001 C CNN
	1    2950 3650
	1    0    0    -1  
$EndComp
Text Label 10600 5100 0    50   ~ 0
SCL0
Text Label 8300 4300 2    50   ~ 0
SDA1
NoConn ~ 8300 4600
NoConn ~ 8300 4800
NoConn ~ 10600 4200
NoConn ~ 10600 4300
Wire Wire Line
	3450 2750 3450 2450
Connection ~ 2950 2450
Wire Wire Line
	2950 2450 2750 2450
$Comp
L device:C C1
U 1 1 5E23C716
P 3450 2300
F 0 "C1" H 3565 2346 50  0000 L CNN
F 1 "1uF" H 3565 2255 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 3488 2150 50  0001 C CNN
F 3 "https://product.tdk.com/info/en/catalog/datasheets/mlcc_commercial_general_en.pdf?ref_disty=digikey" H 3450 2300 50  0001 C CNN
F 4 "445-1604-1-ND" H 3450 2300 50  0001 C CNN "Part#"
	1    3450 2300
	1    0    0    -1  
$EndComp
Connection ~ 3450 2450
$Comp
L power:GND #PWR0105
U 1 1 5E23D3AE
P 3450 2150
F 0 "#PWR0105" H 3450 1900 50  0001 C CNN
F 1 "GND" H 3455 1977 50  0000 C CNN
F 2 "" H 3450 2150 50  0001 C CNN
F 3 "" H 3450 2150 50  0001 C CNN
	1    3450 2150
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 5E23FAAA
P 5850 3050
F 0 "#PWR0106" H 5850 2800 50  0001 C CNN
F 1 "GND" V 5750 3000 50  0000 C CNN
F 2 "" H 5850 3050 50  0001 C CNN
F 3 "" H 5850 3050 50  0001 C CNN
	1    5850 3050
	0    1    1    0   
$EndComp
Wire Wire Line
	3450 2450 3700 2450
Wire Wire Line
	3700 2450 3700 3350
Wire Wire Line
	3700 3350 3450 3350
$Comp
L Ben_custom:NHD-0420H1Z-FL-GBW-33V3 LCD1
U 1 1 5E267979
P 1450 1600
F 0 "LCD1" H 1450 2567 50  0000 C CNN
F 1 "NHD-0420H1Z-FL-GBW-33V3" H 1450 2476 50  0000 C CNN
F 2 "Ben_Custom:NHD-0420H1Z-FL-GBW-33V3_Display_with_Header" H 1450 1600 50  0001 L BNN
F 3 "http://www.newhavendisplay.com/specs/NHD-0420H1Z-FL-GBW-33V3.pdf" H 1450 1600 50  0001 L BNN
F 4 "NHD-0420H1Z-FL-GBW-33V3" H 1450 1600 50  0001 L BNN "Part#"
	1    1450 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5E268F90
P 2050 2300
F 0 "#PWR0107" H 2050 2050 50  0001 C CNN
F 1 "GND" V 2055 2172 50  0000 R CNN
F 2 "" H 2050 2300 50  0001 C CNN
F 3 "" H 2050 2300 50  0001 C CNN
	1    2050 2300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2950 2450 3200 2450
Wire Wire Line
	3200 1900 3200 2450
Connection ~ 3200 2450
Wire Wire Line
	3200 2450 3450 2450
$Comp
L power:GND #PWR0108
U 1 1 5E2D9191
P 5900 4250
F 0 "#PWR0108" H 5900 4000 50  0001 C CNN
F 1 "GND" H 6100 4200 50  0000 R CNN
F 2 "" H 5900 4250 50  0001 C CNN
F 3 "" H 5900 4250 50  0001 C CNN
	1    5900 4250
	1    0    0    -1  
$EndComp
NoConn ~ 5300 4250
$Comp
L Ben_custom:NTJD4001NT1G Q1
U 1 1 5E2E0271
P 2650 1350
F 0 "Q1" V 2450 1400 50  0000 C CNN
F 1 "NTJD4001NT1G" V 2200 1450 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_SC-70-6" H 1900 1200 50  0001 L BNN
F 3 "http://www.onsemi.com/pub/Collateral/NTJD4001N-D.PDF" H 2550 1350 50  0001 L BNN
F 4 "NTJD4001NT1GOSCT-ND" H 2550 1350 50  0001 L BNN "Part#"
	1    2650 1350
	0    1    1    0   
$EndComp
Wire Wire Line
	2050 1550 2050 1900
Wire Wire Line
	2450 1550 2350 1550
Wire Wire Line
	2050 2000 2050 2300
Connection ~ 2050 2300
Wire Wire Line
	2850 1350 2850 1450
Text Label 3250 1450 0    50   ~ 0
3v3
Wire Wire Line
	2450 1350 2450 900 
Wire Wire Line
	2450 900  2050 900 
Wire Wire Line
	5350 5350 5500 5350
$Comp
L dk_Slide-Switches:JS202011SCQN S2
U 1 1 5E261D45
P 6300 4900
F 0 "S2" H 6300 4600 50  0000 C CNN
F 1 "JS202011SCQN" H 6400 4500 50  0000 C CNN
F 2 "digikey-footprints:Switch_Slide_JS202011SCQN" H 6500 5100 50  0001 L CNN
F 3 "https://dznh3ojzb2azq.cloudfront.net/products/Slide/JS/documents/datasheet.pdf" H 6500 5200 60  0001 L CNN
F 4 "401-2002-1-ND" H 6500 6100 60  0001 L CNN "Part#"
	1    6300 4900
	1    0    0    1   
$EndComp
$Comp
L device:D D1
U 1 1 5E1F5931
P 6650 5350
F 0 "D1" H 6650 5134 50  0000 C CNN
F 1 "D" H 6650 5225 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-323" H 6650 5350 50  0001 C CNN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=14077&prodName=CUS10S30" H 6650 5350 50  0001 C CNN
F 4 "CUS10S30H3FCT-ND" H 6650 5350 50  0001 C CNN "Part#"
	1    6650 5350
	0    -1   1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5E1EC268
P 6650 5900
F 0 "#PWR0109" H 6650 5650 50  0001 C CNN
F 1 "GND" H 6655 5727 50  0000 C CNN
F 2 "" H 6650 5900 50  0001 C CNN
F 3 "" H 6650 5900 50  0001 C CNN
	1    6650 5900
	-1   0    0    -1  
$EndComp
Text Label 5900 5350 0    50   ~ 0
Vin
$Comp
L device:Battery BT2
U 1 1 5E1E70C1
P 6650 5700
F 0 "BT2" H 6758 5746 50  0000 L CNN
F 1 "Battery" H 6758 5655 50  0000 L CNN
F 2 "Ben_Custom:BH3AAW-Battery_wire_pads" V 6650 5760 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Memory%20Protection%20PDFs/SBH-331A.pdf" V 6650 5760 50  0001 C CNN
F 4 "SBH331A-ND" H 6650 5700 50  0001 C CNN "Part#"
	1    6650 5700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 1050 2550 1150
Wire Wire Line
	2550 1750 2550 1850
Text Label 850  1500 2    50   ~ 0
DB0
Text Label 850  1600 2    50   ~ 0
DB1
Text Label 850  1700 2    50   ~ 0
DB2
Text Label 850  1800 2    50   ~ 0
DB3
Text Label 850  1900 2    50   ~ 0
DB4
Text Label 850  2000 2    50   ~ 0
DB5
Text Label 850  2100 2    50   ~ 0
DB6
Text Label 850  2200 2    50   ~ 0
DB7
Text Label 850  1300 2    50   ~ 0
E
Text Label 4350 4250 2    50   ~ 0
I2C_Pullup
Text Label 850  1100 2    50   ~ 0
RS
Text Label 850  1200 2    50   ~ 0
RW
Text Label 8300 5100 2    50   ~ 0
16*
Wire Wire Line
	6500 4800 6650 4800
Wire Wire Line
	6650 4800 6650 5200
Wire Wire Line
	6650 5200 6500 5200
$Comp
L device:Jumper_NO_Small JP1
U 1 1 5E1ABBB2
P 7000 4800
F 0 "JP1" H 7000 4985 50  0000 C CNN
F 1 "Jumper_NO_Small" H 7000 4894 50  0000 C CNN
F 2 "Ben_Custom:Banana_Jack_2Pin" H 7000 4800 50  0001 C CNN
F 3 "~" H 7000 4800 50  0001 C CNN
	1    7000 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7100 4800 7100 5200
Connection ~ 6650 5200
Wire Wire Line
	6900 4800 6900 4600
Wire Wire Line
	6900 4600 6500 4600
Wire Wire Line
	6500 5000 6900 5000
Wire Wire Line
	6900 5000 6900 4800
Connection ~ 6900 4800
Text Notes 7150 4850 0    50   ~ 0
Current test
Text Label 4950 2950 0    50   ~ 0
16*
$Comp
L power:GND #PWR0110
U 1 1 5E2154D9
P 10600 4500
F 0 "#PWR0110" H 10600 4250 50  0001 C CNN
F 1 "GND" V 10605 4372 50  0000 R CNN
F 2 "" H 10600 4500 50  0001 C CNN
F 3 "" H 10600 4500 50  0001 C CNN
	1    10600 4500
	0    -1   -1   0   
$EndComp
$Comp
L conn:Conn_01x01 J1
U 1 1 5E1E9BAA
P 5150 5350
F 0 "J1" H 5250 5600 50  0000 C CNN
F 1 "Wire_to_Vbat" H 5150 5500 50  0000 C CNN
F 2 "Wire_Pads:SolderWirePad_single_SMD_5x10mm" H 5150 5350 50  0001 C CNN
F 3 "~" H 5150 5350 50  0001 C CNN
	1    5150 5350
	-1   0    0    1   
$EndComp
Wire Wire Line
	4700 4250 4900 4250
Wire Wire Line
	4900 4250 5100 4250
Wire Wire Line
	4500 4250 4700 4250
Wire Wire Line
	4350 4250 4500 4250
Connection ~ 4700 4250
Connection ~ 4900 4250
Connection ~ 4500 4250
NoConn ~ 8300 3500
NoConn ~ 8300 3400
Text Label 8300 3300 2    50   ~ 0
Wire_to_coin_cell
Text Label 6000 3750 0    50   ~ 0
A0
Text Label 5500 3750 0    50   ~ 0
A1
Wire Wire Line
	5050 2750 5150 2750
Connection ~ 5050 2750
Connection ~ 5900 3750
Wire Wire Line
	5700 3750 5900 3750
$Comp
L Ben_custom:EXB-2HV103JV_-_8x10k_resistor_array R1
U 1 1 5E2CE816
P 5900 3750
F 0 "R1" V 6044 3829 60  0000 L CNN
F 1 "EXB-2HV472JV_-_8x4.7k_resistor_array" V 6150 3829 60  0000 L CNN
F 2 "Ben_Custom:Resistor_array_1506_8x" H 6150 3415 60  0001 C CNN
F 3 "https://industrial.panasonic.com/cdbs/www-https://industrial.panasonic.com/cdbs/www-data/pdf/AOC0000/AOC0000C14.pdf" V 6650 4200 60  0001 L CNN
F 4 "Y1472CT-ND" V 5900 3750 50  0001 C CNN "Part#"
	1    5900 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	6000 3750 5900 3750
Wire Wire Line
	4950 2750 5050 2750
Text Label 5150 1200 0    50   ~ 0
11*
Text Label 4350 1400 2    50   ~ 0
10*
Text Label 5150 1000 0    50   ~ 0
9*
Text Label 4350 1200 2    50   ~ 0
7*
$Comp
L power:GND #PWR0111
U 1 1 5E326B92
P 4350 1000
F 0 "#PWR0111" H 4350 750 50  0001 C CNN
F 1 "GND" V 4355 872 50  0000 R CNN
F 2 "" H 4350 1000 50  0001 C CNN
F 3 "" H 4350 1000 50  0001 C CNN
	1    4350 1000
	0    1    1    0   
$EndComp
$Comp
L dk_Navigation-Switches-Joystick:JS5208 S1
U 1 1 5E3244FC
P 4750 1200
F 0 "S1" H 4750 1647 60  0000 C CNN
F 1 "JS5208" H 4750 1541 60  0000 C CNN
F 2 "digikey-footprints:Switch_Nav_12.4x12.4mm_SMD_JS5208" H 4950 1400 60  0001 L CNN
F 3 "http://spec_sheets.e-switch.com/specs/P200020.pdf" H 4950 1500 60  0001 L CNN
F 4 "EG4561-ND" H 4950 1600 60  0001 L CNN "Part#"
	1    4750 1200
	1    0    0    -1  
$EndComp
Connection ~ 8300 900 
Text Label 8300 3900 2    50   ~ 0
RW
$Comp
L teensy:Teensy3.6 U4
U 1 1 5E31EF8E
P 9450 3050
F 0 "U4" H 9450 5500 60  0000 C CNN
F 1 "Teensy3.6" H 9450 5400 60  0000 C CNN
F 2 "Ben_Custom:Teensy3.6_doublewide_header" H 9450 5381 60  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Sparkfun%20PDFs/DEV-14058_Web.pdf" H 9450 3100 60  0001 C CNN
F 4 "1568-1465-ND" H 9450 3050 50  0001 C CNN "Part#"
	1    9450 3050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 5E31EF84
P 8300 900
F 0 "#PWR0112" H 8300 650 50  0001 C CNN
F 1 "GND" V 8305 772 50  0000 R CNN
F 2 "" H 8300 900 50  0001 C CNN
F 3 "" H 8300 900 50  0001 C CNN
	1    8300 900 
	0    1    1    0   
$EndComp
Text Label 8300 2200 2    50   ~ 0
I2C_Pullup
Text Label 8300 4000 2    50   ~ 0
E
Text Label 8300 1100 2    50   ~ 0
DB7
Text Label 8300 1300 2    50   ~ 0
DB6
Text Label 8300 1400 2    50   ~ 0
DB5
Text Label 8300 1500 2    50   ~ 0
DB4
Text Label 8300 1600 2    50   ~ 0
DB3
Text Label 8300 1800 2    50   ~ 0
DB2
Text Label 8300 3100 2    50   ~ 0
DB1
Text Label 8300 3200 2    50   ~ 0
DB0
Text Label 8300 2100 2    50   ~ 0
11*
Text Label 8300 2000 2    50   ~ 0
10*
Text Label 8300 1900 2    50   ~ 0
9*
Text Label 8300 1700 2    50   ~ 0
7*
NoConn ~ 5300 3750
Wire Wire Line
	6150 1900 6150 3050
Wire Wire Line
	6200 3050 6150 3050
$Comp
L power:GND #PWR0113
U 1 1 5E241861
P 5450 2750
F 0 "#PWR0113" H 5450 2500 50  0001 C CNN
F 1 "GND" V 5455 2622 50  0000 R CNN
F 2 "" H 5450 2750 50  0001 C CNN
F 3 "" H 5450 2750 50  0001 C CNN
	1    5450 2750
	0    -1   -1   0   
$EndComp
NoConn ~ 10600 4100
NoConn ~ 10600 4000
NoConn ~ 10600 3700
NoConn ~ 10600 3600
NoConn ~ 10600 3500
NoConn ~ 10600 3400
NoConn ~ 10600 3300
NoConn ~ 10600 3200
NoConn ~ 10600 3100
NoConn ~ 10600 3000
NoConn ~ 10600 2900
NoConn ~ 10600 2800
NoConn ~ 10600 2700
NoConn ~ 10600 2600
NoConn ~ 10600 2500
NoConn ~ 10600 2400
NoConn ~ 10600 2300
NoConn ~ 10600 2200
NoConn ~ 10600 2100
NoConn ~ 10600 2000
NoConn ~ 10600 1900
NoConn ~ 10600 1800
NoConn ~ 10600 1700
NoConn ~ 10600 1600
NoConn ~ 10600 1500
NoConn ~ 10600 1400
NoConn ~ 10600 1300
NoConn ~ 10600 1200
NoConn ~ 10600 1100
NoConn ~ 10600 1000
NoConn ~ 10600 900 
NoConn ~ 8300 3700
NoConn ~ 8300 3600
Text Label 3950 2950 2    50   ~ 0
SCL0
Text Label 8300 4200 2    50   ~ 0
SCL1
$Comp
L power:GND #PWR0114
U 1 1 5E2152EA
P 6800 2900
F 0 "#PWR0114" H 6800 2650 50  0001 C CNN
F 1 "GND" V 6805 2772 50  0000 R CNN
F 2 "" H 6800 2900 50  0001 C CNN
F 3 "" H 6800 2900 50  0001 C CNN
	1    6800 2900
	0    -1   -1   0   
$EndComp
Text Label 8300 1200 2    50   ~ 0
2*
Wire Wire Line
	6800 3050 6850 3050
$Comp
L power:GND #PWR0115
U 1 1 5E1FF1C2
P 4950 3350
F 0 "#PWR0115" H 4950 3100 50  0001 C CNN
F 1 "GND" V 4955 3222 50  0000 R CNN
F 2 "" H 4950 3350 50  0001 C CNN
F 3 "" H 4950 3350 50  0001 C CNN
	1    4950 3350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0116
U 1 1 5E1FC8E5
P 8300 900
F 0 "#PWR0116" H 8300 650 50  0001 C CNN
F 1 "GND" V 8305 772 50  0000 R CNN
F 2 "" H 8300 900 50  0001 C CNN
F 3 "" H 8300 900 50  0001 C CNN
	1    8300 900 
	0    1    1    0   
$EndComp
Text Label 8300 2300 2    50   ~ 0
3v3
$Comp
L Ben_custom:TSL25911FN U3
U 1 1 5E1E6521
P 6500 3050
F 0 "U3" H 6500 2685 50  0000 C CNN
F 1 "TSL25911FN" H 6500 2776 50  0000 C CNN
F 2 "Ben_Custom:TSL25911FN" H 6100 3500 50  0001 C CNN
F 3 "https://ams.com/documents/20143/36005/TSL2591_DS000338_6-00.pdf" H 6100 3500 50  0001 C CNN
F 4 "TSL25911FNCT-ND" H 6500 3050 50  0001 C CNN "Part#"
	1    6500 3050
	-1   0    0    1   
$EndComp
$Comp
L Ben_custom:TCS34725FN U2
U 1 1 5E1E525D
P 4550 3050
F 0 "U2" H 4550 3617 50  0000 C CNN
F 1 "TCS34725FN" H 4550 3526 50  0000 C CNN
F 2 "Ben_Custom:TCS34725" H 4200 3550 50  0001 L BNN
F 3 "https://ams.com/documents/20143/36005/TCS3472_DS000390_2-00.pdf" H 4200 2550 50  0001 L BNN
F 4 "TCS34725FNCT-ND" H 4550 3050 50  0001 C CNN "Part#"
	1    4550 3050
	1    0    0    -1  
$EndComp
Text Notes 4150 1600 0    50   ~ 0
Joystick designed for 1.3mm PCB
Text Notes 750  2500 0    50   ~ 0
2x SSM-108-L-SV-BE-LC header
Text Notes 8750 5450 0    50   ~ 0
2x SSM-124-L-DV-LC header
Wire Wire Line
	3250 1450 3050 1450
Connection ~ 2850 1450
Wire Wire Line
	2850 1450 2850 1550
$Comp
L device:C C4
U 1 1 5E25C065
P 3050 1300
F 0 "C4" H 2800 1350 50  0000 L CNN
F 1 "100uF" H 2700 1250 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 3088 1150 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsung%20PDFs/CL31A107MQHNNNE_Spec.pdf" H 3050 1300 50  0001 C CNN
F 4 "1276-1782-1-ND" H 3050 1300 50  0001 C CNN "Part#"
	1    3050 1300
	-1   0    0    1   
$EndComp
Connection ~ 3050 1450
Wire Wire Line
	3050 1450 2850 1450
$Comp
L power:GND #PWR0117
U 1 1 5E25D700
P 3050 1150
F 0 "#PWR0117" H 3050 900 50  0001 C CNN
F 1 "GND" H 3055 1022 50  0000 R CNN
F 2 "" H 3050 1150 50  0001 C CNN
F 3 "" H 3050 1150 50  0001 C CNN
	1    3050 1150
	-1   0    0    1   
$EndComp
$Comp
L device:Battery BT3
U 1 1 5E25F4F6
P 7100 5700
F 0 "BT3" H 7208 5746 50  0000 L CNN
F 1 "Battery" H 7208 5655 50  0000 L CNN
F 2 "Ben_Custom:BH3AAW-Battery_wire_pads" V 7100 5760 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Memory%20Protection%20PDFs/SBH-331A.pdf" V 7100 5760 50  0001 C CNN
F 4 "SBH331A-ND" H 7100 5700 50  0001 C CNN "Part#"
	1    7100 5700
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 5E2603B4
P 7100 5900
F 0 "#PWR0118" H 7100 5650 50  0001 C CNN
F 1 "GND" H 7105 5727 50  0000 C CNN
F 2 "" H 7100 5900 50  0001 C CNN
F 3 "" H 7100 5900 50  0001 C CNN
	1    7100 5900
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6650 5200 7100 5200
$Comp
L device:D D2
U 1 1 5E274B07
P 7100 5350
F 0 "D2" H 7100 5134 50  0000 C CNN
F 1 "D" H 7100 5225 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-323" H 7100 5350 50  0001 C CNN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=14077&prodName=CUS10S30" H 7100 5350 50  0001 C CNN
F 4 "CUS10S30H3FCT-ND" H 7100 5350 50  0001 C CNN "Part#"
	1    7100 5350
	0    -1   1    0   
$EndComp
Connection ~ 7100 5200
Text Label 10600 5200 0    50   ~ 0
SDA0
Text Label 6800 3200 3    50   ~ 0
SCL1
Text Label 6200 3200 3    50   ~ 0
SDA1
Wire Wire Line
	5500 4550 5500 4950
$Comp
L power:GND #PWR0101
U 1 1 5E1EC75C
P 5500 5950
F 0 "#PWR0101" H 5500 5700 50  0001 C CNN
F 1 "GND" H 5505 5777 50  0000 C CNN
F 2 "" H 5500 5950 50  0001 C CNN
F 3 "" H 5500 5950 50  0001 C CNN
	1    5500 5950
	1    0    0    -1  
$EndComp
$Comp
L Ben_custom:BU2032SM-BT-GTR BT1
U 1 1 5E1F3175
P 5500 5650
F 0 "BT1" H 5454 5780 50  0000 L CNN
F 1 "BU2032SM-BT-GTR" H 5250 5850 50  0000 L CNN
F 2 "Ben_Custom:BU2032SM-BT-GTR" H 5150 5200 50  0001 L BNN
F 3 "http://www.memoryprotectiondevices.com/datasheets/BU2032SM-BT-GTR-datasheet.pdf" H 4900 5300 50  0001 L BNN
F 4 "BU2032SM-BT-GCT-ND" H 5100 5100 50  0001 L BNN "Part#"
	1    5500 5650
	0    1    1    0   
$EndComp
$Comp
L device:D D3
U 1 1 5E2E8394
P 5500 4400
F 0 "D3" V 5600 4500 50  0000 C CNN
F 1 "D" V 5500 4550 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-323" H 5500 4400 50  0001 C CNN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=14077&prodName=CUS10S30" H 5500 4400 50  0001 C CNN
F 4 "CUS10S30H3FCT-ND" H 5500 4400 50  0001 C CNN "Part#"
	1    5500 4400
	0    -1   1    0   
$EndComp
Text Label 3950 3150 2    50   ~ 0
SDA0
Text Label 8300 3800 2    50   ~ 0
33*
Text Label 6850 3050 0    50   ~ 0
33*
Text Label 4500 3750 1    50   ~ 0
SDA0
Text Label 4700 3750 1    50   ~ 0
SCL0
Text Label 4900 3750 1    50   ~ 0
SDA1
Text Label 5100 3750 1    50   ~ 0
SCL1
Wire Wire Line
	6100 5100 6100 5350
Connection ~ 6100 5100
Wire Wire Line
	5700 4950 5700 4250
$Comp
L Ben_custom:NTJD4001NT1G Q2
U 1 1 5E2C265D
P 5500 5150
F 0 "Q2" H 5450 4800 50  0000 L CNN
F 1 "NTJD4001NT1G" H 5150 4900 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_SC-70-6" H 4750 5000 50  0001 L BNN
F 3 "http://www.onsemi.com/pub/Collateral/NTJD4001N-D.PDF" H 5400 5150 50  0001 L BNN
F 4 "NTJD4001NT1GOSCT-ND" H 5400 5150 50  0001 L BNN "Part#"
	1    5500 5150
	1    0    0    1   
$EndComp
Connection ~ 5500 5350
Wire Wire Line
	6100 4700 6100 5100
Wire Wire Line
	5700 5350 6100 5350
NoConn ~ 10600 4600
Text Label 5150 1400 0    50   ~ 0
2*
NoConn ~ 8300 1000
NoConn ~ 8300 2600
NoConn ~ 8300 2700
NoConn ~ 8300 2800
NoConn ~ 10600 4900
NoConn ~ 10600 4800
Wire Wire Line
	5050 1900 5050 2750
Wire Wire Line
	4150 2950 3450 2950
Wire Wire Line
	4150 3150 3450 3150
$Comp
L device:R R2
U 1 1 5E2EF264
P 2200 1550
F 0 "R2" V 1993 1550 50  0000 C CNN
F 1 "2" V 2084 1550 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2130 1550 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/productsearch/datasheet/rchip/PYu-RC_Group_51_RoHS_L_10.pdf" H 2200 1550 50  0001 C CNN
F 4 "311-2.00HRCT-ND" V 2200 1550 50  0001 C CNN "Part#"
	1    2200 1550
	0    1    1    0   
$EndComp
Text Label 8300 3000 2    50   ~ 0
RS
NoConn ~ 8300 2500
Text Label 8300 2900 2    50   ~ 0
LED_PWM
Text Label 2550 1850 0    50   ~ 0
LED_PWM
Text Label 8300 2400 2    50   ~ 0
LCD_toggle
Text Label 2550 1050 0    50   ~ 0
LCD_toggle
Text Label 10600 5000 0    50   ~ 0
Temp_Vcc
Text Label 3200 1900 0    50   ~ 0
Temp_Vcc
Text Label 10600 4700 0    50   ~ 0
Color_Vcc
Text Label 8300 5200 2    50   ~ 0
Light_Vcc
Text Label 6150 1900 0    50   ~ 0
Light_Vcc
Text Label 5050 1900 0    50   ~ 0
Color_Vcc
Text Label 8300 4100 2    50   ~ 0
Coin_test
Text Label 5300 5050 2    50   ~ 0
Coin_test
Text Label 6050 4950 2    50   ~ 0
Bat_test
Wire Wire Line
	5900 5050 6050 5050
Wire Wire Line
	6050 5050 6050 4950
Text Label 8300 4400 2    50   ~ 0
Bat_test
Text Label 8300 4500 2    50   ~ 0
LCD_contrast
Text Label 2350 1100 2    50   ~ 0
LCD_contrast
Wire Wire Line
	2350 1100 2350 1000
Wire Wire Line
	2350 1000 2050 1000
Text Label 8300 5000 2    50   ~ 0
A1
Text Label 8300 4900 2    50   ~ 0
A0
$Comp
L device:C C2
U 1 1 5E252D52
P 5300 2750
F 0 "C2" V 5150 2700 50  0000 L CNN
F 1 "1uF" V 5450 2700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5338 2600 50  0001 C CNN
F 3 "https://product.tdk.com/info/en/catalog/datasheets/mlcc_commercial_general_en.pdf?ref_disty=digikey" H 5300 2750 50  0001 C CNN
F 4 "445-1604-1-ND" H 5300 2750 50  0001 C CNN "Part#"
	1    5300 2750
	0    1    1    0   
$EndComp
$Comp
L device:C C3
U 1 1 5E2531ED
P 6000 3050
F 0 "C3" V 5850 3000 50  0000 L CNN
F 1 "1uF" V 6150 3000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 6038 2900 50  0001 C CNN
F 3 "https://product.tdk.com/info/en/catalog/datasheets/mlcc_commercial_general_en.pdf?ref_disty=digikey" H 6000 3050 50  0001 C CNN
F 4 "445-1604-1-ND" H 6000 3050 50  0001 C CNN "Part#"
	1    6000 3050
	0    1    1    0   
$EndComp
Connection ~ 6150 3050
Wire Wire Line
	6100 4700 6100 4400
Wire Wire Line
	6100 4400 6900 4400
Wire Wire Line
	6900 4400 6900 4600
Connection ~ 6100 4700
Connection ~ 6900 4600
$EndSCHEMATC
