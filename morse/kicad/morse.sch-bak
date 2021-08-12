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
L power:+5V #PWR0101
U 1 1 6103FD1C
P 5300 1600
F 0 "#PWR0101" H 5300 1450 50  0001 C CNN
F 1 "+5V" H 5315 1773 50  0000 C CNN
F 2 "" H 5300 1600 50  0001 C CNN
F 3 "" H 5300 1600 50  0001 C CNN
	1    5300 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 1600 5300 2250
$Comp
L power:GND #PWR0102
U 1 1 61041E97
P 5200 4750
F 0 "#PWR0102" H 5200 4500 50  0001 C CNN
F 1 "GND" H 5205 4577 50  0000 C CNN
F 2 "" H 5200 4750 50  0001 C CNN
F 3 "" H 5200 4750 50  0001 C CNN
	1    5200 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 4250 5200 4750
$Comp
L Connector_Generic:Conn_01x05 7Seg1
U 1 1 61042CBA
P 3150 3750
F 0 "7Seg1" H 3068 3325 50  0000 C CNN
F 1 "Conn_01x05" H 3068 3416 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 3150 3750 50  0001 C CNN
F 3 "~" H 3150 3750 50  0001 C CNN
	1    3150 3750
	-1   0    0    1   
$EndComp
Wire Wire Line
	4600 3950 3350 3950
Wire Wire Line
	4600 3850 3350 3850
Wire Wire Line
	4600 3650 3950 3650
Wire Wire Line
	3950 3650 3950 3750
Wire Wire Line
	3950 3750 3350 3750
$Comp
L power:+5V #PWR0103
U 1 1 61044D5A
P 3500 3250
F 0 "#PWR0103" H 3500 3100 50  0001 C CNN
F 1 "+5V" H 3515 3423 50  0000 C CNN
F 2 "" H 3500 3250 50  0001 C CNN
F 3 "" H 3500 3250 50  0001 C CNN
	1    3500 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 3650 3800 3650
Wire Wire Line
	3800 3650 3800 3550
Wire Wire Line
	3350 3550 3500 3550
Wire Wire Line
	3500 3550 3500 3250
$Comp
L Connector_Generic:Conn_01x05 Encoder1
U 1 1 61045C91
P 3150 2600
F 0 "Encoder1" H 3068 2175 50  0000 C CNN
F 1 "Conn_01x05" H 3068 2266 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x05_P2.54mm_Vertical" H 3150 2600 50  0001 C CNN
F 3 "~" H 3150 2600 50  0001 C CNN
	1    3150 2600
	-1   0    0    1   
$EndComp
Wire Wire Line
	3350 2700 3550 2700
Wire Wire Line
	3550 2700 3550 2500
Wire Wire Line
	3550 2500 3350 2500
Wire Wire Line
	3550 2700 3800 2700
Wire Wire Line
	3800 2700 3800 3550
Connection ~ 3550 2700
Connection ~ 3800 3550
Wire Wire Line
	3350 2400 4250 2400
Wire Wire Line
	4250 2400 4250 3250
Wire Wire Line
	4250 3250 4600 3250
Wire Wire Line
	3350 2600 4150 2600
Wire Wire Line
	4150 3350 4600 3350
Wire Wire Line
	4150 2600 4150 3350
Wire Wire Line
	3350 2800 4050 2800
Wire Wire Line
	4050 2800 4050 3450
Wire Wire Line
	4050 3450 4600 3450
$Comp
L power:GND #PWR0104
U 1 1 61045251
P 3800 3550
F 0 "#PWR0104" H 3800 3300 50  0001 C CNN
F 1 "GND" V 3805 3422 50  0000 R CNN
F 2 "" H 3800 3550 50  0001 C CNN
F 3 "" H 3800 3550 50  0001 C CNN
	1    3800 3550
	0    -1   -1   0   
$EndComp
$Comp
L MCU_Module:Arduino_Nano_v3.x Nano1
U 1 1 6103F115
P 5100 3250
F 0 "Nano1" H 5100 2161 50  0000 C CNN
F 1 "Arduino_Nano_v3.x" H 5100 2070 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 5100 3250 50  0001 C CIN
F 3 "http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf" H 5100 3250 50  0001 C CNN
	1    5100 3250
	1    0    0    -1  
$EndComp
$Comp
L Transistor_BJT:2N2219 Q1
U 1 1 6104D7BF
P 6200 3050
F 0 "Q1" H 6390 3096 50  0000 L CNN
F 1 "2N2219" H 6390 3005 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 6400 2975 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 6200 3050 50  0001 L CNN
	1    6200 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 2850 4450 2850
Wire Wire Line
	4450 2850 4450 2150
Wire Wire Line
	4450 2150 6000 2150
Wire Wire Line
	6000 2150 6000 3050
Wire Wire Line
	6300 2850 6300 1600
Wire Wire Line
	6300 1600 5300 1600
