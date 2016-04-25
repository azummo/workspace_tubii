/*
 * tubiiXadc.h
 *
 *  Created on: Apr 19, 2016
 *      Author: Ian
 */
/* Various bits and pieces taken from the standalone libraries to run
 * the Xadc code from Adam Taylor's Microzed tutorials (Eps 7-8)
 */

#ifndef TUBIIXADC_H_
#define TUBIIXADC_H_

void *MappedXADCBaseAddress;

#define XPAR_AXI_XADC_0_DEVICE_ID 0

#define XADCPS_CH_TEMP		0x0  /**< On Chip Temperature */
#define XADCPS_CH_VCCINT	0x1  /**< VCCINT */
#define XADCPS_CH_VCCAUX	0x2  /**< VCCAUX */
#define XADCPS_CH_VPVN		0x3  /**< VP/VN Dedicated analog inputs */
#define XADCPS_CH_VREFP		0x4  /**< VREFP */
#define XADCPS_CH_VREFN		0x5  /**< VREFN */
#define XADCPS_CH_VBRAM		0x6  /**< On-chip VBRAM Data Reg, 7 series */
#define XADCPS_CH_SUPPLY_CALIB	0x07 /**< Supply Calib Data Reg */
#define XADCPS_CH_ADC_CALIB	0x08 /**< ADC Offset Channel Reg */
#define XADCPS_CH_GAINERR_CALIB 0x09 /**< Gain Error Channel Reg  */
#define XADCPS_CH_VCCPINT	0x0D /**< On-chip PS VCCPINT Channel , Zynq */
#define XADCPS_CH_VCCPAUX	0x0E /**< On-chip PS VCCPAUX Channel , Zynq */
#define XADCPS_CH_VCCPDRO	0x0F /**< On-chip PS VCCPDRO Channel , Zynq */
#define XADCPS_CH_AUX_MIN	 16 /**< Channel number for 1st Aux Channel */
#define XADCPS_CH_AUX_MAX	 31 /**< Channel number for Last Aux channel */

#define XADCPS_UNLK_OFFSET	 0x034 /**< Unlock Register */
#define XADCPS_UNLK_VALUE	 0x757BDF0D /**< Unlock Value */
#define XADCPS_CFG_OFFSET	 0x100 /**< Configuration Register */
#define XADCPS_INT_STS_OFFSET	 0x104 /**< Interrupt Status Register */
#define XADCPS_INT_MASK_OFFSET	 0x108 /**< Interrupt Mask Register */
#define XADCPS_MSTS_OFFSET	 0x10C /**< Misc status register */
#define XADCPS_CMDFIFO_OFFSET	 0x110 /**< Command FIFO Register */
#define XADCPS_RDFIFO_OFFSET	 0x114 /**< Read FIFO Register */
#define XADCPS_MCTL_OFFSET	 0x118 /**< Misc control register */
#define XADCPS_CFG_ENABLE_MASK	 0x80000000 /**< Enable access from PS mask */
#define XADCPS_CFG_CFIFOTH_MASK  0x00F00000 /**< Command FIFO Threshold mask */
#define XADCPS_CFG_DFIFOTH_MASK  0x000F0000 /**< Data FIFO Threshold mask */
#define XIL_COMPONENT_IS_READY     0x11111111  /**< component has been initialized */

#define XIL_ASSERT_NONE     0
#define XIL_ASSERT_OCCURRED 1

#define XST_SUCCESS                     0L
#define XST_FAILURE                     1L

#define XADCPS_ATR_TEST_VALUE 		0x55

