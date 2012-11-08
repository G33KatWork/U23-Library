# Target file name.
TARGET = usb_keyboard

# List C source files here.
CCSOURCES = Game.c \
USB_DeviceEvents.c USB_DeviceDescriptor.c USB_HID_Constants.c usbd_hid_core.c

# Additional local static libs to link against
LIBS = gaming stm32f4xx fatfs sdcard graphics usbcore usbdevice

# Folder for object files
OBJDIR = obj

# Folder for sourcecode
SRCDIR = src

# Additional defines
DEFINES := -DRGB8Pixels

-include $(ROOT)/build/firmware.mak