Connection ~ 5300 1600
$Comp
L Connector_Generic:Conn_01x02 MorseLED1
U 1 1 610551C1
P 6500 3900
F 0 "MorseLED1" H 6580 3892 50  0000 L CNN
F 1 "Conn_01x02" H 6580 3801 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 6500 3900 50  0001 C CNN
F 3 "~" H 6500 3900 50  0001 C CNN
	1    6500 3900
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 DeactivatedLED1
U 1 1 6105576B
P 7750 3950
F 0 "DeactivatedLED1" H 7830 3942 50  0000 L CNN
F 1 "Conn_01x02" H 7830 3851 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 7750 3950 50  0001 C CNN
F 3 "~" H 7750 3950 50  0001 C CNN
	1    7750 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 4000 6300 4750
Wire Wire Line
	6300 4750 6000 4750
Connection ~ 5200 4750
Wire Wire Line
	7550 4050 7550 4750
Wire Wire Line
	7550 4750 7250 4750
Connection ~ 6300 4750
Wire Wire Line
	7550 1600 6300 1600
Connection ~ 6300 1600
Wire Wire Line
	7000 3050 7000 2000
Wire Wire Line
	7000 2000 4350 2000
Wire Wire Line
	4350 2000 4350 2950
Wire Wire Line
	4350 2950 4600 2950
$Comp
L Connector_Generic:Conn_01x04 J1
U 1 1 6105BDAA
P 8400 1350
F 0 "J1" V 8364 1062 50  0000 R CNN
F 1 "Conn_01x04" V 8273 1062 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 8400 1350 50  0001 C CNN
F 3 "~" H 8400 1350 50  0001 C CNN
	1    8400 1350
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 6105C619
P 8500 1750
F 0 "#PWR0105" H 8500 1500 50  0001 C CNN
F 1 "GND" H 8505 1577 50  0000 C CNN
F 2 "" H 8500 1750 50  0001 C CNN
F 3 "" H 8500 1750 50  0001 C CNN
	1    8500 1750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 6105CC5E
P 8300 1750
F 0 "#PWR0106" H 8300 1600 50  0001 C CNN
F 1 "+5V" H 8315 1923 50  0000 C CNN
F 2 "" H 8300 1750 50  0001 C CNN
F 3 "" H 8300 1750 50  0001 C CNN
	1    8300 1750
	-1   0    0    1   
$EndComp
Wire Wire Line
	8300 1550 8300 1750
Wire Wire Line
	8500 1550 8500 1750
Wire Wire Line
	8400 1550 8400 3650
Wire Wire Line
	8400 3650 5600 3650
Wire Wire Line
	7550 2850 7550 1600
Wire Wire Line
	7250 3050 7000 3050
$Comp
L Transistor_BJT:2N2219 Q2
U 1 1 6105728C
P 7450 3050
F 0 "Q2" H 7640 3096 50  0000 L CNN
F 1 "2N2219" H 7640 3005 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-39-3" H 7650 2975 50  0001 L CIN
F 3 "http://www.onsemi.com/pub_link/Collateral/2N2219-D.PDF" H 7450 3050 50  0001 L CNN
	1    7450 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 3600 7550 3950
Wire Wire Line
	7550 3250 7550 3300
$Comp
L Device:R R2
U 1 1 61056D92
P 7550 3450
F 0 "R2" H 7620 3496 50  0000 L CNN
F 1 "R" H 7620 3405 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 7480 3450 50  0001 C CNN
F 3 "~" H 7550 3450 50  0001 C CNN
	1    7550 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 1550 8600 3750
Wire Wire Line
	8600 3750 5600 3750
$Comp
L Device:R R1
U 1 1 6104E96F
P 6300 3450
F 0 "R1" H 6370 3496 50  0000 L CNN
F 1 "R" H 6370 3405 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 6230 3450 50  0001 C CNN
F 3 "~" H 6300 3450 50  0001 C CNN
	1    6300 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 3250 6300 3300
Wire Wire Line
	6300 3600 6300 3900
Wire Wire Line
	5100 4250 5200 4250
Connection ~ 5200 4250
$Comp
L Device:R R3
U 1 1 6108410E
P 6000 4150
F 0 "R3" H 6070 4196 50  0000 L CNN
F 1 "R" H 6070 4105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 5930 4150 50  0001 C CNN
F 3 "~" H 6000 4150 50  0001 C CNN
	1    6000 4150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 61084C3D
P 7250 4150
F 0 "R4" H 7320 4196 50  0000 L CNN
F 1 "R" H 7320 4105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0309_L9.0mm_D3.2mm_P12.70mm_Horizontal" V 7180 4150 50  0001 C CNN
F 3 "~" H 7250 4150 50  0001 C CNN
	1    7250 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 3050 6000 4000
Connection ~ 6000 3050
Wire Wire Line
	7250 4000 7250 3050
Connection ~ 7250 3050
Wire Wire Line
	7250 4300 7250 4750
Connection ~ 7250 4750
Wire Wire Line
	7250 4750 6300 4750
Wire Wire Line
	6000 4300 6000 4750
Connection ~ 6000 4750
Wire Wire Line
	6000 4750 5200 4750
$EndSCHEMATC
