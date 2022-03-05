# Умный выключатель grib technology
Беспроводной сенсорный выключатель grib technology - устройство системы умного дома на основе API Telegram.

![DSC06544](https://user-images.githubusercontent.com/84660518/156888964-e295e8a9-c531-4ee4-bc5f-3f1df61e0288.jpg)


## Содержание
- [Умный выключатель grib technology](#switch)
- [Подробнее о системе и коде](#code)
- [Плата и основные компоненты](#components)
- [Схема и работа печатной платы](#scheme)
- [Ответственность](#responsibility)
- [Контакты](#contacts)


<a id="switch"></a>
# Умный выключатель реле grib technology

Выключатель — один из компонентов системы умного дома grib technology. Управляет [реле и розетками](https://github.com/grib-technology/grib_socket_and_relay) c помощью [ESP-NOW.](https://www.espressif.com/en/products/software/esp-now/overview).

О системе умного дома grib technology: 

Шлюзом системы является устройство [Miss You Even When We're Together](https://github.com/grib-technology/grib_myewwt_clock) (MYEWWT).

MYEWWT взаимодействует c API Telegram, позволяя пользователю отправлять команды и предоставляет на них ответ в Telegram-боте, заренее созданном пользователем. 

MYEWWT подключается к Интернету через WI-Fi и взаимодействует с Telegram ботом, получая команды от него, обрабатывает их и управляет реле и розеткой с помощью технологии [ESP-NOW.](https://www.espressif.com/en/products/software/esp-now/overview)



Возможности устройства MYEWWT:

* соединение с точкой доступа Wi-Fi и выход в интернет,
* обмен данными с другими устройствами умного дома с применением технологии [ESP-NOW](https://www.espressif.com/en/products/software/esp-now/overview),
* контроль температуры и влажности воздуха в помещении,
* возможность смены цвета сегментов часов на любой из диапазона RGB.


<a id="code"></a>
# Подробнее о системе и коде
Здесь и далее находится описание выключателя grib technology. Информацию о системе умного дома grib technology, особенностях кода прошивки вы можете найти [здесь](https://github.com/grib-technology/grib_myewwt_clock/blob/main/ABOUT.md).

<a id="components"></a>
# Плата и основные компоненты

![Безымянный3](https://user-images.githubusercontent.com/84660518/156888202-56f70ef1-fd2b-4216-8583-a68bb003adaa.png)


Основные компоненты:
 * микроконтроллер Espressif ESP-12F,
 * сенсорные модули ttp-223,
 * батарейный отсек

<a id="scheme"></a>
# Схема и работа печатной платы

![Schematic_switch_2022-03-04](https://user-images.githubusercontent.com/84660518/156888193-6b77c75f-477e-4969-9431-6295f0271405.png)


На плату подаётся питание 3 В от батарейного отсека, в котором находятся две батарейки типа ААА. Питание микроконтроллера осуществляется с помощью режима Power-Down, при котором потребление самого микроконтроллера составляет 3 мкА. Каждый сенсорный модуль потребляет 2 мкА, в сумме весь выключатель в режиме ожидания потребляет ~7мкА, чего хватает на долгое время (от нескольких месяцев до нескольких лет в зависимости от используемых батареек).

В режим Power-Down микроконтроллер ESP-12F вводится благодаря отключению питания с пина EN. 

Логика работы выключателя следующая: при нажатии на кнопку высокий уровень (HIGH) подается на GPIO0, GPIO2 и на пин EN, что запускает микроконтроллер. При включении в коде прошивки HIGH подаётся на GPIO0:
```cpp
pinMode(0, OUTPUT);
digitalWrite(0, HIGH);
```
это нужно для поддержания питания на пине EN даже при незажатой кнопке. Помимо этого, HIGH подаётся также на GPIO14 или GPIO4 в зависимости от нажатой кнопки, для того, чтобы микроконтроллер смог определить какая кнопка была нажата и поднять соответствующий флаг butt1 или butt2:
```cpp
butt1 = digitalRead(14);
butt2 = !butt1;
```

Затем, после отработки цикла (отправки запроса на включение или выключение устройства), микроконтроллер переводит пин GPIO0 в LOW, что в результате переводит пин EN также в LOW, а микроконтроллер в режим Power-Down. На случай, если кнопка не была отпущена, в качестве защитной меры микроконтроллер дополнительно вводится в режим Deep-Sleep командой deepSleepInstant. 

```cpp
digitalWrite(0, LOW);
ESP.deepSleepInstant(0);
```
p.s. (system_deep_sleep() назначает таймер с процедурой system_deep_sleep_instant() через 100 мс, соответственно, используя напрямую данную команду мы экономим 100 мс)


<a id="responsibility"></a>
# Ответственность

В прошивке выключателя используются библиотеки из проектов с открытым исходным кодом на основании лицензий, закреплёнными за авторами библиотек. Таким образом, работоспособность библиотек не может быть гарантирована. Также нет никаких гарантий, что устройство будет работать с вашей электросетью, и вы не получите ущерба во время эксплуатации устройства.

[Лицензия GNU General Public License v3.0](https://github.com/grib-technology/grib_switch/blob/main/LICENSE.md)

<a id="contacts"></a>
# Контакты

www.grib-tech.ru

[Telegram-канал](t.me/grib_tech) 

[Instagram](instagram.com/grib.tech)

Для связи:

[Бот-помощник](t.me/grib_tech_bot) 

[Чат с разработчиком](t.me/rafflezy)

