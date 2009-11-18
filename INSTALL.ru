Инструкции по установке Midgard

Copyright (C) 1999 Jukka Zitting <jukka.zitting@iki.fi>
Copyright (C) 2000 The Midgard Project ry
Русская редакция (C) 1999 Александр Боковой <bokovoy@minsk.lug.net>
Условия распространения приведены в конце документа.

В этом документе вы найдете инструкции по установке библиотеки Midgard.
Эта библиотека позволяет приложениям Midgard взаимодействовать с базой
данных Midgard.

Для настройки и установки библиотека использует систему настройки
configure из проекта GNU. Для установки библиотеки наберите:

 $ ./configure
 $ make
 # make install

Библиотека использует клиентскую часть библиотеки MySQL, библиотеки
expat-lib (http://expat.sourceforge.net/) и glib (http://www.gtk.org) и
не может быть скомпилирована без них. Поэтому в вашей системе должны
быть установлены система разработчика MySQL и компоненты для разработки
приложений на основе Expat и Glib. Если система configure не сможет
определить расположение клиентской части библиотеки MySQL, то вы должны
запустить configure со следующей опцией:

  --with-mysql=DIR        Основная директория, в которую установлена
                          система разработчика MySQL. В директориях
			  DIR/include и DIR/lib (или DIR/inclulde/mysql
			  и DIR/lib/mysql) должны находится подключаемые
			  файлы (*.h) и клиентские библиотеки (*.a)
			  соответственно.
  --with-sitegroups       Включает поддержку сайт-групп (экспериментально).
                          Учтите, что вам необходимо также перекомпилировать
			  mod_midgard и midgard-php.

Дополнительно, утилита репликации Repligard использует zlib для
прозрачной компрессии реплицируемых данных. Практически все системы
содержат эту библиотеку, клиентская подсистема MySQL также использует
ее. Если Вы используете Linux и Midgard не находит zlib на этапе
конфигурации, возможно, что у Вас не установлены библиотеки разработчика
zlib (пакет zlib-devel в системах на базе RPM).

Файл default.h содержит некоторые встроенные настройки базы данных.
Перед тем, как скомпилировать библиотеку, вы можете изменить имя 
компьютера (host), имя пользователя (username) и пароль (password),
используемые для связи с базой данных. Эти значения будут встроены
в библиотеку Midgard и будут использованы для доступа к базе данных
всеми приложениями Midgard.

Убедитесь, что пользовательская запись на SQL-сервере, под которой
будут производится обращения из библотеки Midgard, имеет права на
выполнение команд SELECT, INSERT, UPDATE и DELETE над базой данных
Midgard. Смотрите файл INSTALL.ru пакета midgard-data для получения
дополнительной информации о настройке пользовательской записи. Желательно
не присваивать этой записи другие права, так как в таком случае
есть вероятность того, что злоумышленник сможет подсмотреть информацию
об учетной записи в скомпилированном варианте библиотеки и использовать
ее по своему усмотрению.

----------------------------------------------------------------------
Информация об авторских правах:

Copyright (C) 1999 Jukka Zitting <jukka.zitting@iki.fi>
Copyright (C) 2000 The Midgard Project ry

   Permission is granted to anyone to make or distribute verbatim copies
   of this document as received, in any medium, provided that the
   copyright notice and this permission notice are preserved,
   thus giving the recipient permission to redistribute in turn.

   Permission is granted to distribute modified versions
   of this document, or of portions of it,
   under the above conditions, provided also that they
   carry prominent notices stating who last changed them.
^L
Local variables:
version-control: never
mode: indented-text
end:
