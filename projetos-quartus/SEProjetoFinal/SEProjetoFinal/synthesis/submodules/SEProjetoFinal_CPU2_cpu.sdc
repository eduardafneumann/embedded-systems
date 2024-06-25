# Legal Notice: (C)2024 Altera Corporation. All rights reserved.  Your
# use of Altera Corporation's design tools, logic functions and other
# software and tools, and its AMPP partner logic functions, and any
# output files any of the foregoing (including device programming or
# simulation files), and any associated documentation or information are
# expressly subject to the terms and conditions of the Altera Program
# License Subscription Agreement or other applicable license agreement,
# including, without limitation, that your use is for the sole purpose
# of programming logic devices manufactured by Altera and sold by Altera
# or its authorized distributors.  Please refer to the applicable
# agreement for further details.

#**************************************************************
# Timequest JTAG clock definition
#   Uncommenting the following lines will define the JTAG
#   clock in TimeQuest Timing Analyzer
#**************************************************************

#create_clock -period 10MHz {altera_reserved_tck}
#set_clock_groups -asynchronous -group {altera_reserved_tck}

#**************************************************************
# Set TCL Path Variables 
#**************************************************************

set 	SEProjetoFinal_CPU2_cpu 	SEProjetoFinal_CPU2_cpu:*
set 	SEProjetoFinal_CPU2_cpu_oci 	SEProjetoFinal_CPU2_cpu_nios2_oci:the_SEProjetoFinal_CPU2_cpu_nios2_oci
set 	SEProjetoFinal_CPU2_cpu_oci_break 	SEProjetoFinal_CPU2_cpu_nios2_oci_break:the_SEProjetoFinal_CPU2_cpu_nios2_oci_break
set 	SEProjetoFinal_CPU2_cpu_ocimem 	SEProjetoFinal_CPU2_cpu_nios2_ocimem:the_SEProjetoFinal_CPU2_cpu_nios2_ocimem
set 	SEProjetoFinal_CPU2_cpu_oci_debug 	SEProjetoFinal_CPU2_cpu_nios2_oci_debug:the_SEProjetoFinal_CPU2_cpu_nios2_oci_debug
set 	SEProjetoFinal_CPU2_cpu_wrapper 	SEProjetoFinal_CPU2_cpu_debug_slave_wrapper:the_SEProjetoFinal_CPU2_cpu_debug_slave_wrapper
set 	SEProjetoFinal_CPU2_cpu_jtag_tck 	SEProjetoFinal_CPU2_cpu_debug_slave_tck:the_SEProjetoFinal_CPU2_cpu_debug_slave_tck
set 	SEProjetoFinal_CPU2_cpu_jtag_sysclk 	SEProjetoFinal_CPU2_cpu_debug_slave_sysclk:the_SEProjetoFinal_CPU2_cpu_debug_slave_sysclk
set 	SEProjetoFinal_CPU2_cpu_oci_path 	 [format "%s|%s" $SEProjetoFinal_CPU2_cpu $SEProjetoFinal_CPU2_cpu_oci]
set 	SEProjetoFinal_CPU2_cpu_oci_break_path 	 [format "%s|%s" $SEProjetoFinal_CPU2_cpu_oci_path $SEProjetoFinal_CPU2_cpu_oci_break]
set 	SEProjetoFinal_CPU2_cpu_ocimem_path 	 [format "%s|%s" $SEProjetoFinal_CPU2_cpu_oci_path $SEProjetoFinal_CPU2_cpu_ocimem]
set 	SEProjetoFinal_CPU2_cpu_oci_debug_path 	 [format "%s|%s" $SEProjetoFinal_CPU2_cpu_oci_path $SEProjetoFinal_CPU2_cpu_oci_debug]
set 	SEProjetoFinal_CPU2_cpu_jtag_tck_path 	 [format "%s|%s|%s" $SEProjetoFinal_CPU2_cpu_oci_path $SEProjetoFinal_CPU2_cpu_wrapper $SEProjetoFinal_CPU2_cpu_jtag_tck]
set 	SEProjetoFinal_CPU2_cpu_jtag_sysclk_path 	 [format "%s|%s|%s" $SEProjetoFinal_CPU2_cpu_oci_path $SEProjetoFinal_CPU2_cpu_wrapper $SEProjetoFinal_CPU2_cpu_jtag_sysclk]
set 	SEProjetoFinal_CPU2_cpu_jtag_sr 	 [format "%s|*sr" $SEProjetoFinal_CPU2_cpu_jtag_tck_path]

#**************************************************************
# Set False Paths
#**************************************************************

set_false_path -from [get_keepers *$SEProjetoFinal_CPU2_cpu_oci_break_path|break_readreg*] -to [get_keepers *$SEProjetoFinal_CPU2_cpu_jtag_sr*]
set_false_path -from [get_keepers *$SEProjetoFinal_CPU2_cpu_oci_debug_path|*resetlatch]     -to [get_keepers *$SEProjetoFinal_CPU2_cpu_jtag_sr[33]]
set_false_path -from [get_keepers *$SEProjetoFinal_CPU2_cpu_oci_debug_path|monitor_ready]  -to [get_keepers *$SEProjetoFinal_CPU2_cpu_jtag_sr[0]]
set_false_path -from [get_keepers *$SEProjetoFinal_CPU2_cpu_oci_debug_path|monitor_error]  -to [get_keepers *$SEProjetoFinal_CPU2_cpu_jtag_sr[34]]
set_false_path -from [get_keepers *$SEProjetoFinal_CPU2_cpu_ocimem_path|*MonDReg*] -to [get_keepers *$SEProjetoFinal_CPU2_cpu_jtag_sr*]
set_false_path -from *$SEProjetoFinal_CPU2_cpu_jtag_sr*    -to *$SEProjetoFinal_CPU2_cpu_jtag_sysclk_path|*jdo*
set_false_path -from sld_hub:*|irf_reg* -to *$SEProjetoFinal_CPU2_cpu_jtag_sysclk_path|ir*
set_false_path -from sld_hub:*|sld_shadow_jsm:shadow_jsm|state[1] -to *$SEProjetoFinal_CPU2_cpu_oci_debug_path|monitor_go
