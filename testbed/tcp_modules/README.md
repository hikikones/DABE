### Linux TCP CC Module

```
# Insert
make
sudo insmod reno_abe.ko

# Remove
sudo rmmod reno_abe
```

### Test

```
gcc -o test test.c -I. && ./test
```