/*
 * THIS FILE IS AUTOMAGICALLY GENERATED.  DO NOT EDIT.
 *
 * Generated from:
 * FreeBSD
 */

const struct acpi_q_rule ABit_BP6[] = {
	{ "FADT", OEM, {"AWARD "}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x30302e31} },
	{ "" }
};

const struct acpi_q_rule AMI_INT[] = {
	{ "FADT", OEM, {"AMIINT"}, {""} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 10} },
	{ "" }
};

const struct acpi_q_rule ASUS_P2B_D[] = {
	{ "FADT", OEM, {"ASUS  "}, {"P2B-D   "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x58582e32} },
	{ "" }
};

const struct acpi_q_rule ASUS_P2B_F[] = {
	{ "FADT", OEM, {"ASUS  "}, {"P2B-F   "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x58582e31} },
	{ "" }
};

const struct acpi_q_rule ASUS_P2B_DS[] = {
	{ "FADT", OEM, {"ASUS  "}, {"P2B-DS  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x58582e31} },
	{ "FADT", CREATOR_REV, {.op = OP_LEQ}, {.rev = 0x31303030} },
	{ "" }
};

const struct acpi_q_rule ASUS_P2B_2[] = {
	{ "FADT", OEM, {"ASUS  "}, {"P2B-DS  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x58582e32} },
	{ "FADT", CREATOR_REV, {.op = OP_LEQ}, {.rev = 0x31303030} },
	{ "" }
};

const struct acpi_q_rule ASUS_P2L97_DS[] = {
	{ "FADT", OEM, {"ASUS  "}, {"P2L97-DS"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x58582e31} },
	{ "FADT", CREATOR_REV, {.op = OP_LEQ}, {.rev = 0x31303030} },
	{ "" }
};

const struct acpi_q_rule ASUS_P5A[] = {
	{ "FADT", OEM, {"ASUS  "}, {"P5A     "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x58582e31} },
	{ "" }
};

const struct acpi_q_rule Compaq_Armada_3500[] = {
	{ "FADT", OEM, {"COMPAQ"}, {"CPQB05E "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Compaq_Armada_6500[] = {
	{ "FADT", OEM, {"PTLTD "}, {"  FACP  "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Compaq_M500[] = {
	{ "FADT", OEM, {"COMPAQ"}, {"CPQB151 "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x19990722} },
	{ "" }
};

const struct acpi_q_rule Compaq_Panther[] = {
	{ "FADT", OEM, {"COMPAQ"}, {"PANTHER "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "FADT", CREATOR_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Compaq_Presario_1692[] = {
	{ "FADT", OEM, {"COMPAQ"}, {"VIPER II"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x06040000} },
	{ "FADT", CREATOR, {"PTL "} },
	{ "FADT", CREATOR_REV, {.op = OP_LEQ}, {.rev = 0xF4240} },
	{ "" }
};

const struct acpi_q_rule Compaq_Presario_1925[] = {
	{ "FADT", OEM, {"PTLTD "}, {"  FACP  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x06040000} },
	{ "" }
};

const struct acpi_q_rule Compaq_R3000Z[] = {
	{ "FADT", OEM, {"NVIDIA"}, {"CK8     "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0x6040000} },
	{ "FADT", CREATOR, {"PTL_"} },
	{ "" }
};

const struct acpi_q_rule Compaq_Thunder[] = {
	{ "FADT", OEM, {"COMPAQ"}, {"THUNDER "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "FADT", CREATOR_REV, {.op = OP_LEQ}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Compaq_RaceBait[] = {
	{ "FADT", OEM, {"COMPAQ"}, {"RACEBAIT"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 2} },
	{ "" }
};

const struct acpi_q_rule Dell_CPxxx[] = {
	{ "FADT", OEM, {"Dell  "}, {"CP???  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x27cf011b} },
	{ "" }
};

const struct acpi_q_rule Dell_CPt[] = {
	{ "FADT", OEM, {"DELL  "}, {"CPt C  "} },
	{ "FADT", OEM_REV, {.op = OP_GEQ}, {.rev = 0x27cf090e} },
	{ "" }
};

const struct acpi_q_rule Dell_CPt_A02[] = {
	{ "FADT", OEM, {"DELL  "}, {"CPt C  "} },
	{ "FADT", OEM_REV, {.op = OP_GEQ}, {.rev = 0x27cf090f} },
	{ "" }
};

const struct acpi_q_rule Dell_Latitude_LT[] = {
	{ "FADT", OEM, {"SHARP "}, {"        "} },
	{ "" }
};

const struct acpi_q_rule Dell_Inspiron_3500[] = {
	{ "FADT", OEM, {"Dell  "}, {"I 3500 "} },
	{ "" }
};

const struct acpi_q_rule Dell_PE1300[] = {
	{ "FADT", OEM, {"Dell  "}, {"PE1300  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Dell_PE1300_2[] = {
	{ "FADT", OEM, {"DELL  "}, {"PE1300  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule FIC_PA2013[] = {
	{ "FADT", OEM, {"FIC   "}, {"PA2013  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x30302e31} },
	{ "" }
};

const struct acpi_q_rule FIC_VB601[] = {
	{ "FADT", OEM, {"FIC   "}, {"VB601   "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x30302e31} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_Capricorn[] = {
	{ "FADT", OEM, {"FUJ   "}, {"CAPRICOR"} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_M63E[] = {
	{ "FADT", OEM, {"FUJ000"}, {"M63E    "} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_M65[] = {
	{ "FADT", OEM, {"FUJ   "}, {"M65     "} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_Sprint[] = {
	{ "XSDT", OEM, {"FUJ   "}, {"SPRINT  "} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_Tandem[] = {
	{ "FADT", OEM, {"FUJ   "}, {"TANDEM  "} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_Elm[] = {
	{ "FADT", OEM, {"FUJ   "}, {"M19A/XA "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 1} },
	{ "FADT", CREATOR, {"ACER"} },
	{ "FADT", CREATOR_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_Maple[] = {
	{ "FADT", OEM, {"FUJ   "}, {"M9D     "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 1} },
	{ "FADT", CREATOR, {"ACER"} },
	{ "FADT", CREATOR_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_Sycamore[] = {
	{ "FADT", OEM, {"Fuj   "}, {"M11EJ   "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 1} },
	{ "FADT", CREATOR, {"ACER"} },
	{ "FADT", CREATOR_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Fujitsu_V66NAS[] = {
	{ "FADT", OEM, {"FUJ   "}, {"V66NAS  "} },
	{ "" }
};

const struct acpi_q_rule Gateway_E1200[] = {
	{ "FADT", OEM, {"INTEL "}, {"MAUI    "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Gateway_E1400[] = {
	{ "FADT", OEM, {"SUMATR"}, {"SU81010A"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x1999072} },
	{ "" }
};

const struct acpi_q_rule Gateway_Profile[] = {
	{ "FADT", OEM, {"GATEWA"}, {"PROFILE "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Gateway_Solo[] = {
	{ "FADT", OEM, {"GATEWA"}, {"SOLO9300"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x06040000} },
	{ "" }
};

const struct acpi_q_rule Gigabyte_Award[] = {
	{ "FADT", OEM, {"GBT"}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule GBT_AWRDACPI[] = {
	{ "FADT", OEM, {"GBT   "}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x42302e31} },
	{ "" }
};

const struct acpi_q_rule Hitachi_Flora_220CX[] = {
	{ "FADT", OEM, {"HTCLTD"}, {"HTC2041 "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Hitachi_Flora_220MP[] = {
	{ "FADT", OEM, {"HTCLTD"}, {"        "} },
	{ "" }
};

const struct acpi_q_rule Hitachi_Flora_270EX[] = {
	{ "FADT", OEM, {"HTCLTD"}, {"270-EX  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Hitachi_Flora_270VX_NH7[] = {
	{ "FADT", OEM, {"HTCLTD"}, {"HTC2051 "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule HP_Ashaki[] = {
	{ "FADT", OEM, {"HP    "}, {"ASHAKI  "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule HP_HPBDD[] = {
	{ "FADT", OEM, {"HP    "}, {"HPBDD_IO"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x1006} },
	{ "" }
};

const struct acpi_q_rule HP_HPCCD_HX[] = {
	{ "FADT", OEM, {"HP    "}, {"HPCCD HX"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x10000} },
	{ "" }
};

const struct acpi_q_rule HP_Kayak_XUMP[] = {
	{ "FADT", OEM, {"HP    "}, {"COGNAC  "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule HP_LH_4[] = {
	{ "FADT", OEM, {"HP    "}, {"LH 4    "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule HP_Puma[] = {
	{ "FADT", OEM, {"HP    "}, {"Puma    "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule HP_Scimitar[] = {
	{ "FADT", OEM, {"HP    "}, {"HWPC203 "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule IBM_Aptiva_145[] = {
	{ "FADT", OEM, {"XXXXXX"}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule IBM_TP240_SandShark[] = {
	{ "FADT", OEM, {"IBM   "}, {"SShark-1"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Intel_Kauai[] = {
	{ "FADT", OEM, {"INTEL "}, {"KAUAI   "} },
	{ "" }
};

const struct acpi_q_rule Intel_L440GX[] = {
	{ "FADT", OEM, {"Intel "}, {"L440GX  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Intel_OR840[] = {
	{ "FADT", OEM, {"Intel "}, {"OR840   "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Intel_Seattle2[] = {
	{ "FADT", OEM, {"INTEL "}, {"SEATTLE2"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x19990216} },
	{ "" }
};

const struct acpi_q_rule Iwill_DBS100[] = {
	{ "FADT", OEM, {"AWARD "}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Micron_Atlanta[] = {
	{ "FADT", OEM, {"MICRON"}, {"ATLANTA "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Micron_Trek_2U375[] = {
	{ "FADT", OEM, {"MICRON"}, {"Trek2AGP"} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule NCR_S20GX[] = {
	{ "FADT", OEM, {"Intel "}, {"L440GX  "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule NEC_PowerMate_8100[] = {
	{ "FADT", OEM, {"INTEL "}, {"JN440BX1"} },
	{ "FADT", OEM_REV, {.op = OP_GEQ}, {.rev = 0x19990203} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x19990810} },
	{ "FADT", CREATOR_REV, {.op = OP_EQL}, {.rev = 0xf4240} },
	{ "" }
};

const struct acpi_q_rule NEC_VersaNote[] = {
	{ "FADT", OEM, {"INTEL "}, {"440BX   "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "FADT", CREATOR, {"PTL "} },
	{ "FADT", CREATOR_REV, {.op = OP_EQL}, {.rev = 0xf4240} },
	{ "" }
};

const struct acpi_q_rule NEC_VersaNote_C2[] = {
	{ "FADT", OEM, {"NEC   "}, {"ND000011"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule NEC_VersaNote_LXAGP[] = {
	{ "FADT", OEM, {"NEC   "}, {"ND000010"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x97} },
	{ "" }
};

const struct acpi_q_rule NEC_Z1[] = {
	{ "FADT", OEM, {"AMIINT"}, {""} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule QDIGRP_Award[] = {
	{ "FADT", OEM, {"QDIGRP"}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Siemens_Mobile_750[] = {
	{ "FADT", OEM, {"TSANYO"}, {"        "} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_rule Sony_F290[] = {
	{ "FADT", OEM, {"SONY  "}, {"K0      "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x13108b4} },
	{ "" }
};

const struct acpi_q_rule Sony_N505[] = {
	{ "FADT", OEM, {"SONY "}, {"H0       "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 0x06040000} },
	{ "" }
};

const struct acpi_q_rule Sony_PCG_xxx[] = {
	{ "FADT", OEM, {"SONY  "}, {"        "} },
	{ "" }
};

const struct acpi_q_rule Sony_PCV_L300[] = {
	{ "FADT", OEM, {"ASUS  "}, {"SHACHI  "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Sony_PCV_Sx20[] = {
	{ "FADT", OEM, {"SONY  "}, {"440BX CR"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule SuperMicro_MTB[] = {
	{ "FADT", OEM, {"SUPERM"}, {"SUPERMTB"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 2} },
	{ "" }
};

const struct acpi_q_rule Toshiba_750[] = {
	{ "FADT", OEM, {"TOSHIB"}, {"750     "} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 1} },
	{ "" }
};

const struct acpi_q_rule Tyan_TBLE[] = {
	{ "FADT", OEM, {"TYANCP"}, {"TYANTBLE"} },
	{ "FADT", OEM_REV, {.op = OP_LEQ}, {.rev = 9} },
	{ "" }
};

const struct acpi_q_rule VIA_VP3A[] = {
	{ "FADT", OEM, {"VIAVP3"}, {"AWRDACPI"} },
	{ "FADT", OEM_REV, {.op = OP_EQL}, {.rev = 0} },
	{ "" }
};

const struct acpi_q_entry acpi_quirks_table[] = {
	{ ABit_BP6, 		ACPI_Q_BROKEN },
	{ AMI_INT, 		ACPI_Q_BROKEN },
	{ ASUS_P2B_D, 		ACPI_Q_BROKEN },
	{ ASUS_P2B_F, 		ACPI_Q_BROKEN },
	{ ASUS_P2B_DS, 		ACPI_Q_BROKEN },
	{ ASUS_P2B_2, 		ACPI_Q_BROKEN },
	{ ASUS_P2L97_DS, 		ACPI_Q_BROKEN },
	{ ASUS_P5A, 		ACPI_Q_TIMER },
	{ Compaq_Armada_3500, 		ACPI_Q_BROKEN },
	{ Compaq_Armada_6500, 		ACPI_Q_BROKEN },
	{ Compaq_M500, 		ACPI_Q_BROKEN },
	{ Compaq_Panther, 		ACPI_Q_BROKEN },
	{ Compaq_Presario_1692, 		ACPI_Q_BROKEN },
	{ Compaq_Presario_1925, 		ACPI_Q_BROKEN },
	{ Compaq_R3000Z, 		ACPI_Q_MADT_IRQ0 },
	{ Compaq_Thunder, 		ACPI_Q_BROKEN },
	{ Compaq_RaceBait, 		ACPI_Q_BROKEN },
	{ Dell_CPxxx, 		ACPI_Q_BROKEN },
	{ Dell_CPt, 		ACPI_Q_BROKEN },
	{ Dell_CPt_A02, 		ACPI_Q_BROKEN },
	{ Dell_Latitude_LT, 		ACPI_Q_BROKEN },
	{ Dell_Inspiron_3500, 		ACPI_Q_BROKEN },
	{ Dell_PE1300, 		ACPI_Q_BROKEN },
	{ Dell_PE1300_2, 		ACPI_Q_BROKEN },
	{ FIC_PA2013, 		ACPI_Q_BROKEN },
	{ FIC_VB601, 		ACPI_Q_BROKEN },
	{ Fujitsu_Capricorn, 		ACPI_Q_BROKEN },
	{ Fujitsu_M63E, 		ACPI_Q_BROKEN },
	{ Fujitsu_M65, 		ACPI_Q_BROKEN },
	{ Fujitsu_Sprint, 		ACPI_Q_BROKEN },
	{ Fujitsu_Tandem, 		ACPI_Q_BROKEN },
	{ Fujitsu_Elm, 		ACPI_Q_BROKEN },
	{ Fujitsu_Maple, 		ACPI_Q_BROKEN },
	{ Fujitsu_Sycamore, 		ACPI_Q_BROKEN },
	{ Fujitsu_V66NAS, 		ACPI_Q_BROKEN },
	{ Gateway_E1200, 		ACPI_Q_BROKEN },
	{ Gateway_E1400, 		ACPI_Q_BROKEN },
	{ Gateway_Profile, 		ACPI_Q_BROKEN },
	{ Gateway_Solo, 		ACPI_Q_BROKEN },
	{ Gigabyte_Award, 		ACPI_Q_BROKEN },
	{ GBT_AWRDACPI, 		ACPI_Q_TIMER },
	{ Hitachi_Flora_220CX, 		ACPI_Q_BROKEN },
	{ Hitachi_Flora_220MP, 		ACPI_Q_BROKEN },
	{ Hitachi_Flora_270EX, 		ACPI_Q_BROKEN },
	{ Hitachi_Flora_270VX_NH7, 		ACPI_Q_BROKEN },
	{ HP_Ashaki, 		ACPI_Q_BROKEN },
	{ HP_HPBDD, 		ACPI_Q_BROKEN },
	{ HP_HPCCD_HX, 		ACPI_Q_BROKEN },
	{ HP_Kayak_XUMP, 		ACPI_Q_BROKEN },
	{ HP_LH_4, 		ACPI_Q_BROKEN },
	{ HP_Puma, 		ACPI_Q_BROKEN },
	{ HP_Scimitar, 		ACPI_Q_BROKEN },
	{ IBM_Aptiva_145, 		ACPI_Q_BROKEN },
	{ IBM_TP240_SandShark, 		ACPI_Q_BROKEN },
	{ Intel_Kauai, 		ACPI_Q_BROKEN },
	{ Intel_L440GX, 		ACPI_Q_BROKEN },
	{ Intel_OR840, 		ACPI_Q_BROKEN },
	{ Intel_Seattle2, 		ACPI_Q_BROKEN },
	{ Iwill_DBS100, 		ACPI_Q_BROKEN },
	{ Micron_Atlanta, 		ACPI_Q_BROKEN },
	{ Micron_Trek_2U375, 		ACPI_Q_BROKEN },
	{ NCR_S20GX, 		ACPI_Q_BROKEN },
	{ NEC_PowerMate_8100, 		ACPI_Q_BROKEN },
	{ NEC_VersaNote, 		ACPI_Q_BROKEN },
	{ NEC_VersaNote_C2, 		ACPI_Q_BROKEN },
	{ NEC_VersaNote_LXAGP, 		ACPI_Q_BROKEN },
	{ NEC_Z1, 		ACPI_Q_BROKEN },
	{ QDIGRP_Award, 		ACPI_Q_BROKEN },
	{ Siemens_Mobile_750, 		ACPI_Q_BROKEN },
	{ Sony_F290, 		ACPI_Q_BROKEN },
	{ Sony_N505, 		ACPI_Q_BROKEN },
	{ Sony_PCG_xxx, 		ACPI_Q_BROKEN },
	{ Sony_PCV_L300, 		ACPI_Q_BROKEN },
	{ Sony_PCV_Sx20, 		ACPI_Q_BROKEN },
	{ SuperMicro_MTB, 		ACPI_Q_BROKEN },
	{ Toshiba_750, 		ACPI_Q_BROKEN },
	{ Tyan_TBLE, 		ACPI_Q_BROKEN },
	{ VIA_VP3A, 		ACPI_Q_BROKEN },
	{ NULL, 0 }
};
