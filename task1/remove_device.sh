#!/bin/bash
dev="phonebook"
sudo rm /dev/$dev

make remove
make clean
