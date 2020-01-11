EESchema Schematic File Version 4
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
$Comp
L Ben_custom:TCS34725FN U?
U 1 1 5E1E525D
P 4350 4550
F 0 "U?" H 4350 5117 50  0000 C CNN
F 1 "TCS34725FN" H 4350 5026 50  0000 C CNN
F 2 "Ben_Custom:TCS34725" H 4000 5050 50  0001 L BNN
F 3 "https://ams.com/documents/20143/36005/TCS3472_DS000390_2-00.pdf" H 4000 4050 50  0001 L BNN
F 4 "TCS34725FNCT-ND" H 4350 4550 50  0001 C CNN "Part#"
	1    4350 4550
	1    0    0    -1  
$EndComp
$Comp
L Ben_custom:TSL25911FN U?
U 1 1 5E1E6521
P 3300 4650
F 0 "U?" H 3300 4285 50  0000 C CNN
F 1 "TSL25911FN" H 3300 4376 50  0000 C CNN
F 2 "Ben_Custom:TSL25911FN" H 2900 5100 50  0001 C CNN
F 3 "https://ams.com/documents/20143/36005/TSL2591_DS000338_6-00.pdf" H 2900 5100 50  0001 C CNN
F 4 "TSL25911FNCT-ND" H 3300 4650 50  0001 C CNN "Part#"
	1    3300 4650
	-1   0    0    1   
$EndComp
Text Label 10950 4350 0    50   ~ 0
Vin
$Comp
L power:GND #PWR?
U 1 1 5E1EC75C
P 4350 6750
F 0 "#PWR?" H 4350 6500 50  0001 C CNN
F 1 "GND" H 4355 6577 50  0000 C CNN
F 2 "" H 4350 6750 50  0001 C CNN
F 3 "" H 4350 6750 50  0001 C CNN
	1    4350 6750
	1    0    0    -1  
$EndComp
Text Label 8650 3250 2    50   ~ 0
Vbat
$Comp
L Ben_custom:BU2032SM-BT-GTR BT?
U 1 1 5E1F3175
P 4350 6450
F 0 "BT?" V 4304 6580 50  0000 L CNN
F 1 "BU2032SM-BT-GTR" V 4395 6580 50  0000 L CNN
F 2 "Ben_Custom:BU2032SM-BT-GTR" H 4000 6000 50  0001 L BNN
F 3 "http://www.memoryprotectiondevices.com/datasheets/BU2032SM-BT-GTR-datasheet.pdf" H 3750 6100 50  0001 L BNN
F 4 "BU2032SM-BT-GCT-ND" H 3950 5900 50  0001 L BNN "Part#"
	1    4350 6450
	0    1    1    0   
$EndComp
Text Label 8650 3350 2    50   ~ 0
3v3
Text Label 10950 4550 0    50   ~ 0
3v3
Text Label 8650 2250 2    50   ~ 0
3v3
$Comp
L power:GND #PWR?
U 1 1 5E1FC8E5
P 8650 850
F 0 "#PWR?" H 8650 600 50  0001 C CNN
F 1 "GND" V 8655 722 50  0000 R CNN
F 2 "" H 8650 850 50  0001 C CNN
F 3 "" H 8650 850 50  0001 C CNN
	1    8650 850 
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1FD201
P 8650 3450
F 0 "#PWR?" H 8650 3200 50  0001 C CNN
F 1 "GND" V 8655 3322 50  0000 R CNN
F 2 "" H 8650 3450 50  0001 C CNN
F 3 "" H 8650 3450 50  0001 C CNN
	1    8650 3450
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1FDE5A
P 8650 4650
F 0 "#PWR?" H 8650 4400 50  0001 C CNN
F 1 "GND" V 8655 4522 50  0000 R CNN
F 2 "" H 8650 4650 50  0001 C CNN
F 3 "" H 8650 4650 50  0001 C CNN
	1    8650 4650
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1FF1C2
P 4750 4850
F 0 "#PWR?" H 4750 4600 50  0001 C CNN
F 1 "GND" V 4755 4722 50  0000 R CNN
F 2 "" H 4750 4850 50  0001 C CNN
F 3 "" H 4750 4850 50  0001 C CNN
	1    4750 4850
	0    -1   -1   0   
