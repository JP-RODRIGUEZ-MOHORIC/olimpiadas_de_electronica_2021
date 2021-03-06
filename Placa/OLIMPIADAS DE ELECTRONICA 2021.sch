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
L EESTN5:Mounting_Hole H1
U 1 1 61314898
P 3400 4725
F 0 "H1" H 3500 4725 50  0000 L CNN
F 1 "Mounting_Hole" H 3400 4850 50  0001 C CNN
F 2 "EESTN5:hole_3mm" H 3400 4725 60  0001 C CNN
F 3 "" H 3400 4725 60  0001 C CNN
	1    3400 4725
	-1   0    0    1   
$EndComp
$Comp
L EESTN5:Mounting_Hole H4
U 1 1 61315819
P 4450 4725
F 0 "H4" H 4550 4725 50  0000 L CNN
F 1 "Mounting_Hole" H 4450 4850 50  0001 C CNN
F 2 "EESTN5:hole_3mm" H 4450 4725 60  0001 C CNN
F 3 "" H 4450 4725 60  0001 C CNN
	1    4450 4725
	-1   0    0    1   
$EndComp
$Comp
L EESTN5:ESP32_DEVKITC U1
U 1 1 615A686C
P 3800 3525
F 0 "U1" H 3800 4650 50  0000 C CNN
F 1 "ESP32_DEVKITC" H 3800 4559 50  0000 C CNN
F 2 "EESTN5:ESP32_DEVKITC" H 3500 2525 50  0001 C CNN
F 3 "" H 3500 2525 50  0001 C CNN
	1    3800 3525
	1    0    0    -1  
$EndComp
$Comp
L EESTN5:HEADER_4 J2
U 1 1 615B4AB2
P 5525 2725
F 0 "J2" H 5653 2778 60  0000 L CNN
F 1 "SENSOR CO2" H 5653 2672 60  0000 L CNN
F 2 "EESTN5:pin_strip_4" H 5525 2725 60  0001 C CNN
F 3 "" H 5525 2725 60  0000 C CNN
	1    5525 2725
	1    0    0    -1  
$EndComp
Text Label 3100 4425 2    50   ~ 0
5V
Wire Wire Line
	3250 4425 3100 4425
Text Label 7000 4425 2    50   ~ 0
5V
Wire Wire Line
	7000 4425 7125 4425
Text Label 4475 4125 0    50   ~ 0
PIN_15
Text Label 7000 4625 2    50   ~ 0
PIN_15
Wire Wire Line
	7000 4625 7125 4625
Wire Wire Line
	4350 4125 4475 4125
Text Label 4450 2625 0    50   ~ 0
GND
Wire Wire Line
	4350 2625 4450 2625
Text Label 4475 3525 0    50   ~ 0
PIN_5
Text Label 4475 3425 0    50   ~ 0
PIN_18
Text Label 4475 3325 0    50   ~ 0
PIN_19
Wire Wire Line
	4350 3325 4475 3325
Wire Wire Line
	4475 3425 4350 3425
Wire Wire Line
	4350 3525 4475 3525
Text Label 3100 4025 2    50   ~ 0
PIN_13
Text Label 3100 3825 2    50   ~ 0
PIN_12
Text Label 3100 3725 2    50   ~ 0
PIN_14
Text Label 3100 3625 2    50   ~ 0
PIN_27
Wire Wire Line
	3100 4025 3250 4025
Wire Wire Line
	3250 3825 3100 3825
Wire Wire Line
	3100 3725 3250 3725
Wire Wire Line
	3250 3625 3100 3625
Text Label 3100 3525 2    50   ~ 0
PIN_26
Text Label 3100 3425 2    50   ~ 0
PIN_25
Text Label 3100 3325 2    50   ~ 0
PIN_33
Text Label 3100 3225 2    50   ~ 0
PIN_32
Wire Wire Line
	3250 3525 3100 3525
