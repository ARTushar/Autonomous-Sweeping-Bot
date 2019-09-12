make all
sudo avrdude -p m32 -c usbasp -e -U flash:w:dust_sensor.hex
make clean