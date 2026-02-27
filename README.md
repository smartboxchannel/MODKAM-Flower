# MODKAM Flower

![MODKAM Flower](https://raw.githubusercontent.com/smartboxchannel/MODKAM-Flower/refs/heads/main/IMAGES/MODKAM_Flower6.jpg) 

Поблагодарить авторов: https://yoomoney.ru/fundraise/P2XLTgJsB6k.231012

Thank the authors: https://yoomoney.ru/fundraise/P2XLTgJsB6k.231012

Телеграм чат DIY Devices - https://t.me/diy_devices

Продажа DIY Устройств - https://t.me/diydevmart


### Установка среды разработки

В качестве среды разработки будет использоваться IAR Embedded Workbench, где цифра 8051 это архитектура чипа, а не версия системы, как можно подумать. Для начала необходимо ее [скачать по ссылке](https://www.iar.com/iar-embedded-workbench#!?architecture=8051)


Запускаем, необходимо выбрать пункт "Install IAR Embedded Workbench® for 8051"

При установке iar надо выбрать режим custom:

и убрать галочку с установки Dongle drivers

При попытки запуска проекта может быть такая ошибка:

Поэтому перед запуском проекта необходимо убедиться что python установлен. Проверить это можно в командной строке:

python –-version

Если все хорошо, то покажет текущую установленную версию python, как на примере:

Если же нет, то [идем устанавливать python](https://www.python.org/)

Устанавливаем его. Обязательно необходимо проставить галочку "add python **** to path"

Устанавливаем прошивку [Z-Stack 3.0.2](https://www.ti.com/tool/Z-STACK) — это SDK для разработки прошивок.

Устанавливаем, можно немного уменьшить путь до zstack: C:\Z-Stack 3.0.2. Здесь вопросов быть не должно.

Скачиваем исходный код из репозитория

Добавляем папку с файлами проекта в zstack: C:\Z-Stack 3.0.2\Projects\zstack\HomeAutomation

Теперь можно запускать IAR Embedded Workbench. Открываем проект пунктом меню Open workspace

Находим нашу папку со скаченным проектом, далее в папке CC2530DB находим файл проекта

Выбираем конфигурацию RouterEB и выполняем Rebuild All.