#define XADCPS_ATR_TEMP_UPPER	 0 /**< High user Temperature */
#define XADCPS_ATR_VCCINT_UPPER  1 /**< VCCINT high voltage limit register */
#define XADCPS_ATR_VCCAUX_UPPER  2 /**< VCCAUX high voltage limit register */
#define XADCPS_ATR_OT_UPPER	 3 /**< VCCAUX high voltage limit register */
#define XADCPS_ATR_TEMP_LOWER	 4 /**< Upper Over Temperature limit Reg */
#define XADCPS_ATR_VCCINT_LOWER	 5 /**< VCCINT high voltage limit register */
#define XADCPS_ATR_VCCAUX_LOWER	 6 /**< VCCAUX low voltage limit register  */
#define XADCPS_ATR_OT_LOWER	 7 /**< Lower Over Temperature limit */
#define XADCPS_ATR_VBRAM_UPPER_  8 /**< VRBAM Upper Alarm Reg, 7 Series */
#define XADCPS_ATR_VCCPINT_UPPER 9 /**< VCCPINT Upper Alarm Reg, Zynq */
#define XADCPS_ATR_VCCPAUX_UPPER 0xA /**< VCCPAUX Upper Alarm Reg, Zynq */
#define XADCPS_ATR_VCCPDRO_UPPER 0xB /**< VCCPDRO Upper Alarm Reg, Zynq */
#define XADCPS_ATR_VBRAM_LOWER	 0xC /**< VRBAM Lower Alarm Reg, 7 Series */
#define XADCPS_ATR_VCCPINT_LOWER 0xD /**< VCCPINT Lower Alarm Reg , Zynq */
#define XADCPS_ATR_VCCPAUX_LOWER 0xE /**< VCCPAUX Lower Alarm Reg , Zynq */
#define XADCPS_ATR_VCCPDRO_LOWER 0xF /**< VCCPDRO Lower Alarm Reg , Zynq */

#define XADCPS_ATR_TEMP_UPPER_OFFSET	0x50 /**< Temp Upper Alarm Register */
#define XADCPS_ATR_VCCINT_UPPER_OFFSET	0x51 /**< VCCINT Upper Alarm Reg */
#define XADCPS_ATR_VCCAUX_UPPER_OFFSET	0x52 /**< VCCAUX Upper Alarm Reg */
#define XADCPS_ATR_OT_UPPER_OFFSET	0x53 /**< Over Temp Upper Alarm Reg */
#define XADCPS_ATR_TEMP_LOWER_OFFSET	0x54 /**< Temp Lower Alarm Register */
#define XADCPS_ATR_VCCINT_LOWER_OFFSET	0x55 /**< VCCINT Lower Alarm Reg */
#define XADCPS_ATR_VCCAUX_LOWER_OFFSET	0x56 /**< VCCAUX Lower Alarm Reg */
#define XADCPS_ATR_OT_LOWER_OFFSET	0x57 /**< Over Temp Lower Alarm Reg */
#define XADCPS_ATR_VBRAM_UPPER_OFFSET	0x58 /**< VBRAM Upper Alarm, 7 series */
#define XADCPS_ATR_VCCPINT_UPPER_OFFSET	0x59 /**< VCCPINT Upper Alarm, Zynq */
#define XADCPS_ATR_VCCPAUX_UPPER_OFFSET	0x5A /**< VCCPAUX Upper Alarm, Zynq */
#define XADCPS_ATR_VCCPDRO_UPPER_OFFSET	0x5B /**< VCCPDRO Upper Alarm, Zynq */
#define XADCPS_ATR_VBRAM_LOWER_OFFSET	0x5C /**< VRBAM Lower Alarm, 7 Series */
#define XADCPS_ATR_VCCPINT_LOWER_OFFSET	0x5D /**< VCCPINT Lower Alarm, Zynq */
#define XADCPS_ATR_VCCPAUX_LOWER_OFFSET	0x5E /**< VCCPAUX Lower Alarm, Zynq */
#define XADCPS_ATR_VCCPDRO_LOWER_OFFSET	0x5F /**< VCCPDRO Lower Alarm, Zynq */

#define XADCPS_JTAG_DATA_MASK		0x0000FFFF /**< Mask for the Data */
#define XADCPS_JTAG_ADDR_MASK		0x03FF0000 /**< Mask for the Addr */
#define XADCPS_JTAG_ADDR_SHIFT		16	   /**< Shift for the Addr */
#define XADCPS_JTAG_CMD_MASK		0x3C000000 /**< Mask for the Cmd */
#define XADCPS_JTAG_CMD_WRITE_MASK	0x08000000 /**< Mask for CMD Write */
#define XADCPS_JTAG_CMD_READ_MASK	0x04000000 /**< Mask for CMD Read */
#define XADCPS_JTAG_CMD_SHIFT		26	   /**< Shift for the Cmd */

