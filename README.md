# NOTOS
Proyecto de mediación-investigación basado en el sistema desarrollado por un grupo multidisciplinar compuesto por colaboradores/as del grupo de trabajo Satélites DIY, junto con profesorado y alumnado del IES Gómez Moreno de Madrid @Medialab-Prado.

Colaboradores/as: 
.Damián René, Steve Woolsens, Tania Netsvetaylova, Manuel Patiño, Pablo Fernández Robledo, Alvaro Redondo.
.Alumnado y profesorado (Felipe Yebes e Isabel Blázquez de Paz) del IES Gómez Moreno 
.Alumnado del IES Cervantes

https://www.medialab-prado.es/proyectos/satelites-diy

El video del proceso de los primeros prototipos, LAIKA II y Botellín de bici, puede visitarse aquí: https://vimeo.com/379979688

El sistema NOTOS, es un sistema de medición de la contaminación geolocalizado y portátil, para llevar a cabo estudios medioambientales de la ciudad de Madrid. Este dispositivo está basado en los diferentes prototipos desarrollados por los estudiantes durante las sesiones de trabajo en Medialab-Prado, la presentación del proyecto en la Feria de la Ciencia de IFEMA o el congreso científico para estudiantes, Cientificatel. 

## Funcionamiento

El sistema recoge medidas de la cantidad de partículas volátiles en el aire, PM2.5 y PM10, compuestos orgánicos volátiles (VOC), temperatura, humedad y presión atmosférica. Cuenta con un sistema de geolocalización que devuelve latitud, longitud, altitud, fecha y hora (UTC). Todos estos datos son almacenados en un archivo .csv dentro de la tarjeta microSC para su posterior análisis con un script de iphython notebook, incluido en el repositorio: https://github.com/siteixei/pollution_data

## Componentes

- Arduino MEGA 2560 
  https://store.arduino.cc/arduino-mega-2560-rev3
- Sensor de Partículas Volátiles: PMS5003
  https://www.adafruit.com/product/3686
- Sensor de temperatura, humedad, presión atmosférica y compuestos orgánicos volátiles (VOC): BME680
  https://www.adafruit.com/product/3660
- Adaptardor de tarjeta Micro SD
  https://es.aliexpress.com/item/32907897214.html
- Módulo GNSS, GPS, GLONASS GG-1802
  https://es.aliexpress.com/item/32851353457.html
  
## Datos
  
Los archivos generados tienen el siguiente formato:
 
|| 1um (ug/m3) || 2.5um (ug/m3) || 10um (ug/m3) ||	1um ||	2.5um ||	10um ||	Fecha ||	Hora ||	Latitud ||	Longitud ||	Altitud (GPS) ||	Satelites ||	Temp	|| Presion ||	Humedad ||	Gas (kOhms) ||	Altitud (Bar) ||

- || 1um (ug/m3) || 2.5um (ug/m3) || 10um (ug/m3) || corresponden a los datos en ug/m3 de las partículas volátiles de diferentes diámetros.
- ||	1um ||	2.5um ||	10um || cantidad absoluta de partículas medidas en ese instante.
- ||	Fecha ||	Hora ||
- ||	Latitud ||	Longitud || Localización GPS
- Altitud (GPS) Altitud dada por el GPS
- || Altitud (Bar) || Altitud dada por el sensor BME680 basada en los valores de la presión atmosférica con respecto al nivel del mar.
- || Satelites || Número de satélites disponibles involucrados en los datos recibidos
- ||	Temp	|| Temperatura en grados Celsius
- || Presion || Presión atmosférica con respecto al nivel del mar (QHN)
- ||	Humedad || Humedad relativa (en %)
- ||	Gas (kOhms) || Resistencia del gas, detecta cambios en la resistencia interna del sensor debido a la presencia de VOC