$EndComp
$Comp
L bosch:BME280 U?
U 1 1 5E2027F6
P 1700 4650
F 0 "U?" H 1271 4696 50  0000 R CNN
F 1 "BME280" H 1271 4605 50  0000 R CNN
F 2 "Housings_LGA:LGA-8_2.5x2.5mm_Pitch0.65mm" H 1700 4450 50  0001 C CNN
F 3 "https://www.bosch-sensortec.com/media/boschsensortec/downloads/environmental_sensors_2/humidity_sensors_1/bme280/bst-bme280-ds002.pdf" H 1700 4450 50  0001 C CNN
F 4 "828-1063-1-ND" H 1700 4650 50  0001 C CNN "Part#"
	1    1700 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 4450 3900 4450
Wire Wire Line
	3900 4450 3900 5350
Wire Wire Line
	3600 4650 3650 4650
Text Label 8650 1150 2    50   ~ 0
2*
Text Label 2750 3100 0    50   ~ 0
3v3
$Comp
L power:GND #PWR?
U 1 1 5E213968
P 1600 5250
F 0 "#PWR?" H 1600 5000 50  0001 C CNN
F 1 "GND" H 1605 5077 50  0000 C CNN
F 2 "" H 1600 5250 50  0001 C CNN
F 3 "" H 1600 5250 50  0001 C CNN
	1    1600 5250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E21418B
P 1800 5250
F 0 "#PWR?" H 1800 5000 50  0001 C CNN
F 1 "GND" H 1805 5077 50  0000 C CNN
F 2 "" H 1800 5250 50  0001 C CNN
F 3 "" H 1800 5250 50  0001 C CNN
	1    1800 5250
	1    0    0    -1  
$EndComp
Text Label 10950 5150 0    50   ~ 0
SDA0
Text Label 2300 4750 0    50   ~ 0
SDA0
Text Label 10950 5050 0    50   ~ 0
SCL0
Text Label 2300 4550 0    50   ~ 0
SCL0
$Comp
L power:GND #PWR?
U 1 1 5E2152EA
P 3600 4500
F 0 "#PWR?" H 3600 4250 50  0001 C CNN
F 1 "GND" V 3605 4372 50  0000 R CNN
F 2 "" H 3600 4500 50  0001 C CNN
F 3 "" H 3600 4500 50  0001 C CNN
	1    3600 4500
	0    -1   -1   0   
$EndComp
Text Label 8650 4150 2    50   ~ 0
SCL1
Text Label 8650 4250 2    50   ~ 0
SDA1
Text Label 3600 5150 2    50   ~ 0
SCL1
Text Label 3000 5150 0    50   ~ 0
SDA1
Text Label 8650 1550 2    50   ~ 0
6*
Text Label 8650 1350 2    50   ~ 0
SDA2
Text Label 8650 1250 2    50   ~ 0
SCL2
Text Label 3950 5150 0    50   ~ 0
SDA2
Text Label 3900 5150 2    50   ~ 0
SCL2
Wire Wire Line
	3900 5350 3750 5350
Wire Wire Line
	3950 4650 3950 5350
NoConn ~ 8650 1450
NoConn ~ 8650 1750
NoConn ~ 8650 3550
NoConn ~ 8650 3650
NoConn ~ 8650 3750
NoConn ~ 8650 3850
NoConn ~ 8650 4350
NoConn ~ 8650 4550
NoConn ~ 8650 4750
NoConn ~ 10950 850 
NoConn ~ 10950 950 
NoConn ~ 10950 1050
NoConn ~ 10950 1150
NoConn ~ 10950 1250
NoConn ~ 10950 1350
NoConn ~ 10950 1450
NoConn ~ 10950 1550
NoConn ~ 10950 1650
NoConn ~ 10950 1750
NoConn ~ 10950 1850
NoConn ~ 10950 1950
NoConn ~ 10950 2050
NoConn ~ 10950 2150
NoConn ~ 10950 2250
NoConn ~ 10950 2350
NoConn ~ 10950 2450
NoConn ~ 10950 2550
NoConn ~ 10950 2650
NoConn ~ 10950 2750
NoConn ~ 10950 2850
NoConn ~ 10950 2950
NoConn ~ 10950 3050
NoConn ~ 10950 3150
NoConn ~ 10950 3250
NoConn ~ 10950 3350
NoConn ~ 10950 3450
NoConn ~ 10950 3550
NoConn ~ 10950 3650
NoConn ~ 10950 3950
NoConn ~ 10950 4050
NoConn ~ 10950 4150
NoConn ~ 10950 4250
NoConn ~ 10950 4450
NoConn ~ 10950 4650
Wire Wire Line
	2300 4350 2300 4050