#define XADCPS_SEQ_MODE_SAFE		0  /**< Default Safe Mode */
#define XADCPS_SEQ_MODE_ONEPASS		1  /**< Onepass through Sequencer */
#define XADCPS_SEQ_MODE_CONTINPASS	2  /**< Continuous Cycling Sequencer */
#define XADCPS_SEQ_MODE_SINGCHAN	3  /**< Single channel -No Sequencing */
#define XADCPS_SEQ_MODE_SIMUL_SAMPLING	4  /**< Simultaneous sampling */
#define XADCPS_SEQ_MODE_INDEPENDENT	8  /**< Independent mode */

#define XADCPS_CFR0_CAL_AVG_MASK	0x8000 /**< Averaging enable Mask */
#define XADCPS_CFR0_AVG_VALID_MASK	0x3000 /**< Averaging bit Mask */
#define XADCPS_CFR0_AVG1_MASK		0x0000 /**< No Averaging */
#define XADCPS_CFR0_AVG16_MASK		0x1000 /**< Average 16 samples */
#define XADCPS_CFR0_AVG64_MASK	 	0x2000 /**< Average 64 samples */
#define XADCPS_CFR0_AVG256_MASK 	0x3000 /**< Average 256 samples */
#define XADCPS_CFR0_AVG_SHIFT	 	12     /**< Averaging bits shift */
#define XADCPS_CFR0_MUX_MASK	 	0x0800 /**< External Mask Enable */
#define XADCPS_CFR0_DU_MASK	 	0x0400 /**< Bipolar/Unipolar mode */
#define XADCPS_CFR0_EC_MASK	 	0x0200 /**< Event driven/
						 *  Continuous mode selection
						 */
#define XADCPS_CFR0_ACQ_MASK	 	0x0100 /**< Add acquisition by 6 ADCCLK */
#define XADCPS_CFR0_CHANNEL_MASK	0x001F /**< Channel number bit Mask */

/*@}*/

/**
 * @name Configuration Register 1 (CFR1) mask(s)
 * @{
 */
#define XADCPS_CFR1_SEQ_VALID_MASK	  0xF000 /**< Sequence bit Mask */
#define XADCPS_CFR1_SEQ_SAFEMODE_MASK	  0x0000 /**< Default Safe Mode */
#define XADCPS_CFR1_SEQ_ONEPASS_MASK	  0x1000 /**< Onepass through Seq */
#define XADCPS_CFR1_SEQ_CONTINPASS_MASK	     0x2000 /**< Continuous Cycling Seq */
#define XADCPS_CFR1_SEQ_SINGCHAN_MASK	     0x3000 /**< Single channel - No Seq */
#define XADCPS_CFR1_SEQ_SIMUL_SAMPLING_MASK  0x4000 /**< Simulataneous Sampling Mask */
#define XADCPS_CFR1_SEQ_INDEPENDENT_MASK  0x8000 /**< Independent Mode */
#define XADCPS_CFR1_SEQ_SHIFT		  12     /**< Sequence bit shift */
#define XADCPS_CFR1_ALM_VCCPDRO_MASK	  0x0800 /**< Alm 6 - VCCPDRO, Zynq  */
#define XADCPS_CFR1_ALM_VCCPAUX_MASK	  0x0400 /**< Alm 5 - VCCPAUX, Zynq */
#define XADCPS_CFR1_ALM_VCCPINT_MASK	  0x0200 /**< Alm 4 - VCCPINT, Zynq */
#define XADCPS_CFR1_ALM_VBRAM_MASK	  0x0100 /**< Alm 3 - VBRAM, 7 series */
#define XADCPS_CFR1_CAL_VALID_MASK	  0x00F0 /**< Valid Calibration Mask */
#define XADCPS_CFR1_CAL_PS_GAIN_OFFSET_MASK  0x0080 /**< Calibration 3 -Power
							Supply Gain/Offset
							Enable */
#define XADCPS_CFR1_CAL_PS_OFFSET_MASK	  0x0040 /**< Calibration 2 -Power
							Supply Offset Enable */
#define XADCPS_CFR1_CAL_ADC_GAIN_OFFSET_MASK 0x0020 /**< Calibration 1 -ADC Gain
							Offset Enable */
#define XADCPS_CFR1_CAL_ADC_OFFSET_MASK	 0x0010 /**< Calibration 0 -ADC Offset
							Enable */
