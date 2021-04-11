SHELL = /bin/sh
############ Make cheat sheet ##################################################
# Make docs https://www.gnu.org/software/make/manual/make.html#Overview
# $^ = all target dependency files.
# $< = first target dependency file.
# $@ = target file.
# THE FIRST TARGET = THE DEFAULT TARGET.
# To pass env. variables use e.g. `make TEST_MODE=1`
#### Make conditionals
# ifdef TEST_MODE
# TEST_OBJ_FILES := test_all.o test_assert.o test_stdlib.o test_stdio.o assert.o\
# test_idt.o stdlib.o stdio.o string.o
# else
# TEST_OBJ_FILES :=
# endif
#### Make string substitution e.g.
# C_SOURCES = $(wildcard include/*.c)
# OBJ = ${C_SOURCES:.c=.o}
#### Variable assignments
# = vs. := , the latter skips recursive evaluation of variables value.
################################################################################

CC := cc
CC_FLAGS = -Wall -Wextra
LD := ld

all: sim_vm_mngrv2.bin sim_vm_mngrv1.bin sim_vm_mngrv0.bin

run: all
	sim_vm_mngrv2.bin

run1: all
	sim_vm_mngrv1.bin

run0: all
	sim_vm_mngrv0.bin

clean:
	rm sim_vm_mngrv2.bin sim_vm_mngrv1.bin sim_vm_mngrv0.bin

sim_vm_mngrv2.bin: *.c
	$(CC) $(CC_FLAGS) $^ -o $@ -DWITH_MODIFICATIONS

sim_vm_mngrv1.bin: *.c
	$(CC) $(CC_FLAGS) $^ -o $@

sim_vm_mngrv0.bin: *.c
	$(CC) $(CC_FLAGS) $^ -o $@ -DNO_TLB
