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
L power:+5V #PWR?
U 1 1 6103FD1C
P 5300 1600
F 0 "#PWR?" H 5300 1450 50  0001 C CNN
F 1 "+5V" H 5315 1773 50  0000 C CNN
F 2 "" H 5300 1600 50  0001 C CNN
F 3 "" H 5300 1600 50  0001 C CNN
	1    5300 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 1600 5300 2250
$Comp
L power:GND #PWR?
U 1 1 61041E97
P 5200 4750
F 0 "#PWR?" H 5200 4500 50  0001 C CNN
F 1 "GND" H 5205 4577 50  0000 C CNN
F 2 "" H 5200 4750 50  0001 C CNN
F 3 "" H 5200 4750 50  0001 C CNN
	1    5200 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 4250 5200 4750
$Comp
L Connector_Generic:Conn_01x05 7Seg
U 1 1 61042CBA
P 3150 3750
F 0 "7Seg" H 3068 3325 50  0000 C CNN
F 1 "Conn_01x05" H 3068 3416 50  0000 C CNN
F 2 "" H 3150 3750 50  0001 C CNN
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
L power:+5V #PWR?
U 1 1 61044D5A
P 3500 3250
F 0 "#PWR?" H 3500 3100 50  0001 C CNN
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
L Connector_Generic:Conn_01x05 Encoder
U 1 1 61045C91
P 3150 2600
F 0 "Encoder" H 3068 2175 50  0000 C CNN
F 1 "Conn_01x05" H 3068 2266 50  0000 C CNN
F 2 "" H 3150 2600 50  0001 C CNN
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
L power:GND #PWR?
U 1 1 61045251
P 3800 3550
F 0 "#PWR?" H 3800 3300 50  0001 C CNN
F 1 "GND" V 3805 3422 50  0000 R CNN
F 2 "" H 3800 3550 50  0001 C CNN
F 3 "" H 3800 3550 50  0001 C CNN
	1    3800 3550
	0    -1   -1   0   
$EndComp
$Comp
L MCU_Module:Arduino_Nano_v3.x Nano
U 1 1 6103F115
P 5100 3250
F 0 "Nano" H 5100 2161 50  0000 C CNN
F 1 "Arduino_Nano_v3.x" H 5100 2070 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 5100 3250 50  0001 C CIN
F 3 "http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf" H 5100 3250 50  0001 C CNN
	1    5100 3250
	1    0    0    -1  
$EndComp
$EndSCHEMATC