Connection ~ 1800 4050
Wire Wire Line
	1800 4050 1600 4050
$Comp
L device:C C?
U 1 1 5E23C716
P 2300 3900
F 0 "C?" H 2415 3946 50  0000 L CNN
F 1 "0.1uF" H 2415 3855 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2338 3750 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsunghttps://www.yageo.com/upload/media/product/productsearch/datasheet/mlcc/UPY-GPHC_X7R_6.3V-to-50V_18.pdf" H 2300 3900 50  0001 C CNN
F 4 "311-1344-1-ND" H 2300 3900 50  0001 C CNN "Part#"
	1    2300 3900
	1    0    0    -1  
$EndComp
Connection ~ 2300 4050
$Comp
L power:GND #PWR?
U 1 1 5E23D3AE
P 2300 3750
F 0 "#PWR?" H 2300 3500 50  0001 C CNN
F 1 "GND" H 2305 3577 50  0000 C CNN
F 2 "" H 2300 3750 50  0001 C CNN
F 3 "" H 2300 3750 50  0001 C CNN
	1    2300 3750
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E23FAAA
P 2650 4650
F 0 "#PWR?" H 2650 4400 50  0001 C CNN
F 1 "GND" H 2655 4477 50  0000 C CNN
F 2 "" H 2650 4650 50  0001 C CNN
F 3 "" H 2650 4650 50  0001 C CNN
	1    2650 4650
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E241861
P 5250 4250
F 0 "#PWR?" H 5250 4000 50  0001 C CNN
F 1 "GND" V 5255 4122 50  0000 R CNN
F 2 "" H 5250 4250 50  0001 C CNN
F 3 "" H 5250 4250 50  0001 C CNN
	1    5250 4250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3600 5350 3550 5350
Wire Wire Line
	3600 4800 3600 5350
Wire Wire Line
	3000 4800 3000 5350
Wire Wire Line
	3000 5350 3350 5350
Wire Wire Line
	2300 4050 2550 4050
Wire Wire Line
	2550 4050 2550 4950
Wire Wire Line
	2550 4950 2300 4950
Text Label 8650 3950 2    50   ~ 0
A16
$Comp
L Ben_custom:NHD-0420H1Z-FL-GBW-33V3 LCD?
U 1 1 5E267979
P 1450 1600
F 0 "LCD?" H 1450 2567 50  0000 C CNN
F 1 "NHD-0420H1Z-FL-GBW-33V3" H 1450 2476 50  0000 C CNN
F 2 "LCD_NHD-0420H1Z-FL-GBW-33V3" H 1450 1600 50  0001 L BNN
F 3 "13.0mm" H 1450 1600 50  0001 L BNN
F 4 "IPC-7351B" H 1450 1600 50  0001 L BNN "Field4"
F 5 "Newheaven Display" H 1450 1600 50  0001 L BNN "Field5"
F 6 "02" H 1450 1600 50  0001 L BNN "Field6"
	1    1450 1600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E268F90
P 2050 2300
F 0 "#PWR?" H 2050 2050 50  0001 C CNN
F 1 "GND" V 2055 2172 50  0000 R CNN
F 2 "" H 2050 2300 50  0001 C CNN
F 3 "" H 2050 2300 50  0001 C CNN
	1    2050 2300
	0    -1   -1   0   
