/*
 * YARA rules for ARM PE compiler detection.
 * Copyright (c) 2017 Avast Software, licensed under the MIT license
 */


import "pe"

rule mingw_gcc_440
{
	meta:
		tool = "C"
		name = "MinGW GCC"
		version = "4.4.0"
		pattern = "10402DE90040A0E10CD04DE208108DE504208DE500308DE5??????EB??????EB??????EB??????EB08109DE50400A0E104209DE500309DE5??????EB0040A0E1??????EB0410A0E14200A0E3??????EBFEFFFFEA??????E?"
	strings:
		$1 = { 10 40 2D E9 00 40 A0 E1 0C D0 4D E2 08 10 8D E5 04 20 8D E5 00 30 8D E5 ?? ?? ?? EB ?? ?? ?? EB ?? ?? ?? EB ?? ?? ?? EB 08 10 9D E5 04 00 A0 E1 04 20 9D E5 00 30 9D E5 ?? ?? ?? EB 00 40 A0 E1 ?? ?? ?? EB 04 10 A0 E1 42 00 A0 E3 ?? ?? ?? EB FE FF FF EA ?? ?? ?? E? }
	condition:
		$1 at pe.entry_point
}

rule pocketpc_arm
{
	meta:
		tool = "C"
		name = "PocketPC ARM"
		pattern = "F0402DE90040A0E10150A0E10260A0E10370A0E1??0000EB0730A0E10620A0E10510A0E10400A0E1??????EBF040BDE8??0000EA??402DE9????9FE5??????????00????????????????9FE500????????00"
	strings:
		$1 = { F0 40 2D E9 00 40 A0 E1 01 50 A0 E1 02 60 A0 E1 03 70 A0 E1 ?? 00 00 EB 07 30 A0 E1 06 20 A0 E1 05 10 A0 E1 04 00 A0 E1 ?? ?? ?? EB F0 40 BD E8 ?? 00 00 EA ?? 40 2D E9 ?? ?? 9F E5 ?? ?? ?? ?? ?? 00 ?? ?? ?? ?? ?? ?? ?? ?? 9F E5 00 ?? ?? ?? ?? 00 }
	condition:
		$1 at pe.entry_point
}

rule pocketpc_mib
{
	meta:
		tool = "C"
		name = "PocketPC MIB"
		pattern = "E8FFBD271400BFAF1800A4AF1C00A5AF2000A6AF2400A7AF??????0C000000001800A48F1C00A58F2000A68F??????0C2400A78F??????0C252040001400BF8F0800E0031800BD27??FFBD271800??AF??00"
	strings:
		$1 = { E8 FF BD 27 14 00 BF AF 18 00 A4 AF 1C 00 A5 AF 20 00 A6 AF 24 00 A7 AF ?? ?? ?? 0C 00 00 00 00 18 00 A4 8F 1C 00 A5 8F 20 00 A6 8F ?? ?? ?? 0C 24 00 A7 8F ?? ?? ?? 0C 25 20 40 00 14 00 BF 8F 08 00 E0 03 18 00 BD 27 ?? FF BD 27 18 00 ?? AF ?? 00 }
	condition:
		$1 at pe.entry_point
}

rule pocketpc_sha
{
	meta:
		tool = "C"
		name = "PocketPC SHA"
		pattern = "862F962FA62FB62F224F4368536B636A7369F07F0BD00B40090009D0B365A36693670B408364036404D00B400900107F264FF66BF66AF6690B00F668??????00??????00??????00224FF07F0AD006D406D50B40090008D005D406D50B400900107F264F0B000900??????00??????00??????00??????00??????00??????????????????????????7F"
	strings:
		$1 = { 86 2F 96 2F A6 2F B6 2F 22 4F 43 68 53 6B 63 6A 73 69 F0 7F 0B D0 0B 40 09 00 09 D0 B3 65 A3 66 93 67 0B 40 83 64 03 64 04 D0 0B 40 09 00 10 7F 26 4F F6 6B F6 6A F6 69 0B 00 F6 68 ?? ?? ?? 00 ?? ?? ?? 00 ?? ?? ?? 00 22 4F F0 7F 0A D0 06 D4 06 D5 0B 40 09 00 08 D0 05 D4 06 D5 0B 40 09 00 10 7F 26 4F 0B 00 09 00 ?? ?? ?? 00 ?? ?? ?? 00 ?? ?? ?? 00 ?? ?? ?? 00 ?? ?? ?? 00 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 7F }
	condition:
		$1 at pe.entry_point
}

rule msvc_90_01
{
	meta:
		tool = "C"
		name = "MSVC"
		version = "9.0"
		extra = "Visual Studio 2008 (Windows Mobile 6 Standard SDK)"
		pattern = "0DC0A0E110582DE910B08DE285DF4DE20240A0E1CC309FE5003093E514300BE5BC309FE5000083E5012CA0E3851F4BE2??0?00EB000050E30100001A24421BE51A0000EA??0000EB873F4BE2222E4BE20410A0E1850F4BE253FFFFEB000050E30100001A"
	strings:
		$1 = { 0D C0 A0 E1 10 58 2D E9 10 B0 8D E2 85 DF 4D E2 02 40 A0 E1 CC 30 9F E5 00 30 93 E5 14 30 0B E5 BC 30 9F E5 00 00 83 E5 01 2C A0 E3 85 1F 4B E2 ?? 0? 00 EB 00 00 50 E3 01 00 00 1A 24 42 1B E5 1A 00 00 EA ?? 00 00 EB 87 3F 4B E2 22 2E 4B E2 04 10 A0 E1 85 0F 4B E2 53 FF FF EB 00 00 50 E3 01 00 00 1A }
	condition:
		$1 at pe.entry_point
}

rule msvc_90_02
{
	meta:
		tool = "C"
		name = "MSVC"
		version = "9.0"
		extra = "Visual Studio 2008 (Windows Mobile 6 Standard SDK)"
		pattern = "0DC0A0E1F0582DE91CB08DE204D04DE20340A0E10250A0E10160A0E10070A0E1??0000EB0430A0E10520A0E10610A0E10700A0E1??????EB0040A0E120400BE5010000EA0040A0E1??0000EB0400A0E1??0000EBF0681BE91EFF2FE104E02DE5003090E5"
	strings:
		$1 = { 0D C0 A0 E1 F0 58 2D E9 1C B0 8D E2 04 D0 4D E2 03 40 A0 E1 02 50 A0 E1 01 60 A0 E1 00 70 A0 E1 ?? 00 00 EB 04 30 A0 E1 05 20 A0 E1 06 10 A0 E1 07 00 A0 E1 ?? ?? ?? EB 00 40 A0 E1 20 40 0B E5 01 00 00 EA 00 40 A0 E1 ?? 00 00 EB 04 00 A0 E1 ?? 00 00 EB F0 68 1B E9 1E FF 2F E1 04 E0 2D E5 00 30 90 E5 }
	condition:
		$1 at pe.entry_point
}

rule msvc_uv
{
	 meta:
		tool = "C"
		name = "MSVC"
		pattern = "E90048EB4600F0??F?BDE80048FFF775BF0000"
		source = "Made by RetDec Team"
	strings:
		$1 = { E9 00 48 EB 46 00 F0 ?? F? BD E8 00 48 FF F7 75 BF 00 00 }
	condition:
		$1 at pe.entry_point
}