#define XADCPS_CFR1_CAL_DISABLE_MASK	0x0000 /**< No Calibration */
#define XADCPS_CFR1_ALM_ALL_MASK	0x0F0F /**< Mask for all alarms */
#define XADCPS_CFR1_ALM_VCCAUX_MASK	0x0008 /**< Alarm 2 - VCCAUX Enable */
#define XADCPS_CFR1_ALM_VCCINT_MASK	0x0004 /**< Alarm 1 - VCCINT Enable */
#define XADCPS_CFR1_ALM_TEMP_MASK	0x0002 /**< Alarm 0 - Temperature */
#define XADCPS_CFR1_OT_MASK		0x0001 /**< Over Temperature Enable */

/*@}*/

/**
 * @name Configuration Register 2 (CFR2) mask(s)
 * @{
 */
#define XADCPS_CFR2_CD_VALID_MASK	0xFF00  /**<Clock Divisor bit Mask   */
#define XADCPS_CFR2_CD_SHIFT		8	/**<Num of shift on division */
#define XADCPS_CFR2_CD_MIN		8	/**<Minimum value of divisor */
#define XADCPS_CFR2_CD_MAX		255	/**<Maximum value of divisor */

#define XADCPS_CFR2_CD_MIN		8	/**<Minimum value of divisor */
#define XADCPS_CFR2_PD_MASK		0x0030	/**<Power Down Mask */
#define XADCPS_CFR2_PD_XADC_MASK	0x0030	/**<Power Down XADC Mask */
#define XADCPS_CFR2_PD_ADC1_MASK	0x0020	/**<Power Down ADC1 Mask */
#define XADCPS_CFR2_PD_SHIFT		4	/**<Power Down Shift */
/*@}*/

/**
 * @name Sequence Register (SEQ) Bit Definitions
 * @{
 */
#define XADCPS_SEQ_CH_CALIB	0x00000001 /**< ADC Calibration Channel */
#define XADCPS_SEQ_CH_VCCPINT	0x00000020 /**< VCCPINT, Zynq Only */
#define XADCPS_SEQ_CH_VCCPAUX	0x00000040 /**< VCCPAUX, Zynq Only */
#define XADCPS_SEQ_CH_VCCPDRO	0x00000080 /**< VCCPDRO, Zynq Only */
#define XADCPS_SEQ_CH_TEMP	0x00000100 /**< On Chip Temperature Channel */
#define XADCPS_SEQ_CH_VCCINT	0x00000200 /**< VCCINT Channel */
#define XADCPS_SEQ_CH_VCCAUX	0x00000400 /**< VCCAUX Channel */
#define XADCPS_SEQ_CH_VPVN	0x00000800 /**< VP/VN analog inputs Channel */
#define XADCPS_SEQ_CH_VREFP	0x00001000 /**< VREFP Channel */
#define XADCPS_SEQ_CH_VREFN	0x00002000 /**< VREFN Channel */
#define XADCPS_SEQ_CH_VBRAM	0x00004000 /**< VBRAM Channel, 7 series */
#define XADCPS_SEQ_CH_AUX00	0x00010000 /**< 1st Aux Channel */
#define XADCPS_SEQ_CH_AUX01	0x00020000 /**< 2nd Aux Channel */
#define XADCPS_SEQ_CH_AUX02	0x00040000 /**< 3rd Aux Channel */
#define XADCPS_SEQ_CH_AUX03	0x00080000 /**< 4th Aux Channel */
#define XADCPS_SEQ_CH_AUX04	0x00100000 /**< 5th Aux Channel */
#define XADCPS_SEQ_CH_AUX05	0x00200000 /**< 6th Aux Channel */
#define XADCPS_SEQ_CH_AUX06	0x00400000 /**< 7th Aux Channel */
#define XADCPS_SEQ_CH_AUX07	0x00800000 /**< 8th Aux Channel */
#define XADCPS_SEQ_CH_AUX08	0x01000000 /**< 9th Aux Channel */
#define XADCPS_SEQ_CH_AUX09	0x02000000 /**< 10th Aux Channel */
#define XADCPS_SEQ_CH_AUX10	0x04000000 /**< 11th Aux Channel */
#define XADCPS_SEQ_CH_AUX11	0x08000000 /**< 12th Aux Channel */
#define XADCPS_SEQ_CH_AUX12	0x10000000 /**< 13th Aux Channel */
#define XADCPS_SEQ_CH_AUX13	0x20000000 /**< 14th Aux Channel */
#define XADCPS_SEQ_CH_AUX14	0x40000000 /**< 15th Aux Channel */
#define XADCPS_SEQ_CH_AUX15	0x80000000 /**< 16th Aux Channel */

