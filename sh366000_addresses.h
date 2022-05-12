#ifndef SH366000_ADDRESSES_H
#define SH366000_ADDRESSES_H

#include "types.h"
#include "qstring.h"

struct flash_mem_reg
{
    quint16     regAddr;    //Адрес флеш памяти
    quint8      dataLen;    //Размер данных по этому адресу
    quint8      unsign;     //1-unsigned, 0-signed
    QString     name;       //Название параметра
    QString     unit;       //Единица измерения
};

flash_mem_reg flash [] =
{
    {0x0000, 0x02, 1, "RemaningTimeAlarm",                     "min"},
    {0x0002, 0x02, 1, "RemaningCapacityAlarm",                 "mAh"},
    {0x0004, 0x02, 1, "CycleCount",                            "-"},
    {0x0006, 0x02, 1, "FullChargeCapacity",                    "mAh"},
    {0x000C, 0x02, 1, "CycleThreshold",                        "mAh"},
    {0x000E, 0x02, 1, "DesignVoltage",                         "mV"},
    {0x0010, 0x02, 1, "DesignCapacity",                        "mAh"},
    {0x0012, 0x02, 1, "ChemistryID",                           "HEX"},
    {0x0016, 0x01, 1, "ElectronicsLoad",                       "mA/100"},
    {0x0017, 0x01, 1, "SelfDischargeRate",                     "%/100"},
    {0x0018, 0x01, 1, "GasgaugeConfig",                        "HEX"},
    {0x001B, 0x02, 1, "NearFull",                              "mAh"},
    {0x001D, 0x01, 1, "PackConfig",                            "HEX"},
    {0x001E, 0x02, 1, "Specification Info",                    "HEX"},
    {0x0024, 0x02, 0, "Overload",                              "mA"},
    {0x0026, 0x02, 1, "MaxOverCharge",                         "mAh"},
    {0x0028, 0x02, 1, "NormalChargeCurrent",                   "mA"},
    {0x002A, 0x02, 1, "ChargeVoltage",                         "mV"},
    {0x002C, 0x02, 1, "PrechargeVoltageThreshold",             "mV"},
    {0x002E, 0x02, 1, "PrechargeCurrent",                      "mA"},
    {0x0030, 0x02, 1, "PrechargeTemperatureThreshold",         "°C"},
    {0x0033, 0x02, 1, "PrechargeVoltageResetMargin",           "mV"},
    {0x0035, 0x01, 1, "FullChargeClear",                       "%"},
    {0x0036, 0x02, 1, "TaperCurrent",                          "mA"},
    {0x0038, 0x02, 1, "TaperVoltage",                          "mV"},
    {0x003A, 0x01, 1, "CurrentTaperWindows",                   "sec"},
    {0x003C, 0x02, 1, "LearnLowTemperature",                   "°C"},
    {0x003E, 0x02, 1, "MaxChargeTime",                         "sec"},
    {0x0044, 0x02, 1, "CurrentDetectThreshold",                "mA"},
    {0x0046, 0x02, 1, "ChargeSuspendHighTemp",                 "°C"},
    {0x0048, 0x02, 1, "ChargeSuspendLowTemp",                  "°C"},
    {0x0052, 0x02, 1, "ShutDownVoltage",                       "mV"},
    {0x0055, 0x02, 1, "IdleCurrent",                           "mA"},
    {0x0058, 0x02, 1, "VPackThreshhold",                       "mV"},
    {0x005C, 0x02, 1, "CellOverVoltageThreshhold",             "mV"},
    {0x005E, 0x02, 1, "OverVoltageTimeThreshhold",             "sec"},
    {0x0060, 0x02, 1, "CellOverVoltageResetThreshhold",        "mV"},
    {0x0064, 0x02, 1, "PackOverVoltageResetMargin",            "mV"},
    {0x0066, 0x02, 1, "PackOverVoltageMargin",                 "mV"},
    {0x0068, 0x02, 1, "ChargeUnderVoltageThreshold",           "mV"},
    {0x006C, 0x02, 1, "ChargeUnderVoltageResetThreshold",      "mV"},
    {0x0070, 0x02, 1, "ChargeOverCurrentThreshold",            "mA"},
    {0x0072, 0x02, 1, "OverCurrentTimeThreshold",              "sec"},
    {0x0074, 0x02, 1, "ChargeOverCurrentResetThreshold",       "mA"},
    {0x0076, 0x02, 0, "DischargeOverCurrentThreshold",         "mA"},
    {0x007A, 0x02, 0, "DischargeOverCurrentResetThreshold",    "mA"},
    {0x007E, 0x02, 1, "OverTempDishargeThreshold",             "°C"},
    {0x0080, 0x02, 1, "OverTempTimeThreshold",                 "sec"},
    {0x0082, 0x02, 1, "OverTempDischargeResetThreshold",       "°C"},
    {0x0086, 0x02, 1, "OverTempChargeThreshold",               "°C"},
    {0x008A, 0x02, 1, "OverTempChargeResetThreshold",          "°C"},
    {0x008E, 0x02, 1, "SafetyOverVolThreshold",                "mA"},
    {0x0092, 0x02, 1, "SafetyOverCurrentThreshold",            "mA"},
    {0x0096, 0x02, 1, "SafetyOverTempThreshold",               "°C"},
    {0x0098, 0x02, 1, "FETFailCurrent",                        "mA"},
    {0x009C, 0x02, 1, "AFEFailTime",                           "Cnts"},
    {0x009E, 0x02, 1, "VOC75",                                 "mV"},
    {0x00A0, 0x02, 1, "VOC50",                                 "mV"},
    {0x00A2, 0x02, 1, "VOC25",                                 "mV"},
    {0x00A4, 0x01, 1, "Battery Low",                           "0,1%"},
    {0x00A5, 0x02, 1, "ADJP0",                                 "-"},
    {0x00A7, 0x02, 1, "ADJP1",                                 "-"},
    {0x00A9, 0x01, 1, "ADJP2",                                 "-"},
    {0x00AA, 0x02, 1, "ADJP3",                                 "-"},
    {0x00AC, 0x02, 1, "ADJP4",                                 "-"},
    {0x00AE, 0x02, 1, "ADJP5",                                 "-"},
    {0x00AF, 0x02, 1, "ADJP6",                                 "-"},
    {0x00B1, 0x02, 1, "ADJP7",                                 "-"},
    {0x00B3, 0x02, 1, "ADJP8",                                 "-"},
    {0x00B5, 0x01, 1, "ADJP9",                                 "-"},
    {0x00B6, 0x02, 1, "FEDV2",                                 "mV"},
    {0x00B8, 0x02, 1, "FEDV1",                                 "mV"},
    {0x00BA, 0x02, 1, "FEDV0",                                 "mV"},
    {0x00C0, 0x02, 1, "ReferenceResistor",                     "mOhm"},
    {0x00DC, 0x02, 1, "CellBalanceThreshold",                  "mV"},
    {0x00DE, 0x02, 1, "CellBalanceWindow",                     "mV"},
    {0x00E0, 0x01, 1, "Cell Balance Min",                      "mV"},
    {0x00E2, 0x02, 1, "Cell Imbalance Max",                    "mV"},
    {0x00E6, 0x02, 1, "Balance I Min",                         "mA"},
    {0x00EC, 0x01, 1, "Pflag",                                 "HEX"},
    {0x00F2, 0x01, 1, "AFE OLV",                               "HEX"},
    {0x00F3, 0x01, 1, "AFE OLT",                               "HEX"},
    {0x00F4, 0x01, 1, "AFE SCC",                               "HEX"},
    {0x00F5, 0x01, 1, "AFE SCD",                               "HEX"},
    {0x00FB, 0x02, 1, "Manufacture Date",                      "d-m-y"},
    {0x00FD, 0x02, 1, "Serial Number",                         "-"},
    {0x0101, 0x02, 1, "Manufacture Password1",                 "HEX"},
    {0x0103, 0x02, 1, "Manufacture Password2",                 "HEX"},
    {0x0105, 0x02, 1, "Manufacture Password3",                 "HEX"},
    {0x0107, 0x02, 1, "Manufacture Password4",                 "HEX"},
    {0x010A, 0x10, 1, "ManufactureName",                       "ASCII"},
    {0x011A, 0x10, 1, "DeviceName",                            "ASCII"},
    {0x012A, 0x10, 1, "ChemistryName",                         "ASCII"},
};

