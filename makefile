slab_logger: slab_logger.o
	gcc slab_logger.c -o slab_logger -pthread

slab_logger.o: slab_logger.c