#define XADCPS_SEQ00_CH_VALID_MASK  0x7FE1 /**< Mask for the valid channels */
#define XADCPS_SEQ01_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XADCPS_SEQ02_CH_VALID_MASK  0x7FE0 /**< Mask for the valid channels */
#define XADCPS_SEQ03_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XADCPS_SEQ04_CH_VALID_MASK  0x0800 /**< Mask for the valid channels */
#define XADCPS_SEQ05_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XADCPS_SEQ06_CH_VALID_MASK  0x0800 /**< Mask for the valid channels */
#define XADCPS_SEQ07_CH_VALID_MASK  0xFFFF /**< Mask for the valid channels */

#define XADCPS_SEQ_CH_AUX_SHIFT	16 /**< Shift for the Aux Channel */

#define XADCPS_CFR0_OFFSET	0x40	/**< Configuration Register 0 */
#define XADCPS_CFR1_OFFSET	0x41	/**< Configuration Register 1 */
#define XADCPS_CFR2_OFFSET	0x42	/**< Configuration Register 2 */

#define XADCPS_SEQ00_OFFSET	0x48 /**< Seq Reg 00 Adc Channel Selection */
#define XADCPS_SEQ01_OFFSET	0x49 /**< Seq Reg 01 Adc Channel Selection */
#define XADCPS_SEQ02_OFFSET	0x4A /**< Seq Reg 02 Adc Average Enable */
#define XADCPS_SEQ03_OFFSET	0x4B /**< Seq Reg 03 Adc Average Enable */
#define XADCPS_SEQ04_OFFSET	0x4C /**< Seq Reg 04 Adc Input Mode Select */
#define XADCPS_SEQ05_OFFSET	0x4D /**< Seq Reg 05 Adc Input Mode Select */
#define XADCPS_SEQ06_OFFSET	0x4E /**< Seq Reg 06 Adc Acquisition Select */
#define XADCPS_SEQ07_OFFSET	0x4F /**< Seq Reg 07 Adc Acquisition Select */

#define XADCPS_TEMP_OFFSET		  0x00 /**< On-chip Temperature Reg */
#define XADCPS_VCCINT_OFFSET		  0x01 /**< On-chip VCCINT Data Reg */
#define XADCPS_VCCAUX_OFFSET		  0x02 /**< On-chip VCCAUX Data Reg */
#define XADCPS_VPVN_OFFSET		  0x03 /**< ADC out of VP/VN	   */
#define XADCPS_VREFP_OFFSET		  0x04 /**< On-chip VREFP Data Reg */
#define XADCPS_VREFN_OFFSET		  0x05 /**< On-chip VREFN Data Reg */
#define XADCPS_VBRAM_OFFSET		  0x06 /**< On-chip VBRAM , 7 Series */
#define XADCPS_ADC_A_SUPPLY_CALIB_OFFSET  0x08 /**< ADC A Supply Offset Reg */
#define XADCPS_ADC_A_OFFSET_CALIB_OFFSET  0x09 /**< ADC A Offset Data Reg */
#define XADCPS_ADC_A_GAINERR_CALIB_OFFSET 0x0A /**< ADC A Gain Error Reg  */
#define XADCPS_VCCPINT_OFFSET		  0x0D /**< On-chip VCCPINT Reg, Zynq */
#define XADCPS_VCCPAUX_OFFSET		  0x0E /**< On-chip VCCPAUX Reg, Zynq */
#define XADCPS_VCCPDRO_OFFSET		  0x0F /**< On-chip VCCPDRO Reg, Zynq */

