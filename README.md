# lru_cache [thread unsafe]
### Build

```
prod
$ clang++ -O3 main.cpp -o lru_cache && ./lru_cache
```
```
test
$ clang++ -O0 -Wall -g -fsanitize=fuzzer,address,undefined,leak -DFUZZING -DNOPRINT main.cpp  -o lru_cache && ./lru_cache -use_value_profile=1 -workers=2 -reload=0 
```
### Output
```
size:1 [new] key1:999
size:2 [new] key2:888
size:3 [new] key3:777
size:4 [new] key4:666
size:5 [new] key5:555
size:6 [new] key6:444
size:6 [key6:444][key5:555][key4:666][key3:777][key2:888][key1:999]
size:5 [deleted] key2:888
size:5 [update] key5:1000
size:5 [key5:1000][key6:444][key4:666][key3:777][key1:999]
size:4 [deleted] key5:1000
size:3 [deleted] key6:444
size:2 [deleted] key4:666
size:1 [deleted] key3:777
size:0 [deleted] key1:999
```
### Difficulty
```
Get O(1) average
Set O(1) with no reorder
```
