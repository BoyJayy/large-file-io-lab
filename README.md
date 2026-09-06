# large-file-io-lab


для начала работы нужно запустить скрипт и заполнить файлы с датой 

на маке: 

``` python3 scripts/generate_data.py ```

```python3 scripts/generate_data.py --profile bench``` (если хочется с бенчами и файлы по 4 гб)


после:

```for file in data/*; do
    echo "=== $file ==="
    ./build/largeio_smoke "$file"
    echo
done
```