$EndComp
$Comp
L Ben_custom:NTJD4001NT1G Q?
U 1 1 5E29A98C
P 2750 3300
F 0 "Q?" H 3200 3450 50  0000 L CNN
F 1 "NTJD4001NT1G" H 3200 3350 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_SC-70-6" H 2000 3150 50  0001 L BNN
F 3 "http://www.onsemi.com/pub/Collateral/NTJD4001N-D.PDF" H 2650 3300 50  0001 L BNN
F 4 "NTJD4001NT1GOSCT-ND" H 2650 3300 50  0001 L BNN "Part#"
	1    2750 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 3100 2950 3100
Wire Wire Line
	3000 4650 2950 4650
Wire Wire Line
	1800 4050 2050 4050
Wire Wire Line
	2950 3500 2950 4650
Wire Wire Line
	2750 3500 2050 3500
Wire Wire Line
	2050 3500 2050 4050
Connection ~ 2050 4050
Wire Wire Line
	2050 4050 2300 4050
$Comp
L device:C C?
U 1 1 5E2CD370
P 2800 4650
F 0 "C?" H 2915 4696 50  0000 L CNN
F 1 "0.1uF" H 2915 4605 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 2838 4500 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsunghttps://www.yageo.com/upload/media/product/productsearch/datasheet/mlcc/UPY-GPHC_X7R_6.3V-to-50V_18.pdf" H 2800 4650 50  0001 C CNN
F 4 "311-1344-1-ND" H 2800 4650 50  0001 C CNN "Part#"
	1    2800 4650
	0    1    1    0   
$EndComp
Connection ~ 2950 4650
$Comp
L device:C C?
U 1 1 5E2CDF37
P 5100 4250
F 0 "C?" H 5215 4296 50  0000 L CNN
F 1 "0.1uF" H 5215 4205 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603" H 5138 4100 50  0001 C CNN
F 3 "https://media.digikey.com/pdf/Data%20Sheets/Samsunghttps://www.yageo.com/upload/media/product/productsearch/datasheet/mlcc/UPY-GPHC_X7R_6.3V-to-50V_18.pdf" H 5100 4250 50  0001 C CNN
F 4 "311-1344-1-ND" H 5100 4250 50  0001 C CNN "Part#"
	1    5100 4250
	0    1    1    0   
$EndComp
$Comp
L Ben_custom:EXB-2HV103JV_-_8x10k_resistor_array R?
U 1 1 5E2CE816
P 4750 5350
F 0 "R?" V 4894 5429 60  0000 L CNN
F 1 "EXB-2HV103JV_-_8x10k_resistor_array" V 5000 5429 60  0000 L CNN
F 2 "Ben_Custom:Resistor_array_1506_8x" H 5000 5015 60  0001 C CNN
F 3 "https://industrial.panasonic.com/cdbs/www-data/pdf/AOC0000/AOC0000C14.pdf" V 5500 5800 60  0001 L CNN
F 4 "Y1103CT-ND" V 4750 5350 50  0001 C CNN "Part#"
	1    4750 5350
	0    1    1    0   
$EndComp
Wire Wire Line
	3350 5850 3550 5850
Connection ~ 3550 5850
Wire Wire Line
	3550 5850 3750 5850
Connection ~ 3750 5850
Wire Wire Line
	3750 5850 3950 5850
Wire Wire Line
	3200 5850 3350 5850
Connection ~ 3350 5850
Wire Wire Line
	5100 4100 5600 4100
Wire Wire Line
	5600 4100 5600 5350
Wire Wire Line
	5600 5350 4750 5350
Wire Wire Line
	4750 5850 4550 5850
$Comp
L power:GND #PWR?
U 1 1 5E2D9191
P 4550 5350
F 0 "#PWR?" H 4550 5100 50  0001 C CNN
F 1 "GND" V 4555 5222 50  0000 R CNN
F 2 "" H 4550 5350 50  0001 C CNN
F 3 "" H 4550 5350 50  0001 C CNN
	1    4550 5350
	-1   0    0    1   
$EndComp
NoConn ~ 4150 5350
NoConn ~ 4150 5850
Text Label 4850 5850 0    50   ~ 0
A16
Wire Wire Line
	4850 5850 4750 5850
