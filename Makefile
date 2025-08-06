CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

BINDIR = bin
BINARIES = $(BINDIR)/server $(BINDIR)/client $(BINDIR)/port-audio-recorder


UTILSDIR = utils
UTILS_SRC = $(wildcard $(UTILSDIR)/*.c)

all: $(BINDIR) $(BINARIES)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/server: server.c $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $@ server.c $(UTILS_SRC)

$(BINDIR)/client: client.c $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $@ client.c $(UTILS_SRC)

$(BINDIR)/port-audio-recorder: port-audio-recorder.c $(UTILS_SRC)
	$(CC) $(CFLAGS) -o $@ port-audio-recorder.c $(UTILS_SRC) -lportaudio

clean:
	rm -f $(BINARIES)
