# MODKAM Flower

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/MODKAM_Flower6.jpg) 

Поблагодарить авторов: https://yoomoney.ru/fundraise/P2XLTgJsB6k.231012

Thank the authors: https://yoomoney.ru/fundraise/P2XLTgJsB6k.231012

Телеграм чат DIY Devices - https://t.me/diy_devices

Продажа DIY Устройств - https://t.me/diydevmart


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