Connection ~ 4750 5850
Text Label 8650 4050 2    50   ~ 0
A17
Text Label 4350 5350 2    50   ~ 0
A17
Text Label 4200 6150 2    50   ~ 0
Vbat
Text Label 8650 4450 2    50   ~ 0
DAC0
Text Label 2050 1000 0    50   ~ 0
DAC0
$Comp
L Ben_custom:NTJD4001NT1G Q?
U 1 1 5E2E0271
P 2650 1350
F 0 "Q?" V 2350 1600 50  0000 C CNN
F 1 "NTJD4001NT1G" V 2450 1600 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_SC-70-6" H 1900 1200 50  0001 L BNN
F 3 "http://www.onsemi.com/pub/Collateral/NTJD4001N-D.PDF" H 2550 1350 50  0001 L BNN
F 4 "NTJD4001NT1GOSCT-ND" H 2550 1350 50  0001 L BNN "Part#"
	1    2650 1350
	0    1    1    0   
$EndComp
$Comp
L device:D D?
U 1 1 5E2E8394
P 4350 6000
F 0 "D?" H 4350 5784 50  0000 C CNN
F 1 "D" H 4350 5875 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-323" H 4350 6000 50  0001 C CNN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=14077&prodName=CUS10S30" H 4350 6000 50  0001 C CNN
F 4 "CUS10S30H3FCT-ND" H 4350 6000 50  0001 C CNN "Part#"
	1    4350 6000
	0    -1   1    0   
$EndComp
Wire Wire Line
	2050 1550 2050 1900
$Comp
L device:R R?
U 1 1 5E2EF264
P 2200 1550
F 0 "R?" V 1993 1550 50  0000 C CNN
F 1 "2" V 2084 1550 50  0000 C CNN
F 2 "Resistors_SMD:R_0603" V 2130 1550 50  0001 C CNN
F 3 "https://www.yageo.com/upload/media/product/productsearch/datasheet/rchip/PYu-RC_Group_51_RoHS_L_10.pdf" H 2200 1550 50  0001 C CNN
F 4 "311-2.00HRCT-ND" V 2200 1550 50  0001 C CNN "Part#"
	1    2200 1550
	0    1    1    0   
$EndComp
Wire Wire Line
	2450 1550 2350 1550
Wire Wire Line
	2050 2000 2050 2300
Connection ~ 2050 2300
Wire Wire Line
	2850 1350 2850 1550
Text Label 2850 1450 0    50   ~ 0
3v3
Wire Wire Line
	2450 1350 2450 900 
Wire Wire Line
	2450 900  2050 900 
Wire Wire Line
	4200 6150 4350 6150
Connection ~ 4350 6150
Wire Wire Line
	4750 4250 4900 4250
Text Label 4900 3550 2    50   ~ 0
3v3
$Comp
L Ben_custom:NTJD4001NT1G Q?
U 1 1 5E2C265D
P 4900 3750
F 0 "Q?" H 4600 3800 50  0000 L CNN
F 1 "NTJD4001NT1G" H 4200 3900 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_SC-70-6" H 4150 3600 50  0001 L BNN
F 3 "http://www.onsemi.com/pub/Collateral/NTJD4001N-D.PDF" H 4800 3750 50  0001 L BNN
F 4 "NTJD4001NT1GOSCT-ND" H 4800 3750 50  0001 L BNN "Part#"
	1    4900 3750
	1    0    0    -1  
$EndComp
Connection ~ 5900 3800
Wire Wire Line
	5900 3400 5900 3800
NoConn ~ 5500 3900
NoConn ~ 5500 3500
Wire Wire Line
	5300 3300 5500 3300
Connection ~ 5300 3300
Wire Wire Line
	5300 3700 5500 3700
Wire Wire Line
	5300 3300 5300 3700