Wire Wire Line
	3100 3425 3250 3425
Wire Wire Line
	3250 3325 3100 3325
Wire Wire Line
	3100 3225 3250 3225
Text Label 5300 2675 2    50   ~ 0
5V
Text Label 5300 2575 2    50   ~ 0
GND
Wire Wire Line
	5300 2575 5425 2575
Wire Wire Line
	5425 2675 5300 2675
Text Label 5300 2875 2    50   ~ 0
PIN_35
Wire Wire Line
	5425 2875 5300 2875
Text Label 3100 3125 2    50   ~ 0
PIN_35
Wire Wire Line
	3100 3125 3250 3125
Text Label 4475 4025 0    50   ~ 0
PIN_2
Text Label 4475 3825 0    50   ~ 0
PIN_4
Wire Wire Line
	4475 4025 4350 4025
Wire Wire Line
	4350 3825 4475 3825
$Comp
L EESTN5:Mounting_Hole H3
U 1 1 61315813
P 4075 4725
F 0 "H3" H 4175 4725 50  0000 L CNN
F 1 "Mounting_Hole" H 4075 4850 50  0001 C CNN
F 2 "EESTN5:hole_3mm" H 4075 4725 60  0001 C CNN
F 3 "" H 4075 4725 60  0001 C CNN
	1    4075 4725
	-1   0    0    1   
$EndComp
$Comp
L EESTN5:Mounting_Hole H2
U 1 1 61314EB0
P 3750 4725
F 0 "H2" H 3850 4725 50  0000 L CNN
F 1 "Mounting_Hole" H 3750 4850 50  0001 C CNN
F 2 "EESTN5:hole_3mm" H 3750 4725 60  0001 C CNN
F 3 "" H 3750 4725 60  0001 C CNN
	1    3750 4725
	-1   0    0    1   
$EndComp
Wire Wire Line
	7150 2600 7025 2600
Wire Wire Line
	7025 2800 7150 2800
Wire Wire Line
	7150 3000 7025 3000
Wire Wire Line
	7025 3200 7150 3200
Text Label 7025 2600 2    50   ~ 0
PIN_32
Text Label 7025 2800 2    50   ~ 0
PIN_33
Text Label 7025 3000 2    50   ~ 0
PIN_25
Text Label 7025 3200 2    50   ~ 0
PIN_26
Wire Wire Line
	7025 3400 7150 3400
Wire Wire Line
	7025 3600 7150 3600
Wire Wire Line
	7150 3800 7025 3800
Wire Wire Line
	7025 4000 7150 4000
Text Label 7025 3400 2    50   ~ 0
PIN_27
Text Label 7025 3600 2    50   ~ 0
PIN_14
Text Label 7025 3800 2    50   ~ 0
PIN_12
Text Label 7025 4000 2    50   ~ 0
PIN_13
$Comp
L EESTN5:HEADER_8 J1
U 1 1 615AF59D
P 7250 3200
F 0 "J1" H 7478 3153 60  0000 L CNN
F 1 "TECLADO MATRICIAL" H 7478 3047 60  0000 L CNN
F 2 "EESTN5:Pin_Header_Straight_1x08" H 7250 3850 60  0001 C CNN
F 3 "" H 7250 3850 60  0000 C CNN
	1    7250 3200
	1    0    0    -1  
$EndComp
Text Label 4475 3225 0    50   ~ 0
GND
Wire Wire Line
	4350 3225 4475 3225
$Comp
L Connector:Conn_01x03_Female J3
U 1 1 615FB65C
P 5650 3325
F 0 "J3" H 5678 3351 50  0000 L CNN
F 1 "MOD. RELAY 0" H 5678 3260 50  0000 L CNN
F 2 "EESTN5:BORNERA3_AZUL" H 5650 3325 50  0001 C CNN
F 3 "~" H 5650 3325 50  0001 C CNN
	1    5650 3325
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x03_Female J4
U 1 1 615FC063
P 5650 3725
F 0 "J4" H 5678 3751 50  0000 L CNN
F 1 "MOD. RELAY 1" H 5678 3660 50  0000 L CNN
F 2 "EESTN5:BORNERA3_AZUL" H 5650 3725 50  0001 C CNN
F 3 "~" H 5650 3725 50  0001 C CNN
	1    5650 3725
	1    0    0    -1  
