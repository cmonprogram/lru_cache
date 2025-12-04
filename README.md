# lru_cache
```
$ g++ -Wall -g -fsanitize=address,undefined  main.cpp  -o lru_cache && ./lru_cache
```
```
size:1 [new] key1:999
size:2 [new] key2:888
size:3 [new] key3:777
size:4 [new] key4:666
size:5 [new] key5:555
size:6 [new] key6:444
before replace 555
size:7 [new] key5:1000
size:6 [deleted] key5:555
after replace 1000
size:5 [deleted] key5:1000
size:4 [deleted] key6:444
size:3 [deleted] key4:666
size:2 [deleted] key3:777
size:1 [deleted] key2:888
size:0 [deleted] key1:999
```
