define remote_write
	monitor long $arg0 = $arg1
end

define target-init
	target remote localhost:2331
	monitor speed 30
	monitor endian little
	monitor reset
	
	monitor reg cpsr=0xd3
	monitor speed auto
	
	remote_write 0x48000000 0x22011110 
	remote_write 0x48000004 0x00000700 
	remote_write 0x48000008 0x00000700 
	remote_write 0x4800000c 0x00000700 
	remote_write 0x48000010 0x00000700 
	remote_write 0x48000014 0x00000700 
	remote_write 0x48000018 0x00000700
	remote_write 0x4800001c 0x00018005
	remote_write 0x48000020 0x00018005
	remote_write 0x48000024 0x008c04f4
	remote_write 0x48000028 0x000000b1
	remote_write 0x4800002c 0x00000030
	remote_write 0x48000030 0x00000030
	load armboot.elf
	symbol-file armboot.elf
	monitor reg pc = 0x30000000
end

define ohcidump
	printf "HcRevision          (0x49000000): 	0x%08x\n",*(unsigned int*)0x49000000
	printf "HcControl           (0x49000004): 	0x%08x\n",*(unsigned int*)0x49000004
	printf "HcCommonStatus      (0x49000008): 	0x%08x\n",*(unsigned int*)0x49000008
	printf "HcInterruptStatus   (0x4900000c): 	0x%08x\n",*(unsigned int*)0x4900000c
	printf "HcInterruptEnable   (0x49000010):	0x%08x\n",*(unsigned int*)0x49000010
	printf "HcInterruptDisable  (0x49000014):	0x%08x\n",*(unsigned int*)0x49000014
	printf "HcHCCA              (0x49000018):	0x%08x\n",*(unsigned int*)0x49000018
	printf "HcPeriodCurrentED   (0x4900001c):	0x%08x\n",*(unsigned int*)0x4900001c
	printf "HcControlHeadED     (0x49000020):	0x%08x\n",*(unsigned int*)0x49000020
	printf "HcControlCurrentED  (0x49000024):	0x%08x\n",*(unsigned int*)0x49000024
	printf "HcBulkHeadED        (0x49000028):	0x%08x\n",*(unsigned int*)0x49000028
	printf "HcBulkCurrentED     (0x4900002c):	0x%08x\n",*(unsigned int*)0x4900002c
	printf "HcDoneHead          (0x49000030):	0x%08x\n",*(unsigned int*)0x49000030
	printf "HcRmInterval        (0x49000034):	0x%08x\n",*(unsigned int*)0x49000034
	printf "HcFmRemaining       (0x49000038):	0x%08x\n",*(unsigned int*)0x49000038
	printf "HcFmNumber          (0x4900003c):	0x%08x\n",*(unsigned int*)0x4900003c
	printf "HcPeriodStart       (0x49000040):	0x%08x\n",*(unsigned int*)0x49000040
	printf "HcLSThreshold       (0x49000044):	0x%08x\n",*(unsigned int*)0x49000044
	printf "HcRhDescriptorA     (0x49000048):	0x%08x\n",*(unsigned int*)0x49000048
	printf "HcRhDescriptorB     (0x4900004c):	0x%08x\n",*(unsigned int*)0x4900004c
	printf "HcRhStatus          (0x49000050):	0x%08x\n",*(unsigned int*)0x49000050
	printf "HcRhPortStatus1     (0x49000054):	0x%08x\n",*(unsigned int*)0x49000054
	printf "HcRhPortStatus2     (0x49000058):	0x%08x\n",*(unsigned int*)0x49000058
end

define bpl
	info breakpoints
end

define skyeye_init
	target remote localhost:12345
	load armboot.elf
end

define cpsr
	set var $_t = (unsigned int)$cpsr
	if $_t & (1 << 31)
		printf "N"
	else
		printf "n"
	end

	if $_t & (1 << 30)
		printf "Z"
	else
		printf "z"
	end

	if $_t & (1 << 29)
		printf "C"
	else
		printf "c"
	end

	if $_t & (1 << 28)
		printf "V"
	else
		printf "v"
	end

	if $_t & (1 << 27)
		printf "Q"
	else
		printf "q"
	end

	if $_t & (1 << 7)
		printf "I"
	else
		printf "i"
	end

	if $_t & (1 << 6)
		printf "F"
	else
		printf "f"
	end

	if $_t & (1 << 5)
		printf "T"
	else
		printf "t"
	end

	printf "-"
	set var $_m = (unsigned int)($_t & 0x1f)
	set var $_mode_invalid = (unsigned int)0
	if $_m == 0x10
		printf "User"
		set $_mode_invalid = 1
	end

	if $_m == 0x11
		printf "FIQ"
		set $_mode_invalid = 1
	end

	if $_m == 0x12
		printf "IRQ"
		set $_mode_invalid = 1
	end

	if $_m == 0x13
		printf "SVC"
		set $_mode_invalid = 1
	end

	if $_m == 0x17
		printf "Abort"
		set $_mode_invalid = 1
	end

	if $_m == 0x1b
		printf "Undef"
		set $_mode_invalid = 1
	end

	if $_m == 0x1f
		printf "System"
		set $_mode_invalid = 1
	end

	printf "\n"
end

set var $_linesize = 16

define linesize
	if $argc > 0
		set var $_b = 31
		while ($_b >= 0)
			if ($arg0 & (0x1 << $_b))
				set $_linesize = (1 << $_b)
				loop_break
			end
			set $_b = $_b - 1
		end
	end
	printf "linesize = %d\n",$_linesize
end

define memdump
	set var $_p = (unsigned int)$pc
	set var $_c = (unsigned int)$_linesize << 2

	if ($argc > 0)
		set $_p = (unsigned int)$arg0
	end
	
	if ($argc > 1)
		set $_c = (unsigned int)$arg1
	end

	set $_p = (unsigned int*)($_p & ~($_linesize - 1))
	set $_c = (($_c + $_linesize - 1) & (~($_linesize - 1)))
	set var $_ws = (unsigned int)($_c >> 2)

	while ($_ws > 0)
		set var $_tmp = (unsigned int)($_linesize >> 2)
		printf "%08x : ",(unsigned int)$_p
		while ($_tmp > 0)
			printf "%08x ",*$_p
			set $_p = $_p + 1
			set $_tmp = $_tmp - 1
		end
		printf "\n"
		set $_ws = $_ws - ($_linesize >> 2)
	end
end

define goto
	advance $arg0
end

define g
	goto $arg0
end

target-init
set print pretty
set print union
b C_Entry