enum ACCESS
{
    R,
    RW
};

enum UNSIGN
{
    I,
    U
};

struct SBS_commands
{
    quint8      addr;          //Адрес команды
    ACCESS      access;        //Запись/Чтение
    quint8      dataLen;       //Длина в байтах
    UNSIGN      unsign;        //1-unsigned, 0-signed
    QString     name;          //Имя параметра
    QString     unit;          //Единица измерения
};

enum SBS_column
{
    SBS_ADDR_COLUMN,
    SBS_ACCESS_COLUMN,
    SBS_NAME_COLUMN,
    SBS_DATA_COLUMN,
    SBS_UNIT_CLOUNM
};

const SBS_commands Standard_Commands [] =
{
    {   0x00,   RW, 2,  U,  "ManufacturerAccess",       "HEX"   },
    {   0x01,   RW, 2,  U,  "RemainingCapacityAlarm",   "mAh"   },
    {   0x02,   RW, 2,  U,  "RemainingTimeAlarm",       "min"   },
    {   0x03,   RW, 2,  U,  "BatteryMode",              "HEX"   },
    {   0x05,   R,  2,  U,  "AtRateTimeToFull",         "min"   },
    {   0x06,   R,  2,  U,  "AtRateTimeToEmpty",        "min"   },
    {   0x07,   R,  2,  U,  "AtRateOK",                 "-"     },
    {   0x08,   R,  2,  U,  "Temperature",              "°C"    },
    {   0x09,   R,  2,  U,  "Voltage",                  "mV"    },
    {   0x0a,   R,  2,  I,  "Current",                  "mA"    },
    {   0x0b,   R,  2,  I,  "AverageCurrent",           "mA"    },
    {   0x0c,   R,  1,  U,  "MaxError",                 "%"     },
    {   0x0d,   R,  1,  U,  "RelativeStateOfCharge",    "%"     },
    {   0x0e,   R,  1,  U,  "AbsoluteStateOfCharge",    "%"     },
    {   0x0f,   R,  2,  U,  "RemainingCapacity",        "mAh"   },
    {   0x10,   R,  2,  U,  "FullChargeCapacity",       "mAh"   },
    {   0x11,   R,  2,  U,  "RunTimeToEmpty",           "min"   },
    {   0x12,   R,  2,  U,  "AverageTimeToEmpty",       "min"   },
    {   0x13,   R,  2,  U,  "AverageTimeToFull",        "min"   },
    {   0x14,   R,  2,  U,  "ChargingCurrent",          "mA"    },
    {   0x15,   R,  2,  U,  "ChargingVoltage",          "mV"    },
    {   0x16,   R,  2,  U,  "BatteryStatus",            "HEX"   },
    {   0x17,   R,  2,  U,  "CycleCount",               "-"     },
    {   0x18,   R,  2,  U,  "DesignCapacity",           "mAh"   },
    {   0x19,   R,  2,  U,  "DesignVoltage",            "mV"    },
    {   0x1a,   R,  2,  U,  "SpecificationInfo",        "HEX"   },
    {   0x1b,   R,  2,  U,  "ManufactureDate",          "d-m-y" },
    {   0x1c,   R,  2,  U,  "SerialNumber",             "HEX"   },
    {   0x20,   R,  20, U,  "ManufacturerName",       "ASCII"   },
    {   0x21,   R,  20, U,  "DeviceName",             "ASCII"   },
    {   0x22,   R,  4,  U,  "DeviceChemistry",        "ASCII"   },
    {   0x23,   R,  14, U,  "ManufacturerData",       "Block"   },
    {   0x2f,   R,  1,  U,  "Pack Status",              "LBS"   },
    {   0x2f,   R,  1,  U,  "Pack Config",              "MBS"   },
    {   0x3c,   R,  2,  U,  "CellVoltage4",             "mV"    },
    {   0x3d,   R,  2,  U,  "CellVoltage3",             "mV"    },
    {   0x3e,   R,  2,  U,  "CellVoltage2",             "mV"    },
    {   0x3f,   R,  2,  U,  "Cellvoltage1",             "mV"    },
    {   0x46,   R,  2,  U,  "AFEData",                  "BIN"   },
    {   0x47,   R,  2,  U,  "Ssafe",                    "BIN"   },
};

