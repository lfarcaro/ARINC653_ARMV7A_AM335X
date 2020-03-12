java -jar Tools\TemplateManager.jar C COLLAPSE Application Application.art "^.+\.[ch]$" "" YES
java -jar Tools\TemplateGenerator.jar Application\module.xml AM335X Application
java -jar Tools\TemplateManager.jar C EXPAND Application Application.art "^.+\.[ch]$" "" YES
java -jar Tools\ScheduleGenerator.jar Application\module.xml PNG Application\module.png
pause