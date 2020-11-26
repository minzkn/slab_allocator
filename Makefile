# Copyright (C) JAEHYUK CHO
# All rights reserved.
# 
# Author: JaeHyuk Cho <minzkn@minzkn.com>

CROSS_COMPILE                ?=#

CC                           :=$(CROSS_COMPILE)gcc#
LD                           :=$(CROSS_COMPILE)ld#
AR                           :=$(CROSS_COMPILE)ar#
RM                           :=rm -f#
STRIP                        :=$(CROSS_COMPILE)strip#

THIS_NAME                    :=mzslab#

CFLAGS                       :=-Os -pipe -ansi -fomit-frame-pointer -fPIC -I.#
CFLAGS                       +=-Wall -W -Wconversion -Wshadow -Wcast-qual -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Winline -Wwrite-strings#
#CFLAGS                       +=-Wall -W -Wshadow -Wcast-qual -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -Winline -Wwrite-strings#

LDFLAGS                      :=-s#
ARFLAGS                      :=rc#
STRIPFLAGS                   :=--remove-section=.comment --remove-section=.note#

TARGET_lib                   :=lib$(THIS_NAME).lo lib$(THIS_NAME).a lib$(THIS_NAME).so#
TARGET_bin                   :=$(THIS_NAME)#
TARGET                       :=$(TARGET_lib) $(TARGET_bin)#

OBJECTS_lib                  :=$(THIS_NAME).o#
OBJECTS_bin                  :=main.o#
OBJECTS                      :=$(OBJECTS_lib) $(OBJECTS_bin)#

DEPEND                       :=Makefile $(THIS_NAME).h#

.PHONY: all clean

all: $(TARGET)
clean: ; $(RM) *.o $(TARGET)

$(TARGET_bin): $(OBJECTS_bin) lib$(THIS_NAME).a
	$(CC) $(LDFLAGS) -o $(@) $(^)
	$(STRIP) $(STRIPFLAGS) $(@)
$(TARGET_lib): $(OBJECTS_lib)
$(OBJECTS): $(DEPEND)

# %.so:    ; $(CC) $(LDFLAGS) -shared -Wl,-soname,$(@).0 -o $(@) $(^)
%.so:    ; $(CC) $(LDFLAGS) -shared -o $(@) $(^)
%.a:     ; $(AR) $(ARFLAGS) $(@) $(^)
%.lo:    ; $(LD) $(LDFLAGS) -r -o $(@) $(^)
%.o: %.c ; $(CC) $(CFLAGS) -c -o $(@) $(<)

# End of Makefile
