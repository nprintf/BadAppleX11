LIBX11 = -lX11

BadApple: bdx11.c
	$(CC) -o $@ $^ $(LIBX11)

clean:
	rm BadApple