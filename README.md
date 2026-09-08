# large-file-io-lab


для начала работы нужно запустить скрипт и заполнить файлы с датой 

на маке: 

```python3 scripts/generate_data.py --profile bench```

после:

```
    cmake −S . −B build−release −DCMAKE_BUILD_TYPE=Release
    cmake −−build build−release −j
    ./build−release/largeio_smoke data/100MiB.bin data/256MiB.bin \
    data/1GiB.bin data/4GiB.bin
```

