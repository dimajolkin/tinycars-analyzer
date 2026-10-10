# tinycars-analyzer

Слушает эфир штатного пульта TinyCars и печатает пакеты в USB. Модуль NRF24 только на приёме. В машинку ничего не ставится.

```text
штатный пульт  ~~~2.4 ГГц~~~>  NRF24L01  --SPI--  XIAO ESP32-S3  --USB--  лог
```

## Железо

- [Seeed XIAO ESP32-S3](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- NRF24L01+ со склада (модуль с PA+LNA тоже подходит). Питание только 3.3 В

| NRF24 | XIAO | GPIO |
|-------|------|------|
| CE | D0 | 1 |
| CSN | D1 | 2 |
| SCK | D8 | 7 |
| MISO | D9 | 8 |
| MOSI | D10 | 9 |
| VCC | 3V3 | |
| GND | GND | |

IRQ не подключён.

## Сборка

Нужен [PlatformIO](https://platformio.org/) CLI (`pio`).

```bash
make firmware
make flash
make monitor
```

Первая прошивка проверяет, что модуль отвечает по SPI, и включает приём. Разбор канала и пакета — следующий шаг.
