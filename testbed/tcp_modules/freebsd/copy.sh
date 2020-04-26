GATEWAY=192.168.10.100
TEMP=/root
HOST=pi3host2
TARGET=/root/abe/

# Copy files to gateway and then to host
scp abe.c Makefile root@$GATEWAY:$TEMP
ssh root@$GATEWAY scp $TEMP/abe.c $TEMP/Makefile $HOST:$TARGET

# Remove files from gateway
ssh root@$GATEWAY rm $TEMP/abe.c $TEMP/Makefile