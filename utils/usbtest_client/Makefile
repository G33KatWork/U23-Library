TARGET = usbtest_client.elf
OBJ = main.o

CFLAGS = -std=gnu99 -Wall $(shell pkg-config libusb-1.0 --cflags)
LDFLAGS = $(shell pkg-config libusb-1.0 --libs)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGET)
