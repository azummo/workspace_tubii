@echo off
arm-xilinx-linux-gnueabi-gcc -Wall -O0 -g3 -I"C:\Users\Ian\Documents\GitHub\workspace_tubii\new_tubii_server\src" -c -fmessage-length=0 -MMD -MP -MF"src/tubii_client.d" -MT"src/tubii_client.d" -o "src/tubii_client.o" "../src/tubii_client.c"