$EndComp
Wire Wire Line
	5325 3225 5450 3225
Wire Wire Line
	5350 3625 5450 3625
Text Label 5350 3625 2    50   ~ 0
5V
Text Label 5325 3225 2    50   ~ 0
5V
Wire Wire Line
	5450 3425 5325 3425
Wire Wire Line
	5325 3325 5450 3325
Wire Wire Line
	5350 3725 5450 3725
Text Label 5350 3725 2    50   ~ 0
GND
Text Label 5325 3325 2    50   ~ 0
GND
Wire Wire Line
	5350 3825 5450 3825
Text Label 5325 3425 2    50   ~ 0
PIN_4
Text Label 5350 3825 2    50   ~ 0
PIN_2
$Comp
L EESTN5:HEADER_4 J5
U 1 1 61639D01
P 5900 4400
F 0 "J5" H 6028 4453 60  0000 L CNN
F 1 "LED RGB" H 6028 4347 60  0000 L CNN
F 2 "EESTN5:pin_strip_4" H 5900 4400 60  0001 C CNN
F 3 "" H 5900 4400 60  0000 C CNN
	1    5900 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5275 4250 5800 4250
Text Label 5275 4350 2    50   ~ 0
PIN_19
Text Label 5275 4450 2    50   ~ 0
PIN_18
Text Label 5275 4550 2    50   ~ 0
PIN_5
Text Label 5275 4250 2    50   ~ 0
GND
$Comp
L EESTN5:R R1
U 1 1 61666CE5
P 5575 4350
F 0 "R1" V 5625 4375 50  0000 L CNN
F 1 "1k" V 5625 4250 50  0000 L CNN
F 2 "EESTN5:RES0.2" H 5575 4350 60  0001 C CNN
F 3 "" H 5575 4350 60  0000 C CNN
	1    5575 4350
	0    -1   -1   0   
$EndComp
$Comp
L EESTN5:R R2
U 1 1 6166A880
P 5575 4450
F 0 "R2" V 5625 4475 50  0000 L CNN
F 1 "1k" V 5625 4350 50  0000 L CNN
F 2 "EESTN5:RES0.2" H 5575 4450 60  0001 C CNN
F 3 "" H 5575 4450 60  0000 C CNN
	1    5575 4450
	0    -1   -1   0   
$EndComp
$Comp
L EESTN5:R R3
U 1 1 6166858E
P 5575 4550
F 0 "R3" V 5625 4575 50  0000 L CNN
F 1 "1k" V 5625 4450 50  0000 L CNN
F 2 "EESTN5:RES0.2" H 5575 4550 60  0001 C CNN
F 3 "" H 5575 4550 60  0000 C CNN
	1    5575 4550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5675 4350 5800 4350
Wire Wire Line
	5675 4450 5800 4450
Wire Wire Line
	5675 4550 5800 4550
Wire Wire Line
	5275 4350 5475 4350
Wire Wire Line
	5275 4450 5475 4450
Wire Wire Line
	5275 4550 5475 4550
$Comp
L EESTN5:Buzzer BZ1
U 1 1 61679CEE
P 7225 4525
F 0 "BZ1" H 7377 4554 50  0000 L CNN
F 1 "Buzzer" H 7377 4463 50  0000 L CNN
F 2 "EESTN5:Buzzer_12mm" V 7200 4625 50  0001 C CNN
F 3 "" V 7200 4625 50  0001 C CNN
	1    7225 4525
	1    0    0    -1  
$EndComp
$EndSCHEMATC
