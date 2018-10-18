CC = gcc
CFLAGS  = -Wall
TARGET_1 = server
TARGET_2 = client
POSTFIX = _w6e1.c
HEADERS = myfunc.h

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): $(TARGET_1)$(POSTFIX) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET_1) $(TARGET_1)$(POSTFIX)

$(TARGET_2): $(TARGET_2)$(POSTFIX) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET_2) $(TARGET_2)$(POSTFIX)

clean:
	$(RM) $(TARGET_1) $(TARGET_2) *.o