struct bin_data
{
    quint8      addr;          //Адрес регистра
    quint8      bit;           //Запись/Чтение
    QString     name;          //Имя параметра
    QString     state0;        //Имя параметра в стостоянии 0
    QString     state1;        //Имя параметра в стостоянии 1
};

enum BIN_column
{
    BIT_NUM,
    BIT_NAME,
    BIT_STATUS
};

//0x03 Battery Mode
const bin_data BatteryMode [] =
{
    {   0x03,   0,   "Internal Charge Controller",  "Not Supported ",                   "Supported"},
    {   0x03,   1,   "Primary Battery Support",     "Not Supported",                    "Supported"},
    {   0x03,   7,   "Condition Flag",              "Battery OK",                       "Conditioning Cycle Requested"},
    {   0x03,   8,   "Internal Charge Control",     "Disabled (Default)",               "Enabled"},
    {   0x03,   9,   "Primary Battery",             "Secondary (Default)",              "Primary"},
    {   0x03,   13,  "Alarm Mode",                  "Enable AlarmWarning broadcasts",   "Disable AlarmWarning broadcast"},
    {   0x03,   14,  "Charger Mode",                "Enable ChV and ChC broadcasts",    "Disable ChV and ChC broadcasts"},
    {   0x03,   15,  "Capacitt Mode",               "Report in mA or mAh (default)",    "Report in 10mW or 10mWh"}
};
//0x16 Battery Status
const bin_data BatteryStatus [] =
{
    {   0x16,   0,   "Error Code 0 *",                  "0",   "1"},
    {   0x16,   1,   "Error Code 1 *",                  "0",   "1"},
    {   0x16,   2,   "Error Code 2 *",                  "0",   "1"},
    {   0x16,   3,   "Error Code 3 *",                  "0",   "1"},
    {   0x16,   4,   "Fully Discharged (FD)",           "0",   "1"},
    {   0x16,   5,   "Fully Charged (FC)",              "0",   "1"},
    {   0x16,   6,   "Discharging (DSG)",               "0",   "1"},
    {   0x16,   7,   "Initialized ( INT)",              "0",   "1"},
    {   0x16,   8,   "Remaining Time Alarm (RTA)",      "0",   "1"},
    {   0x16,   9,   "Remaining Capacity Alarm (RCA)",  "0",   "1"},
    {   0x16,   10,  "Reserved",                        "0",   "1"},
    {   0x16,   11,  "Terminate Discharge Alarm (TDA)", "0",   "1"},
    {   0x16,   12,  "Over Temp Alarm (OTA)",           "0",   "1"},
    {   0x16,   13,  "Reserved",                        "0",   "1"},
    {   0x16,   14,  "Terminate Charge Alarm (TCA)",    "0",   "1"},
    {   0x16,   15,  "Over Charged Alarm (OCA)",        "0",   "1"}
};
//0x2f LBS
const bin_data Pack_Status [] =
{
    {   0x2f,   0,  "Discharge MOSFET (CVUV)",          "On",           "Off"       },
    {   0x2f,   1,  "Charge MOSFET (CVOV)",             "On",           "Off"       },
    {   0x2f,   2,  "Protection Flag (PF)",             "Normal",       "Safe"      },
    {   0x2f,   3,  "Analog Front End (AFE) Status",    "Normal",       "Abnormal"  },
    {   0x2f,   4,  "Discharge Cycle Valid (VDQ)",      "Not Valid",    "Valid"     },
    {   0x2f,   5,  "System Seal",                      "Unseal",       "Seal"      },
    {   0x2f,   6,  "Discharge Low Voltage (EDV2)",     "Normal",       "Lower"     },
    {   0x2f,   7,  "System Present (PRES)",            "Off",          "On"        }
};
//0x2f MBS
const bin_data Pack_Config [] =
{
    {   0x2f,   8,   "Disconnected Battery (NR)",           "Disconnected", "Connected"     },
    {   0x2f,   9,   "Charging MOS state in NR=1 (NRCHG)",  "Turned Off",   "Turned On"     },
    {   0x2f,   10,  "Precharge (ENPCHG)",                  "Support",      "Not Support"   },
    {   0x2f,   11,  "Number of Cell strings (CC)*",         "0",            "1"             },
    {   0x2f,   12,  "Number of Cell strings (CC)*",         "0",            "1"             },
    {   0x2f,   13,  "Broadcast Function (SM)",             "Enable",       "Disable"       },
    {   0x2f,   14,  "LED Display (LED)",                   "4 Led",        "5 Led"         },
    {   0x2f,   15,  "LED Display (DMODE)",                 "ASOC",         "RSOC"          }
};
//0x46 AFE_Status
const bin_data AFE_Status [] =
{
    {   0x46,   0,  "Short Circuit in Discharge (SCDSG)",   "Normal",   "Detected"      },
    {   0x46,   1,  "Short Circuit in Charge (SCCSG)",      "Normal",   "Detected"      },
    {   0x46,   2,  "AFE Overload (OL)",                    "Normal",   "Detected"      },
    {   0x46,   3,  "Analog Front End (AFE) Status",        "Normal",   "Alarm"         }
};
//0x47 Ssafe
const bin_data Ssafe [] =
{
    {   0x47,   4,   "Safety Protection Fail (PFF)",        "Normal",   "Fail"          },
    {   0x47,   5,   "ADC Error ADCF",                      "Normal",   "Fail"          },
    {   0x47,   6,   "AFE Communication Fail (AFEF)",       "Normal",   "Error"         },
    {   0x47,   7,   "Safety Protection (PFIN)",            "Normal",   "FUSE Action"   },
    {   0x47,   8,   "Discharge MOSFET Fail (DFETF)",       "Normal",   "Fail"          },
    {   0x47,   9,   "Charge MOSFET Fail (CFETF)",          "Normal",   "Fail"          },
    {   0x47,   10,  "Safety Discharge Over Temp (SOTD)",   "Normal",   "Over Temp"     },
    {   0x47,   11,  "Safety Charge Over Temp (SOTC)",      "Normal",   "Over Temp"     },
    {   0x47,   12,  "Safety Discharge Over Current (SCD)", "Normal",   "Over Current"  },
    {   0x47,   13,  "Safety Charge Over Current (SCC)",    "Normal",   "Over Current"  },
    {   0x47,   14,  "Safety Over Voltage (SOV)",           "Normal",   "Over Voltage"  },
    {   0x47,   15,  "Cell Imbalance (CIM)",                "Normal",   "Imbalance"     }
};

#endif // SH366000_ADDRESSES_H
