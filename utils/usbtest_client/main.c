#include <libusb.h>
#include <stdio.h>
#include <stdlib.h>

#include "USB.h"

static struct libusb_device_handle *devh = NULL;

int main(int argc, char* argv[])
{
  int res = libusb_init(NULL);
	if(res < 0) {
    fprintf(stderr, "Unable to intialize libusb\n");
    return -1;
  }

	devh = libusb_open_device_with_vid_pid(NULL, 0x1337, 0x7331);
	if(devh == 0) {
    fprintf(stderr, "Unable to find USB device\n");
    return -2;
  }

	res = libusb_claim_interface(devh, 0);
	if (res < 0) {
		fprintf(stderr, "usb_claim_interface error %d\n", res);
		return -3;
	}
	printf("claimed interface\n");

  uint16_t value;
	res = libusb_control_transfer(devh, CTRL_IN, REQUEST_GETVALUE, 0, 0, (unsigned char*)&value, sizeof(value), 0);
	if(res < 0) {
    fprintf(stderr, "libusb_control_transfer error %d\n", res);
    return -4;
  }
  printf("Value is %u\r\n", value);

  res = libusb_control_transfer(devh, CTRL_OUT, REQUEST_SETVALUE, value+1, 0, NULL, 0, 0);
  if(res < 0) {
    fprintf(stderr, "libusb_control_transfer error %d\n", res);
    return -5;
  }
  printf("Value set\r\n");

  res = libusb_control_transfer(devh, CTRL_IN, REQUEST_GETVALUE, 0, 0, (unsigned char*)&value, sizeof(value), 0);
  if(res < 0) {
    fprintf(stderr, "libusb_control_transfer error %d\n", res);
    return -6;
  }
  printf("Value is %u\r\n", value);

  libusb_release_interface(devh, 0);
  libusb_close(devh);
  exit(0);
}
