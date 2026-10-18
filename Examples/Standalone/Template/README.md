# DasStarter

Отдельный UE-проект с подключённым исходным плагином DasForUE.
Путь checkout указан в AdditionalPluginDirectories файла DasStarter.uproject.
Полная инструкция находится в Examples/Standalone/README.md этого checkout.

После подготовки откройте проект и запустите /Game/Starter/StarterMap через Play.
WASD — движение, мышь — камера. Куб добавляет Score при пересечении и
возвращается по таймеру. Игровой код: Content/Das/Scripts; свои изменения
следует вносить туда, а не в папку Scripts плагина.

Из checkout плагина:

```powershell
./Scripts/TestStandalone.ps1 -Project C:/src/DasStarter/DasStarter.uproject
# После сборки, только целевые игровые проверки:
./Scripts/TestStandalone.ps1 -Project C:/src/DasStarter/DasStarter.uproject -SkipBuild
```

Если проект расположен в другой папке, укажите её путь.
Эти команды пересоздают собственную StarterMap и Blueprint в /Game/Starter.
Сохраните копию перед повторной генерацией, если изменяли их вручную.
Шаблон использует подготовленный checkout UE и daScript; бинарный дистрибутив
плагина и packaged-сборка здесь не проверялись.

Карта использует Movable DirectionalLight и не требует запекания освещения.