unsigned int Xil_AssertStatus;
int Xil_AssertWait = 1;
typedef void (*Xil_AssertCallback) (const char *File, int Line);
static Xil_AssertCallback Xil_AssertCallbackRoutine = NULL;
void Xil_Assert(const char *File, int Line)
{
	/* if the callback has been set then invoke it */
	if (Xil_AssertCallbackRoutine != 0) {
		(*Xil_AssertCallbackRoutine)(File, Line);
	}

	/* if specified, wait indefinitely such that the assert will show up in testing */
	while (Xil_AssertWait) {
	}
}
#define Xil_AssertNonvoid(Expression)             \
{                                                  \
    if (Expression) {                              \
        Xil_AssertStatus = XIL_ASSERT_NONE;       \
    } else {                                       \
        Xil_Assert(__FILE__, __LINE__);            \
        Xil_AssertStatus = XIL_ASSERT_OCCURRED;   \
        return 0;                                  \
    }                                              \
}
#define Xil_AssertVoid(Expression)                \
{                                                  \
    if (Expression) {                              \
        Xil_AssertStatus = XIL_ASSERT_NONE;       \
    } else {                                       \
        Xil_Assert(__FILE__, __LINE__);            \
        Xil_AssertStatus = XIL_ASSERT_OCCURRED;   \
        return;                                    \
    }                                              \
}

typedef struct {
	u16  DeviceId;		/**< Unique ID of device */
	u32  BaseAddress;	/**< Device base address */
} XAdcPs_Config;

typedef struct {
	XAdcPs_Config Config;	/**< XAdcPs_Config of current device */
	u32  IsReady;		/**< Device is initialized and ready  */

} XAdcPs;

#define XPAR_XADCPS_NUM_INSTANCES 1
#define XPAR_XADCPS_0_DEVICE_ID   0
#define XPAR_XADCPS_0_BASEADDR	  (0xF8007000)
XAdcPs_Config XAdcPs_ConfigTable[XPAR_XADCPS_NUM_INSTANCES] =
{
	{
		XPAR_XADCPS_0_DEVICE_ID,	/**< Unique ID of device */
		XPAR_XADCPS_0_BASEADDR		/**< Base address of device */
	}
};