$Comp
L dk_Slide-Switches:JS202011SCQN S?
U 1 1 5E261D45
P 5700 3600
F 0 "S?" H 5700 4083 50  0000 C CNN
F 1 "JS202011SCQN" H 5700 3992 50  0000 C CNN
F 2 "digikey-footprints:Switch_Slide_JS202011SCQN" H 5900 3800 50  0001 L CNN
F 3 "https://www.ckswitches.com/media/1422/js.pdf" H 5900 3900 60  0001 L CNN
F 4 "401-2002-1-ND" H 5900 4800 60  0001 L CNN "Part#"
	1    5700 3600
	-1   0    0    -1  
$EndComp
Connection ~ 5100 3300
Wire Wire Line
	5100 3300 5100 3550
Wire Wire Line
	5100 3300 5300 3300
$Comp
L device:D D?
U 1 1 5E1F5931
P 4950 3300
F 0 "D?" H 4950 3084 50  0000 C CNN
F 1 "D" H 4950 3175 50  0000 C CNN
F 2 "Diodes_SMD:D_SOD-323" H 4950 3300 50  0001 C CNN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=14077&prodName=CUS10S30" H 4950 3300 50  0001 C CNN
F 4 "CUS10S30H3FCT-ND" H 4950 3300 50  0001 C CNN "Part#"
	1    4950 3300
	1    0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1EC268
P 5900 4200
F 0 "#PWR?" H 5900 3950 50  0001 C CNN
F 1 "GND" H 5905 4027 50  0000 C CNN
F 2 "" H 5900 4200 50  0001 C CNN
F 3 "" H 5900 4200 50  0001 C CNN
	1    5900 4200
	-1   0    0    -1  
$EndComp
Text Label 4800 3300 2    50   ~ 0
Vin
$Comp
L device:Battery BT?
U 1 1 5E1E70C1
P 5900 4000
F 0 "BT?" H 6008 4046 50  0000 L CNN
F 1 "Battery" H 6008 3955 50  0000 L CNN
F 2 "Ben_Custom:BH3AAW-Battery_wire_pads" V 5900 4060 50  0001 C CNN
F 3 "http://www.memoryprotectiondevices.com/datasheets/BH3AAW-datasheet.pdf" V 5900 4060 50  0001 C CNN
F 4 "BH3AA-W-ND" H 5900 4000 50  0001 C CNN "Part#"
	1    5900 4000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5100 3950 5100 4100
Wire Wire Line
	4900 3950 4900 4250
Connection ~ 4900 4250
Wire Wire Line
	4900 4250 4950 4250
Text Label 2550 1050 2    50   ~ 0
2*
Text Label 2550 1850 2    50   ~ 0
6*
Wire Wire Line
	2550 1050 2550 1150
Wire Wire Line
	2550 1750 2550 1850
Text Label 8650 1650 2    50   ~ 0
7*
Text Label 2550 3400 2    50   ~ 0
7*
Text Label 8650 1850 2    50   ~ 0
9*
Text Label 3150 3400 0    50   ~ 0
9*
Text Label 8650 1950 2    50   ~ 0
10*
Text Label 8650 2050 2    50   ~ 0
11*
Text Label 4700 3850 2    50   ~ 0
10*
Text Label 5300 3850 0    50   ~ 0
11*
$Comp
L teensy:Teensy3.6 U?
U 1 1 5E1DCCE2
P 9800 3000
F 0 "U?" H 9800 5543 60  0000 C CNN
F 1 "Teensy3.6" H 9800 5437 60  0000 C CNN
F 2 "Ben_Custom:Teensy3_5_Socket" H 9800 5331 60  0000 C CNN
F 3 "" H 9800 3050 60  0000 C CNN
	1    9800 3000
	1    0    0    -1  
