# large-file-io-lab


для начала работы нужно запустить скрипт и заполнить файлы с датой 

на маке: 

``` python3 scripts/generate_data.py ```


после:

```for file in data/*; do
    echo "=== $file ==="
    ./build/largeio_smoke "$file"
    echo
done
```

