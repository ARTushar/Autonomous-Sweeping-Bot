make all
sudo avrdude -p m32 -c usbasp -e -U flash:w:backup_main.hex
make clean