XAdcPs_Config *XAdcPs_LookupConfig(u16 DeviceId)
{
	XAdcPs_Config *CfgPtr = NULL;
	u32 Index;

	for (Index=0; Index < 1; Index++) {
		if (XAdcPs_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XAdcPs_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}

int XAdcPs_CfgInitialize(XAdcPs *InstancePtr, XAdcPs_Config *ConfigPtr, u32 EffectiveAddr)
{

	u32 RegValue;
	/* Assert the input arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(ConfigPtr != NULL);

	/* Set the values read from the device config and the base address. */
	InstancePtr->Config.DeviceId = ConfigPtr->DeviceId;
	InstancePtr->Config.BaseAddress = EffectiveAddr;

	/* Write Unlock value to Device Config Unlock register */
	mWriteReg((InstancePtr)->Config.BaseAddress, XADCPS_UNLK_OFFSET, XADCPS_UNLK_VALUE);
	printf("%x \t %x\n",XADCPS_UNLK_VALUE,mReadReg((InstancePtr)->Config.BaseAddress,XADCPS_UNLK_OFFSET));

	/* Enable the PS access of xadc and set FIFO thresholds */
	RegValue = mReadReg((InstancePtr)->Config.BaseAddress, XADCPS_CFG_OFFSET);

	RegValue = RegValue | XADCPS_CFG_ENABLE_MASK | XADCPS_CFG_CFIFOTH_MASK | XADCPS_CFG_DFIFOTH_MASK;

	mWriteReg((InstancePtr)->Config.BaseAddress, XADCPS_CFG_OFFSET, RegValue);

	/* Release xadc from reset */
	mWriteReg((InstancePtr)->Config.BaseAddress, XADCPS_MCTL_OFFSET, 0x00);

	/* Indicate the instance is now ready to use and initialized without error. */
	InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

	return XST_SUCCESS;
}

void XAdcPs_Reset(XAdcPs *InstancePtr)
{
	/* Assert the arguments. */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/* Generate the reset by Control register and release from reset */
	mWriteReg((InstancePtr)->Config.BaseAddress, XADCPS_MCTL_OFFSET, 0x10);
	mWriteReg((InstancePtr)->Config.BaseAddress, XADCPS_MCTL_OFFSET, 0x00);
}

#define XAdcPs_FormatWriteData(RegOffset, Data, ReadWrite) 	    \
    ((ReadWrite ? XADCPS_JTAG_CMD_WRITE_MASK : XADCPS_JTAG_CMD_READ_MASK ) | ((RegOffset << XADCPS_JTAG_ADDR_SHIFT) & XADCPS_JTAG_ADDR_MASK) | (Data & XADCPS_JTAG_DATA_MASK))

#define XAdcPs_WriteFifo(InstancePtr, Data)	mWriteReg((InstancePtr)->Config.BaseAddress, XADCPS_CMDFIFO_OFFSET, Data);
#define XAdcPs_ReadFifo(InstancePtr)	    mReadReg((InstancePtr)->Config.BaseAddress, XADCPS_RDFIFO_OFFSET);


void XAdcPs_WriteInternalReg(XAdcPs *InstancePtr, u32 RegOffset, u32 Data)
{
	u32 RegData;

	/* Write the Data into the FIFO Register. */
	printf("FORMAT\n");
	RegData = XAdcPs_FormatWriteData(RegOffset, Data, 1);

	printf("WRITEF\n");
	XAdcPs_WriteFifo(InstancePtr, RegData);
	printf("%x \t %x \n", RegData, mReadReg((InstancePtr)->Config.BaseAddress, XADCPS_CMDFIFO_OFFSET));

	/* Read the Read FIFO after any write since for each write one location of Read FIFO gets updated */
	printf("READF\n");
	XAdcPs_ReadFifo(InstancePtr);
}

u32 XAdcPs_ReadInternalReg(XAdcPs *InstancePtr, u32 RegOffset)
{
	u32 RegData;

	RegData = XAdcPs_FormatWriteData(RegOffset, 0x0, 0);
	/* Read cmd to FIFO*/
	XAdcPs_WriteFifo(InstancePtr, RegData);

	/* Do a Dummy read */
	RegData = XAdcPs_ReadFifo(InstancePtr);

	/* Do a Dummy write to get the actual read */
	XAdcPs_WriteFifo(InstancePtr, RegData);

	/* Do the Actual read */
	RegData = XAdcPs_ReadFifo(InstancePtr);

	return RegData;
}

void XAdcPs_SetAlarmThreshold(XAdcPs *InstancePtr, u8 AlarmThrReg, u16 Value)
{
	/* Assert the arguments. */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(AlarmThrReg <= XADCPS_ATR_VCCPDRO_LOWER);

	printf("SET\n");
	/* Write the value into the specified Alarm Threshold Register. */
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_ATR_TEMP_UPPER_OFFSET + AlarmThrReg,Value);
	printf("SOT\n");
}

u16 XAdcPs_GetAlarmThreshold(XAdcPs *InstancePtr, u8 AlarmThrReg)
{
	u32 RegData;
	/* Assert the arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(AlarmThrReg <= XADCPS_ATR_VCCPDRO_LOWER);

	/* Read the specified Alarm Threshold Register and return the value */
	RegData = XAdcPs_ReadInternalReg(InstancePtr, (XADCPS_ATR_TEMP_UPPER_OFFSET + AlarmThrReg));

	return (u16) RegData;
}

int XAdcPs_SelfTest(XAdcPs *InstancePtr)
{
	int Status;
	u32 RegValue;

	/* Assert the argument */
	Xil_AssertNonvoid(InstancePtr != NULL);
	printf("Assert\n");
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/* Reset the device to get it back to its default state */
	printf("Reset\n");
	XAdcPs_Reset(InstancePtr);

	/* Write a value into the Alarm Threshold registers, read it back, and do the comparison */
	printf("Write %x \t %x \t %x\n", XADCPS_ATR_TEST_VALUE, MappedXADCBaseAddress, InstancePtr->Config.BaseAddress);
	XAdcPs_SetAlarmThreshold(InstancePtr, XADCPS_ATR_VCCINT_UPPER, XADCPS_ATR_TEST_VALUE);
	RegValue = XAdcPs_GetAlarmThreshold(InstancePtr, XADCPS_ATR_VCCINT_UPPER);
	printf("Read %x\n", RegValue);

	if (RegValue == XADCPS_ATR_TEST_VALUE) {
		Status = XST_SUCCESS;
	} else {
		Status = XST_FAILURE;
	}

	/* Reset the device again to its default state. */
	XAdcPs_Reset(InstancePtr);
	/* Return the test result. */
	return Status;
}

void XAdcPs_SetSequencerMode(XAdcPs *InstancePtr, u8 SequencerMode)
{
	u32 RegValue;

	/* Assert the arguments. */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid((SequencerMode <= XADCPS_SEQ_MODE_SIMUL_SAMPLING) || (SequencerMode == XADCPS_SEQ_MODE_INDEPENDENT));

	/* Set the specified sequencer mode in the Configuration Register 1. */
	RegValue = XAdcPs_ReadInternalReg(InstancePtr, XADCPS_CFR1_OFFSET);
	RegValue &= (~ XADCPS_CFR1_SEQ_VALID_MASK);
	RegValue |= ((SequencerMode  << XADCPS_CFR1_SEQ_SHIFT) & XADCPS_CFR1_SEQ_VALID_MASK);
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_CFR1_OFFSET, RegValue);
}

