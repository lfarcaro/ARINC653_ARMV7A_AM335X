java -jar Tools/TemplateManager.jar C COLLAPSE Application Application.art "^.+\.[ch]$" "" YES
java -jar Tools/TemplateGenerator.jar ARMV7A_AM335X Application Application/module.xml
java -jar Tools/TemplateManager.jar C EXPAND Application Application.art "^.+\.[ch]$" "" YES
java -jar Tools/ScheduleGenerator.jar Application/module.xml PNG Application/module.png