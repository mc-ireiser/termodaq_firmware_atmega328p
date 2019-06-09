# termoDaQ Firmware ATmega328p
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fmc-ireiser%2Ftermodaq_firmware_atmega328p.svg?type=shield)](https://app.fossa.io/projects/git%2Bgithub.com%2Fmc-ireiser%2Ftermodaq_firmware_atmega328p?ref=badge_shield)


## Como iniciar?

Clone o Descarge el **Repositorio**.

Si Clona:
> Entre en la carpeta termodaq_firmware_atmega328p-master.

Si Descarga:
> Descomprima el archivo descargado, luego entre en la carpeta termodaq_firmware_atmega328p-master.

### Instalar PlatformIO Core CLI

En el terminal ejecute el siguiente comando para instalar platformio, este es un generador de código multiplataforma y administrador de librerias.

```bash
# Instalar platformio
pip install -U platformio
```

Nota: `Python 2.7` es neesario para ejecutar el comando anterior.

### Compilar y Cargar el código en la placa

Conecte la palca termoDaQ a su computador, luego en el terminar ejecute:

```bash
# Para Compilar
platformio run

# Para Compilar y Cargar
platformio run --target upload
```

#### Ejemplo de compilacion correcta

```bash
Linking .pioenvs\uno\firmware.elf
Building .pioenvs\uno\firmware.hex
Checking size .pioenvs\uno\firmware.elf
Memory Usage -> http://bit.ly/pio-memory-usage
DATA:    [======    ]  62.3% (used 1276 bytes from 2048 bytes)
PROGRAM: [=======   ]  69.5% (used 22406 bytes from 32256 bytes)
=================================================== [SUCCESS] Took 26.58 seconds
```

#### Ejemplo de compilacion y carga correcta

```bash
DATA:    [======    ]  62.3% (used 1276 bytes from 2048 bytes)
PROGRAM: [=======   ]  69.5% (used 22406 bytes from 32256 bytes)
Configuring upload protocol...
AVAILABLE: arduino
CURRENT: upload_protocol = arduino
Looking for upload port...
Auto-detected: COM7
Uploading .pioenvs\uno\firmware.hex

avrdude: AVR device initialized and ready to accept instructions

Reading | ################################################## | 100% 0.00s

avrdude: Device signature = 0x1e950f (probably m328p)
avrdude: reading input file ".pioenvs\uno\firmware.hex"
avrdude: writing flash (22406 bytes):

Writing | ################################################## | 100% 8.44s

avrdude: 22406 bytes of flash written
avrdude: verifying flash memory against .pioenvs\uno\firmware.hex:
avrdude: load data flash data from input file .pioenvs\uno\firmware.hex:
avrdude: input file .pioenvs\uno\firmware.hex contains 22406 bytes
avrdude: reading on-chip flash data:

Reading | ################################################## | 100% 7.56s

avrdude: verifying ...
avrdude: 22406 bytes of flash verified

avrdude: safemode: Fuses OK (E:00, H:00, L:00)

avrdude done.  Thank you.

=================================================== [SUCCESS] Took 23.15 seconds
```

## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2Fmc-ireiser%2Ftermodaq_firmware_atmega328p.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2Fmc-ireiser%2Ftermodaq_firmware_atmega328p?ref=badge_large)