u8 XAdcPs_GetSequencerMode(XAdcPs *InstancePtr)
{
	/* Assert the arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/* Read the channel sequencer mode from the Configuration Register 1. */
	return ((u8) ((XAdcPs_ReadInternalReg(InstancePtr, XADCPS_CFR1_OFFSET) & XADCPS_CFR1_SEQ_VALID_MASK) >> XADCPS_CFR1_SEQ_SHIFT));
}

void XAdcPs_SetAlarmEnables(XAdcPs *InstancePtr, u16 AlmEnableMask)
{
	u32 RegValue;

	/* Assert the arguments. */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	RegValue = XAdcPs_ReadInternalReg(InstancePtr, XADCPS_CFR1_OFFSET);
	RegValue &= (u32)~XADCPS_CFR1_ALM_ALL_MASK;
	RegValue |= (~AlmEnableMask & XADCPS_CFR1_ALM_ALL_MASK);

	/* Enable/disables the alarm enables for the specified alarm bits in the Configuration Register 1. */
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_CFR1_OFFSET, RegValue);
}

int XAdcPs_SetSeqInputMode(XAdcPs *InstancePtr, u32 InputModeChMask)
{
	/* Assert the arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/* The sequencer must be disabled for writing any of these registers.
	 * Return XST_FAILURE if the channel sequencer is enabled. */
	if ((XAdcPs_GetSequencerMode(InstancePtr) != XADCPS_SEQ_MODE_SAFE)) {
		return XST_FAILURE;
	}

	/* Set the input mode for the specified channels in the ADC Channel
	 * Analog-Input Mode Sequencer Registers. */
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_SEQ04_OFFSET, (InputModeChMask & XADCPS_SEQ04_CH_VALID_MASK));
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_SEQ05_OFFSET, (InputModeChMask >> XADCPS_SEQ_CH_AUX_SHIFT) & XADCPS_SEQ05_CH_VALID_MASK);

	return XST_SUCCESS;
}

int XAdcPs_SetSeqChEnables(XAdcPs *InstancePtr, u32 ChEnableMask)
{
	/* Assert the arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/* The sequencer must be disabled for writing any of these registers
	 * Return XST_FAILURE if the channel sequencer is enabled. */
	if ((XAdcPs_GetSequencerMode(InstancePtr) != XADCPS_SEQ_MODE_SAFE)) {
		return XST_FAILURE;
	}

	/* Enable the specified channels in the ADC Channel Selection Sequencer Registers. */
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_SEQ00_OFFSET, (ChEnableMask & XADCPS_SEQ00_CH_VALID_MASK));
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_SEQ01_OFFSET, (ChEnableMask >> XADCPS_SEQ_CH_AUX_SHIFT) & XADCPS_SEQ01_CH_VALID_MASK);

	return XST_SUCCESS;
}

#define XAdcPs_RawToTemperature(AdcData) ((((float)(AdcData)/65536.0f)/0.00198421639f ) - 273.15f)

#define XAdcPs_RawToVoltage(AdcData) ((((float)(AdcData))* (3.0f))/65536.0f)

u16 XAdcPs_GetAdcData(XAdcPs *InstancePtr, u8 Channel)
{
	u32 RegData;

	/* Assert the arguments. */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid((Channel <= XADCPS_CH_VBRAM) || ((Channel >= XADCPS_CH_VCCPINT) && (Channel <= XADCPS_CH_AUX_MAX)));

	RegData = XAdcPs_ReadInternalReg(InstancePtr, (XADCPS_TEMP_OFFSET +	Channel));
	return (u16) RegData;
}


#endif /* TUBIIXADC_H_ */
