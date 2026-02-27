# MODKAM Flower

## Проект датчика влажности почвы

(основан на исходном коде проекта DIYRUZ Flower - https://github.com/diyruz/flower)

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/MODKAM_Flower6.jpg) 

Датчик, работает на базе SoC CC2530, основан на емкостном методе измерения(у датчика нет электрического контакта с почвой). Питание датчика осуществляется от батареек CR 2032 и AAA.

Печатная плата четырехслойная, с медным сенсором во внутреннем слое. Это решение обеспечивает долговечность. Медный сенсор со временем не коррозирует от проникающей влаги через микротрещины в маске, которые появляются со временем.

### Основные передаваемые данные:

Identify - идентификация устройства

Soil moisture - измеренное значение влажности почвы

Battery - оставшийся заряд в %

### Дополнительные передаваемые данные:

Temperature - измеренное значение температуры

Illuminance - измеренная освещенность(версия zFlora S Max)

Устройство может быть сконфигурировано внешними командами, отправляемыми через интерфейс контроллера умного дома. 

Что бы датчик получило конфигурационную команду, после отправки нового значения со страницы устройства в з2м, нужно нажать кнопку на датчике, он выйдет из сна, примет данные и запишет в энергонезависимую память.

### Конфигурационные команды:

Read sensors delay - время сна датчика, в минутах, по умолчанию 3 минуты

Smart sleep - Режим проверки устройством наличия для него пакетов на роутерах и координаторе

Lifetime - время работы датчика с момента прошивки

Tx radio power - Мощность радиопередатчика

Resolution - Разрешение сенсора DS18B20

Temperature compensation - Температурная компенсация

Lux factor - множитель для подстройки сенсора освещенности, диапазон 0.1-10.0, шаг 0.1.

#

### Ввод датчика в сеть, выход из сети
Для джойна (вход в сеть), нажать и удерживать кнопку 2 секунды, загорится светодиод, потухнет либо при начале входа в сеть, (в среднем через 5-8 секунд) либо через 15 секунд, что будет означать что датчик не нашел открытой сети.

На время ввода в сеть расположитесь не далеко от координатора (1-2 метра) или роутера с хорошим уровнем сигнала. 

Если вы не видите на вкладке свойства всех значений конфигурационных атрибутов(пустые поля, переключатели в неопределенном состоянии) или на странице отчеты нет строк о батарейке, температуре, то скорее всего конфигурация, которая следует сразу за интервью, не была пройдена до конца.

Для повторного прохождения конфигурации, перейдите на главную страницу z2m, найдите строку датчика и справа нажмите на кнопку - реконфигурация (желтая кнопка), после этого нажмите 2-5 раз кнопку на датчике с интервалом 1-2 секунды. При успешном прохождении конфигурации в интерфейсе з2м должно появится "всплывающее" сообщение об успешно пройденной реконфигурации. В разделе датчика, на вкладке свойства должны появится значения и установки всех конфигурационных свойств, на странице отчеты должны появится строки с конфигурационными настройками отчетов.

Для выхода из сети зажать кнопку на 10 секунд, по истечении этого времени загорится светодиод, кнопку можно отпустить. Датчик отправит сообщение о выходе из сети, сотрет у себя все настройки в памяти.

### Рекомендации
При начальном использовании датчика рекомендуется вставить датчик в грунт и обильно пролить. Что бы из почвы вокруг сенсора вышел воздух и грунт уплотнился. Если этого не сделать, то в первые 1-2 суток может наблюдаться более низкий уровень измеренной влажности, который к тому же может расти немного вверх.

### Тест сенсора влажности

Возьмите датчик в руку (держаться за батарейный отсек, не за плату!).

Не касаясь сенсора нажать кнопку - soil_moisture должно быть 0%.

Зажать второй рукой сенсор, или опустить сенсор в воду, нажать на кнопку - должно быть около больше 0% (в воде 100%).

#


Поблагодарить авторов: https://yoomoney.ru/fundraise/P2XLTgJsB6k.231012

Thank the authors: https://yoomoney.ru/fundraise/P2XLTgJsB6k.231012

Телеграм чат DIY Devices - https://t.me/diy_devices

Продажа DIY Устройств - https://t.me/diydevmart

#

### Установка среды разработки

В качестве среды разработки будет использоваться IAR Embedded Workbench, где цифра 8051 это архитектура чипа, а не версия системы, как можно подумать. Для начала необходимо ее [скачать по ссылке](https://www.iar.com/iar-embedded-workbench#!?architecture=8051)

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/1.png) 

Запускаем, необходимо выбрать пункт "Install IAR Embedded Workbench® for 8051"

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/2.png) 

При установке iar надо выбрать режим custom:

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/3.png) 

и убрать галочку с установки Dongle drivers

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/4.png) 

При попытки запуска проекта может быть такая ошибка:

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/5.png) 

Поэтому перед запуском проекта необходимо убедиться что python установлен. Проверить это можно в командной строке:

python –-version

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/6.png) 

Если все хорошо, то покажет текущую установленную версию python, как на примере:

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/7.png) 

Если же нет, то [идем устанавливать python](https://www.python.org/)

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/8.png) 

Устанавливаем его. Обязательно необходимо проставить галочку "add python **** to path"

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/9.png) 

Устанавливаем прошивку [Z-Stack 3.0.2](https://www.ti.com/tool/Z-STACK) — это SDK для разработки прошивок.

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/10.png) 

Устанавливаем, можно немного уменьшить путь до zstack: C:\Z-Stack 3.0.2. 

Скачиваем исходный код из репозитория

Добавляем папку с файлами проекта в zstack: C:\Z-Stack 3.0.2\Projects\zstack\HomeAutomation

Теперь можно запускать IAR Embedded Workbench. Открываем проект пунктом меню Open workspace

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/11.png) 

Находим нашу папку со скаченным проектом, далее в папке CC2530DB находим файл проекта

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/12.png) 

Выполняем Rebuild All.

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/13.png) 

#

### How to flash the device

Download the Smart RF Flash Programmer V1 https://www.ti.com/tool/FLASH-PROGRAMMER

Open the application select the HEX firmware file

Connect the device with wires to CCDebugger, first erase the chip, then flash it.
