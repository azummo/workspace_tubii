
 PARAMETER VERSION = 2.2.0


BEGIN OS
 PARAMETER OS_NAME = standalone
 PARAMETER OS_VER = 6.7
 PARAMETER PROC_INSTANCE = ps7_cortexa9_0
 PARAMETER sleep_timer = ps7_globaltimer_0
 PARAMETER stdin = ps7_uart_1
 PARAMETER stdout = ps7_uart_1
END


BEGIN PROCESSOR
 PARAMETER DRIVER_NAME = cpu_cortexa9
 PARAMETER DRIVER_VER = 2.6
 PARAMETER HW_INSTANCE = ps7_cortexa9_0
END


BEGIN DRIVER
 PARAMETER DRIVER_NAME = testpulser
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = MZ_Happy
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = shiftregisters
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = ShiftRegisters_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = bursttrigger
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = burstTrigger_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = clocklogic
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = clockLogic_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = combotrigger
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = comboTrigger_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = countdisplay
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = countDisplay_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = ellie_control
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = ellie_control_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = fifo_readout
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = fifo_readout_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testdelay
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = genericDelay
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testpulser
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = genericPulser
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testdelay
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = gtDelay
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = implement_gtid
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = implement_gtid_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = oneshot_pulse
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = oneshot_pulse_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = oneshot_pulse
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = oneshot_pulse_1
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = prescalesignal
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = prescaleSignal_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = prescalesignal
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = prescaleSignal_1
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = prescaletrigger
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = prescaleTrigger_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_afi_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_afi_1
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_afi_2
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_afi_3
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = coresightps_dcc
 PARAMETER DRIVER_VER = 1.4
 PARAMETER HW_INSTANCE = ps7_coresight_comp_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = ddrps
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = ps7_ddr_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_ddrc_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = devcfg
 PARAMETER DRIVER_VER = 3.5
 PARAMETER HW_INSTANCE = ps7_dev_cfg_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = dmaps
 PARAMETER DRIVER_VER = 2.3
 PARAMETER HW_INSTANCE = ps7_dma_ns
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = dmaps
 PARAMETER DRIVER_VER = 2.3
 PARAMETER HW_INSTANCE = ps7_dma_s
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = emacps
 PARAMETER DRIVER_VER = 3.7
 PARAMETER HW_INSTANCE = ps7_ethernet_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_globaltimer_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = gpiops
 PARAMETER DRIVER_VER = 3.3
 PARAMETER HW_INSTANCE = ps7_gpio_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_gpv_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_intc_dist_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_iop_bus_config_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_l2cachec_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_ocmc_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_pl310_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_pmu_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = qspips
 PARAMETER DRIVER_VER = 3.4
 PARAMETER HW_INSTANCE = ps7_qspi_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_qspi_linear_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_ram_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_ram_1
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_scuc_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = scugic
 PARAMETER DRIVER_VER = 3.9
 PARAMETER HW_INSTANCE = ps7_scugic_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = scutimer
 PARAMETER DRIVER_VER = 2.1
 PARAMETER HW_INSTANCE = ps7_scutimer_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = scuwdt
 PARAMETER DRIVER_VER = 2.1
 PARAMETER HW_INSTANCE = ps7_scuwdt_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = sdps
 PARAMETER DRIVER_VER = 3.5
 PARAMETER HW_INSTANCE = ps7_sd_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = generic
 PARAMETER DRIVER_VER = 2.0
 PARAMETER HW_INSTANCE = ps7_slcr_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = uartps
 PARAMETER DRIVER_VER = 3.6
 PARAMETER HW_INSTANCE = ps7_uart_1
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = usbps
 PARAMETER DRIVER_VER = 2.4
 PARAMETER HW_INSTANCE = ps7_usb_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = xadcps
 PARAMETER DRIVER_VER = 2.2
 PARAMETER HW_INSTANCE = ps7_xadc_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testdelay
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = smellieDelay
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testpulser
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = smelliePulser
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testdelay
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = tellieDelay
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testpulser
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = telliePulser
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = testpulser
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = testPulser_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = triggers
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = triggers_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = trigwordfifo
 PARAMETER DRIVER_VER = 1.00.a
 PARAMETER HW_INSTANCE = trigwordfifo_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = anticoincTrigger
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = anticoincTrigger_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = coincTrigger
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = coincTrigger_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = register_mux
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = register_mux_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = clockComparison
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = clockComparison_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = clockCounter
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = clockCounter_0
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = clockCounter
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = clockCounter_1
END

BEGIN DRIVER
 PARAMETER DRIVER_NAME = register_mux
 PARAMETER DRIVER_VER = 1.0
 PARAMETER HW_INSTANCE = register_mux_1
END


BEGIN LIBRARY
 PARAMETER LIBRARY_NAME = xilffs
 PARAMETER LIBRARY_VER = 3.9
 PARAMETER PROC_INSTANCE = ps7_cortexa9_0
END