$EndComp
Text Label 8650 2350 2    50   ~ 0
DB0
Text Label 8650 2450 2    50   ~ 0
DB1
Text Label 8650 2550 2    50   ~ 0
DB2
Text Label 8650 2650 2    50   ~ 0
DB3
Text Label 8650 2750 2    50   ~ 0
DB4
Text Label 8650 2850 2    50   ~ 0
DB5
Text Label 8650 2950 2    50   ~ 0
DB6
Text Label 8650 3050 2    50   ~ 0
DB7
Text Label 8650 3150 2    50   ~ 0
E
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
Text Label 10950 4850 0    50   ~ 0
21*
Text Label 10950 4750 0    50   ~ 0
22*
Text Label 4750 4450 0    50   ~ 0
22*
Text Label 3650 4650 0    50   ~ 0
21*
Text Label 8650 2150 2    50   ~ 0
I2C_Pullup
Text Label 3200 5850 2    50   ~ 0
I2C_Pullup
$Comp
L power:GND #PWR?
U 1 1 5E31EF84
P 8650 850
F 0 "#PWR?" H 8650 600 50  0001 C CNN
F 1 "GND" V 8655 722 50  0000 R CNN
F 2 "" H 8650 850 50  0001 C CNN
F 3 "" H 8650 850 50  0001 C CNN
	1    8650 850 
	0    1    1    0   
$EndComp
$Comp
L teensy:Teensy3.6 U?
U 1 1 5E31EF8E
P 9800 3000
F 0 "U?" H 9800 5543 60  0000 C CNN
F 1 "Teensy3.6" H 9800 5437 60  0000 C CNN
F 2 "Ben_Custom:Teensy3_5_Socket" H 9800 5331 60  0000 C CNN
F 3 "" H 9800 3050 60  0000 C CNN
	1    9800 3000
	1    0    0    -1  
$EndComp
Text Label 8650 950  2    50   ~ 0
RS
Text Label 8650 1050 2    50   ~ 0
RW
Connection ~ 8650 850 
Connection ~ 8650 3450
Connection ~ 8650 4650
Text Label 850  1100 2    50   ~ 0
RS
Text Label 850  1200 2    50   ~ 0
RW
$Comp
L dk_Navigation-Switches-Joystick:JS5208 S?
U 1 1 5E3244FC
P 4750 1650
F 0 "S?" H 4750 2097 60  0000 C CNN
F 1 "JS5208" H 4750 1991 60  0000 C CNN
F 2 "digikey-footprints:Switch_Nav_12.4x12.4mm_SMD_JS5208" H 4950 1850 60  0001 L CNN
F 3 "http://spec_sheets.e-switch.com/specs/P200020.pdf" H 4950 1950 60  0001 L CNN
F 4 "EG4561-ND" H 4950 2050 60  0001 L CNN "Digi-Key_PN"
F 5 "JS5208" H 4950 2150 60  0001 L CNN "MPN"
F 6 "Switches" H 4950 2250 60  0001 L CNN "Category"
F 7 "Navigation Switches, Joystick" H 4950 2350 60  0001 L CNN "Family"
F 8 "http://spec_sheets.e-switch.com/specs/P200020.pdf" H 4950 2450 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/e-switch/JS5208/EG4561-ND/1739634" H 4950 2550 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH NAVIGATION 20MA 15V" H 4950 2650 60  0001 L CNN "Description"
F 11 "E-Switch" H 4950 2750 60  0001 L CNN "Manufacturer"
F 12 "Active" H 4950 2850 60  0001 L CNN "Status"
	1    4750 1650
	1    0    0    -1  
$EndComp
Text Label 8650 5050 2    50   ~ 0
16*
Text Label 4350 1850 2    50   ~ 0
16*
$Comp
L power:GND #PWR?
U 1 1 5E326B92
P 4350 1450
F 0 "#PWR?" H 4350 1200 50  0001 C CNN
F 1 "GND" V 4355 1322 50  0000 R CNN
F 2 "" H 4350 1450 50  0001 C CNN
F 3 "" H 4350 1450 50  0001 C CNN
	1    4350 1450
	0    1    1    0   
$EndComp
Text Label 8650 4850 2    50   ~ 0
A
Text Label 8650 4950 2    50   ~ 0
B
Text Label 8650 5150 2    50   ~ 0
C
Text Label 10950 4950 0    50   ~ 0
D
Text Label 5150 1450 0    50   ~ 0
A
Text Label 5150 1650 0    50   ~ 0
D
Text Label 5150 1850 0    50   ~ 0
C
Text Label 4350 1650 2    50   ~ 0
B
$EndSCHEMATC
