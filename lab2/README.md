# Лабораторная работа 2

**Название:** "Разработка драйверов блочных устройств"

**Цель работы:** Получить знания и навыки разработки драйверов блочных устройств для операционной системы Linux.

## Описание функциональности драйвера

Var5

Драйвер создает виртуальный жесткий диск в оперативной памяти с размером 50 Мбайт. Один первичный раздел размером 20Мбайт и один расширенный
раздел, содержащий три логических раздела размером 10Мбайт каждый.

## Инструкция по сборке

*build:* make
*remove:* (in sudo) rmmod main.ko
*include:* (in sudo) insmod main.ko

## Инструкция пользователя

*look at created sections:* fdisk -l

*format:* mkfs.vfat /dev/mydisk1

*P.S. we've got only mydisk1, mydisk2(but it's extended), mydisk5, mydisk6, mydisk7*

*create mount points:* mkdir -p /mnt/point1\
		       mount /dev/mydisk1 /mnt/point1
		 
		 
*read-write:* dd if=/dev/urandom of=file.txt bs=1MB count=1

*copy files from section to section:* cp /mnt/point1/file.txt /mnt/point2/file.txt

## Примеры использования

*virtual -> virtual:*
sudo dd if=/dev/mydisk1 of=/dev/mydisk2 count=1

512 bytes copied, 0,000634369 s, 807 kB/s


*real -> virtual:*
sudo dd if=/dev/sda1 of=/dev/mydisk2 count=1

512 bytes copied, 0,000274587 s, 1,9 MB/s

