CC = gcc
CFLAGS  = -Wall
TARGET_1 = server
TARGET_2 = client
POSTFIX = _w7e2.c

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): $(TARGET_1)$(POSTFIX)
	$(CC) $(CFLAGS) -o $(TARGET_1) $(TARGET_1)$(POSTFIX)

$(TARGET_2): $(TARGET_2)$(POSTFIX)
	$(CC) $(CFLAGS) -o $(TARGET_2) $(TARGET_2)$(POSTFIX)

clean:
	$(RM) $(TARGET_1) $(TARGET_2) *.o
