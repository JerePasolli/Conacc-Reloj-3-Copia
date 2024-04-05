# Conacc-Reloj-3

## Indice
 

## Introduccion

Este repositorio contiene el programa ConAcc 3 el cual es un reloj de asistencia con UI, manejado por una pantalla IPS de 5 pulgadas tactil, conectada por DSI. El usuario unicamente tiene contacto con la interfaz a travez de la opcion tactil de la pantalla.

## Desarrollo

### Instalacion pantalla DSI

El flex de la pantalla se conecta en el puerto DSI0, descripto por la waveshare como DISP 0. 

Una vez dentro del sistema, por defecto DSI esta desabilitado, para habilitarlo abrir una terminal y correr los siguientes comandos

```bash
$ wget https://files.waveshare.com/upload/7/75/CM4_dt_blob_Source.zip
$ unzip -o  CM4_dt_blob_Source.zip -d ./CM4_dt_blob_Source
$ sudo chmod 777 -R CM4_dt_blob
$ cd CM4_dt_blob/
$ sudo dtc -I dts -O dtb -o /boot/dt-blob.bin dt-blob-disp1-double_cam.dts
```
Luego se necesita modificar el archivo de configuracion que se encuentra en /boot/config.txt, el cual debe quedar de la siguiente manera:

```
# For more options and information see
# http://rptl.io/configtxt
# Some settings may impact device functionality. See link above for details

# Uncomment some or all of these to enable the optional hardware interfaces
#dtparam=i2c_arm=on
#dtparam=i2s=on
dtparam=spi=on

# Enable audio (loads snd_bcm2835)
#dtparam=audio=on

# Additional overlays and parameters are documented
# /boot/firmware/overlays/README

# Automatically load overlays for detected cameras
#camera_auto_detect=1

# Automatically load overlays for detected DSI displays
display_auto_detect=1

# Automatically load initramfs files, if found
auto_initramfs=1

# Enable DRM VC4 V3D driver
#dtoverlay=vc4-kms-v3d
max_framebuffers=2

# Don't have the firmware create an initial video= setting in cmdline.txt.
# Use the kernel's default instead.
#disable_fw_kms_setup=1

# Run in 64-bit mode
arm_64bit=1

# Disable compensation for displays with overscan
disable_overscan=1

# Run as fast as firmware / board allows
arm_boost=1

[cm4]
# Enable host mode on the 2711 built-in XHCI USB controller.
# This line should be removed if the legacy DWC2 controller is required
# (e.g. for USB device mode) or if USB support is not required.
otg_mode=1

[all]
dtoverlay=dwc2,dr_mode=host
start_x=1
dtoverlay=vc4-kms-v3d
dtoverlay=vc4-kms-dsi-7inch,dsi0
```
Luego se debe reiniciar para aplicar estos cambios. Se debe esperar en torno a los 2 a 5 minutos para que inicien los drivers de la pantalla, en caso de no iniciar, ejecutar nuevamente 
```bash
$ sudo dtc -I dts -O dtb -o /boot/dt-blob.bin dt-blob-disp0-double_cam.dts
```
esperar 5 minutos y reiniciar.

## Estilo
clang-format style=Microsoft -i archivo.cpp


## Links

https://forums.raspberrypi.com/viewtopic.php?t=311167
https://raspberrypi.stackexchange.com/questions/139860/cm4-cm4io-board-waveshares-5inch-dsi-lcd-display-problem
https://www.waveshare.com/wiki/5inch_DSI_LCD#Features
https://www.waveshare.com/wiki/Compute-Module-4#CSI_DSI

